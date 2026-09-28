# Device tree for SHARP AQUOS wish4 (sx4)

| Item | Value |
|---|---|
| SoC | MediaTek MT6833 (Dimensity 700) |
| Codename | `sx4` (stock `ro.product.device=SX4`) |
| Stock vendor | `SHARP/SX4/SX4:15/AP3A.240905.015.A2/00WW_3_20C000` |
| Stock system | `AquosMTK:16/SB387/03.00.08` |
| Kernel | GKI 6.6.118 (`android15-8`), boot/vendor_boot/init_boot header v4 |
| Partitions | Virtual A/B, super 8 GiB, vbmeta chain: vbmeta_system(2) / boot(3) / vbmeta_vendor(4) |

## Where each file comes from

| File | Source |
|---|---|
| `BoardConfig.mk` | `motorola/mt6768-common` + `motorola/lamu` (same partition set, same 8 GiB super, 6.6 GKI). Offsets, sizes, AVB locations and fs types measured from the 2026-09-03 readback |
| `device.mk` | `xelex/Q25` (A/B, IMS). Permissions mirror stock `/vendor/etc/permissions` |
| `extract-files.py` | `xelex/Q25` IMS fixups; everything else still to add |
| `proprietary-files.txt` | IMS block (Q25 list, all 29 present on stock) + stage-1 vendor/odm list (2314) from a local generator script; dropped files and reasons in a local exclusion list |
| `proprietary-firmware.txt` | A/B firmware partitions from the scatter file |
| `rootdir/etc/fstab.mt6833` | stock first-stage fstab |
| `manifest.xml` | stock `/vendor/etc/vintf/manifest.xml` (target-level 6) |
| `vendor.prop` | GRF lock (`ro.board.api_level=30`) + stock radio/IMS props |
| `system_ext.prop` | `persist.dbg.*_avail_ovr`, same as Q25 |
| `ims-patches/0001-*` | Regenerated against the stock SX4 `ImsService.apk` with the tree's own apktool (`apktool d -r`, as extract-utils does). Same 9 call sites as bengris32's original patch in `xelex/Q25`; generator: a local script |
| `../sx4-kernels/6.6` | stock slot `_b`: `Image.gz`, `mt6833.dtb`, `dtbo.img`, 419 modules, 4 load lists |

## Stages

- [ ] 1. Extract vendor/odm/*_dlkm blobs, rootdir init scripts, HAL packages; first boot with own vendor
- [ ] 2. Ship firmware (`proprietary-firmware.txt`), pin hashes
- [ ] 3. Kernel modules from source (Sharp GPL `kernel_device_modules-6.6`); gen4m has no Sharp source
- [x] 4a. Regenerate `ims-patches` for the SX4 APK (applies, rebuilds, 0 unsafe flagless receivers left)
- [ ] 4b. Verify IMS on device with 23.2
- [ ] 5. Hardware checklist from the LineageOS device support requirements

## Known constraints

- Never raise `ro.board.api_level` above 30: KeyMint is `trustonic_norkp`.
- Connectivity modules (wlan/wmt/bt/gps/fm) are not in any `modules.load`; vendor rc files insmod them.
- Stock slot `_a` boot/vendor_boot are 6.6.89 and do not match the 6.6.118 modules in super.
- `vendor/lib*/libmnl.so` and `libformatter.so` do not collide on lineage-23.2 (no `external/libmnl`
  in the manifest), but lineage-24.0 has it: when moving to 24.0, rename them the way `xelex/Q25`
  does (`libmnl_mtk.so`, `libformatter_mtk.so` + `replace_needed` in their users).
