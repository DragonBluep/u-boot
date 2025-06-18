make \
CROSS_COMPILE=~/openwrt-sdk-19.07.10-ipq40xx-generic_gcc-7.5.0_musl_eabi.Linux-x86_64/staging_dir/toolchain-arm_cortex-a7+neon-vfpv4_gcc-7.5.0_musl_eabi/bin/arm-openwrt-linux- \
STAGING_DIR=~/openwrt-sdk-19.07.10-ipq40xx-generic_gcc-7.5.0_musl_eabi.Linux-x86_64/staging_dir \
KCFLAGS="-Wno-error=implicit-function-declaration -Wno-error=maybe-uninitialized -Wno-error=attributes -Wno-error=misleading-indentation -Wno-error=address" \
-j 4


TARGETCC="$(CC) -lbsd"

-DUSE_SHA256

	
make ipq40xx_standard_defconfig


/home/db/u-boot/board/qca/arm/common/fdt_fixup.c:1162: undefined reference to `fdt_fixup_set_qca_cold_reboot_enable'
/home/db/u-boot/board/qca/arm/common/fdt_fixup.c:1163: undefined reference to `fdt_fixup_for_atf'


cp u-boot u-boot.elf
~/openwrt-sdk-19.07.10-ipq40xx-generic_gcc-7.5.0_musl_eabi.Linux-x86_64/staging_dir/toolchain-arm_cortex-a7+neon-vfpv4_gcc-7.5.0_musl_eabi/bin/arm-openwrt-linux-strip u-boot.elf