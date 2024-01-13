# linuxcnc-ethercat

This is a LinuxCNC EtherCAT HAL driver.

It was forked from
[sittner/linuxcnc-ethercat](https://github.com/sittner/linuxcnc-ethercat),
and it intended to become the new default version of LinuxCNC
EtherCAT.  Please bear with us while we do a bit of cleanup here, and
then we'll start looking at adding additional maintainers and merging
the backlog of drivers that has built up over the past few years.

## Installing

The recommended way to install this driver is to use the `.deb` apt
repository managed by the Etherlab folks.  It should contain
everything that you need to install Ethercat support for LinuxCNC with
minimal manual work.

### Initial setup

First, you need to tell `apt` how to find the Etherlab repository,
hosted at https://build.opensuse.org/projects/science:EtherLab.  This
is the preferred mechanism from the [LinuxCNC
forum](https://forum.linuxcnc.org/ethercat/45336-ethercat-installation-from-repositories-how-to-step-by-step):


```
sudo mkdir -p /usr/local/share/keyrings/
wget -O- https://build.opensuse.org/projects/science:EtherLab/signing_keys/download?kind=gpg | gpg --dearmor | sudo dd of=/etc/apt/trusted.gpg.d/science_EtherLab.gpg
sudo tee -a /etc/apt/sources.list.d/ighvh.sources > /dev/null <<EOT
Types: deb
Signed-By: /etc/apt/trusted.gpg.d/science_EtherLab.gpg
Suites: ./
URIs: http://download.opensuse.org/repositories/science:/EtherLab/Debian_12/
EOT
sudo apt update
sudo apt install -y linux-headers-$(uname -r) ethercat-master linuxcnc-ethercat
```

(These directions are for Debian 12.  Debian 11 should be very similar,
just change `Debian_12` to `Debian_11`.)

You will then need to do a bit of setup for Ethercat; at a minimum
you'll need to edit `/etc/ethercat.conf` to tell it which interface it
should use.  See the forum link, above, for additional details.

You can verify that Ethercat is working when `ethercat slaves` shows
the devices attached to your system.  See the forum link above for
additional helpful steps.

### Updates

Ongoing updates should be easy and *mostly* handled automatically by
`apt`.  Just run `sudo apt update` followed by `sudo apt upgrade` and
things will mostly work, with one possible exception.  If the kernel
is upgraded, then you *may* need to re-run this command in order to
get Ethercat working again:

```
sudo apt install -y linux-headers-$(uname -r
```

This is because the real-time kernel that LinuxCNC prefers doesn't get
its headers intalled by default, and this breaks compiling the
Ethercat modules for the new kernel.  Just run this `apt` command and
then either reboot or run `sudo systemctl start ethercat`.

## Contributing

See [the contributing documentation](CONTRIBUTING.md) for details.  If
you have any issues with the contributing process, *please* file an
issue here.  Everything is new, and it may be broken.


## Devices Supported

See [the device documentation](documentation/DEVICES.md) for a partial
list of Ethercat devices supported by this project.


## Differences That Will Impact You

There are several differences between this version of
LinuxCNC-Ethercat and [the
original](https://github.com/sittner/linuxcnc-ethercat) that will
(eventually) make life easier, but in the short term will make
patching more complicated.

1. In this version, all device-specific code
([`lcec_el1xxx.c`](src/devices/lcec_el1xxx.c), for example) lives in
`src/devices`, while it used to be in `src/`.
2. The mapping between Ethercat VID/PID and device drivers now lives
   in the device source files themselves, *not* in `lcec_main.c` and
   `lcec_conf.c`.  See example below.
3. There is no need to edit `Kbuild` when adding new devices.

In short, to add a new device, you should just be able to drop source
files into `src/devices` and everything should build and work, as long
as you make one minor addition to the source.  Near the top of the
`.c` file for your driver, add a block like this to replace the code
that was in `lcnc_main.c`:

```c
static lcec_typelist_t types[]={
  { "EL1002", LCEC_EL1xxx_VID, LCEC_EL1002_PID, LCEC_EL1002_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1004", LCEC_EL1xxx_VID, LCEC_EL1004_PID, LCEC_EL1004_PDOS, 0, NULL, lcec_el1xxx_init},
  ...
  { NULL },
};

ADD_TYPES(types);
```

This is from `lcec_el1xxx.c`, your names will vary, of course.  The
first field is the string that identifies the device in
`ethercat.xml`, and the other fields match up with the fields that
used to be in `lcec_main.c`.

If your driver needs `<modParam>`s in `ethercat.xml` (like the AX* and
assorted TwinSAFE devices), then you'll need to define the module
parameters in your `.c` file as well, see
[`lcec_el6900.c`](src/devices/lcec_el6900.c) for an example.
