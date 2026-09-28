#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from device makefile.
$(call inherit-product, device/sharp/sx4/device.mk)

# Inherit some common LineageOS stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

PRODUCT_NAME := lineage_sx4
PRODUCT_DEVICE := sx4
PRODUCT_MANUFACTURER := SHARP
PRODUCT_BRAND := SHARP
PRODUCT_MODEL := AQUOS wish4

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildFingerprint=SHARP/SX4/SX4:15/AP3A.240905.015.A2/00WW_3_20A000:user/release-keys \
    DeviceProduct=SX4
