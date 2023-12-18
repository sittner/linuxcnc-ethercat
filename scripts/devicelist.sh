#!/bin/bash

# Create list of supported devices by decoding the `types` list in lcec_main.c
#
# This returns a CSV of (name, VID, PID).
#
# VID should match https://www.ethercat.org/en/vendor_id_list.html
#
# Not sure where to find a list of PID values yet.

CFLAGS="$(grep EXTRA_CFLAGS config.mk | cut -d= -f2-)"

gcc -E  $CFLAGS src/lcec_main.c \
    | sed -ne '/static const lcec_typelist_t types/,$ p' \
    | sed '/};/q' \
    | egrep -v '^#' \
    | grep '  {' \
    | cut -d'{' -f2 \
    | cut -d, -f1-3 \
    | grep -v 'lcecSlaveTypeInvalid' \
    | cut -c2- \
    | sed 's/lcecSlaveType//' \
    | tr -d ' ' \
    | while read line; do 

    DEVICE=$(echo $line | cut -d, -f1)
    VID=$(echo $line | cut -d, -f2)
    PID=$(echo $line | cut -d, -f3)

    file=documentation/devices/$DEVICE.yml

    if [ ! -f $file ]; then
	(
	    echo "---"
	    echo "Device: $DEVICE"
	    echo "VendorID: $VID"
	    echo "VendorName: TODO"
	    echo "PID: $PID"
	    echo "Description: TODO"
	    echo "DocumentationURL: TODO"
	    echo "DeviceType: TODO"
	) > $file
    fi
																	      
done

