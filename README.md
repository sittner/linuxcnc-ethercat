# linuxcnc-ethercat

This is a LinuxCNC EtherCAT HAL driver.

It was forked from
[sittner/linuxcnc-ethercat](https://github.com/sittner/linuxcnc-ethercat),
and it intended to become the new default version of LinuxCNC
EtherCAT.  Please bear with us while we do a bit of cleanup here, and
then we'll start looking at adding additional maintainers and merging
the backlog of drivers that has built up over the past few years.

## Installing

TBD.  Include instructions for installing LinuxCNC and Ethercat from
their Debian repositories.

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
