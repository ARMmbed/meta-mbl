DESCRIPTION = "ARM Trusted Firmware Warp7"

DEPENDS += " coreutils-native optee-os u-boot linux-fslc u-boot-mkimage-native atf-fiptool-native"

SRC_URI = "git://git@git.linaro.org/landing-teams/working/mbl/arm-trusted-firmware.git;protocol=https;branch=linaro-warp7;name=atf"
SRCREV = "ac2ad596404e3f81c4a6e6d1a53e8de6375b3972"
SRC_URI +="file://u-boot.cfgout.warp7;name=uboot.cfgout;"
SRCREV_uboot.cfgout="6bb815da1bc986dc717a59cc6d2552f8"

# Notes on uboot.cfgout
# This is a file automatically generated by u-boot when compiling up a warp7
# image. uboot.cfgout is a necessary input when generating a .imx image
# To regenerate uboot.cfgout just do
# "make warp7_config;make u-boot.imx CROSS_COMPILE=your-x-compiler-"

LICENSE = "BSD-3-Clause & GPLv2"
LICENSE-atf = "BSD-3-Clause"
LICENSE-uboot.cfgout = "GPLv2"
LIC_FILES_CHKSUM = "file://license.rst;md5=e927e02bca647e14efd87e9e914b2443 \
                    file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"
COMPATIBLE_MACHINE = "(mx7|vf|use-mainline-bsp)"

PLATFORM_imx7s-warp = "warp7"

LDFLAGS[unexport] = "1"

do_compile() {
   oe_runmake -C ${S} BUILD_BASE=${B} \
      BUILD_PLAT=${B}/${PLATFORM} \
      PLAT=${PLATFORM} \
      ARCH=aarch32 \
      ARM_ARCH_MAJOR=7 \
      ARM_CORTEX_A7=yes \
      CROSS_COMPILE=${TARGET_PREFIX} \
      LOG_LEVEL=50 V=1 \
      AARCH32_SP=optee \
      all

    # Get the entry point
    ENTRY=`${HOST_PREFIX}readelf ${B}/${PLATFORM}/bl2/bl2.elf -h | grep "Entry" | awk '{print $4}'`

    # Generate the .imx binary
    uboot-mkimage -n ${WORKDIR}/u-boot.cfgout.warp7 -T imximage -e ${ENTRY} -d ${B}/${PLATFORM}/bl2.bin ${B}/${PLATFORM}/bl2.bin.imx
}

do_install() {
    # Copy required FIP collateral to a staging point
    cp ${B}/${PLATFORM}/bl2.bin.imx ${DEPLOY_DIR_IMAGE}
}