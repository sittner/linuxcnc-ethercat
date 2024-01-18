# Adding New Drivers to LinuxCNC-Ethercat

Before writing a new driver, consider opening a new issue in the
[issue
tracker](http://github.com/linuxcnc-ethercat/linuxcnc-ethercat/issues/new).
Use a subject like "Add support for Fooco AB-15", and then say that
you're intending to write it.  Ideally include a link to the
manufacturer's website, and maybe a very short description of the
hardware.

## Writing Drivers

Drivers are written in C and live in `src/devices/`.  Follow the
naming scheme that already exists.  Each driver needs to live in
`src/devices` and include a `types[]` array that defines the specific
devices supported by this driver:

```C
static lcec_typelist_t types[] = {
    {"EL7041", LCEC_BECKHOFF_VID, 0x1B813052, LCEC_EL7041_PDOS, 0, NULL, lcec_el7041_init},
    {"EL7041-1000", LCEC_BECKHOFF_VID, 0x1B813052, LCEC_EL7041_1000_PDOS, 0, NULL, lcec_el7041_init},
    {"EP7041", LCEC_BECKHOFF_VID, 0x1B813052, LCEC_EP7041_PDOS, 0, NULL, lcec_el7041_init},
    {NULL},
};
ADD_TYPES(types);
```

The `ADD_TYPES(types);` line is mandatory; it does the
behind-the-scenes work to make sure that the driver is linked in and
available.

You shouldn't have to edit any other files; the `Makefile` and all of
the LinuxCNC-Ethercat support code should pick up your new driver
automatically.

When a manufacturer makes several similar devices, try to produce a
single driver that covers them all, or at least can be trivially
extended to handle them in the future.  See the
[`lcec_el3xxx.c`](../src/devices/lcec_el3xxx.c) driver for one example
of how to do this.  If you need to distinguish between different types
of devices within your code, then consider using the `flags` field in
`types[]` (again, see `lcec_el3xxx.c`).  This field is purely for
driver use, so use it however works best for you.

There are two basic styles of drivers in the tree right now; some
explicitly set up EtherCAT PDOs and Sync Managers via
`ec_sync_info_t`, `ec_pdo_info_t`, `ec_pdo_entry_info_t`, and similar
structures.  Others skip all of this and just call `LCEC_PDO_INIT()`
let LinuxCNC-Ethercat handle things behind the scenes.  It seems like
the syncmanager style should perform better, but the non-syncmanager
style is easier to read and work with, and there really isn't much
difference under the hood.

Compare [`lcec_el2xxx.c`](../src/devices/lcec_el2xxx.c) with
[`lcec_el2202.c`](../src/devices/lcec_el2202.c) to see the two styles
side-by-side with similar devices.

Generally, I'd rather see the non-syncmanager version, without the
blocks of `ec_pdo_entry_into_t` structures.

### Style points

- Run `clang-format` on your code.  There's a [default
  format](../.clang-format) specifier in the tree.  Feel free to argue
  about better defaults in a new issue on Github.
- Declare all functions as `static` whenever possible.  Most drivers
  don't need to export anything into the global C namespace for other
  code to link against.

## Contributing Drivers

The best way to contribute a new driver is to sent a Github pull
request with your new driver, along with any information needed to use
the driver.  A few things that would be good to add in the pull request:

- An explanation of which hardware the new driver is for, ideally with
  links to the manufacturer's site.
- Documentation in the `documentation/` directory that explains how to
  use the new driver, including any parameters or configuration
  needed.  Some devices are so trivial that this doesn't matter
  (digital in/out boards, for example), while others are probably
  unusable without documentation.
- An entry (or entries) in `documentation/devices/` that say which
  devices the driver supports.  These can be created automatically by
  `scripts/update-devicelist.sh`, but you'll want to edit the
  resulting file(s).  Please include your Github and/or forum handles
  so people can contact you if they have issues or questions in the
  future.


