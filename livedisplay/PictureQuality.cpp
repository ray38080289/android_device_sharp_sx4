/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.lineage.livedisplay-service.sx4"

#include "PictureQuality.h"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_stability.h>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {
namespace sx4 {

namespace {

constexpr const char* kDescriptor = "vendor.mediatek.hardware.pq_aidl.IPictureQuality_AIDL";
constexpr const char* kInstance = "vendor.mediatek.hardware.pq_aidl.IPictureQuality_AIDL/default";

constexpr transaction_code_t kSetFunction = 0x37;  // setFunction(int, boolean)
constexpr transaction_code_t kSetPQMode = 0x58;    // setPQMode(int mode, int step)

// Proxy-only class: we never host this interface, we only need the descriptor so
// AIBinder_prepareTransaction() writes the right interface token.
void* onCreate(void*) {
    return nullptr;
}
void onDestroy(void*) {}
binder_status_t onTransact(AIBinder*, transaction_code_t, const AParcel*, AParcel*) {
    return STATUS_UNKNOWN_TRANSACTION;
}

AIBinder_Class* pqClass() {
    static AIBinder_Class* clazz = AIBinder_Class_define(kDescriptor, onCreate, onDestroy, onTransact);
    return clazz;
}

}  // namespace

PictureQuality& PictureQuality::get() {
    static PictureQuality instance;
    return instance;
}

bool PictureQuality::transact(transaction_code_t code,
                              const std::function<binder_status_t(AParcel*)>& write) {
    std::lock_guard<std::mutex> lock(lock_);

    // One retry: the PQ HAL may have restarted since we last talked to it.
    for (int attempt = 0; attempt < 2; attempt++) {
        if (binder_.get() == nullptr) {
            binder_ = ndk::SpAIBinder(AServiceManager_waitForService(kInstance));
            if (binder_.get() == nullptr || !AIBinder_associateClass(binder_.get(), pqClass())) {
                LOG(ERROR) << "Unable to get " << kInstance;
                binder_ = ndk::SpAIBinder();
                return false;
            }
        }

        ndk::ScopedAParcel in, out;
        binder_status_t status = AIBinder_prepareTransaction(binder_.get(), in.getR());
        if (status == STATUS_OK) status = write(in.get());
        if (status == STATUS_OK) {
            status = AIBinder_transact(binder_.get(), code, in.getR(), out.getR(),
                                       FLAG_PRIVATE_LOCAL);
        }
        if (status == STATUS_DEAD_OBJECT) {
            binder_ = ndk::SpAIBinder();
            continue;
        }
        if (status != STATUS_OK) {
            LOG(ERROR) << "Transaction " << code << " failed: " << status;
            return false;
        }

        ndk::ScopedAStatus result;
        status = AParcel_readStatusHeader(out.get(), result.getR());
        if (status != STATUS_OK || !result.isOk()) {
            LOG(ERROR) << "Transaction " << code << " returned " << result.getDescription();
            return false;
        }
        return true;
    }
    return false;
}

bool PictureQuality::setPQMode(int32_t mode, int32_t step) {
    return transact(kSetPQMode, [&](AParcel* p) {
        binder_status_t s = AParcel_writeInt32(p, mode);
        return s == STATUS_OK ? AParcel_writeInt32(p, step) : s;
    });
}

bool PictureQuality::setFunction(int32_t function, bool persist) {
    return transact(kSetFunction, [&](AParcel* p) {
        binder_status_t s = AParcel_writeInt32(p, function);
        return s == STATUS_OK ? AParcel_writeBool(p, persist) : s;
    });
}

}  // namespace sx4
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
