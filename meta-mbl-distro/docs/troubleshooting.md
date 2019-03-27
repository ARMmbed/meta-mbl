# Mbed Linux OS (mbl) Troubleshooting

Copyright © 2018 Arm Limited.

## General tips

### Using the test images

Troubleshooting and development can sometimes be much easier when using the
"test" variant of the mbl images rather than the standard images due to the
inclusion of an SSH client and server, allowing files to be easily copied
between the IoT device and a development PC using `scp`. To create test images,
specify `mbl-image-development` rather than `mbl-image-production` when running
Bitbake:
```
bitbake mbl-image-development
```
In particular, this makes collecting log files much easier. See the
[logs][mbl-logs] documentation for more information.


## Wi-Fi

### Ensure Wi-Fi traffic is not being redirected to a login server
When using open Wi-Fi hotspots it is common for all traffic to be redirected to
a login server until the connection has been properly authenticated. When this
is the case, pinging public web servers may appear to work and indicate that the
device is connected to the Internet when, in fact, it isn't. For example:
```
ping google.com
```
may appear to work when in reality the replies are coming from a login server
for the Wi-Fi network. The standard mbl images do not come with sophisticated
network troubleshooting tools, but Wget, which is included, can be useful to
determine whether a connection to the Internet is actually available because
the contents of a file downloaded with Wget file can be examined.


## BitBake

### Cache cleaning
There is a class of build failures when doing incremental builds that do not
appear to be related to the code that has changed since the last build and can
be fixed by clearing BitBake's caches and rebuilding. One typical failure in
this class is missing files during the `do_package_write_ipk` tasks of recipes.
For example:
```
ERROR: docker-18.03.0+git708b068d3095c6a6be939eb2da78c921d2e945e2-r0 do_package_write_ipk: Error executing a python function in exec_python_func() autogenerated:

The stack trace of python calls that resulted in this exception/failure was:
File: 'exec_python_func() autogenerated', lineno: 2, function:
0001:
*** 0002:do_package_ipk(d)
0003:
File: '/home/user01/mbl/mbl-master/layers/meta-mbl/conf/../../../layers/openembedded-core/meta/classes/package_ipk.bbclass', lineno: 87, function: do_package_ipk
0083:
0084: os.chdir(oldcwd)
0085:
0086: if error:
*** 0087: raise error
0088:}
0089:do_package_ipk[vardeps] += "ipk_write_pkg"
0090:do_package_ipk[vardepsexclude] = "BB_NUMBER_THREADS"
0091:
Exception: FileNotFoundError: [Errno 2] No such file or directory: '/home/user01/mbl/mbl-master/build-mbl/tmp-mbl-glibc/work/cortexa7hf-neon-oe-linux-gnueabi/docker/18.03.0+git708b068d3095c6a6be939eb2da78c921d2e945e2-r0/packages-split/docker'

ERROR: docker-18.03.0+git708b068d3095c6a6be939eb2da78c921d2e945e2-r0 do_package_write_ipk: Function failed: do_package_ipk
ERROR: Logfile of failure stored in: /home/user01/mbl/mbl-master/build-mbl/tmp-mbl-glibc/work/cortexa7hf-neon-oe-linux-gnueabi/docker/18.03.0+git708b068d3095c6a6be939eb2da78c921d2e945e2-r0/temp/log.do_package_write_ipk.15559
ERROR: Task (/home/user01/mbl/mbl-master/layers/meta-mbl/conf/../../../layers/meta-virtualization/recipes-containers/docker/docker_git.bb:do_package_write_ipk) failed with exit code '1'
```

To clear BitBake's caches you can run the following commands from the root of
your work area (`/home/user01/mbl/mbl-master` in the example above):
```
rm -rf sstate-cache
rm -rf build-mbl/cache
rm -rf build-mbl/tmp-mbl-glibc
```

### Multiple build directories (don't use them)
It is possible to create multiple build directories (default `build-mbl`) in
the meta-mbl workspace at the same level as the .repo subdirectory by
specifying a directory to the setup-environment script:
```
MACHINE=raspberrypi3-mbl DISTRO=mbl . setup-environment mybuild-dir1
```

The `*.conf` files must then be modified to store the sstate-cache directory in
`mybuild-dir1` so as not to be shared with other builds. However, this
configuration has been found to cause (unspecified) problems.

### Wget `trust_server_names=on` option (don't use it)
Do not to use the Wget `trust_server_names=on` option in a `.wgetrc` file. i.e.:
```
trust_server_names = on
```

This results in Wget using the last component of a redirection URL (e.g.
1.20170405) for the local file name rather than using the requested URL
filename (e.g. `1.20170405.tar.gz`).  This causes problems for Bitbake recipes
that expect the file name requested in the URL (with the .tar.gz extension).


## Update

### Ensure the correct Pelion account is used
A Pelion developer certificate (`mbed_cloud_dev_credentials.c`) [is injected into a build](https://os.mbed.com/docs/linux-os/v0.5/getting-started/building-an-mbl-image.html/) to allow a device running that image to connect to Device Management Portal. The device will be viewable and manageable only using the Pelion account from which the developer certificate was generated.

If the user has multiple Mbed.com accounts, it can happen to build an image with a developer certificate from one account and attempts to manage a device running the image on Device Management with a different Mbed.com account.

If the device logs indicates that a device is connected but it is not visible in Device Management, the user should login using his/her other Mbed.com accounts to see if the device is visible in one of the other accounts.

### Ensure the correct update resources directory is used
An update resources file (`update_default_resources.c`) [is injected into a build](https://os.mbed.com/docs/linux-os/v0.5/getting-started/building-an-mbl-image.html/) to allow a device running that image to be updatable over-the-air (OTA).

The directory from which the update resources file was created needs to be used for starting an OTA. This is necessary for updating the applications or the root file system on a device running that image.
Verify that the correct directory is used by ensuring that the update resources file in the build directory (`builddir/machine-<MACHINE>/mbl-manifest/build-mbl`) is identical to the update resources file found in the directory from which the update command (`manifest-tool update device`) is ran.

### Ensure the the update resources is valid
By default, the update resource is valid for up to 90 days.
Verify the validity of the update resource if unable to update the device.
From the directory where the update resources was created (where `manifest-tool init` was run), run the following command to check the validity:

```
$ openssl x509 -inform DER -in .update-certificates/default.der -text -noout
```
Inspect the values of the attributes named `Not Before` and `Not After`.

## Bootup
### Ethernet gadget driver does not complete
The Warp7 has been seen to hang during boot up when plugged to certain types of USB hubs.
See an example of the bootup log when the startup is halted:
```
...
[   34.316177] random: crng init done
Configuring network interfaces... ip: SIOCGIFFLAGS: No such device
Starting system message bus: dbus.
Starting Connection Manager
Mounting cgroups...Done
Starting rpcbind daemon...done.
Starting bluetooth: bluetoothd.
/etc/rc5.d/S20docker.init: line 43: syntax error: unexpected redirection
Starting tee-supplicant...
Starting syslogd/klogd: done
 * Starting Avahi mDNS/DNS-SD Daemon: avahi-daemon
   ...done.
Starting Telephony daemon
Starting mbl-app-update-manager-daemon...
mbl-app-update-manager-daemon started
Starting mbl-cloud-client...
mbl-cloud-client started
Starting mbl-app-manager...
Looking for installed containers in /home/app...
[   36.943021] using random self ethernet address
[   36.947833] using random host ethernet address
[   36.978068] usb0: HOST MAC 0e:bc:b3:90:ee:d5
[   37.010025] usb0: MAC de:3a:52:94:f2:fe
[   37.014619] using random self ethernet address
[   37.019315] using random host ethernet address
[   37.038809] g_ether gadget: Ethernet Gadget, version: Memorial Day 2008
[   37.045463] g_ether gadget: g_ether ready
[   37.327967] IPv6: ADDRCONF(NETDEV_UP): usb0: link is not ready
```
Use a different USB hub or plug the Warp7 directly to a host machine if the device does not successfully boot up.

[mbl-logs]: logs.md