#!/bin/sh

set -e

cd devicetable/

DEVICEDIR=../../documentation/devices
DEVICESMD=../../documentation/DEVICES.md
DEVICESNEW=$DEVICESMD-new

go build devicetable.go
if ./devicetable --path=$DEVICEDIR > $DEVICESNEW; then
    mv $DEVICESNEW $DEVICESMD
else
    rm $DEVICESNEW
fi

