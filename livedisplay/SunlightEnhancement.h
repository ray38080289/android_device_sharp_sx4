/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <aidl/vendor/lineage/livedisplay/BnSunlightEnhancement.h>

#include <mutex>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {
namespace sx4 {

// Sharp "Outdoor view": MTK AAL DRE on, CABC off. With DRE on the AAL service
// follows the ambient light sensor by itself.
class SunlightEnhancement : public BnSunlightEnhancement {
  public:
    ndk::ScopedAStatus getEnabled(bool* _aidl_return) override;
    ndk::ScopedAStatus setEnabled(bool enabled) override;

  private:
    std::mutex lock_;  // serializes the read-modify-write of the AAL mask
};

}  // namespace sx4
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
