#!/bin/bash

echo "=== Starting halrun"
halrun -f testbench-init.hal > /tmp/testbench-init.out &

echo "=== Sleeping for 2s to allow ethercat to finish initializing"
sleep 2 # let ethercat states settle

OUT=/tmp/testbench-1.out
halcmd show > $OUT

echo "=== Testing for failed devices"
if ! grep 'lcec.state-op' $OUT > /dev/null; then
    echo "ERROR: lcec is not loaded correctly."
    exit 1
fi

if ! grep 'lcec.link-up' $OUT > /dev/null; then
    echo "ERROR: lcec Ethernet link is not up."
    exit 1
fi

if grep 'FALSE  lcec.0.D[0-9]+.slave-state-op' $OUT > /dev/null; then
    echo "ERROR: not all slaves in state 'OP'"
    exit 1
fi


# This doesn't really *do* anything, but it's reasonable to verify
# that it's in a sane state.
echo "=== Testing initial config of D0 (EK1100)"
if ! grep 'TRUE  lcec.0.D0.slave-online' $OUT > /dev/null; then
    echo "ERROR: device D0 (EK1100) is not 'slave-online'"
    exit 1
fi

if ! grep 'TRUE  lcec.0.D0.slave-oper' $OUT > /dev/null; then
    echo "ERROR: device D0 (EK1100) is not 'slave-oper'"
    exit 1
fi


echo "=== Testing initial config of D1 (EL1008)"
if ! grep 'lcec.0.D1.din-7' $OUT > /dev/null; then
    echo "ERROR: device D1 (EL1008) does not have pin 'din-7'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D1.' $OUT | wc -l) != 22 ]; then
    echo "ERROR: device D1 (EL1008) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D2 (EL1008)"
if ! grep 'lcec.0.D2.din-7' $OUT > /dev/null; then
    echo "ERROR: device D2 (EL1008) does not have pin 'din-7'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D2.' $OUT | wc -l) != 22 ]; then
    echo "ERROR: device D2 (EL1008) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D3 (EL2008)"
if ! grep 'lcec.0.D3.dout-7' $OUT > /dev/null; then
    echo "ERROR: device D3 (EL2008) does not have pin 'dout-7'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D3.' $OUT | wc -l) != 22 ]; then
    echo "ERROR: device D3 (EL2008) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D4 (EL2008)"
if ! grep 'lcec.0.D4.dout-7' $OUT > /dev/null; then
    echo "ERROR: device D4 (EL2008) does not have pin 'dout-7'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D4.' $OUT | wc -l) != 22 ]; then
    echo "ERROR: device D4 (EL2008) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D5 (EL2084)"
if ! grep 'lcec.0.D5.dout-3' $OUT > /dev/null; then
    echo "ERROR: device D5 (EL2084) does not have pin 'dout-3'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D5.' $OUT | wc -l) != 14 ]; then
    echo "ERROR: device D5 (EL2084) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D6 (EL2022)"
if ! grep 'lcec.0.D6.dout-1' $OUT > /dev/null; then
    echo "ERROR: device D6 (EL2022) does not have pin 'dout-1'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D6.' $OUT | wc -l) != 10 ]; then
    echo "ERROR: device D6 (EL2022) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D7 (EL2022)"
if ! grep 'lcec.0.D7.dout-1' $OUT > /dev/null; then
    echo "ERROR: device D7 (EL2022) does not have pin 'dout-1'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D7.' $OUT | wc -l) != 10 ]; then
    echo "ERROR: device D7 (EL2022) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D8 (EL2034)"
if ! grep 'lcec.0.D8.dout-3' $OUT > /dev/null; then
    echo "ERROR: device D8 (EL2034) does not have pin 'dout-3'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D8.' $OUT | wc -l) != 14 ]; then
    echo "ERROR: device D8 (EL2034) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D9 (EL2798)"
if ! grep 'lcec.0.D9.dout-7' $OUT > /dev/null; then
    echo "ERROR: device D9 (EL2798) does not have pin 'dout-7'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D9.' $OUT | wc -l) != 22 ]; then
    echo "ERROR: device D9 (EL2798) has too many pins"
    exit 1
fi


#echo "=== Testing initial config of D10 (EL3068)"
#if ! grep 'lcec.0.D10.dout-7' $OUT > /dev/null; then
#    echo "ERROR: device D10 (EL3068) does not have pin 'dout-7'"
#    exit 1
#fi
#
#if [ $(fgrep 'lcec.0.D10.' $OUT | wc -l) != 22 ]; then
#    echo "ERROR: device D10 (EL3068) has too many pins"
#    exit 1
#fi


#echo "=== Testing initial config of D11 (EL6001)"
#if ! grep 'lcec.0.D11.dout-7' $OUT > /dev/null; then
#    echo "ERROR: device D11 (EL6001) does not have pin 'dout-7'"
#    exit 1
#fi
#
#if [ $(fgrep 'lcec.0.D11.' $OUT | wc -l) != 22 ]; then
#    echo "ERROR: device D11 (EL6001) has too many pins"
#    exit 1
#fi


echo "=== Testing initial config of D12 (EL6900)"
if ! grep 'lcec.0.D12.control' $OUT > /dev/null; then
    echo "ERROR: device D12 (EL6900) does not have pin 'control'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D12.' $OUT | wc -l) != 11 ]; then
    echo "ERROR: device D12 (EL6900) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D13 (EL1904)"
if ! grep 'lcec.0.D13.fsoe-in-3' $OUT > /dev/null; then
    echo "ERROR: device D13 (EL1904) does not have pin 'fsoe-in-3'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D13.' $OUT | wc -l) != 20 ]; then
    echo "ERROR: device D13 (EL1904) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D14 (EL2904)"
if ! grep 'lcec.0.D14.out-3' $OUT > /dev/null; then
    echo "ERROR: device D14 (EL2904) does not have pin 'out-3'"
    exit 1
fi

if ! grep 'lcec.0.D14.fsoe-out-3' $OUT > /dev/null; then
    echo "ERROR: device D14 (EL2904) does not have pin 'fsoe-out-3'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D14.' $OUT | wc -l) != 20 ]; then
    echo "ERROR: device D14 (EL2904) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D15 (EL3403)"
if ! grep 'lcec.0.D15.l2.cosphi' $OUT > /dev/null; then
    echo "ERROR: device D15 (EL3403) does not have pin 'l2.cosphi'"
    exit 1
fi

if [ $(fgrep 'lcec.0.D15.' $OUT | wc -l) != 44 ]; then
    echo "ERROR: device D15 (EL3403) has too many pins"
    exit 1
fi


echo "=== Testing initial config of D16 (EK1101)"
if ! grep 'TRUE  lcec.0.D16.slave-online' $OUT > /dev/null; then
    echo "ERROR: device D16 (EK1101) is not 'slave-online'"
    exit 1
fi

if ! grep 'TRUE  lcec.0.D16.slave-oper' $OUT > /dev/null; then
    echo "ERROR: device D16 (EK1101) is not 'slave-oper'"
    exit 1
fi


echo "=== Initial config tests pass"


echo "=== Testing Digital I/O"


echo "=== Verifying initial state"
if grep 'TRUE  lcec.0.D[0-9]+.din-[0-9]+$' $OUT ; then
    echo "ERROR: some digital input pins are already true"
    exit 1
fi


echo "=== Turning on D3.dout-1" 
halcmd setp lcec.0.D3.dout-1 true; sleep 0.1

echo "=== Checking D1.din-1"
OUT=/tmp/testbench-2.out
halcmd show > $OUT
if ! grep 'TRUE  lcec.0.D1.din-1$' $OUT > /dev/null; then
    echo "ERROR: D1.din-1 is not true.  Check wiring?"
    exit 1
fi
if [ $(egrep 'TRUE  lcec.0\.D[0-9]+\.din-[0-9]+$' $OUT | wc -l) != 1 ]; then
    echo "ERROR: too many 'true' pins, that should have only flipped one bit."
    exit 1
fi

echo "=== Turning off D3.dout-1" 
halcmd setp lcec.0.D3.dout-1 false; sleep 0.1


echo "=== Verifying"
if grep 'TRUE  lcec.0.D[0-9]+.din-[0-9]+$' $OUT ; then
    echo "ERROR: some digital input pins are already true"
    exit 1
fi


echo "=== Turning on D4.dout-2" 
halcmd setp lcec.0.D4.dout-2 true; sleep 0.1

echo "=== Checking D2.din-3"
OUT=/tmp/testbench-3.out
halcmd show > $OUT
if ! grep 'TRUE  lcec.0.D2.din-3$' $OUT > /dev/null; then
    echo "ERROR: D2.din-3 is not true.  Check wiring?"
    exit 1
fi
if [ $(egrep 'TRUE  lcec.0\.D[0-9]+\.din-[0-9]+$' $OUT | wc -l) != 1 ]; then
    echo "ERROR: too many 'true' pins, that should have only flipped one bit."
    exit 1
fi


echo "=== Turning off D4.dout-3" 
halcmd setp lcec.0.D4.dout-3 false; sleep 0.1

echo "=== Verifying"
if grep 'TRUE  lcec.0.D[0-9]+.din-[0-9]+$' $OUT ; then
    echo "ERROR: some digital input pins are already true"
    exit 1
fi



echo "=== ALL TESTS PASS ==="
halrun -U -Q
