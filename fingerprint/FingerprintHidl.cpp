/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

// The stock FPC fingerprint HAL is a self-contained HIDL 2.1 service (it talks
// to its trustlet itself, there is no fingerprint.*.so). This module exposes it
// as a legacy fingerprint_device_t: every call is forwarded to the HIDL service
// and its callbacks are turned back into fingerprint_msg_t. It is the reverse of
// hardware/interfaces/biometrics/fingerprint/2.1/default.

#define LOG_TAG "fingerprint.sx4"

#include <android/hardware/biometrics/fingerprint/2.1/IBiometricsFingerprint.h>
#include <hardware/fingerprint.h>
#include <hardware/hardware.h>
#include <hidl/HidlTransportSupport.h>
#include <hwbinder/ProcessState.h>
#include <log/log.h>

#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <mutex>

using ::android::sp;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo;
using ::android::hardware::biometrics::fingerprint::V2_1::FingerprintError;
using ::android::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprint;
using ::android::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprintClientCallback;
using ::android::hardware::biometrics::fingerprint::V2_1::RequestStatus;
using ::android::hidl::base::V1_0::IBase;

static_assert(sizeof(hw_auth_token_t) == 69, "HIDL auth token is 69 bytes");

namespace {

std::mutex gLock;
fingerprint_notify_t gNotify = nullptr;
sp<IBiometricsFingerprint> gHal;

void deliver(const fingerprint_msg_t& msg) {
    fingerprint_notify_t notify;
    {
        std::lock_guard<std::mutex> lock(gLock);
        notify = gNotify;
    }
    if (notify == nullptr) {
        ALOGE("callback %d before set_notify", msg.type);
        return;
    }
    notify(&msg);
}

class Callback : public IBiometricsFingerprintClientCallback {
  public:
    Return<void> onEnrollResult(uint64_t, uint32_t fid, uint32_t gid, uint32_t remaining) override {
        fingerprint_msg_t msg = {};
        msg.type = FINGERPRINT_TEMPLATE_ENROLLING;
        msg.data.enroll.finger = {gid, fid};
        msg.data.enroll.samples_remaining = remaining;
        deliver(msg);
        return Void();
    }

    Return<void> onAcquired(uint64_t, FingerprintAcquiredInfo info, int32_t vendorCode) override {
        fingerprint_msg_t msg = {};
        msg.type = FINGERPRINT_ACQUIRED;
        msg.data.acquired.acquired_info = static_cast<fingerprint_acquired_info_t>(
                info == FingerprintAcquiredInfo::ACQUIRED_VENDOR
                        ? FINGERPRINT_ACQUIRED_VENDOR_BASE + vendorCode
                        : static_cast<int32_t>(info));
        deliver(msg);
        return Void();
    }

    Return<void> onAuthenticated(uint64_t, uint32_t fid, uint32_t gid,
                                 const hidl_vec<uint8_t>& token) override {
        fingerprint_msg_t msg = {};
        msg.type = FINGERPRINT_AUTHENTICATED;
        msg.data.authenticated.finger = {gid, fid};
        if (token.size() == sizeof(hw_auth_token_t)) {
            memcpy(&msg.data.authenticated.hat, token.data(), sizeof(hw_auth_token_t));
        }
        deliver(msg);
        return Void();
    }

    Return<void> onError(uint64_t, FingerprintError error, int32_t vendorCode) override {
        fingerprint_msg_t msg = {};
        msg.type = FINGERPRINT_ERROR;
        msg.data.error = static_cast<fingerprint_error_t>(
                error == FingerprintError::ERROR_VENDOR
                        ? FINGERPRINT_ERROR_VENDOR_BASE + vendorCode
                        : static_cast<int32_t>(error));
        deliver(msg);
        return Void();
    }

    Return<void> onRemoved(uint64_t, uint32_t fid, uint32_t gid, uint32_t remaining) override {
        fingerprint_msg_t msg = {};
        msg.type = FINGERPRINT_TEMPLATE_REMOVED;
        msg.data.removed.finger = {gid, fid};
        msg.data.removed.remaining_templates = remaining;
        deliver(msg);
        return Void();
    }

    Return<void> onEnumerate(uint64_t, uint32_t fid, uint32_t gid, uint32_t remaining) override {
        fingerprint_msg_t msg = {};
        msg.type = FINGERPRINT_TEMPLATE_ENUMERATING;
        msg.data.enumerated.finger = {gid, fid};
        msg.data.enumerated.remaining_templates = remaining;
        deliver(msg);
        return Void();
    }
};

// The stock service keeps session state (active group, callback) that cannot be
// rebuilt behind the AIDL service. Exit and let init restart the AIDL service,
// which opens this module again from scratch.
class DeathRecipient : public hidl_death_recipient {
    void serviceDied(uint64_t, const android::wp<IBase>&) override {
        ALOGE("stock fingerprint HIDL service died, restarting");
        _exit(1);
    }
};

int status(const Return<RequestStatus>& ret) {
    if (!ret.isOk()) return -EIO;
    return static_cast<int>(static_cast<RequestStatus>(ret));
}

int set_notify(fingerprint_device_t*, fingerprint_notify_t notify) {
    std::lock_guard<std::mutex> lock(gLock);
    gNotify = notify;
    return 0;
}

uint64_t pre_enroll(fingerprint_device_t*) {
    auto ret = gHal->preEnroll();
    return ret.isOk() ? static_cast<uint64_t>(ret) : 0;
}

int enroll(fingerprint_device_t*, const hw_auth_token_t* hat, uint32_t gid, uint32_t timeout_sec) {
    hidl_array<uint8_t, 69> token;
    memcpy(token.data(), hat, sizeof(hw_auth_token_t));
    return status(gHal->enroll(token, gid, timeout_sec));
}

int post_enroll(fingerprint_device_t*) {
    return status(gHal->postEnroll());
}

uint64_t get_authenticator_id(fingerprint_device_t*) {
    auto ret = gHal->getAuthenticatorId();
    return ret.isOk() ? static_cast<uint64_t>(ret) : 0;
}

int cancel(fingerprint_device_t*) {
    return status(gHal->cancel());
}

int enumerate(fingerprint_device_t*) {
    return status(gHal->enumerate());
}

int remove(fingerprint_device_t*, uint32_t gid, uint32_t fid) {
    return status(gHal->remove(gid, fid));
}

int set_active_group(fingerprint_device_t*, uint32_t gid, const char* store_path) {
    return status(gHal->setActiveGroup(gid, hidl_string(store_path)));
}

int authenticate(fingerprint_device_t*, uint64_t operation_id, uint32_t gid) {
    return status(gHal->authenticate(operation_id, gid));
}

int close_device(hw_device_t* dev) {
    delete reinterpret_cast<fingerprint_device_t*>(dev);
    return 0;
}

int open_device(const hw_module_t* module, const char*, hw_device_t** device) {
    // HIDL callbacks arrive on hwbinder threads of this process.
    android::hardware::configureRpcThreadpool(1, false /* callerWillJoin */);
    android::hardware::ProcessState::self()->startThreadPool();

    gHal = IBiometricsFingerprint::getService();
    if (gHal == nullptr) {
        ALOGE("stock fingerprint HIDL service not found");
        return -ENODEV;
    }
    gHal->linkToDeath(new DeathRecipient(), 0);
    if (gHal->setNotify(new Callback()) == 0u) {
        ALOGE("setNotify failed");
        return -ENODEV;
    }

    auto* dev = new fingerprint_device_t();
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = FINGERPRINT_MODULE_API_VERSION_2_1;
    dev->common.module = const_cast<hw_module_t*>(module);
    dev->common.close = close_device;
    dev->set_notify = set_notify;
    dev->pre_enroll = pre_enroll;
    dev->enroll = enroll;
    dev->post_enroll = post_enroll;
    dev->get_authenticator_id = get_authenticator_id;
    dev->cancel = cancel;
    dev->enumerate = enumerate;
    dev->remove = remove;
    dev->set_active_group = set_active_group;
    dev->authenticate = authenticate;
    *device = &dev->common;
    return 0;
}

hw_module_methods_t methods = {
        .open = open_device,
};

}  // namespace

fingerprint_module_t HAL_MODULE_INFO_SYM = {
        .common =
                {
                        .tag = HARDWARE_MODULE_TAG,
                        .module_api_version = FINGERPRINT_MODULE_API_VERSION_2_1,
                        .hal_api_version = HARDWARE_HAL_API_VERSION,
                        .id = FINGERPRINT_HARDWARE_MODULE_ID,
                        .name = "sx4 FPC HIDL bridge",
                        .author = "The LineageOS Project",
                        .methods = &methods,
                },
};
