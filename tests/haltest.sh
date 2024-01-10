bg
#!/bin/bash


test-pin-exists() {
    var="lcec.0.$1.$2"
    if ! egrep " +$var$" $OUT > /dev/null; then
	echo "ERROR: $var does not exist"
	halrun -U
	exit 1
    fi
}


test-pin-notexists() {
    var="lcec.0.$1.$2"
    if egrep " +$var$" $OUT > /dev/null; then
	echo "ERROR: $var exists but should not"
	halrun -U
	exit 1
    fi
}


test-pin-true() {
    var="lcec.0.$1.$2"
    if ! egrep "TRUE +$var$" $OUT > /dev/null; then
	echo "ERROR: $var is not true"
	halrun -U
	exit 1
    fi
}

test-pin-false() {
    var="lcec.0.$1.$2"
    if ! egrep "FALSE +$var$" $OUT > /dev/null; then
	echo "ERROR: $var is not true"
	halrun -U
	exit 1
    fi
}

test-pin-greater() {
    var="lcec.0.$1.$2"
    val=$(halcmd getp $var)
    result=$(echo "$val > $3" | bc -l)
    if [ $result == 0 ]; then
	echo "ERROR: $var ($val) is not greater than $3"
	halrun -U
	exit 1
    fi
}

test-pin-less() {
    var="lcec.0.$1.$2"
    val=$(halcmd getp $var)
    result=$(echo "$val < $3" | bc -l)
    if [ $result == 0 ]; then
	echo "ERROR: $var ($val) is not less than $3"
	halrun -U
	exit 1
    fi
}

test-pin-count() {
    var=" lcec\.0\.$1"
    pincount=$(egrep " +$var\." $OUT | wc -l)
    if [ $pincount -ne $2 ]; then
	echo "ERROR: slave $1 has $pincount pins, expected $2"
	halrun -U
	exit 1
    fi
}


test-slave-oper() {
    test-pin-true $1 slave-oper
}


echo "*** Test #1: invalid device type in XML"
echo "=== Killing old halrun"
halrun -U

echo "=== Starting halrun"
halrun -f test1.hal &> /tmp/testbench-init.out

echo "=== Verifying clean failure"
if ! grep "ERROR: Cannot find slave type" /tmp/testbench-init.out > /dev/null; then
    echo "ERROR: expected error not found in output."
    halrun -U; exit 1
fi
echo "*** Test #1 passes"
echo

echo "*** Test #2: Exercise I/O interfaces"
echo "=== Killing old halrun"
halrun -U
sleep 1

echo "=== Starting halrun"
halrun -f test2.hal > /tmp/testbench-init.out &

echo "=== Sleeping for 3s to allow ethercat to finish initializing"
sleep 3 # let ethercat states settle

OUT=/tmp/testbench-1.out
halcmd show > $OUT

echo "=== Testing for failed devices"
if ! grep 'lcec.state-op' $OUT > /dev/null; then
    echo "ERROR: lcec is not loaded correctly."
    halrun -U; exit 1
fi

if ! grep 'lcec.link-up' $OUT > /dev/null; then
    echo "ERROR: lcec Ethernet link is not up."
    halrun -U; exit 1
fi

if grep 'FALSE  lcec.0.D[0-9]+.slave-state-op' $OUT > /dev/null; then
    echo "ERROR: not all slaves in state 'OP'"
    halrun -U; exit 1
fi


# This doesn't really *do* anything, but it's reasonable to verify
# that it's in a sane state.
echo "=== Testing initial config of D0 (EK1100)"
test-pin-true D0 slave-online
test-slave-oper D0


echo "=== Testing initial config of D1 (EL1008)"
test-pin-exists D1 din-7
test-pin-count D1 22


echo "=== Testing initial config of D2 (EL1008)"
test-pin-exists D2 din-7
test-pin-count D2 22


echo "=== Testing initial config of D3 (EL2008)"
test-pin-exists D3 dout-7
test-pin-count D3 22


echo "=== Testing initial config of D4 (EL2008)"
test-pin-exists D4 dout-7
test-pin-count D4 22

echo "=== Testing initial config of D5 (EL2084)"
test-pin-exists D5 dout-3
test-pin-count D5 14


echo "=== Testing initial config of D6 (EL2022)"
test-pin-exists D6 dout-1
test-pin-count D6 10


echo "=== Testing initial config of D7 (EL2022)"
test-pin-exists D7 dout-1
test-pin-count D7 10


echo "=== Testing initial config of D8 (EL2034)"
test-pin-exists D8 dout-3
test-pin-count D8 14


echo "=== Testing initial config of D9 (EL2798)"
test-pin-exists D9 dout-7
test-pin-count D9 22


echo "=== Testing initial config of D10 (EL3068)"
test-pin-exists D10 ain-7-val
test-pin-notexists D10 ain-7-sync-err
test-pin-count D10 62
test-pin-greater D10 ain-0-val 0.1

#echo "=== Testing initial config of D11 (EL6001)"
#if ! grep 'lcec.0.D11.dout-7' $OUT > /dev/null; then
#    echo "ERROR: device D11 (EL6001) does not have pin 'dout-7'"
#    halrun -U; exit 1
#fi
#
#if [ $(fgrep 'lcec.0.D11.' $OUT | wc -l) != 22 ]; then
#    echo "ERROR: device D11 (EL6001) has the wrong number of pins"
#    halrun -U; exit 1
#fi

echo "=== Testing initial config of D12 (EL6900)"
test-pin-exists D12 control
test-pin-count D12 13


echo "=== Testing initial config of D13 (EL1904)"
test-pin-exists D13 fsoe-in-3
test-pin-count D13 20


echo "=== Testing initial config of D14 (EL2904)"
test-pin-exists D14 out-3
test-pin-exists D14 fsoe-out-3
test-pin-count D14 20


echo "=== Testing initial config of D15 (EL3403)"
test-pin-exists D15 l2.cosphi
test-pin-count D15 44


echo "=== Testing initial config of D16 (EL3204)"
test-pin-exists D16 temp-3-temperature
test-pin-count D16 30


echo "=== Testing initial config of D17 (EL1859)"
test-pin-exists D17 din-7
test-pin-exists D17 dout-7
test-pin-count D17 38


echo "=== Testing initial config of D18 (EL4032)"
test-pin-exists D18 aout-1-value
test-pin-exists D18 aout-1-min-dc
test-pin-count D18 28

#echo "=== Testing initial config of D19 (EK1101)"
#if ! grep 'TRUE  lcec.0.D19.slave-online' $OUT > /dev/null; then
#    echo "ERROR: device D19 (EK1101) is not 'slave-online'"
#    halrun -U; exit 1
#fi
#
#if ! grep 'TRUE  lcec.0.D19.slave-oper' $OUT > /dev/null; then
#    echo "ERROR: device D19 (EK1101) is not 'slave-oper'"
#    halrun -U; exit 1
#fi


echo "=== Initial config tests pass"


echo "=== Testing Digital I/O"


echo "=== Verifying initial state"
if grep 'TRUE  lcec.0.D[0-9]+.din-[0-9]+$' $OUT ; then
    echo "ERROR: some digital input pins are already true"
    halrun -U; exit 1
fi


echo "=== Turning on D3.dout-1" 
halcmd setp lcec.0.D3.dout-1 true; sleep 0.1


echo "=== Checking D1.din-1"
OUT=/tmp/testbench-2.out
halcmd show > $OUT
test-pin-true D1 din-1

if [ $(egrep 'TRUE  lcec.0\.D[0-9]+\.din-[0-9]+$' $OUT | wc -l) != 1 ]; then
    echo "ERROR: too many 'true' pins, that should have only flipped one bit."
    halrun -U; exit 1
fi

echo "=== Turning off D3.dout-1" 
halcmd setp lcec.0.D3.dout-1 false; sleep 0.1


echo "=== Verifying"
if grep 'TRUE  lcec.0.D[0-9]+.din-[0-9]+$' $OUT ; then
    echo "ERROR: some digital input pins are already true"
    halrun -U; exit 1
fi


echo "=== Turning on D4.dout-2" 
halcmd setp lcec.0.D4.dout-2 true; sleep 0.1


echo "=== Checking D2.din-3"
OUT=/tmp/testbench-3.out
halcmd show > $OUT
test-pin-true D2 din-3
if [ $(egrep 'TRUE  lcec.0\.D[0-9]+\.din-[0-9]+$' $OUT | wc -l) != 1 ]; then
    echo "ERROR: too many 'true' pins, that should have only flipped one bit."
    halrun -U; exit 1
fi


echo "=== Turning off D4.dout-3" 
halcmd setp lcec.0.D4.dout-3 false; sleep 0.1


echo "=== Verifying"
if grep 'TRUE  lcec.0.D[0-9]+.din-[0-9]+$' $OUT ; then
    echo "ERROR: some digital input pins are already true"
    halrun -U; exit 1
fi


echo "=== Checking D16 temperatures"
test-pin-greater D16 temp-0-temperature 17
test-pin-less D16 temp-0-temperature 25

test-pin-greater D16 temp-1-temperature 950
test-pin-less D16 temp-1-temperature 1050

test-pin-greater D16 temp-2-temperature 17
test-pin-less D16 temp-2-temperature 25

test-pin-greater D16 temp-3-temperature 950
test-pin-less D16 temp-3-temperature 1050



echo "=== ALL TESTS PASS ==="
halrun -U -Q
