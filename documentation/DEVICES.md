# Devices Supported by LinuxCNC-Ethercat

*This is a work in progress, listing all of the devices that LinuxCNC-Ethercat
has code to support today.  Not all of these are well-tested.*

Description | Source | EtherCAT VID:PID | Device Type | Testing Status | Notes
----------- | ------ | ---------------- | ----------- | -------------- | ------
[Beckhoff EL1018 8Ch. Dig. Input 24V, 10µs](http://www.beckhoff.com/EL1018) | [EL1018](../src/lcec_el1xxx.c) | 0x00000002:0x03fa3052 | Digital Input | Part of @scottlaird's test suite. | 
[Beckhoff EL3064 4Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3064) | [EL3064](../src/lcec_el30x4.c) | 0x00000002:0x0bf83052 | Analog Input Terminals | Uncertain; @scottlaird has one | 
[Beckhoff EL7041 1Ch. Stepper motor output stage (50V, 5A)](http://www.beckhoff.com/EL7041) | [EP7041](../src/lcec_el7041.c) | 0x00000002:0x1b813052 | Stepper Drive | Uncertain; @scottlaird has several EP7041-0002 | 
[Beckhoff AX5101-0000-0214 EtherCAT Drive (SoE, 1 Ch.)](http://www.beckhoff.com/ax51xx/) | [AX5101](../src/lcec_ax5100.c) | 0x00000002:0x13ed6012 | Servo Drive |  | 
[Beckhoff AX5103-0000-0214 EtherCAT Drive (SoE, 1 Ch.)](http://www.beckhoff.com/ax51xx/) | [AX5103](../src/lcec_ax5100.c) | 0x00000002:0x13ef6012 | Servo Drive |  | 
[Beckhoff AX5106-0000-0214 EtherCAT Drive (SoE, 1 Ch.)](http://www.beckhoff.com/ax51xx/) | [AX5106](../src/lcec_ax5100.c) | 0x00000002:0x13f26012 | Servo Drive |  | 
[Beckhoff AX5112-0000-0214 EtherCAT Drive (SoE, 1 Ch.)](http://www.beckhoff.com/ax51xx/) | [AX5112](../src/lcec_ax5100.c) | 0x00000002:0x13f86012 | Servo Drive |  | 
[Beckhoff AX5118-0000-0214 EtherCAT Drive (SoE, 1 Ch.)](http://www.beckhoff.com/ax51xx/) | [AX5118](../src/lcec_ax5100.c) | 0x00000002:0x13fe6012 | Servo Drive |  | 
[Beckhoff AX5203-0000-0214 EtherCAT Drive (SoE, 2 Ch.)](http://www.beckhoff.com/ax52xx/) | [AX5203](../src/lcec_ax5200.c) | 0x00000002:0x14536012 | Servo Drive |  | 
[Beckhoff AX5206-0000-0214 EtherCAT Drive (SoE, 2 Ch.)](http://www.beckhoff.com/ax52xx/) | [AX5206](../src/lcec_ax5200.c) | 0x00000002:0x14566012 | Servo Drive |  | 
[Beckhoff AX5805 (Safety Drive Option)](http://beckhoff.com/AX5805) | [AX5805](../src/lcec_ax5805.c) | 0x00000002:0x16ad6012 | Safety Terminals |  | 
[Delta ASDA-A2-E](https://www.deltaww.com/en-us/products/Servo-Systems-AC-Servo-Motors-and-Drives/23) | [DEASDA](../src/lcec_deasda.c) | 0x000001dd:0x10305070 | Servo Drive |  | 
[Delta MS-300 AC Motor Drive](https://www.deltaww.com/en-us/products/AC-Motor-Drives/3449) | [DEMS300](../src/lcec_dems300.c) | 0x000001dd:0x10400200 | AC Motor Drive |  | 
[Beckhoff EK1100 EtherCAT Coupler (0.5A E-Bus)](http://www.beckhoff.com/EK1100) | [EK1100](../src/lcec_ek1100.c) | 0x00000002:0x044c2c52 | System Couplers | Part of @scottlaird's test suite. | 
[Beckhoff EK1101 EtherCAT Coupler (2A E-Bus, ID switch)](http://www.beckhoff.com/EK1101) | [EK1101](../src/lcec_ek1100.c) | 0x00000002:0x044d2c52 | System Couplers |  | 
[Beckhoff EK1110 EtherCAT extension](http://www.beckhoff.com/EK1110) | [EK1110](../src/lcec_ek1100.c) | 0x00000002:0x04562c52 | System Terminals | Part of @scottlaird's test suite | 
[Beckhoff EK1122 2 port EtherCAT junction](http://www.beckhoff.com/EK1122) | [EK1122](../src/lcec_ek1100.c) | 0x00000002:0x04622c52 | System Terminals |  | 
[Beckhoff EL1002 2Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1002) | [EL1002](../src/lcec_el1xxx.c) | 0x00000002:0x03ea3052 | Digital Input Terminals |  | 
[Beckhoff EL1004 4Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1004) | [EL1004](../src/lcec_el1xxx.c) | 0x00000002:0x03ec3052 | Digital Input Terminals |  | 
[Beckhoff EL1008 8Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1008) | [EL1008](../src/lcec_el1xxx.c) | 0x00000002:0x03f03052 | Digital Input Terminals |  | 
[Beckhoff EL1012 2Ch. Dig. Input 24V, 10µs](http://www.beckhoff.com/EL1012) | [EL1012](../src/lcec_el1xxx.c) | 0x00000002:0x03f43052 | Digital Input |  | 
[Beckhoff EL1014 4Ch. Dig. Input 24V, 10µs](http://www.beckhoff.com/EL1014) | [EL1014](../src/lcec_el1xxx.c) | 0x00000002:0x03f63052 | Digital Input |  | 
[Beckhoff EL1018 8Ch. Dig. Input 24V, 10µs](http://www.beckhoff.com/EL1018) | [EL1018](../src/lcec_el1xxx.c) | 0x00000002:0x03fa3052 | Digital Input | Part of @scottlaird's test suite. | 
[Beckhoff EL1024 4Ch. Dig. Input 24V, Type 2, 3ms](http://www.beckhoff.com/EL1024) | [EL1024](../src/lcec_el1xxx.c) | 0x00000002:0x04003052 | Digital Input |  | 
[Beckhoff EL1034 4Ch. Dig. Input 24V, potential-free, 10µs](http://www.beckhoff.com/EL1034) | [EL1034](../src/lcec_el1xxx.c) | 0x00000002:0x040a3052 | Digital Input |  | 
[Beckhoff EL1084 4Ch. Dig. Input 24V, 3ms, negative](http://www.beckhoff.com/EL1084) | [EL1084](../src/lcec_el1xxx.c) | 0x00000002:0x043c3052 | Digital Input |  | 
[Beckhoff EL1088 8Ch. Dig. Input 24V, 3ms, negative](http://www.beckhoff.com/EL1088) | [EL1088](../src/lcec_el1xxx.c) | 0x00000002:0x04403052 | Digital Input |  | 
[Beckhoff EL1094 4Ch. Dig. Input 24V, 10µs, negative](http://www.beckhoff.com/EL1094) | [EL1094](../src/lcec_el1xxx.c) | 0x00000002:0x04463052 | Digital Input |  | 
[Beckhoff EL1098 8Ch. Dig. Input 24V, 10µs, negative](http://www.beckhoff.com/EL1098) | [EL1098](../src/lcec_el1xxx.c) | 0x00000002:0x044a3052 | Digital Input |  | 
[Beckhoff EL1104 4Ch. Dig. Input 24V, 3ms, Sensor Power](http://www.beckhoff.com/EL1104) | [EL1104](../src/lcec_el1xxx.c) | 0x00000002:0x04503052 | Digital Input |  | 
[Beckhoff EL1114 4Ch. Dig. Input 24V, 10µs, Sensor Power](http://www.beckhoff.com/EL1114) | [EL1114](../src/lcec_el1xxx.c) | 0x00000002:0x045a3052 | Digital Input |  | 
[Beckhoff EL1124 4Ch. Dig. Input 5V, 10µs, Sensor Power](http://www.beckhoff.com/EL1124) | [EL1124](../src/lcec_el1xxx.c) | 0x00000002:0x04643052 | Digital Input |  | 
[Beckhoff EL1134 4Ch. Dig. Input 48V, 10µs, Sensor Power](http://www.beckhoff.com/EL1134) | [EL1134](../src/lcec_el1xxx.c) | 0x00000002:0x046e3052 | Digital Input |  | 
[Beckhoff EL1144 4Ch. Dig. Input 12V, 10µs, Sensor Power](http://www.beckhoff.com/EL1144) | [EL1144](../src/lcec_el1xxx.c) | 0x00000002:0x04783052 | Digital Input |  | 
[Beckhoff EL1252 2Ch. Fast Dig. Input 24V, 1µs, DC Latch](http://www.beckhoff.com/EL1252) | [EL1252](../src/lcec_el1252.c) | 0x00000002:0x04e43052 | Digital Input |  | 
[Beckhoff EL1804 4Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1804) | [EL1804](../src/lcec_el1xxx.c) | 0x00000002:0x070c3052 | Digital Input |  | 
[Beckhoff EL1808 8Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1808) | [EL1808](../src/lcec_el1xxx.c) | 0x00000002:0x07103052 | Digital Input | Uncertain; @scottlaird has one | 
[Beckhoff EL1809 16Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1809) | [EL1809](../src/lcec_el1xxx.c) | 0x00000002:0x07113052 | Digital Input |  | 
[Beckhoff EL1819 16Ch. Dig. Input 24V, 10µs](http://www.beckhoff.com/EL1819) | [EL1819](../src/lcec_el1xxx.c) | 0x00000002:0x071b3052 | Digital Input |  | 
[Beckhoff EL1859 8Ch. Dig. Input 24V, 3ms, 8Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL1859) | [EL1859](../src/lcec_el1859.c) | 0x00000002:0x07433052 | Digital Input |  | 
[Beckhoff EL1904, 4 Ch. Safety Input 24V, TwinSAFE](http://www.beckhoff.com/EL1904) | [EL1904](../src/lcec_el1904.c) | 0x00000002:0x07703052 | Safety Terminals | Part of @scottlaird's test suite, but not currently being evaluated. | 
[Beckhoff EL1918, 8Ch. Safety Input 24V, TwinSAFE](http://www.beckhoff.com/EL1918) | [EL1918_LOGIC](../src/lcec_el1918_logic.c) | 0x00000002:0x077e3052 | Safety Terminals |  | 
[Beckhoff EL2002 2Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL2002) | [EL2002](../src/lcec_el2xxx.c) | 0x00000002:0x07d23052 | Digital Output |  | 
[Beckhoff EL2004 4Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL2004) | [EL2004](../src/lcec_el2xxx.c) | 0x00000002:0x07d43052 | Digital Output |  | 
[Beckhoff EL2008 8Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL2008) | [EL2008](../src/lcec_el2xxx.c) | 0x00000002:0x07d83052 | Digital Output | Part of @scottlaird's test suite, hardware is actively exercised for most releases. | 
[Beckhoff EL2022 2Ch. Dig. Output 24V, 2A](http://www.beckhoff.com/EL2022) | [EL2022](../src/lcec_el2xxx.c) | 0x00000002:0x07e63052 | Digital Output | Part of @scottlaird's test suite. | 
[Beckhoff EL2024 4Ch. Dig. Output 24V, 2A](http://www.beckhoff.com/EL2024) | [EL2024](../src/lcec_el2xxx.c) | 0x00000002:0x07e83052 | Digital Output |  | 
[Beckhoff EL2032 2Ch. Dig. Output 24V, 2A Diag](http://www.beckhoff.com/EL2032) | [EL2032](../src/lcec_el2xxx.c) | 0x00000002:0x07f03052 | Digital Output |  | 
[Beckhoff EL2034 4Ch. Dig. Output 24V, 2A, Diagnostic](http://www.beckhoff.com/EL2034) | [EL2034](../src/lcec_el2xxx.c) | 0x00000002:0x07f23052 | Digital Output | Part of @scottlaird's test suite. | 
[Beckhoff EL2042 2Ch. Dig. Output 24V, 4A](http://www.beckhoff.com/EL2042) | [EL2042](../src/lcec_el2xxx.c) | 0x00000002:0x07fa3052 | Digital Output |  | 
[Beckhoff EL2084 4Ch. Dig. Output 24V, 0.5A, switching to negative](http://www.beckhoff.com/EL2084) | [EL2084](../src/lcec_el2xxx.c) | 0x00000002:0x08243052 | Digital Output | Part of @scottlaird's test suite. | 
[Beckhoff EL2088 8Ch. Dig. Output 24V, 0.5A, switching to negative](http://www.beckhoff.com/EL2088) | [EL2088](../src/lcec_el2xxx.c) | 0x00000002:0x08283052 | Digital Output |  | 
[Beckhoff EL2124 4Ch. Dig. Output 5V, 20mA](http://www.beckhoff.com/EL2124) | [EL2124](../src/lcec_el2xxx.c) | 0x00000002:0x084c3052 | Digital Output |  | 
[Beckhoff EL2202 2Ch. Dig. Output 24V, 0.5A, DC Sync](http://www.beckhoff.com/EL2202) | [EL2202](../src/lcec_el2202.c) | 0x00000002:0x089a3052 | Digital Output |  | 
[Beckhoff EL2521 1Ch. Pulse Train Output](http://www.beckhoff.com/EL2521) | [EL2521](../src/lcec_el2521.c) | 0x00000002:0x09d93052 | Digital Output |  | 
[Beckhoff EL2612 2Ch. Relay Output, CO (125V AC / 30V DC)](http://www.beckhoff.com/EL2612) | [EL2612](../src/lcec_el2xxx.c) | 0x00000002:0x0a343052 | Digital Output |  | 
[Beckhoff EL2622 2Ch. Relay Output, NO (230V AC / 30V DC)](http://www.beckhoff.com/EL2622) | [EL2622](../src/lcec_el2xxx.c) | 0x00000002:0x0a3e3052 | Digital Output |  | 
[Beckhoff EL2634 4Ch. Relay Output, NO (250V AC / 30V DC)](http://www.beckhoff.com/EL2634) | [EL2634](../src/lcec_el2xxx.c) | 0x00000002:0x0a4a3052 | Digital Output |  | 
[Beckhoff EL2652 2Ch. Relay Output, CO (230V AC / 30V DC)](http://www.beckhoff.com/EL2652) | [EL2652](../src/lcec_el2xxx.c) | 0x00000002:0x0a5c3052 | Digital Output |  | 
[Beckhoff EL2798 8Ch. Dig. Output 24V AC/DC, 2A, potential-free](http://www.beckhoff.com/EL2798) | [EL2798](../src/lcec_el2xxx.c) | 0x00000002:0x0aee3052 | Digital Output | Part of @scottlaird's test suite. | 
[Beckhoff EL2808 8Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL2808) | [EL2808](../src/lcec_el2xxx.c) | 0x00000002:0x0af83052 | Digital Output | Uncertain; @scottlaird has several | 
[Beckhoff EL2809 16Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL2809) | [EL2809](../src/lcec_el2xxx.c) | 0x00000002:0x0af93052 | Digital Output |  | 
[Beckhoff EL2904, 4 Ch. Safety Output 24V, 0.5A, TwinSAFE](http://www.beckhoff.com/EL2904) | [EL2904](../src/lcec_el2904.c) | 0x00000002:0x0b583052 | Safety Terminals | Part of @scottlaird's test suite, but not currently being evaluated. | 
[Beckhoff EL3004 4Ch. Ana. Input +/-10V](http://www.beckhoff.com/EL3004) | [EL3004](../src/lcec_el30x4.c) | 0x00000002:0x0bbc3052 | Analog Input |  | 
[Beckhoff EL3044 4Ch. Ana. Input 0-20mA](http://www.beckhoff.com/EL3044) | [EL3044](../src/lcec_el30x4.c) | 0x00000002:0x0be43052 | Analog Input |  | 
[Beckhoff EL3054 4Ch. Ana. Input 4-20mA](http://www.beckhoff.com/EL3054) | [EL3054](../src/lcec_el30x4.c) | 0x00000002:0x0bee3052 | Analog Input |  | 
[Beckhoff EL3064 4Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3064) | [EL3064](../src/lcec_el30x4.c) | 0x00000002:0x0bf83052 | Analog Input |  | 
[Beckhoff EL3102 2Ch. Ana. Input +/-10V, Diff.](http://www.beckhoff.com/EL3102) | [EL3102](../src/lcec_el31x2.c) | 0x00000002:0x0c1e3052 | Analog Input |  | 
[Beckhoff EL3112 2Ch. Ana. Input 0-20mA, Diff.](http://www.beckhoff.com/EL3112) | [EL3112](../src/lcec_el31x2.c) | 0x00000002:0x0c283052 | Analog Input |  | 
[Beckhoff EL3122 2Ch. Ana. Input 4-20mA, Diff.](http://www.beckhoff.com/EL3122) | [EL3122](../src/lcec_el31x2.c) | 0x00000002:0x0c323052 | Analog Input |  | 
[Beckhoff EL3142 2Ch. Ana. Input 0-20mA](http://www.beckhoff.com/EL3142) | [EL3142](../src/lcec_el31x2.c) | 0x00000002:0x0c463052 | Analog Input |  | 
[Beckhoff EL3152 2Ch. Ana. Input 4-20mA](http://www.beckhoff.com/EL3152) | [EL3152](../src/lcec_el31x2.c) | 0x00000002:0x0c503052 | Analog Input |  | 
[Beckhoff EL3162 2Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3162) | [EL3162](../src/lcec_el31x2.c) | 0x00000002:0x0c5a3052 | Analog Input | Uncertain; @scottlaird has one | 
[Beckhoff EL3164 4Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3164) | [EL3164](../src/lcec_el31x4.c) | 0x00000002:0x0c5c3052 | Analog Input |  | 
[Beckhoff EL3202 2Ch. Ana. Input PT100 (RTD)](http://www.beckhoff.com/EL3202) | [EL3202](../src/lcec_el3202.c) | 0x00000002:0x0c823052 | Analog Input |  | 
[Beckhoff EL3255 5Ch. potentiometer measurement with sensor supply](http://www.beckhoff.com/EL3255) | [EL3255](../src/lcec_el3255.c) | 0x00000002:0x0cb73052 | Analog Input |  | 
[Beckhoff EL3403 3Ch. Power Measuring](http://www.beckhoff.com/EL3403) | [EL3403](../src/lcec_el3403.c) | 0x00000002:0x0d4b3052 | Analog Input | Uncertain; @scottlaird has several | 3-phase AC power measurement
[Beckhoff EL4001 1Ch. Ana. Output 0-10V, 12bit](http://www.beckhoff.com/EL4001) | [EL4001](../src/lcec_el40x1.c) | 0x00000002:0x0fa13052 | Analog Output |  | 
[Beckhoff EL4002 2Ch. Ana. Output 0-10V, 12bit](http://www.beckhoff.com/EL4002) | [EL4002](../src/lcec_el40x2.c) | 0x00000002:0x0fa23052 | Analog Output |  | 
[Beckhoff EL4008 8Ch. Ana. Output 0-10V, 12bit](http://www.beckhoff.com/EL4008) | [EL4008](../src/lcec_el40x8.c) | 0x00000002:0x0fa83052 | Analog Output |  | 
[Beckhoff EL4011 1Ch. Ana. Output 0-20mA, 12bit](http://www.beckhoff.com/EL4011) | [EL4011](../src/lcec_el40x1.c) | 0x00000002:0x0fab3052 | Analog Output |  | 
[Beckhoff EL4012 2Ch. Ana. Output 0-20mA, 12bit](http://www.beckhoff.com/EL4012) | [EL4012](../src/lcec_el40x2.c) | 0x00000002:0x0fac3052 | Analog Output |  | 
[Beckhoff EL4018 8Ch. Ana. Output 0-20mA, 12bit](http://www.beckhoff.com/EL4018) | [EL4018](../src/lcec_el40x8.c) | 0x00000002:0x0fb23052 | Analog Output |  | 
[Beckhoff EL4021 1Ch. Ana. Output 4-20mA, 12bit](http://www.beckhoff.com/EL4021) | [EL4021](../src/lcec_el40x1.c) | 0x00000002:0x0fb53052 | Analog Output |  | 
[Beckhoff EL4022 2Ch. Ana. Output 4-20mA, 12bit](http://www.beckhoff.com/EL4022) | [EL4022](../src/lcec_el40x2.c) | 0x00000002:0x0fb63052 | Analog Output |  | 
[Beckhoff EL4028 8Ch. Ana. Output 4-20mA, 12bit](http://www.beckhoff.com/EL4028) | [EL4028](../src/lcec_el40x8.c) | 0x00000002:0x0fbc3052 | Analog Output |  | 
[Beckhoff EL4031 1Ch. Ana. Output -10/+10V, 12bit](http://www.beckhoff.com/EL4031) | [EL4031](../src/lcec_el40x1.c) | 0x00000002:0x0fbf3052 | Analog Output |  | 
[Beckhoff EL4032 2Ch. Ana. Output -10/+10V, 12bit](http://www.beckhoff.com/EL4032) | [EL4032](../src/lcec_el40x2.c) | 0x00000002:0x0fc03052 | Analog Output |  | 
[Beckhoff EL4038 8Ch. Ana. Output -10/+10V, 12bit](http://www.beckhoff.com/EL4038) | [EL4038](../src/lcec_el40x8.c) | 0x00000002:0x0fc63052 | Analog Output |  | 
[Beckhoff EL4102 2Ch. Ana. Output 0-10V](http://www.beckhoff.com/EL4102) | [EL4102](../src/lcec_el41x2.c) | 0x00000002:0x10063052 | Analog Output |  | 
[Beckhoff EL4104 4Ch. Ana. Output 0-10V, 16bit](http://www.beckhoff.com/EL4104) | [EL4104](../src/lcec_el41x4.c) | 0x00000002:0x10083052 | Analog Output |  | 
[Beckhoff EL4112 2Ch. Ana. Output 0-20mA](http://www.beckhoff.com/EL4112) | [EL4112](../src/lcec_el41x2.c) | 0x00000002:0x10103052 | Analog Output |  | 
[Beckhoff EL4122 2Ch. Ana. Output 4-20mA](http://www.beckhoff.com/EL4122) | [EL4122](../src/lcec_el41x2.c) | 0x00000002:0x101a3052 | Analog Output |  | 
[Beckhoff EL4132 2Ch. Ana. Output +/-10V](http://www.beckhoff.com/EL4132) | [EL4132](../src/lcec_el41x2.c) | 0x00000002:0x10243052 | Analog Output |  | 
[Beckhoff EL4134 4Ch. Ana. Output -10/+10V, 16bit](http://www.beckhoff.com/EL4134) | [EL4134](../src/lcec_el41x4.c) | 0x00000002:0x10263052 | Analog Output |  | 
[Beckhoff EL5002 2Ch. SSI Encoder](http://www.beckhoff.com/EL5002) | [EL5002](../src/lcec_el5002.c) | 0x00000002:0x138a3052 | Measuring Input |  | 
[Beckhoff EL5032 2Ch. EnDat Encoder](http://www.beckhoff.com/EL5032) | [EL5032](../src/lcec_el5032.c) | 0x00000002:0x13a83052 | Measuring Input |  | 
[Beckhoff EL5101 1Ch. Encoder 5V](http://www.beckhoff.com/EL5101) | [EL5101](../src/lcec_el5101.c) | 0x00000002:0x13ed3052 | Measuring Input |  | 
[Beckhoff EL5151 1Ch. Inc. Encoder](http://www.beckhoff.com/EL5151) | [EL5151](../src/lcec_el5151.c) | 0x00000002:0x141f3052 | Measuring Input |  | 
[Beckhoff EL5152 2Ch. Inc. Encoder](http://www.beckhoff.com/EL5152) | [EL5152](../src/lcec_el5152.c) | 0x00000002:0x14203052 | Measuring Input |  | 
[Beckhoff EL6090 Display terminal](http://www.beckhoff.com/EL6090) | [EL6090](../src/lcec_el6090.c) | 0x00000002:0x17ca3052 | LCD Display | Uncertain; @scottlaird has one, but the driver was just merged. | 
[Beckhoff EL6900, TwinSAFE PLC](http://www.beckhoff.com/EL6900) | [EL6900](../src/lcec_el6900.c) | 0x00000002:0x1af43052 | Safety Terminals | Part of @scottlaird's test suite, but not currently being evaluated. | 
[Beckhoff EL7031 1Ch. Stepper motor output stage (24V, 1.5A)](http://www.beckhoff.com/EL7031) | [EL7031](../src/lcec_el70x1.c) | 0x00000002:0x1b773052 | Stepper Drive |  | 
[Beckhoff EL7041 1Ch. Stepper motor output stage (50V, 5A)](http://www.beckhoff.com/EL7041) | [EL7041](../src/lcec_el7041.c) | 0x00000002:0x1b813052 | Stepper Drive |  | 
[Beckhoff EL7041 1Ch. Stepper motor output stage (50V, 5A)](http://www.beckhoff.com/EL7041) | [EL7041_0052](../src/lcec_el70x1.c) | 0x00000002:0x1b813052 | Stepper Drive |  | 
[Beckhoff EL7041 1Ch. Stepper motor output stage (50V, 5A)](http://www.beckhoff.com/EL7041) | [EL7041_1000](../src/lcec_el7041.c) | 0x00000002:0x1b813052 | Stepper Drive |  | 
[Beckhoff EL7201 1Ch. MDP742 Servo motor output stage (50V, 4A)](http://www.beckhoff.com/EL7201) | [EL7201_9014](../src/lcec_el7211.c) | 0x00000002:0x1c213052 | Servo Drive |  | 
[Beckhoff EL7211 1Ch. MDP742 Servo motor output stage (50V, 4.5A RMS)](http://www.beckhoff.com/EL7211) | [EL7211](../src/lcec_el7211.c) | 0x00000002:0x1c2b3052 | Servo Drive |  | 
[Beckhoff EL7221 1Ch. MDP742 Servo motor output stage (50V, 8A RMS)](http://www.beckhoff.com/EL7211) | [EL7221](../src/lcec_el7211.c) | 0x00000002:0x1c353052 | Servo Drive |  | 
[Beckhoff EL7342 2Ch. DC motor output stage (50V, 3.5A)](http://www.beckhoff.com/EL7342) | [EL7342](../src/lcec_el7342.c) | 0x00000002:0x1cae3052 | DC Drive |  | 
[Beckhoff EL7411 BLDC Terminal with incremental encoder/Hall, 50 V DC, 4.5 A](http://www.beckhoff.com/EL7411) | [EL7411](../src/lcec_el7411.c) | 0x00000002:0x1cf33052 | BLDC Drive |  | 
[Beckhoff EL9505 Power supply terminal 5V](http://www.beckhoff.com/EL9505) | [EL9505](../src/lcec_el95xx.c) | 0x00000002:0x25213052 | System Terminals |  | 
[Beckhoff EL9508 Power supply terminal 8V](http://www.beckhoff.com/EL9508) | [EL9508](../src/lcec_el95xx.c) | 0x00000002:0x25243052 | System Terminals |  | 
[Beckhoff EL9510 Power supply terminal 10V](http://www.beckhoff.com/EL9510) | [EL9510](../src/lcec_el95xx.c) | 0x00000002:0x25263052 | System Terminals |  | 
[Beckhoff EL9512 Power supply terminal 12V](http://www.beckhoff.com/EL9512) | [EL9512](../src/lcec_el95xx.c) | 0x00000002:0x25283052 | System Terminals |  | 
[Beckhoff EL9515 Power supply terminal 15V](http://www.beckhoff.com/EL9515) | [EL9515](../src/lcec_el95xx.c) | 0x00000002:0x252b3052 | System Terminals |  | 
[Beckhoff EL9576 Brake chopper terminal](http://www.beckhoff.com/EL9576) | [EL9576](../src/lcec_el95xx.c) | 0x00000002:0x25683052 | System Terminals |  | 
[Beckhoff EM3701, 1Ch. differential pressure measuring terminal 100hPa (100mbar)](http://www.beckhoff.com/EM3701) | [EM3701](../src/lcec_em37xx.c) | 0x00000002:0x0e753452 | Analog Input |  | 
[Beckhoff EM3702  2Ch. relative pressure measuring terminal 7,500 hPa (7.5 bar)](http://www.beckhoff.com/EM3702) | [EM3702](../src/lcec_em37xx.c) | 0x00000002:0x0e763452 | Analog Input | Uncertain; @scottlaird has one | 
[Beckhoff EM3712  2Ch relative pressure measuring terminal -1,000...+1,000 hPa (-1...+1 bar)](http://www.beckhoff.com/EM3712) | [EM3712](../src/lcec_em37xx.c) | 0x00000002:0x0e803452 | Analog Input |  | 
[Beckhoff EM7004 4-Axis Interface Unit](http://www.beckhoff.com/EM7004) | [EM7004](../src/lcec_em7004.c) | 0x00000002:0x1b5c3452 | Servo Drive |  | 
[Beckhoff EP1008-0001 8 Ch. Dig. Input 24V, 3ms, M8](https://www.beckhoff.com/EP1008-0001) | [EP1008](../src/lcec_el1xxx.c) | 0x00000002:0x03f04052 | Digital Input | Uncertain; @scottlaird has several | 
[Beckhoff EP1018-0001 8 Ch. Dig. Input 24V, 10µs, M8](https://www.beckhoff.com/EP1018-0001) | [EP1018](../src/lcec_el1xxx.c) | 0x00000002:0x03fa4052 | Digital Input | Uncertain; @scottlaird has several | 
[Beckhoff EP2008-0001 8 Ch. Dig. Output 24V, 0,5A, M8](https://www.beckhoff.com/EP2008-0001) | [EP2008](../src/lcec_el2xxx.c) | 0x00000002:0x07d84052 | Digital Output |  | 
[Beckhoff EP2028-0001 8 Ch. Dig. Output 24V, 2A, M8](https://www.beckhoff.com/EP2028-0001) | [EP2028](../src/lcec_el2xxx.c) | 0x00000002:0x07ec4052 | Digital Output |  | 
[Beckhoff EP2308-0000 4 Ch. Dig. In, 3ms, 4 Ch. Dig. Out 24V, 0,5A, S8](https://www.beckhoff.com/EP2308-0000) | [EP2308](../src/lcec_ep23xx.c) | 0x00000002:0x09044052 | Digital Input/Output |  | 
[Beckhoff EP2316-0003 8 Ch. Dig. In, 10µs, 8Ch. Dig. Out 24V, 0,5A, Diagnostic, 10-Pole Plug Connector](https://www.beckhoff.com/EP2316-0003) | [EP2316](../src/lcec_ep2316.c) | 0x00000002:0x090c4052 | Digital Input/Output |  | 
[Beckhoff EP2318-0001 4 Ch. Dig. In, 10µs, 4 Ch. Dig. Out 24V, 0,5A, M8](https://www.beckhoff.com/EP2318-0001) | [EP2318](../src/lcec_ep23xx.c) | 0x00000002:0x090e4052 | Digital Input/Output |  | 
[Beckhoff EP2328-0001 4 CH. Dig. In, 3ms, 4 Ch. Dig. Out 24V, 2A, M8](https://www.beckhoff.com/EP2328-0001) | [EP2328](../src/lcec_ep23xx.c) | 0x00000002:0x09184052 | Digital Input/Output |  | 
[Beckhoff EP2338-0001 8 Ch. Dig. Input/Output 24V, 0,5A, M8](https://www.beckhoff.com/EP2338-0001) | [EP2338](../src/lcec_ep23xx.c) | 0x00000002:0x09224052 | Digital Input/Output | Uncertain; @scottlaird has several | 
[Beckhoff EP2349-0021 16 Ch. Dig. Input/Output 24V, 10µs, 0,5A, M8](https://www.beckhoff.com/EP2349-0021) | [EP2349](../src/lcec_ep23xx.c) | 0x00000002:0x092d4052 | Digital Input/Output |  | 
[Beckhoff EP2809-0021 16 Ch. Dig. Output 24V, 0,5A, M8](https://www.beckhoff.com/EP2809-0021) | [EP2809](../src/lcec_el2xxx.c) | 0x00000002:0x0af94052 | Digital Output |  | 
[Beckhoff EL7041 1Ch. Stepper motor output stage (50V, 5A)](http://www.beckhoff.com/EL7041) | [EP7041](../src/lcec_el7041.c) | 0x00000002:0x1b813052 | Stepper Drive | Uncertain; @scottlaird has several EP7041-0002 | 
[Omron R88D-KN01H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN01H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000005 | Servo Drive |  | 
[Omron R88D-KN01L-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN01L_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000002 | Servo Drive |  | 
[Omron R88D-KN02H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN02H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000006 | Servo Drive |  | 
[Omron R88D-KN02L-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN02L_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000003 | Servo Drive |  | 
[Omron R88D-KN04H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN04H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000007 | Servo Drive |  | 
[Omron R88D-KN04L-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN04L_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000004 | Servo Drive |  | 
[Omron R88D-KN06F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN06F_ECT](../src/lcec_omrg5.c) | 0x00000083:0x0000000b | Servo Drive |  | 
[Omron R88D-KN08H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN08H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000008 | Servo Drive |  | 
[Omron R88D-KN10F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN10F_ECT](../src/lcec_omrg5.c) | 0x00000083:0x0000000c | Servo Drive |  | 
[Omron R88D-KN10H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN10H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000009 | Servo Drive |  | 
[Omron R88D-KN150F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN150F_ECT](../src/lcec_omrg5.c) | 0x00000083:0x0000005f | Servo Drive |  | 
[Omron R88D-KN150H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN150H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x0000005a | Servo Drive |  | 
[Omron R88D-KN15F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN15F_ECT](../src/lcec_omrg5.c) | 0x00000083:0x0000000d | Servo Drive |  | 
[Omron R88D-KN15H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN15H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x0000000a | Servo Drive |  | 
[Omron R88D-KN20F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN20F_ECT](../src/lcec_omrg5.c) | 0x00000083:0x0000005b | Servo Drive |  | 
[Omron R88D-KN20H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN20H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000056 | Servo Drive |  | 
[Omron R88D-KN30F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN30F_ECT](../src/lcec_omrg5.c) | 0x00000083:0x0000005c | Servo Drive |  | 
[Omron R88D-KN30H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN30H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000057 | Servo Drive |  | 
[Omron R88D-KN50F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN50F_ECT](../src/lcec_omrg5.c) | 0x00000083:0x0000005d | Servo Drive |  | 
[Omron R88D-KN50H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN50H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000058 | Servo Drive |  | 
[Omron R88D-KN75F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN75F_ECT](../src/lcec_omrg5.c) | 0x00000083:0x0000005e | Servo Drive |  | 
[Omron R88D-KN75H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KN75H_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000059 | Servo Drive |  | 
[Omron R88D-KNA5L-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [OMRG5_R88D_KNA5L_ECT](../src/lcec_omrg5.c) | 0x00000083:0x00000001 | Servo Drive |  | 
[Stoeber Posidrive MDS 5000](https://www.ethercat.org/en/products/CE4C65D2ED8B4F39A8166D6CEDB2C872.htm) | [STMDS5K](../src/lcec_stmds5k.c) | 0x000000b9:0x00001388 | Servo Drive |  | 

There are an additional 1 devices supported that do not have enough
documentation to display here.  Please look at the `documentation/devices/` files
and update them if you're able.
