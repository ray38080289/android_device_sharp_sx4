#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

DEVICE_PATH := device/sharp/sx4
KERNEL_PATH := $(DEVICE_PATH)-kernels

# A/B
AB_OTA_UPDATER := true
AB_OTA_PARTITIONS := \
    boot \
    dtbo \
    init_boot \
    odm \
    odm_dlkm \
    product \
    system \
    system_dlkm \
    system_ext \
    vbmeta \
    vbmeta_system \
    vbmeta_vendor \
    vendor \
    vendor_boot \
    vendor_dlkm

# Architecture (MT6833: 2x Cortex-A76 + 6x Cortex-A55)
TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-2a-dotprod
TARGET_CPU_ABI := arm64-v8a
TARGET_CPU_ABI2 :=
TARGET_CPU_VARIANT := generic
TARGET_CPU_VARIANT_RUNTIME := cortex-a76

# Stock is zygote64_32 and the vendor ships 32-bit daemons started by rc
# (fuelgauged, aee_aedv, v3avpud) plus a full lib/ tree
TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv8-2a
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_2ND_CPU_VARIANT := generic
TARGET_2ND_CPU_VARIANT_RUNTIME := cortex-a55

# Boot image
# Stock vendor_boot header stores absolute addresses:
#   kernel 0x40080000, ramdisk 0x51100000, tags/dtb 0x47c80000
BOARD_KERNEL_BASE              := 0x40078000
BOARD_KERNEL_OFFSET            := 0x00008000
BOARD_KERNEL_PAGESIZE          := 4096
BOARD_RAMDISK_OFFSET           := 0x11088000
BOARD_KERNEL_TAGS_OFFSET       := 0x07c08000
BOARD_DTB_OFFSET               := 0x07c08000
BOARD_BOOT_HEADER_VERSION      := 4
BOARD_INIT_BOOT_HEADER_VERSION := $(BOARD_BOOT_HEADER_VERSION)

BOARD_KERNEL_IMAGE_NAME := Image.gz
BOARD_INCLUDE_DTB_IN_BOOTIMG := true
BOARD_KERNEL_SEPARATED_DTBO := true

BOARD_RAMDISK_USE_LZ4 := true
BOARD_USES_GENERIC_KERNEL_IMAGE := true
BOARD_MOVE_GSI_AVB_KEYS_TO_VENDOR_BOOT := true

BOARD_MKBOOTIMG_ARGS := --base $(BOARD_KERNEL_BASE)
BOARD_MKBOOTIMG_ARGS += --kernel_offset $(BOARD_KERNEL_OFFSET)
BOARD_MKBOOTIMG_ARGS += --pagesize $(BOARD_KERNEL_PAGESIZE)
BOARD_MKBOOTIMG_ARGS += --ramdisk_offset $(BOARD_RAMDISK_OFFSET)
BOARD_MKBOOTIMG_ARGS += --tags_offset $(BOARD_KERNEL_TAGS_OFFSET)
BOARD_MKBOOTIMG_ARGS += --dtb_offset $(BOARD_DTB_OFFSET)
BOARD_MKBOOTIMG_ARGS += --header_version $(BOARD_BOOT_HEADER_VERSION)
BOARD_MKBOOTIMG_INIT_ARGS += --header_version $(BOARD_INIT_BOOT_HEADER_VERSION)

BOARD_KERNEL_CMDLINE := bootopt=64S3,32N2,64N2

# DEBUG(bring-up): permissive to rule out sepolicy. Remove once it boots.
# On the kernel cmdline: Sharp's LK builds its own bootconfig and drops the
# vendor_boot one (measured: vendor_boot bootconfig had it, device ran enforcing).
# (hung_task_panic dropped: MTK mivr_thread sleeps >120s by design and the
# panic killed an OTA mid-install.)
BOARD_KERNEL_CMDLINE += androidboot.selinux=permissive

# Bootloader
BOARD_VENDOR := sharp
TARGET_BOOTLOADER_BOARD_NAME := mt6833
TARGET_BOARD_PLATFORM := mt6833
TARGET_NO_BOOTLOADER := true

# Display
TARGET_SCREEN_DENSITY := 280
TARGET_USES_VULKAN := true

# Kernel
# ponytail: stock kernel + modules as prebuilts (stage 1). The charter requires
# GKI devices to build all feasible modules from source; replace with the Sharp
# GPL drop (kernel_device_modules-6.6) before submission.
TARGET_KERNEL_DIR := $(KERNEL_PATH)/6.6
TARGET_PROVIDES_STATIC_MODULE_LISTS := true

BOARD_SYSTEM_KERNEL_MODULES_LOAD := $(strip $(shell cat $(TARGET_KERNEL_DIR)/system_dlkm.modules.load))
BOARD_VENDOR_KERNEL_MODULES_LOAD := $(strip $(shell cat $(TARGET_KERNEL_DIR)/vendor_dlkm.modules.load))
BOARD_VENDOR_RAMDISK_KERNEL_MODULES_LOAD := $(strip $(shell cat $(TARGET_KERNEL_DIR)/vendor_ramdisk.modules.load))
BOARD_VENDOR_RAMDISK_RECOVERY_KERNEL_MODULES_LOAD := $(strip $(shell cat $(TARGET_KERNEL_DIR)/vendor_ramdisk.modules.load.recovery))

BOARD_KERNEL_MODULE_DIR := $(TARGET_KERNEL_DIR)
BOARD_PREBUILT_DTBIMAGE_DIR := $(TARGET_KERNEL_DIR)
BOARD_PREBUILT_DTBOIMAGE := $(TARGET_KERNEL_DIR)/dtbo.img
TARGET_PREBUILT_KERNEL := $(TARGET_KERNEL_DIR)/$(BOARD_KERNEL_IMAGE_NAME)

ALL_VENDOR_RAMDISK_MODULES := $(sort $(BOARD_VENDOR_RAMDISK_KERNEL_MODULES_LOAD) $(BOARD_VENDOR_RAMDISK_RECOVERY_KERNEL_MODULES_LOAD))
BOARD_SYSTEM_KERNEL_MODULES := $(addprefix $(BOARD_KERNEL_MODULE_DIR)/,$(BOARD_SYSTEM_KERNEL_MODULES_LOAD))
BOARD_VENDOR_KERNEL_MODULES := $(addprefix $(BOARD_KERNEL_MODULE_DIR)/,$(BOARD_VENDOR_KERNEL_MODULES_LOAD))
BOARD_VENDOR_RAMDISK_KERNEL_MODULES := $(addprefix $(BOARD_KERNEL_MODULE_DIR)/,$(ALL_VENDOR_RAMDISK_MODULES))

# Connectivity modules are insmod'ed by vendor rc files, not modules.load
BOARD_VENDOR_KERNEL_MODULES += \
    $(BOARD_KERNEL_MODULE_DIR)/bt_drv_connac1x.ko \
    $(BOARD_KERNEL_MODULE_DIR)/fmradio_drv_mt6631_6635.ko \
    $(BOARD_KERNEL_MODULE_DIR)/gps_drv_stp.ko \
    $(BOARD_KERNEL_MODULE_DIR)/wlan_drv_gen4m_6833.ko \
    $(BOARD_KERNEL_MODULE_DIR)/wmt_chrdev_wifi.ko \
    $(BOARD_KERNEL_MODULE_DIR)/wmt_drv.ko

# Partitions
BOARD_FLASH_BLOCK_SIZE := 262144 # (BOARD_KERNEL_PAGESIZE * 64)
BOARD_BOOTIMAGE_PARTITION_SIZE := 41943040
BOARD_DTBOIMG_PARTITION_SIZE := 8388608
BOARD_INIT_BOOT_IMAGE_PARTITION_SIZE := 8388608
BOARD_VENDOR_BOOTIMAGE_PARTITION_SIZE := 67108864

BOARD_SUPER_PARTITION_SIZE := 8589934592
BOARD_SUPER_PARTITION_GROUPS := sharp_dynamic_partitions
BOARD_SHARP_DYNAMIC_PARTITIONS_PARTITION_LIST := odm odm_dlkm product system system_dlkm system_ext vendor vendor_dlkm
BOARD_SHARP_DYNAMIC_PARTITIONS_SIZE := $(shell expr $(BOARD_SUPER_PARTITION_SIZE) - 4194304) # (BOARD_SUPER_PARTITION_SIZE - "reasonable overhead of 4 MiB")

# Stock: ext4 for system/system_ext/product/vendor, erofs for odm and *_dlkm
BOARD_ODMIMAGE_FILE_SYSTEM_TYPE := erofs
BOARD_ODM_DLKMIMAGE_FILE_SYSTEM_TYPE := erofs
BOARD_PRODUCTIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_SYSTEMIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_SYSTEM_DLKMIMAGE_FILE_SYSTEM_TYPE := erofs
BOARD_SYSTEM_EXTIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_VENDOR_DLKMIMAGE_FILE_SYSTEM_TYPE := erofs

TARGET_COPY_OUT_ODM := odm
TARGET_COPY_OUT_ODM_DLKM := odm_dlkm
TARGET_COPY_OUT_PRODUCT := product
TARGET_COPY_OUT_SYSTEM_DLKM := system_dlkm
TARGET_COPY_OUT_SYSTEM_EXT := system_ext
TARGET_COPY_OUT_VENDOR := vendor
TARGET_COPY_OUT_VENDOR_DLKM := vendor_dlkm

BOARD_USES_VENDOR_DLKMIMAGE := true
BOARD_USES_METADATA_PARTITION := true

-include vendor/lineage/config/BoardConfigReservedSize.mk

# Platform
# Stock GRF: ro.board.first_api_level=30 and ro.board.api_level=30 (see vendor.prop).
# KeyMint is trustonic_norkp; raising the vendor API level makes the framework
# require RKP, which the HAL does not provide.
BOARD_SHIPPING_API_LEVEL := 30

# Security patch of the stock vendor blobs; KeyMint reports it as the vendor patch level
VENDOR_SECURITY_PATCH := 2026-06-05

# Filesystem config (vendor AIDs)
TARGET_FS_CONFIG_GEN := $(DEVICE_PATH)/config.fs

# Properties
TARGET_PRODUCT_PROP += $(DEVICE_PATH)/product.prop
TARGET_SYSTEM_EXT_PROP += $(DEVICE_PATH)/system_ext.prop
TARGET_VENDOR_PROP += $(DEVICE_PATH)/vendor.prop

# Recovery
BOARD_INCLUDE_RECOVERY_RAMDISK_IN_VENDOR_BOOT := true
BOARD_MOVE_RECOVERY_RESOURCES_TO_VENDOR_BOOT := true
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/rootdir/etc/fstab.mt6833
TARGET_RECOVERY_PIXEL_FORMAT := RGBX_8888
TARGET_USERIMAGES_USE_F2FS := true

# SEPolicy
include device/mediatek/sepolicy_vndr/SEPolicy.mk
BOARD_VENDOR_SEPOLICY_DIRS += $(DEVICE_PATH)/sepolicy/vendor

# Verified Boot
# Rollback index locations follow stock (vbmeta_system=2, boot=3, vbmeta_vendor=4);
# every stock rollback index is 0.
BOARD_AVB_ENABLE := true
BOARD_AVB_ALGORITHM := SHA256_RSA2048
BOARD_AVB_KEY_PATH := external/avb/test/data/testkey_rsa2048.pem
BOARD_AVB_MAKE_VBMETA_IMAGE_ARGS += --set_hashtree_disabled_flag

BOARD_AVB_BOOT_KEY_PATH := external/avb/test/data/testkey_rsa2048.pem
BOARD_AVB_BOOT_ALGORITHM := SHA256_RSA2048
BOARD_AVB_BOOT_ROLLBACK_INDEX := 0
BOARD_AVB_BOOT_ROLLBACK_INDEX_LOCATION := 3

BOARD_AVB_VBMETA_SYSTEM := product system system_ext
BOARD_AVB_VBMETA_SYSTEM_KEY_PATH := external/avb/test/data/testkey_rsa2048.pem
BOARD_AVB_VBMETA_SYSTEM_ALGORITHM := SHA256_RSA2048
BOARD_AVB_VBMETA_SYSTEM_ROLLBACK_INDEX := 0
BOARD_AVB_VBMETA_SYSTEM_ROLLBACK_INDEX_LOCATION := 2

BOARD_AVB_VBMETA_VENDOR := odm vendor
BOARD_AVB_VBMETA_VENDOR_KEY_PATH := external/avb/test/data/testkey_rsa2048.pem
BOARD_AVB_VBMETA_VENDOR_ALGORITHM := SHA256_RSA2048
BOARD_AVB_VBMETA_VENDOR_ROLLBACK_INDEX := 0
BOARD_AVB_VBMETA_VENDOR_ROLLBACK_INDEX_LOCATION := 4

# VINTF
DEVICE_MANIFEST_FILE := $(DEVICE_PATH)/manifest.xml
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += \
    $(DEVICE_PATH)/framework_compatibility_matrix.xml \
    hardware/mediatek/vintf/mediatek_framework_compatibility_matrix_aidl.xml \
    hardware/mediatek/vintf/mediatek_framework_compatibility_matrix.xml

# Wi-Fi (gen4m via wmt, same interface as mt6768-common)
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_HOSTAPD_DRIVER := NL80211
WIFI_DRIVER_FW_PATH_STA := "STA"
WIFI_DRIVER_FW_PATH_AP := "AP"
WIFI_DRIVER_FW_PATH_P2P := "P2P"
WIFI_HIDL_FEATURE_DUAL_INTERFACE := true
WIFI_HIDL_UNIFIED_SUPPLICANT_SERVICE_RC_ENTRY := true
WIFI_DRIVER_FW_PATH_PARAM := "/dev/wmtWifi"
WIFI_DRIVER_STATE_CTRL_PARAM := "/dev/wmtWifi"
WIFI_DRIVER_STATE_ON := "1"
WIFI_DRIVER_STATE_OFF := "0"

# Inherit the proprietary files
include vendor/sharp/sx4/BoardConfigVendor.mk
