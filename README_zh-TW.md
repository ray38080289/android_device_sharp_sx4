<!--
SPDX-FileCopyrightText: The LineageOS Project
SPDX-License-Identifier: CC-BY-SA-4.0
-->

SHARP AQUOS wish4 裝置設定
==========================

[English](README.md) | 繁體中文

SHARP AQUOS wish4（代號 _"sx4"_）是 SHARP 的中階三防手機。於 2024 年 7 月上市。

本裝置樹以台版（SH-M27）測試。日本版 AQUOS wish4 是不同的版本（後置單鏡頭、記憶體配置不同），不支援。

## 規格

| 項目         | 規格                                                   |
| -----------: | :----------------------------------------------------- |
| 處理器       | MediaTek 天璣 700（MT6833）                            |
| CPU          | 八核心（2 × 2.2 GHz Cortex-A76 + 6 × 2.0 GHz Cortex-A55） |
| GPU          | Mali-G57 MC2                                           |
| 記憶體       | 6/8 GB，LPDDR4X                                        |
| 儲存空間     | 128/256 GB，UFS 2.1                                    |
| 擴充儲存     | microSDXC，最高 1 TB                                   |
| 出廠 Android | 14                                                     |
| 電池         | 不可拆式鋰離子電池 5000 mAh                            |
| 尺寸         | 166 × 76 × 8.8 mm                                      |
| 重量         | 190 g                                                  |
| 螢幕         | 6.6 吋 LCD，720 × 1612，90Hz                           |
| 後置相機 1   | 5000 萬畫素，f/1.9（主鏡頭）                           |
| 後置相機 2   | 200 萬畫素（景深）                                     |
| 前置相機     | 800 萬畫素                                             |
| 指紋辨識     | 有                                                     |
| 感測器       | 加速度、陀螺儀、距離、電子羅盤、環境光                 |
| NFC          | 有                                                     |
| 音訊         | 3.5 mm 耳機孔                                          |
| 連線         | 5G、Wi-Fi a/b/g/n/ac、藍牙 5.3、USB-C 2.0、nanoSIM + eSIM |
| 防護         | IP68 防塵防水、MIL 軍規                                |
| 型號         | SH-M27                                                 |

## 機型照片

![SHARP AQUOS wish4](https://tw.sharp/sites/default/files/styles/resize_640x640/public/2024-06/wish4%20%E6%9C%88%E5%85%89%E7%99%BD_%E5%85%A8.png?itok=LH-5eHlJ "SHARP AQUOS wish4（月光白）")

_圖片來源：tw.sharp_

## 建置

> **非官方、實驗性質。** 建置版本為 `userdebug`，使用測試金鑰簽署。設定
> `WITH_ADB_INSECURE=true`（除錯用）時，adb 不需授權即以 root 執行。刷機前請先完整備份手機
> （見[備份與救援](#備份與救援)）。

所有內容都從原始碼建置，或從你自己的手機抽取；不散布預先編好的 kernel 或原廠專有檔案。

### 1. 原始碼

```
repo init -u https://github.com/LineageOS/android.git -b lineage-23.2 --git-lfs
```

建立 `.repo/local_manifests/sx4.xml`，內容同[英文版](README.md#1-source)，然後：

```
repo sync
```

### 2. Kernel

Kernel 是 Google 認證的 GKI（android15-6.6，ci.android.com build 16457230）。Vendor kernel
模組與裝置樹映像（`mt6833.dtb`、`dtbo.img`）由
[android_kernel_sharp_sx4-modules](https://github.com/ray38080289/android_kernel_sharp_sx4-modules)
從原始碼建置，並組出 `device/sharp/sx4-kernels`。ACK 原始碼、clang 與 GKI 產物會自動下載：

```
mkdir sx4kroot && cd sx4kroot
git clone -b lineage-23.2 https://github.com/ray38080289/android_kernel_sharp_sx4-modules kernel_device_modules-6.6
kernel_device_modules-6.6/build_sx4.sh lineage /path/to/android/device/sharp/sx4-kernels/6.6
```

### 3. 原廠專有檔案

從你自己的手機抽取，slot `_a` 須為台版原廠韌體 `00WW_3_20C000`：

1. 在下載模式（BROM）讀回 `super` 與各韌體分割區，例如用 GeekFlashTool 或
   [mtkclient](https://github.com/bkerler/mtkclient)。
2. 放進同一個資料夾，去掉 slot 字尾：`super.img`、`dpm.img`、`gz.img`、`lk.img`、
   `mcupm.img`、`md1img.img`、`pi_img.img`、`scp.img`、`spmfw.img`、`sspm.img`、`tee.img`
   （例如 `lk_a.img` 改名為 `lk.img`）。
3. 執行：

```
cd device/sharp/sx4
./extract-files.py /path/to/folder
```

1080p60 的相機 HAL 修補（`camera_hal_patches.py`）會在抽取時套用，每個位置都會先核對原廠指令。

### 4. 編譯

```
source build/envsetup.sh
lunch lineage_sx4-bp4a-userdebug
m bacon
```

### 5. 安裝

Bootloader 必須已解鎖。這支手機沒有 OEM 解鎖選項，解鎖是在下載模式改寫 `seccfg` 分割區
（`lock_state` 1 → 3，再用手機的硬體金鑰重新簽）。GeekFlashTool 的解鎖做的就是這件事，mtkclient 的
`da seccfg unlock` 也是（mtkclient 未在這支手機上測過）。解鎖後開機會顯示 orange（已解鎖）狀態。

原廠系統會擋 `adb reboot bootloader`，所以：

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

在 LineageOS recovery 裡選 **Factory reset → Format data**，再選 **Apply update → Apply from ADB**，
電腦端執行 `adb sideload lineage-23.2-*-UNOFFICIAL-sx4.zip`。

### 備份與救援

刷機前，請在下載模式讀回**全部**分割區並保存好：其中包含這支手機的 IMEI 與校正資料（`nvram`、
`nvdata`、`proinfo` 等），遺失就無法重建。在下載模式把備份寫回去，即可還原原廠韌體。解鎖前做的備份裡，
`seccfg` 是上鎖狀態：寫回去會重新上鎖 bootloader，所以不要在裝著 LineageOS 的手機上單獨寫回它。

進入下載模式（手機關機狀態）：

1. 按住 **電源 + 音量上**，數手機震動。正確的一輪是**剛好 6 下震動後明顯停頓**；若出現第 7 下，
   繼續按著等下一輪。
2. 第 6 下震動後，**馬上加按音量下**（音量上繼續按著）。
3. 約再震 **2** 下後，放開**音量上**。
4. 下一次震動停頓時，**按一下音量上**。

問題回報：這是概念驗證版本。歡迎附上 log（`adb logcat`、`adb shell dmesg`）回報問題，也歡迎送修正。
