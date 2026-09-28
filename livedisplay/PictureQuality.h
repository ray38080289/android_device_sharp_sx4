/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <android/binder_auto_utils.h>
#include <android/binder_ibinder.h>

#include <functional>
#include <mutex>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {
namespace sx4 {

// Client for vendor.mediatek.hardware.pq_aidl.IPictureQuality_AIDL/default.
// MTK ships this interface only as prebuilt NDK libraries (no .aidl, no headers),
// so the two calls used here are marshalled by hand. Transaction codes come from
// BpPictureQuality_AIDL in vendor.mediatek.hardware.pq_aidl-V2-ndk.so and match
// what stock's libjni_pq.so sends.
class PictureQuality {
  public:
    static PictureQuality& get();

    // Picture mode: 0 standard, 1 vivid, 2 user defined ("natural" on Sharp).
    bool setPQMode(int32_t mode, int32_t step);
    // MTK AAL function bitmask; persist=true also stores it in
    // persist.vendor.sys.mtkaal.function (stock setAALFunctionProperty()).
    bool setFunction(int32_t function, bool persist);

  private:
    PictureQuality() = default;
    bool transact(transaction_code_t code, const std::function<binder_status_t(AParcel*)>& write);

    std::mutex lock_;
    ndk::SpAIBinder binder_;
};

}  // namespace sx4
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
