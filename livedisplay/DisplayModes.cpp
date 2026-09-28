/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.lineage.livedisplay-service.sx4"

#include "DisplayModes.h"
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

// id = MTK PIC_MODE_*; names are LineageParts color profile keys.
const std::vector<DisplayMode> kModes = {
        {0, "standard"},  // PIC_MODE_STANDARD, Sharp "Standard"
        {1, "dynamic"},   // PIC_MODE_VIVID, Sharp "Vivid"
        {2, "natural"},   // PIC_MODE_USER_DEF, Sharp "Natural"
};
constexpr int32_t kDefaultMode = 0;

// stock libjni_pq: PictureQuality.getDefaultOffTransitionStep() == 1
constexpr int32_t kTransitionStep = 1;

}  // namespace

ndk::ScopedAStatus DisplayModes::getDisplayModes(std::vector<DisplayMode>* _aidl_return) {
    *_aidl_return = kModes;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus DisplayModes::getCurrentDisplayMode(DisplayMode* _aidl_return) {
    int32_t id = GetIntProperty("persist.vendor.sys.pq.picmode", kDefaultMode);
    if (id < 0 || id >= static_cast<int32_t>(kModes.size())) id = kDefaultMode;
    *_aidl_return = kModes[id];
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus DisplayModes::getDefaultDisplayMode(DisplayMode* _aidl_return) {
    // The HAL-persisted mode is what comes back after a reboot.
    return getCurrentDisplayMode(_aidl_return);
}

ndk::ScopedAStatus DisplayModes::setDisplayMode(int32_t modeID, bool /* makeDefault */) {
    if (modeID < 0 || modeID >= static_cast<int32_t>(kModes.size())) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
    }
    if (!PictureQuality::get().setPQMode(modeID, kTransitionStep)) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_SERVICE_SPECIFIC);
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace sx4
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
