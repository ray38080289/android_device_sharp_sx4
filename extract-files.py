#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from functools import partial

from camera_hal_patches import (
    CAMERA_HAL_PATCHES,
    apply_camera_hal_patch,
)
from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.fixups_lib import (
    lib_fixups,
    lib_fixups_user_type,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)

namespace_imports = [
    'device/sharp/sx4',
    'hardware/mediatek',
    'hardware/mediatek/libmtkperf_client',
]


def lib_fixup_vendor_suffix(lib: str, partition: str, *args, **kwargs):
    return f'{lib}_{partition}' if partition == 'vendor' else None


lib_fixups: lib_fixups_user_type = {
    **lib_fixups,
    (
        'vendor.mediatek.hardware.videotelephony@1.0',
        'vendor.mediatek.hardware.videotelephony-V1-ndk',
    ): lib_fixup_vendor_suffix,
}

# ponytail: IMS fixups only. Everything else gets added as the first vendor
# build and boot report what is missing.
blob_fixups: blob_fixups_user_type = {
    # Factory fingerprint test triggers: fpc_tee_test is not shipped, and vendor_init
    # can't read the system.fqc.* trigger property
    'vendor/etc/init/hw/init.fingerprint.fpc1560_sx4.rc': blob_fixup()
        .regex_replace(r'(?s)\non property:vendor\.fqc\..*', '\n'),
    # PMIC engineer-mode debugfs nodes (EM app not shipped)
    'vendor/etc/init/hw/init.mt6833.rc': blob_fixup()
        .regex_replace(r'\n *#EM of MT636[02]\n( *chmod 0664 sys/kernel/debug/\S+\n)+', '\n'),
    # mmstat trace instance: its only reader (aee) is not shipped
    'vendor/etc/init/hw/init.project.rc': blob_fixup()
        .regex_replace(r'# start mmstat\non property:sys\.boot_completed=1\n( +\S.*\n)+\n', ''),
    (
        'system_ext/priv-app/ImsService/ImsService.apk'
    ): blob_fixup()
        .apktool_patch('ims-patches'),
    (
        'system_ext/lib64/libimsma.so',
    ): blob_fixup()
        .replace_needed('libsink.so', 'libsink-mtk.so'),
    (
        'system_ext/lib64/libsink-mtk.so',
    ): blob_fixup()
        .fix_soname(),
    # AIDL: camera.common V2 is not frozen on 23.2 (same fix as motorola/lamu)
    'vendor/lib64/libmtkcam_hal_aidl_common.so': blob_fixup()
        .replace_needed('android.hardware.camera.common-V2-ndk.so', 'android.hardware.camera.common-V1-ndk.so'),
    # AIDL: graphics.common is frozen: false on 23.2, so the platform links the
    # current version (V7). Types-only package; same fix as motorola/mt6768-common.
    (
        'vendor/bin/hw/android.hardware.graphics.allocator-V2-service-mediatek',
        'vendor/lib/egl/libGLES_mali.so',
        'vendor/lib/hw/android.hardware.graphics.allocator-V2-mediatek.so',
        'vendor/lib/hw/mapper.mediatek.so',
        'vendor/lib/libcodec2_fsr.so',
        'vendor/lib/libgpud.so',
        'vendor/lib/vendor.mediatek.hardware.pq_aidl-V2-ndk.so',
        'vendor/lib64/egl/libGLES_mali.so',
        'vendor/lib64/hw/android.hardware.graphics.allocator-V2-mediatek.so',
        'vendor/lib64/hw/mapper.mediatek.so',
        'vendor/lib64/libaimemc.so',
        'vendor/lib64/libcodec2_fsr.so',
        'vendor/lib64/libgpud.so',
        'vendor/lib64/libmtkcam_grallocutils.so',
        'vendor/lib64/vendor.mediatek.hardware.camera.isphal-V1-ndk.so',
        'vendor/lib64/vendor.mediatek.hardware.pq_aidl-V2-ndk.so',
        'vendor/lib64/vendor.mediatek.hardware.pq_aidl-V4-ndk.so',
    ): blob_fixup()
        .replace_needed('android.hardware.graphics.common-V5-ndk.so', 'android.hardware.graphics.common-V7-ndk.so'),
    (
        'vendor/lib/vendor.mediatek.hardware.pq_aidl-V7-ndk.so',
        'vendor/lib64/vendor.mediatek.hardware.pq_aidl-V7-ndk.so',
    ): blob_fixup()
        .replace_needed('android.hardware.graphics.common-V4-ndk.so', 'android.hardware.graphics.common-V7-ndk.so'),
    # Audio HAL: use the stock AIDL conversion lib (platform's links newer
    # media.audio.common.types); same as motorola/mt6768-common
    (
        'vendor/bin/hw/android.hardware.audio.service-aidl.mediatek',
        'vendor/lib/hw/android.hardware.soundtrigger3-impl.so',
        'vendor/lib64/hw/android.hardware.soundtrigger3-impl.so',
    ): blob_fixup()
        .replace_needed('libaudio_aidl_conversion_common_ndk.so', 'libaudio_aidl_conversion_common_ndk_prebuilt.so'),
    (
        'vendor/lib/android.hardware.audio.core-impl-mediatek.so',
        'vendor/lib64/android.hardware.audio.core-impl-mediatek.so',
    ): blob_fixup()
        .add_needed('libaudioutils_shim.so')
        .replace_needed('libaudio_aidl_conversion_common_ndk.so', 'libaudio_aidl_conversion_common_ndk_prebuilt.so'),
    # A15 blobs vs A16 libtinyxml2 ABI; same as motorola/mt6768-common
    (
        'vendor/lib/hw/audio.primary.mt6833.so',
        'vendor/lib/hw/vendor.mediatek.hardware.pq_aidl-impl.so',
        'vendor/lib/lib_power_applist.so',
        'vendor/lib/libpowerhal.so',
        'vendor/lib/libpqxmlparser.so',
        'vendor/lib/librt_extamp_intf.so',
        'vendor/lib64/hw/android.hardware.audio.effect.aidl-impl-mediatek.so',
        'vendor/lib64/hw/audio.primary.mt6833.so',
        'vendor/lib64/hw/hwcomposer.mt6833.so',
        'vendor/lib64/hw/vendor.mediatek.hardware.pq_aidl-impl.so',
        'vendor/lib64/lib_power_applist.so',
        'vendor/lib64/libpowerhal.so',
        'vendor/lib64/libpqxmlparser.so',
        'vendor/lib64/librt_extamp_intf.so',
        'vendor/lib64/libsilkybrightnesscore.so',
    ): blob_fixup()
        .replace_needed('libtinyxml2.so', 'libtinyxml2-v34.so'),
    # 32-bit blobs binding bionic symbols to LIBC_PRIVATE (check_elf_file)
    (
        'vendor/lib/libmp4enc_sa.ca7.so',
        'vendor/lib/libthha.so',
        'vendor/lib/libvcodec_oal.so',
    ): blob_fixup()
        .clear_symbol_version('__aeabi_memclr')
        .clear_symbol_version('__aeabi_memcpy')
        .clear_symbol_version('__aeabi_memset')
        .clear_symbol_version('__gnu_Unwind_Find_exidx'),
    # vendor libnativewindow exports no symbol versions
    'vendor/lib64/libANCBeauty.so': blob_fixup()
        .clear_symbol_version('AHardwareBuffer_allocate')
        .clear_symbol_version('AHardwareBuffer_createFromHandle')
        .clear_symbol_version('AHardwareBuffer_describe')
        .clear_symbol_version('AHardwareBuffer_getNativeHandle')
        .clear_symbol_version('AHardwareBuffer_lock')
        .clear_symbol_version('AHardwareBuffer_release')
        .clear_symbol_version('AHardwareBuffer_unlock'),
    (
        'vendor/lib/libudf.so',
        'vendor/lib64/libudf.so',
    ): blob_fixup()
        .clear_symbol_version('_ZN11unwindstack3Elf8GetRelPcEyPNS_7MapInfoE')
        .clear_symbol_version('_ZN11unwindstack3Elf8GetRelPcEmPNS_7MapInfoE')
        .clear_symbol_version('_ZN11unwindstack4Maps4FindEy')
        .clear_symbol_version('_ZN11unwindstack4Maps4FindEm')
        .clear_symbol_version('_ZN11unwindstack4Maps5ParseEv')
        .clear_symbol_version('_ZN11unwindstack4Regs11CurrentArchEv')
        .clear_symbol_version('_ZN11unwindstack6Memory19CreateProcessMemoryEi')
        .clear_symbol_version('_ZN11unwindstack7MapInfo6GetElfERKNSt3__110shared_ptrINS_6MemoryEEENS_8ArchEnumE')
        .clear_symbol_version('_ZNK11unwindstack10RemoteMaps11GetMapsFileEv')
        .clear_symbol_version('_ZTVN11unwindstack4MapsE'),
    # SONAME must match the file name (check_elf_file)
    (
        'vendor/lib/hw/awinic.audio.effect.so',
        'vendor/lib/libnir_neon_driver_ndk.mtk.vndk.so',
        'vendor/lib/libspeech_enh_lib.so',
        'vendor/lib64/hw/awinic.audio.effect.so',
        'vendor/lib64/libnir_neon_driver_ndk.mtk.vndk.so',
        'vendor/lib64/libspeech_enh_lib.so',
    ): blob_fixup()
        .fix_soname(),
    # AIDL: sensors clients, V2 -> V3 (platform version); same as mt6768-common/lamu
    (
        'vendor/bin/mnld',
        'vendor/lib/libaalservice.so',
        'vendor/lib64/libaalservice.so',
        'vendor/lib64/libcam.utils.sensorprovider.so',
    ): blob_fixup()
        .replace_needed('android.hardware.sensors-V2-ndk.so', 'android.hardware.sensors-V3-ndk.so'),
    # 1080p60 recording on the main camera, see camera_hal_patches.py
    **{
        blob: blob_fixup().call(partial(apply_camera_hal_patch, sites))
        for blob, sites in CAMERA_HAL_PATCHES.items()
    },
}  # fmt: skip

module = ExtractUtilsModule(
    'sx4',
    'sharp',
    blob_fixups=blob_fixups,
    lib_fixups=lib_fixups,
    namespace_imports=namespace_imports,
    add_firmware_proprietary_file=True,
)

if __name__ == '__main__':
    utils = ExtractUtils.device(module)
    utils.run()
