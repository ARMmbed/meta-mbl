SRCREV = "a4d78f31498261b2e2bb1b7d611cfb3a66c48b54"
DEPENDS_append += " mbl-boot-scr"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI = "git://git.linaro.org/landing-teams/working/mbl/u-boot.git;protocol=https;nobranch=1"

LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

UBOOT_CONFIG[sd] = "warp7_bl33_defconfig,sdcard"

DEPENDS += "flex-native bison-native"

do_deploy_append_imx7s-warp-mbl() {

	install -d ${DEPLOYDIR}
	install -m 0644 ${B}/warp7_bl33_defconfig/u-boot.cfg ${DEPLOYDIR}
	install -m 0644 ${B}/warp7_bl33_defconfig/u-boot.bin ${DEPLOYDIR}
}
