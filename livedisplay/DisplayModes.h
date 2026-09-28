/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <aidl/vendor/lineage/livedisplay/BnDisplayModes.h>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {
namespace sx4 {

// Sharp "Image quality mode" (standard / vivid / natural) = MTK PQ picture mode.
// The PQ HAL persists the mode itself (persist.vendor.sys.pq.picmode) and
// restores it at boot, so this service keeps no state of its own.
class DisplayModes : public BnDisplayModes {
  public:
    ndk::ScopedAStatus getDisplayModes(std::vector<DisplayMode>* _aidl_return) override;
    ndk::ScopedAStatus getCurrentDisplayMode(DisplayMode* _aidl_return) override;
    ndk::ScopedAStatus getDefaultDisplayMode(DisplayMode* _aidl_return) override;
    ndk::ScopedAStatus setDisplayMode(int32_t modeID, bool makeDefault) override;
};

}  // namespace sx4
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
