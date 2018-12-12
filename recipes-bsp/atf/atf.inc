# Copyright (c) 2018 Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: MIT

DESCRIPTION = "ARM Trusted Firmware"

DEPENDS += " openssl-native coreutils-native optee-os u-boot virtual/kernel"

SRC_URI_append = " git://github.com/ARMmbed/mbedtls.git;protocol=https;branch=development;name=mbedtls;destsuffix=git/mbedtls "
SRCREV_mbedtls = "1ab9b5714852c6810c0a0bfd8c3b5c60a9a15482"

LICENSE = "BSD-3-Clause & GPLv2 & Apache-2.0"
LICENSE-atf = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=e927e02bca647e14efd87e9e914b2443 \
                    file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6 \
                    file://mbedtls/apache-2.0.txt;md5=3b83ef96387f14655fc854ddc3c6bd57"

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

# LDFLAGS is configured in bitbake.conf as linker flags to be passed to CC.
# It sets it to include "-Wl,-O1". The ATF build system inherits LDFLAGS
# from the environment and passes it directly to LD when building BL1,
# in conflict with the bitbake view. This then causes an error message
# in the ATF trace (aarch64-linux-gnu-ld.bfd: unrecognized option '-Wl,-O1').
# This problem is avoided by clearing LDFLAGS.
LDFLAGS[unexport] = "1"

ATF_COMPILE_FLAGS = "-C ${S} \
      BL32=${DEPLOY_DIR_IMAGE}/optee/tee-header_v2.bin \
      BL32_EXTRA1=${DEPLOY_DIR_IMAGE}/optee/tee-pager_v2.bin \
      BL32_EXTRA2=${DEPLOY_DIR_IMAGE}/optee/tee-pageable_v2.bin \
      BL33=${DEPLOY_DIR_IMAGE}/u-boot.bin \
      BUILD_BASE=${B} \
      BUILD_PLAT=${B}/${PLATFORM} \
      CRASH_REPORTING=1 \
      CRTTOOL=${S}/tools/cert_create/cert_create \
      CRTTOOLPATH=${S}/tools/fiptool \
      GENERATE_COT=1 \
      LOG_LEVEL=40 \
      MBEDTLS_DIR=mbedtls \
      PLAT=${PLATFORM} \
      TRUSTED_BOARD_BOOT=1 \
      USE_TBBR_DEFS=1 \
"

# Default name of fip binary from ATF build.
FIP_BIN = "fip.bin"
MBL_UNIFIED_BIN_PATH ?= ""

do_compile[depends] += " virtual/kernel:do_deploy virtual/bootloader:do_deploy optee-os:do_deploy"
do_compile () {
    # Use existing key if provided to create signed FIP image.
    if [ -e "${MBL_ROT_KEY}" ]; then
	mkdir -p ${B}/${PLATFORM}
	cp ${MBL_ROT_KEY} ${B}/${PLATFORM}/rot_key.pem
    fi

   export PATH=${STAGING_DIR_NATIVE}${bindir}/aarch64-linux-gnu/bin:$PATH

   # We build cert_create here prior because we need to change the OPENSSL_DIR for using
   # the lib from openssl-native. And disable build of the cert_create later when building ATF.
   oe_runmake -C ${S}/tools/cert_create \
       PLAT=${PLATFORM} \
       OPENSSL_DIR="${STAGING_DIR_NATIVE}/usr"

   # Due to LDFLAGS is unexported to solve the build fail, we need to
   # manually add the libdir back. As well as changing the LDLIBS to
   # link to the libraries we want.
   export LD_LIBRARY_PATH=${STAGING_DIR_NATIVE}${libdir}:$LD_LIBRARY_PATH
   oe_runmake -C ${S}/tools/fiptool \
      LDLIBS="-lcrypto -L${STAGING_DIR_NATIVE}${libdir}" \
      INCLUDE_PATHS="-I../../include/tools_share -I${STAGING_DIR_NATIVE}${includedir}"

   oe_runmake ${ATF_COMPILE_FLAGS} all
}

inherit deploy

do_deploy() {
    install -d ${DEPLOYDIR}/${MBL_UNIFIED_BIN_PATH}
    install -D -p -m 0644 ${B}/${PLATFORM}/${MBL_UNIFIED_BIN} ${DEPLOYDIR}/${MBL_UNIFIED_BIN_PATH}/${MBL_UNIFIED_BIN}
    install -D -p -m 0644 ${B}/${PLATFORM}/rot_key.pem ${DEPLOYDIR}
}

addtask deploy before do_image_wic after do_compile