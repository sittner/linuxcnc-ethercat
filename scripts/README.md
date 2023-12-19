# Scripts

This directory contains tools used to maintain this package; nothing
in here should be needed to build or use LinuxCNC-Ethercat.

## `update-esi.sh`

This updates `scripts/esi.yml` using data from manufacturer websites.
This should provide details on all known Ethercat devices.  This data
is used by `update-devicelist.sh`, beloww.

Currently, we fetch ESI data from Beckhoff and Omron.  Additional
manufacturers can be added fairly easily.

This uses Go code in `scripts/esidecoder/esidecoder.go`.

## `update-devicelist.sh`

This shell script generates a list of all Ethercat PIDs supported by
all drivers included in this package and creates files in
`documentation/drivers/*.yml` that describe the supported devices.
This uses ESI data from `update-esi.sh`, above.

This uses Go code in `scripts/devicelist/devicelist.go`.

## `update-devicetable.sh`

This updates `documentation/DEVICES.md`, using the data in
`documentation/drivers/*.yml`, to show a table of all supported
devices.

This uses Go code in `scripts/devicetable/devicetable.go`

