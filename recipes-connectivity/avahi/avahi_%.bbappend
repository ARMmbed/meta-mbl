# Copyright (c) 2018 Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: MIT

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Avahi-disable-ipv4-mDNS.patch"

# By default avahi will install config files to advertise ssh and ssh-sftp
# services, even if there is no SSH service to advertise. Remove these files
# and let packages that actually provide services add service files if
# required.
do_install_append() {
    rm ${D}${sysconfdir}/avahi/services/*
}
