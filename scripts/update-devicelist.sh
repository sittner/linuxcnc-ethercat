#!/bin/sh

set -e

cd devicelist/

DEVICEDIR=../../documentation/devices

go build devicelist.go
./devicelist --esi=../esi.yml --src=../../src --device_directory=$DEVICEDIR

