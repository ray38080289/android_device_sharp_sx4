/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.lineage.livedisplay-service.sx4"

#include "SunlightEnhancement.h"
#include "PictureQuality.h"

#include <android-base/logging.h>
#include <android-base/properties.h>

using ::android::base::GetIntProperty;

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {
namespace sx4 {

namespace {

// MTK AAL function bits (MtkPictureQualityWrapper in stock ShDisplayExtentionService)
constexpr int32_t kAalCabc = 0x2;
constexpr int32_t kAalDre = 0x4;

// Stored by the PQ HAL; stock reads it back as the current AAL function.
constexpr const char* kAalFunctionProp = "persist.vendor.sys.mtkaal.function";
// Stock default (outdoor view on): every function but CABC. Measured on 3.20C:
// outdoor view off -> -7 (DRE cleared), back on -> -3.
constexpr int32_t kAalFunctionDefault = -3;

}  // namespace

ndk::ScopedAStatus SunlightEnhancement::getEnabled(bool* _aidl_return) {
    // The PQ HAL persists the function mask, so this survives reboots.
    *_aidl_return = (GetIntProperty(kAalFunctionProp, kAalFunctionDefault) & kAalDre) != 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus SunlightEnhancement::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(lock_);

    // Same transform as stock mtksetOutdoorView(): toggle DRE, always drop CABC.
    int32_t function = GetIntProperty(kAalFunctionProp, kAalFunctionDefault);
    function = enabled ? (function | kAalDre) : (function & ~kAalDre);
    function &= ~kAalCabc;

    if (!PictureQuality::get().setFunction(function, true)) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_SERVICE_SPECIFIC);
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace sx4
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
