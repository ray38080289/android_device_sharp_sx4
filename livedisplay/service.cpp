/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.lineage.livedisplay-service.sx4"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include "DisplayModes.h"
#include "SunlightEnhancement.h"

using ::aidl::vendor::lineage::livedisplay::sx4::DisplayModes;
using ::aidl::vendor::lineage::livedisplay::sx4::SunlightEnhancement;

template <typename T>
static void registerService(const std::shared_ptr<T>& service) {
    std::string instance = std::string() + T::descriptor + "/default";
    binder_status_t status = AServiceManager_addService(service->asBinder().get(), instance.c_str());
    CHECK_EQ(status, STATUS_OK) << "Failed to register " << instance;
}

int main() {
    // One binder thread for incoming calls; outgoing PQ calls are serialized anyway.
    ABinderProcess_setThreadPoolMaxThreadCount(0);

    LOG(INFO) << "LiveDisplay HAL service is starting.";

    auto dm = ndk::SharedRefBase::make<DisplayModes>();
    auto se = ndk::SharedRefBase::make<SunlightEnhancement>();
    registerService(dm);
    registerService(se);

    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;  // should not reach
}
