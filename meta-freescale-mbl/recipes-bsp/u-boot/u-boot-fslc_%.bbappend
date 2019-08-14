# Copyright (c) 2018 Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: MIT

# MBL_UBOOT_VERSION should be updated to match version pointed to by SRCREV
MBL_UBOOT_VERSION = "2018.11-rc1"

inherit mbl-uboot-sign

SRCREV = "c0c4ee5fce01ec0818c4f27ce029d9b16c8849ad"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI = "git://git.linaro.org/landing-teams/working/mbl/u-boot.git;protocol=https;nobranch=1 "

# To override the boot menu - set the default configuration
SRC_URI_append_imx6ul-pico-mbl = " file://set-pico-imx6ul-config-default-fdt-file-mbl.cfg"

LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

do_compile_append_imx7s-warp-mbl() {
	# Copy device tree to default name for fit image signature verification usage.
	cp ${B}/dts/dt.dtb ${B}/${UBOOT_DTB_BINARY}
	# Generate u-boot-dtb.cfgout for board early initlization.
	oe_runmake u-boot-dtb.imx
}

DCD_FILE_PATH_imx7s-warp-mbl = "${B}"
DCD_FILE_PATH_imx7d-pico-mbl = "${B}"
DCD_FILE_PATH_imx6ul-pico-mbl = "${B}"
DCD_FILE_PATH_imx6ul-des0258-mbl = "${B}"

do_deploy_prepend_imx6ul-pico-mbl() {
	cp ${B}/spl/u-boot-spl.cfgout ${B}/u-boot-dtb.cfgout
}

# Temporary prepend to create u-boot-dtb.cfgout
do_deploy_prepend_imx6ul-des0258-mbl() {
	cp ${B}/mx6ul_14x14_evk_config/spl/u-boot-spl.cfgout ${B}/u-boot-dtb.cfgout
}

do_deploy_append() {
	install -D -p -m 0644 ${DCD_FILE_PATH}/u-boot-dtb.cfgout ${DEPLOYDIR}
}
