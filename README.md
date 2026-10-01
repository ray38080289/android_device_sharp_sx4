<!--
SPDX-FileCopyrightText: The LineageOS Project
SPDX-License-Identifier: CC-BY-SA-4.0
-->

Device configuration for SHARP AQUOS wish4
==========================================

English | [繁體中文](README_zh-TW.md)

The SHARP AQUOS wish4 (codenamed _"sx4"_) is a mid-range rugged smartphone from SHARP.
It was released in July 2024.

This tree is tested on the TW version (SH-M27). The Japanese AQUOS wish4 is a separate
variant (single rear camera, different memory options) and is not supported.

## Device specifications

| Basic                   | Spec sheet                                              |
| ----------------------: | :------------------------------------------------------ |
| SoC                     | MediaTek Dimensity 700 (MT6833)                         |
| CPU                     | Octa-core (2x2.2 GHz Cortex-A76 & 6x2.0 GHz Cortex-A55) |
| GPU                     | Mali-G57 MC2                                            |
| Memory                  | 6/8 GB, LPDDR4X                                         |
| Storage                 | 128/256 GB, UFS 2.1                                     |
| Expandable Storage      | microSDXC, up to 1 TB                                   |
| Shipped Android Version | 14                                                      |
| Battery                 | Non-removable Li-Ion 5000 mAh                           |
| Dimensions              | 166 x 76 x 8.8 mm                                       |
| Weight                  | 190 g                                                   |
| Display                 | 6.6 inches LCD, 720 x 1612 pixels, 90Hz                 |
| Rear Camera 1           | 50 MP, f/1.9, (wide)                                    |
| Rear Camera 2           | 2 MP, (depth)                                           |
| Front Camera            | 8 MP                                                    |
| Fingerprint             | Yes                                                     |
| Sensors                 | Accelerometer, gyro, proximity, compass, light          |
| NFC                     | Yes                                                     |
| Audio                   | 3.5mm jack                                              |
| Connectivity            | 5G, Wi-Fi a/b/g/n/ac, BT 5.3, USB-C 2.0, nanoSIM + eSIM |
| Protection              | IP68 dust/water resistant, MIL-STD                      |
| Models                  | SH-M27                                                  |

## Device picture

![SHARP AQUOS wish4](https://tw.sharp/sites/default/files/styles/resize_640x640/public/2024-06/wish4%20%E6%9C%88%E5%85%89%E7%99%BD_%E5%85%A8.png?itok=LH-5eHlJ "SHARP AQUOS wish4 (Moonlight White)")

_Image: tw.sharp_

## Building

> **Unofficial and experimental.** Builds are `userdebug` and signed with test keys. With
> `WITH_ADB_INSECURE=true` (used for debugging) adb runs as root without authorization. Keep a
> full backup of the phone before flashing anything (see [Backup and recovery](#backup-and-recovery)).

Everything is built from source or extracted from your own phone; no prebuilt kernel or
proprietary files are distributed.

### 1. Source

```
repo init -u https://github.com/LineageOS/android.git -b lineage-23.2 --git-lfs
```

Create `.repo/local_manifests/sx4.xml`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
  <remote name="sx4" fetch="https://github.com/ray38080289" />

  <project path="device/sharp/sx4" name="android_device_sharp_sx4" remote="sx4" revision="lineage-23.2" />
  <project path="device/mediatek/sepolicy_vndr" name="LineageOS/android_device_mediatek_sepolicy_vndr" remote="github" />
  <project path="hardware/mediatek" name="android_hardware_mediatek" remote="sx4" revision="sx4-iwlan" />

  <remove-project name="LineageOS/android_frameworks_base" />
  <project path="frameworks/base" name="android_frameworks_base" remote="sx4" revision="sx4-wakelock-frozen-leak" groups="pdk-cw-fs,pdk-fs,sysui-studio" />
  <remove-project name="LineageOS/android_packages_apps_Aperture" />
  <project path="packages/apps/Aperture" name="android_packages_apps_Aperture" remote="sx4" revision="sx4-video-quality" />
  <remove-project name="LineageOS/android_vendor_apn" />
  <project path="vendor/apn" name="android_vendor_apn" remote="sx4" revision="sx4-stock-apns" />
</manifest>
```

```
repo sync
```

### 2. Kernel

The kernel is Google's certified GKI (android15-6.6, ci.android.com build 16457230). The vendor
kernel modules and the device tree images (`mt6833.dtb`, `dtbo.img`) are built from
[android_kernel_sharp_sx4-modules](https://github.com/ray38080289/android_kernel_sharp_sx4-modules),
which also assembles `device/sharp/sx4-kernels`. It fetches the ACK source, clang and the GKI
artifacts itself:

```
mkdir sx4kroot && cd sx4kroot
git clone -b lineage-23.2 https://github.com/ray38080289/android_kernel_sharp_sx4-modules kernel_device_modules-6.6
kernel_device_modules-6.6/build_sx4.sh lineage /path/to/android/device/sharp/sx4-kernels/6.6
```

### 3. Proprietary files

Extract them from your own phone, running the stock TW firmware `00WW_3_20C000` in slot `_a`:

1. Take the full backup described in [Backup and recovery](#backup-and-recovery).
2. Copy these into one folder, named without the slot suffix and with `.img`: `super.img`,
   `dpm.img`, `gz.img`, `lk.img`, `mcupm.img`, `md1img.img`, `pi_img.img`, `scp.img`, `spmfw.img`,
   `sspm.img`, `tee.img` (for example mtkclient's `lk_a.bin` becomes `lk.img`).
3. Run:

```
cd device/sharp/sx4
./extract-files.py /path/to/folder
```

The camera HAL patches for 1080p60 (`camera_hal_patches.py`) are applied during extraction;
every patched instruction is checked against the stock one first.

### 4. Build

```
source build/envsetup.sh
lunch lineage_sx4-bp4a-userdebug
m bacon
```

### 5. Install

The bootloader must be unlocked. There is no OEM unlock: it is done in download mode by rewriting
the `seccfg` partition (`lock_state` 1 → 3, re-signed with the phone's hardware key). With the
phone in download mode (after the backup):

```
python mtk.py da seccfg unlock
```

The phone then boots in the orange (unlocked) state. The stock firmware refuses
`adb reboot bootloader`, so:

```
adb reboot fastboot
fastboot reboot bootloader
fastboot flash boot_a boot.img
fastboot flash init_boot_a init_boot.img
fastboot flash vendor_boot_a vendor_boot.img
fastboot flash dtbo_a dtbo.img
fastboot flash vbmeta_a vbmeta.img
fastboot reboot recovery
```

In the LineageOS recovery: **Factory reset → Format data**, then **Apply update → Apply from ADB**,
and on the computer `adb sideload lineage-23.2-*-UNOFFICIAL-sx4.zip`.

### Backup and recovery

Before unlocking or flashing anything, read back **all** partitions in download mode and keep the
files: they include the phone's IMEI and calibration data (`nvram`, `nvdata`, `proinfo`, ...),
which cannot be recreated. With [mtkclient](https://github.com/bkerler/mtkclient) and the phone in
download mode:

```
python mtk.py rl backup --skip userdata
```

Writing it back restores the stock firmware:

```
python mtk.py wl backup
```

The backup holds the locked `seccfg`, so writing it back also relocks the bootloader; never write
`seccfg` back alone over a LineageOS install.

The mtkclient commands above are not tested on this phone; the port was developed with
GeekFlashTool, which does the same backup, unlock and restore from a GUI (Chinese only).

Entering download mode (phone powered off):

1. Hold **Power + Volume up** and count the vibrations. The right round is exactly **6
   vibrations followed by a clear pause**; if a 7th comes, keep holding for the next round.
2. Right after the 6th vibration, also press **Volume down** (keep Volume up held).
3. After about **2** more vibrations, release **Volume up and Volume down**; keep holding **Power**.
4. At the next pause in the vibrations, press **Volume up** once.

Bugs: this is a proof of concept. Logs (`adb logcat`, `adb shell dmesg`) with issue reports are
welcome; so are fixes.
