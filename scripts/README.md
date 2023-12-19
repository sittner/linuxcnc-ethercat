# Scripts

This directory contains tools used to maintain this package; nothing
in here should be needed to build or use LinuxCNC-Ethercat.

## `devicelist.sh`

This shell script generates a list of all Ethercat PIDs supported by
all drivers included in this package and creates files in
`documentation/drivers/*.yml` that describe (very briefly) the supported devices.

At the moment, it only generates new files and doesn't touch existing
ones, allowing them to be edited manually for added detail.  This may
change in the future.

## `update-devicetable.sh`

This updates `documentation/DEVICES.md`, using the data in
`documentation/drivers/*.yml`, to show a table of all supported
devices.

This uses Go code in `scripts/devicetable/devicetable.go`

## `update-esi.sh`

This updates `scripts/esi.yml` using data from manufacturer websites.
This should provide details on all known Ethercat devices, and can be
used to make `documentation/drivers/*.yml` files more accurate,
although this final step hasn't been done yet.

Currently, we fetch ESI data from Beckhoff and Omron.  Additional
manufacturers can be added fairly easily.

This uses Go code in `scripts/esidecoder/esidecoder.go`.