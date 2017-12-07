SUMMARY = "i.MX7 One Time Programmable fuse utility"
DESCRIPTION = "Enable viewing and manipulation of OTP fuse settings."
SECTION = "base"
LICENSE="GPLv2"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRCREV_efuseutils = "be95550b9b122a726206d83000aa416d1b8e9c5e"
SRC_URI = "git://git@github.com/ARMmbed/mbl-tools.git;protocol=ssh;nobranch=1;destsuffix=${S}/git1;name=efuseutils"

inherit image_sign_mbl
DEPENDS += "warp7-keys-native"

COMPATIBLE_MACHINE = "(imx7s-warp)"

do_install(){
    install -d ${D}${base_sbindir}
    install -m 755 git1/warp7-tools/imx7-efuse-util.py ${D}${base_sbindir}
    install -d ${D}${sysconfdir}/cst/warp7/crts
    install -m 755 ${UBOOT_SHARED_DATA}/SRK_1_2_3_4_fuse.bin ${D}${sysconfdir}/cst/warp7/crts
}

RDEPENDS_${PN} = "python"
FILES_${PN}="${base_sbindir}/imx7-efuse-util.py"
FILES_${PN}+="${sysconfdir}/cst/warp7/crts/SRK_1_2_3_4_fuse.bin"