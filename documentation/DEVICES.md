# Devices Supported by LinuxCNC-Ethercat

*This is a work in progress, listing all of the devices that LinuxCNC-Ethercat
has code to support today.  Not all of these are well-tested.*

Description | Driver | EtherCAT VID:PID | Device Type | Testing Status | Notes
----------- | ------ | ---------------- | ----------- | -------------- | ------
[Beckhoff AX5101-0000-0214 EtherCAT Drive (SoE, 1 Ch.)](http://www.beckhoff.com/ax51xx/) | [ax5100](../src/devices/lcec_ax5100.c) | 0x00000002:0x13ed6012 | Servo Drive |  | 
[Beckhoff AX5103-0000-0214 EtherCAT Drive (SoE, 1 Ch.)](http://www.beckhoff.com/ax51xx/) | [ax5100](../src/devices/lcec_ax5100.c) | 0x00000002:0x13ef6012 | Servo Drive |  | 
[Beckhoff AX5106-0000-0214 EtherCAT Drive (SoE, 1 Ch.)](http://www.beckhoff.com/ax51xx/) | [ax5100](../src/devices/lcec_ax5100.c) | 0x00000002:0x13f26012 | Servo Drive |  | 
[Beckhoff AX5112-0000-0214 EtherCAT Drive (SoE, 1 Ch.)](http://www.beckhoff.com/ax51xx/) | [ax5100](../src/devices/lcec_ax5100.c) | 0x00000002:0x13f86012 | Servo Drive |  | 
[Beckhoff AX5118-0000-0214 EtherCAT Drive (SoE, 1 Ch.)](http://www.beckhoff.com/ax51xx/) | [ax5100](../src/devices/lcec_ax5100.c) | 0x00000002:0x13fe6012 | Servo Drive |  | 
[Beckhoff AX5203-0000-0214 EtherCAT Drive (SoE, 2 Ch.)](http://www.beckhoff.com/ax52xx/) | [ax5200](../src/devices/lcec_ax5200.c) | 0x00000002:0x14536012 | Servo Drive |  | 
[Beckhoff AX5206-0000-0214 EtherCAT Drive (SoE, 2 Ch.)](http://www.beckhoff.com/ax52xx/) | [ax5200](../src/devices/lcec_ax5200.c) | 0x00000002:0x14566012 | Servo Drive |  | 
[Beckhoff AX5805 (Safety Drive Option)](http://beckhoff.com/AX5805) | [ax5805](../src/devices/lcec_ax5805.c) | 0x00000002:0x16ad6012 | Safety Terminals |  | 
[Delta ASDA-A2-E](https://www.deltaww.com/en-us/products/Servo-Systems-AC-Servo-Motors-and-Drives/23) | [deasda](../src/devices/lcec_deasda.c) | 0x000001dd:0x10305070 | Servo Drive |  | 
[Delta MS-300 AC Motor Drive](https://www.deltaww.com/en-us/products/AC-Motor-Drives/3449) | [dems300](../src/devices/lcec_dems300.c) | 0x000001dd:0x10400200 | AC Motor Drive |  | 
[Beckhoff EK1100 EtherCAT Coupler (0.5A E-Bus)](http://www.beckhoff.com/EK1100) | [ek1100](../src/devices/lcec_ek1100.c) | 0x00000002:0x044c2c52 | System Couplers | Part of @scottlaird's test suite. | 
[Beckhoff EK1101 EtherCAT Coupler (2A E-Bus, ID switch)](http://www.beckhoff.com/EK1101) | [ek1100](../src/devices/lcec_ek1100.c) | 0x00000002:0x044d2c52 | System Couplers |  | 
[Beckhoff EK1110 EtherCAT extension](http://www.beckhoff.com/EK1110) | [ek1100](../src/devices/lcec_ek1100.c) | 0x00000002:0x04562c52 | System Terminals | Part of @scottlaird's test suite | 
[Beckhoff EK1122 2 port EtherCAT junction](http://www.beckhoff.com/EK1122) | [ek1100](../src/devices/lcec_ek1100.c) | 0x00000002:0x04622c52 | System Terminals |  | 
[Beckhoff EL1002 2Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1002) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x03ea3052 | Digital Input Terminals |  | 
[Beckhoff EL1004 4Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1004) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x03ec3052 | Digital Input Terminals |  | 
[Beckhoff EL1008 8Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1008) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x03f03052 | Digital Input Terminals |  | 
[Beckhoff EL1012 2Ch. Dig. Input 24V, 10µs](http://www.beckhoff.com/EL1012) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x03f43052 | Digital Input |  | 
[Beckhoff EL1014 4Ch. Dig. Input 24V, 10µs](http://www.beckhoff.com/EL1014) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x03f63052 | Digital Input |  | 
[Beckhoff EL1018 8Ch. Dig. Input 24V, 10µs](http://www.beckhoff.com/EL1018) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x03fa3052 | Digital Input | Part of @scottlaird's test suite, hardware is actively exercised for most releases. | 
[Beckhoff EL1024 4Ch. Dig. Input 24V, Type 2, 3ms](http://www.beckhoff.com/EL1024) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x04003052 | Digital Input |  | 
[Beckhoff EL1034 4Ch. Dig. Input 24V, potential-free, 10µs](http://www.beckhoff.com/EL1034) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x040a3052 | Digital Input |  | 
[Beckhoff EL1084 4Ch. Dig. Input 24V, 3ms, negative](http://www.beckhoff.com/EL1084) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x043c3052 | Digital Input |  | 
[Beckhoff EL1088 8Ch. Dig. Input 24V, 3ms, negative](http://www.beckhoff.com/EL1088) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x04403052 | Digital Input |  | 
[Beckhoff EL1094 4Ch. Dig. Input 24V, 10µs, negative](http://www.beckhoff.com/EL1094) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x04463052 | Digital Input |  | 
[Beckhoff EL1098 8Ch. Dig. Input 24V, 10µs, negative](http://www.beckhoff.com/EL1098) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x044a3052 | Digital Input |  | 
[Beckhoff EL1104 4Ch. Dig. Input 24V, 3ms, Sensor Power](http://www.beckhoff.com/EL1104) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x04503052 | Digital Input |  | 
[Beckhoff EL1114 4Ch. Dig. Input 24V, 10µs, Sensor Power](http://www.beckhoff.com/EL1114) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x045a3052 | Digital Input |  | 
[Beckhoff EL1124 4Ch. Dig. Input 5V, 10µs, Sensor Power](http://www.beckhoff.com/EL1124) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x04643052 | Digital Input |  | 
[Beckhoff EL1134 4Ch. Dig. Input 48V, 10µs, Sensor Power](http://www.beckhoff.com/EL1134) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x046e3052 | Digital Input |  | 
[Beckhoff EL1144 4Ch. Dig. Input 12V, 10µs, Sensor Power](http://www.beckhoff.com/EL1144) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x04783052 | Digital Input |  | 
[Beckhoff EL1252 2Ch. Fast Dig. Input 24V, 1µs, DC Latch](http://www.beckhoff.com/EL1252) | [el1252](../src/devices/lcec_el1252.c) | 0x00000002:0x04e43052 | Digital Input |  | 
[Beckhoff EL1804 4Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1804) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x070c3052 | Digital Input |  | 
[Beckhoff EL1808 8Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1808) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x07103052 | Digital Input | Uncertain; @scottlaird has one | 
[Beckhoff EL1809 16Ch. Dig. Input 24V, 3ms](http://www.beckhoff.com/EL1809) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x07113052 | Digital Input |  | 
[Beckhoff EL1819 16Ch. Dig. Input 24V, 10µs](http://www.beckhoff.com/EL1819) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x071b3052 | Digital Input |  | 
[Beckhoff EL1859 8Ch. Dig. Input 24V, 3ms, 8Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL1859) | [el1859](../src/devices/lcec_el1859.c) | 0x00000002:0x07433052 | Digital Input |  | 
[Beckhoff EL1904, 4 Ch. Safety Input 24V, TwinSAFE](http://www.beckhoff.com/EL1904) | [el1904](../src/devices/lcec_el1904.c) | 0x00000002:0x07703052 | Safety Terminals | Part of @scottlaird's test suite, but not currently being evaluated. | 
[Beckhoff EL1918, 8Ch. Safety Input 24V, TwinSAFE](http://www.beckhoff.com/EL1918) | [el1918_logic](../src/devices/lcec_el1918_logic.c) | 0x00000002:0x077e3052 | Safety Terminals |  | 
[Beckhoff EL2002 2Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL2002) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x07d23052 | Digital Output |  | 
[Beckhoff EL2004 4Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL2004) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x07d43052 | Digital Output |  | 
[Beckhoff EL2008 8Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL2008) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x07d83052 | Digital Output | Part of @scottlaird's test suite, hardware is actively exercised for most releases. | 
[Beckhoff EL2022 2Ch. Dig. Output 24V, 2A](http://www.beckhoff.com/EL2022) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x07e63052 | Digital Output | Part of @scottlaird's test suite. | 
[Beckhoff EL2024 4Ch. Dig. Output 24V, 2A](http://www.beckhoff.com/EL2024) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x07e83052 | Digital Output |  | 
[Beckhoff EL2032 2Ch. Dig. Output 24V, 2A Diag](http://www.beckhoff.com/EL2032) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x07f03052 | Digital Output |  | 
[Beckhoff EL2034 4Ch. Dig. Output 24V, 2A, Diagnostic](http://www.beckhoff.com/EL2034) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x07f23052 | Digital Output | Part of @scottlaird's test suite. | 
[Beckhoff EL2042 2Ch. Dig. Output 24V, 4A](http://www.beckhoff.com/EL2042) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x07fa3052 | Digital Output |  | 
[Beckhoff EL2084 4Ch. Dig. Output 24V, 0.5A, switching to negative](http://www.beckhoff.com/EL2084) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x08243052 | Digital Output | Part of @scottlaird's test suite. | 
[Beckhoff EL2088 8Ch. Dig. Output 24V, 0.5A, switching to negative](http://www.beckhoff.com/EL2088) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x08283052 | Digital Output |  | 
[Beckhoff EL2124 4Ch. Dig. Output 5V, 20mA](http://www.beckhoff.com/EL2124) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x084c3052 | Digital Output |  | 
[Beckhoff EL2202 2Ch. Dig. Output 24V, 0.5A, DC Sync](http://www.beckhoff.com/EL2202) | [el2202](../src/devices/lcec_el2202.c) | 0x00000002:0x089a3052 | Digital Output |  | 
[Beckhoff EL2521 1Ch. Pulse Train Output](http://www.beckhoff.com/EL2521) | [el2521](../src/devices/lcec_el2521.c) | 0x00000002:0x09d93052 | Digital Output |  | 
[Beckhoff EL2612 2Ch. Relay Output, CO (125V AC / 30V DC)](http://www.beckhoff.com/EL2612) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x0a343052 | Digital Output |  | 
[Beckhoff EL2622 2Ch. Relay Output, NO (230V AC / 30V DC)](http://www.beckhoff.com/EL2622) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x0a3e3052 | Digital Output |  | 
[Beckhoff EL2634 4Ch. Relay Output, NO (250V AC / 30V DC)](http://www.beckhoff.com/EL2634) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x0a4a3052 | Digital Output |  | 
[Beckhoff EL2652 2Ch. Relay Output, CO (230V AC / 30V DC)](http://www.beckhoff.com/EL2652) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x0a5c3052 | Digital Output |  | 
[Beckhoff EL2798 8Ch. Dig. Output 24V AC/DC, 2A, potential-free](http://www.beckhoff.com/EL2798) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x0aee3052 | Digital Output | Part of @scottlaird's test suite. | 
[Beckhoff EL2808 8Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL2808) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x0af83052 | Digital Output | Uncertain; @scottlaird has several | 
[Beckhoff EL2809 16Ch. Dig. Output 24V, 0.5A](http://www.beckhoff.com/EL2809) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x0af93052 | Digital Output |  | 
[Beckhoff EL2904, 4 Ch. Safety Output 24V, 0.5A, TwinSAFE](http://www.beckhoff.com/EL2904) | [el2904](../src/devices/lcec_el2904.c) | 0x00000002:0x0b583052 | Safety Terminals | Part of @scottlaird's test suite, but not currently being evaluated. | 
[Beckhoff EL3001 1Ch. Ana. Input +/-10V](http://www.beckhoff.com/EL3001) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bb93052 | Analog Input | New, untested. | 
[Beckhoff EL3002 2Ch. Ana. Input +/-10V](http://www.beckhoff.com/EL3002) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bba3052 | Analog Input | New, untested. | 
[Beckhoff EL3004 4Ch. Ana. Input +/-10V](http://www.beckhoff.com/EL3004) | [el30x4](../src/devices/lcec_el30x4.c) | 0x00000002:0x0bbc3052 | Analog Input |  | 
[Beckhoff EL3008 8Ch. Ana. Input +/-10V](http://www.beckhoff.com/EL3008) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bc03052 | Analog Input | New, untested. | 
[Beckhoff EL3011 4Ch. Ana. Input 0-20mA DIFF](http://www.beckhoff.com/EL3011) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bc33052 | Analog Input | New, untested. | 
[Beckhoff EL3012 4Ch. Ana. Input 0-20mA DIFF](http://www.beckhoff.com/EL3012) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bc43052 | Analog Input | New, untested. | 
[Beckhoff EL3014 4Ch. Ana. Input 0-20mA DIFF](http://www.beckhoff.com/EL3014) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bc63052 | Analog Input | New, untested. | 
[Beckhoff EL3021 4Ch. Ana. Input 4-20mA DIFF](http://www.beckhoff.com/EL3021) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bcd3052 | Analog Input | New, untested. | 
[Beckhoff EL3022 4Ch. Ana. Input 4-20mA DIFF](http://www.beckhoff.com/EL3022) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bce3052 | Analog Input | New, untested. | 
[Beckhoff EL3024 4Ch. Ana. Input 4-20mA DIFF](http://www.beckhoff.com/EL3024) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bd03052 | Analog Input | New, untested. | 
[Beckhoff EL3041 1Ch. Ana. Input 0-20mA](http://www.beckhoff.com/EL3041) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0be13052 | Analog Input | New, untested. | 
[Beckhoff EL3042 2Ch. Ana. Input 0-20mA](http://www.beckhoff.com/EL3042) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0be23052 | Analog Input | New, untested. | 
[Beckhoff EL3044 4Ch. Ana. Input 0-20mA](http://www.beckhoff.com/EL3044) | [el30x4](../src/devices/lcec_el30x4.c) | 0x00000002:0x0be43052 | Analog Input |  | 
[Beckhoff EL3048 8Ch. Ana. Input 0-20mA](http://www.beckhoff.com/EL3048) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0be83052 | Analog Input | New, untested. | 
[Beckhoff EL3051 1Ch. Ana. Input 4-20mA](http://www.beckhoff.com/EL3051) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0beb3052 | Analog Input | New, untested. | 
[Beckhoff EL3052 2Ch. Ana. Input 4-20mA](http://www.beckhoff.com/EL3052) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bec3052 | Analog Input | New, untested. | 
[Beckhoff EL3054 4Ch. Ana. Input 4-20mA](http://www.beckhoff.com/EL3054) | [el30x4](../src/devices/lcec_el30x4.c) | 0x00000002:0x0bee3052 | Analog Input |  | 
[Beckhoff EL3058 8Ch. Ana. Input 4-20mA](http://www.beckhoff.com/EL3058) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bf23052 | Analog Input | New, untested. | 
[Beckhoff EL3061 1Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3061) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bf53052 | Analog Input | New, untested. | 
[Beckhoff EL3062 2Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3062) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bf63052 | Analog Input | New, untested. | 
[Beckhoff EL3064 4Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3064) | [el30x4](../src/devices/lcec_el30x4.c) | 0x00000002:0x0bf83052 | Analog Input |  | 
[Beckhoff EL3068 8Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3068) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0bfc3052 | Analog Input | Part of @scottlaird's test suite, hardware is actively exercised for most releases. | 
[Beckhoff EL3101 1Ch. Ana. Input +/-10V Diff.](http://www.beckhoff.com/EL3101) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c1d3052 | Analog Input | New, untested. | 
[Beckhoff EL3102 2Ch. Ana. Input +/-10V, Diff.](http://www.beckhoff.com/EL3102) | [el31x2](../src/devices/lcec_el31x2.c) | 0x00000002:0x0c1e3052 | Analog Input |  | 
[Beckhoff EL3104 4Ch. Ana. Input +/-10V Diff.](http://www.beckhoff.com/EL3104) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c203052 | Analog Input | New, untested. | 
[Beckhoff EL3111 1Ch. Ana. Input 0-20mA, Diff.](http://www.beckhoff.com/EL3111) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c273052 | Analog Input | New, untested. | 
[Beckhoff EL3112 2Ch. Ana. Input 0-20mA, Diff.](http://www.beckhoff.com/EL3112) | [el31x2](../src/devices/lcec_el31x2.c) | 0x00000002:0x0c283052 | Analog Input |  | 
[Beckhoff EL3114 4Ch. Ana. Input 0-20mA, Diff.](http://www.beckhoff.com/EL3114) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c2a3052 | Analog Input | New, untested. | 
[Beckhoff EL3121 1Ch. Ana. Input 4-20mA Diff.](http://www.beckhoff.com/EL3121) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c313052 | Analog Input | New, untested. | 
[Beckhoff EL3122 2Ch. Ana. Input 4-20mA, Diff.](http://www.beckhoff.com/EL3122) | [el31x2](../src/devices/lcec_el31x2.c) | 0x00000002:0x0c323052 | Analog Input |  | 
[Beckhoff EL3141 1Ch. Ana. Input 0-20mA](http://www.beckhoff.com/EL3141) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c453052 | Analog Input | New, untested. | 
[Beckhoff EL3142 2Ch. Ana. Input 0-20mA](http://www.beckhoff.com/EL3142) | [el31x2](../src/devices/lcec_el31x2.c) | 0x00000002:0x0c463052 | Analog Input |  | 
[Beckhoff EL3144 4Ch. Ana. Input 0-20mA](http://www.beckhoff.com/EL3144) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c483052 | Analog Input | New, untested. | 
[Beckhoff EL3151 1Ch. Ana. Input 4-20mA](http://www.beckhoff.com/EL3151) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c4f3052 | Analog Input | New, untested. | 
[Beckhoff EL3152 2Ch. Ana. Input 4-20mA](http://www.beckhoff.com/EL3152) | [el31x2](../src/devices/lcec_el31x2.c) | 0x00000002:0x0c503052 | Analog Input |  | 
[Beckhoff EL3154 4Ch. Ana. Input 4-20mA](http://www.beckhoff.com/EL3154) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c523052 | Analog Input | New, untested. | 
[Beckhoff EL3161 1Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3161) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c593052 | Analog Input | New, untested. | 
[Beckhoff EL3162 2Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3162) | [el31x2](../src/devices/lcec_el31x2.c) | 0x00000002:0x0c5a3052 | Analog Input | Uncertain; @scottlaird has one | 
[Beckhoff EL3164 4Ch. Ana. Input 0-10V](http://www.beckhoff.com/EL3164) | [el31x4](../src/devices/lcec_el31x4.c) | 0x00000002:0x0c5c3052 | Analog Input |  | 
[Beckhoff EL3182 2Ch. Ana. Input 4-20mA, HART](http://www.beckhoff.com/EL3182) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x0c6e3052 | Analog Input | New, untested. | 
[Beckhoff EL3202 2Ch. Ana. Input PT100 (RTD)](http://www.beckhoff.com/EL3202) | [el3202](../src/devices/lcec_el3202.c) | 0x00000002:0x0c823052 | Analog Input |  | 
[Beckhoff EL3255 5Ch. potentiometer measurement with sensor supply](http://www.beckhoff.com/EL3255) | [el3255](../src/devices/lcec_el3255.c) | 0x00000002:0x0cb73052 | Analog Input |  | 
[Beckhoff EL3403 3Ch. Power Measuring](http://www.beckhoff.com/EL3403) | [el3403](../src/devices/lcec_el3403.c) | 0x00000002:0x0d4b3052 | Analog Input | Uncertain; @scottlaird has several | 3-phase AC power measurement
[Beckhoff EL4001 1Ch. Ana. Output 0-10V, 12bit](http://www.beckhoff.com/EL4001) | [el40x1](../src/devices/lcec_el40x1.c) | 0x00000002:0x0fa13052 | Analog Output |  | 
[Beckhoff EL4002 2Ch. Ana. Output 0-10V, 12bit](http://www.beckhoff.com/EL4002) | [el40x2](../src/devices/lcec_el40x2.c) | 0x00000002:0x0fa23052 | Analog Output |  | 
[Beckhoff EL4008 8Ch. Ana. Output 0-10V, 12bit](http://www.beckhoff.com/EL4008) | [el40x8](../src/devices/lcec_el40x8.c) | 0x00000002:0x0fa83052 | Analog Output |  | 
[Beckhoff EL4011 1Ch. Ana. Output 0-20mA, 12bit](http://www.beckhoff.com/EL4011) | [el40x1](../src/devices/lcec_el40x1.c) | 0x00000002:0x0fab3052 | Analog Output |  | 
[Beckhoff EL4012 2Ch. Ana. Output 0-20mA, 12bit](http://www.beckhoff.com/EL4012) | [el40x2](../src/devices/lcec_el40x2.c) | 0x00000002:0x0fac3052 | Analog Output |  | 
[Beckhoff EL4018 8Ch. Ana. Output 0-20mA, 12bit](http://www.beckhoff.com/EL4018) | [el40x8](../src/devices/lcec_el40x8.c) | 0x00000002:0x0fb23052 | Analog Output |  | 
[Beckhoff EL4021 1Ch. Ana. Output 4-20mA, 12bit](http://www.beckhoff.com/EL4021) | [el40x1](../src/devices/lcec_el40x1.c) | 0x00000002:0x0fb53052 | Analog Output |  | 
[Beckhoff EL4022 2Ch. Ana. Output 4-20mA, 12bit](http://www.beckhoff.com/EL4022) | [el40x2](../src/devices/lcec_el40x2.c) | 0x00000002:0x0fb63052 | Analog Output |  | 
[Beckhoff EL4028 8Ch. Ana. Output 4-20mA, 12bit](http://www.beckhoff.com/EL4028) | [el40x8](../src/devices/lcec_el40x8.c) | 0x00000002:0x0fbc3052 | Analog Output |  | 
[Beckhoff EL4031 1Ch. Ana. Output -10/+10V, 12bit](http://www.beckhoff.com/EL4031) | [el40x1](../src/devices/lcec_el40x1.c) | 0x00000002:0x0fbf3052 | Analog Output |  | 
[Beckhoff EL4032 2Ch. Ana. Output -10/+10V, 12bit](http://www.beckhoff.com/EL4032) | [el40x2](../src/devices/lcec_el40x2.c) | 0x00000002:0x0fc03052 | Analog Output |  | 
[Beckhoff EL4038 8Ch. Ana. Output -10/+10V, 12bit](http://www.beckhoff.com/EL4038) | [el40x8](../src/devices/lcec_el40x8.c) | 0x00000002:0x0fc63052 | Analog Output |  | 
[Beckhoff EL4102 2Ch. Ana. Output 0-10V](http://www.beckhoff.com/EL4102) | [el41x2](../src/devices/lcec_el41x2.c) | 0x00000002:0x10063052 | Analog Output |  | 
[Beckhoff EL4104 4Ch. Ana. Output 0-10V, 16bit](http://www.beckhoff.com/EL4104) | [el41x4](../src/devices/lcec_el41x4.c) | 0x00000002:0x10083052 | Analog Output |  | 
[Beckhoff EL4112 2Ch. Ana. Output 0-20mA](http://www.beckhoff.com/EL4112) | [el41x2](../src/devices/lcec_el41x2.c) | 0x00000002:0x10103052 | Analog Output |  | 
[Beckhoff EL4122 2Ch. Ana. Output 4-20mA](http://www.beckhoff.com/EL4122) | [el41x2](../src/devices/lcec_el41x2.c) | 0x00000002:0x101a3052 | Analog Output |  | 
[Beckhoff EL4132 2Ch. Ana. Output +/-10V](http://www.beckhoff.com/EL4132) | [el41x2](../src/devices/lcec_el41x2.c) | 0x00000002:0x10243052 | Analog Output |  | 
[Beckhoff EL4134 4Ch. Ana. Output -10/+10V, 16bit](http://www.beckhoff.com/EL4134) | [el41x4](../src/devices/lcec_el41x4.c) | 0x00000002:0x10263052 | Analog Output |  | 
[Beckhoff EL5002 2Ch. SSI Encoder](http://www.beckhoff.com/EL5002) | [el5002](../src/devices/lcec_el5002.c) | 0x00000002:0x138a3052 | Measuring Input |  | 
[Beckhoff EL5032 2Ch. EnDat Encoder](http://www.beckhoff.com/EL5032) | [el5032](../src/devices/lcec_el5032.c) | 0x00000002:0x13a83052 | Measuring Input |  | 
[Beckhoff EL5101 1Ch. Encoder 5V](http://www.beckhoff.com/EL5101) | [el5101](../src/devices/lcec_el5101.c) | 0x00000002:0x13ed3052 | Measuring Input |  | 
[Beckhoff EL5151 1Ch. Inc. Encoder](http://www.beckhoff.com/EL5151) | [el5151](../src/devices/lcec_el5151.c) | 0x00000002:0x141f3052 | Measuring Input |  | 
[Beckhoff EL5152 2Ch. Inc. Encoder](http://www.beckhoff.com/EL5152) | [el5152](../src/devices/lcec_el5152.c) | 0x00000002:0x14203052 | Measuring Input |  | 
[Beckhoff EL6090 Display terminal](http://www.beckhoff.com/EL6090) | [el6090](../src/devices/lcec_el6090.c) | 0x00000002:0x17ca3052 | LCD Display | Uncertain; @scottlaird has one, but the driver was just merged. | 
[Beckhoff EL6900, TwinSAFE PLC](http://www.beckhoff.com/EL6900) | [el6900](../src/devices/lcec_el6900.c) | 0x00000002:0x1af43052 | Safety Terminals | Part of @scottlaird's test suite, but not currently being evaluated. | 
[Beckhoff EL7031 1Ch. Stepper motor output stage (24V, 1.5A)](http://www.beckhoff.com/EL7031) | [el70x1](../src/devices/lcec_el70x1.c) | 0x00000002:0x1b773052 | Stepper Drive |  | 
[Beckhoff EL7041 1Ch. Stepper motor output stage (50V, 5A)](http://www.beckhoff.com/EL7041) | [el7041](../src/devices/lcec_el7041.c) | 0x00000002:0x1b813052 | Stepper Drive |  | 
[Beckhoff EL7041 1Ch. Stepper motor output stage (50V, 5A)](http://www.beckhoff.com/EL7041) | [el70x1](../src/devices/lcec_el70x1.c) | 0x00000002:0x1b813052 | Stepper Drive |  | 
[Beckhoff EL7041 1Ch. Stepper motor output stage (50V, 5A)](http://www.beckhoff.com/EL7041) | [el7041](../src/devices/lcec_el7041.c) | 0x00000002:0x1b813052 | Stepper Drive |  | 
[Beckhoff EL7201 1Ch. MDP742 Servo motor output stage (50V, 4A)](http://www.beckhoff.com/EL7201) | [el7211](../src/devices/lcec_el7211.c) | 0x00000002:0x1c213052 | Servo Drive |  | 
[Beckhoff EL7211 1Ch. MDP742 Servo motor output stage (50V, 4.5A RMS)](http://www.beckhoff.com/EL7211) | [el7211](../src/devices/lcec_el7211.c) | 0x00000002:0x1c2b3052 | Servo Drive |  | 
[Beckhoff EL7221 1Ch. MDP742 Servo motor output stage (50V, 8A RMS)](http://www.beckhoff.com/EL7211) | [el7211](../src/devices/lcec_el7211.c) | 0x00000002:0x1c353052 | Servo Drive |  | 
[Beckhoff EL7342 2Ch. DC motor output stage (50V, 3.5A)](http://www.beckhoff.com/EL7342) | [el7342](../src/devices/lcec_el7342.c) | 0x00000002:0x1cae3052 | DC Drive |  | 
[Beckhoff EL7411 BLDC Terminal with incremental encoder/Hall, 50 V DC, 4.5 A](http://www.beckhoff.com/EL7411) | [el7411](../src/devices/lcec_el7411.c) | 0x00000002:0x1cf33052 | BLDC Drive |  | 
[Beckhoff EL9505 Power supply terminal 5V](http://www.beckhoff.com/EL9505) | [el95xx](../src/devices/lcec_el95xx.c) | 0x00000002:0x25213052 | System Terminals |  | 
[Beckhoff EL9508 Power supply terminal 8V](http://www.beckhoff.com/EL9508) | [el95xx](../src/devices/lcec_el95xx.c) | 0x00000002:0x25243052 | System Terminals |  | 
[Beckhoff EL9510 Power supply terminal 10V](http://www.beckhoff.com/EL9510) | [el95xx](../src/devices/lcec_el95xx.c) | 0x00000002:0x25263052 | System Terminals |  | 
[Beckhoff EL9512 Power supply terminal 12V](http://www.beckhoff.com/EL9512) | [el95xx](../src/devices/lcec_el95xx.c) | 0x00000002:0x25283052 | System Terminals |  | 
[Beckhoff EL9515 Power supply terminal 15V](http://www.beckhoff.com/EL9515) | [el95xx](../src/devices/lcec_el95xx.c) | 0x00000002:0x252b3052 | System Terminals |  | 
[Beckhoff EL9576 Brake chopper terminal](http://www.beckhoff.com/EL9576) | [el95xx](../src/devices/lcec_el95xx.c) | 0x00000002:0x25683052 | System Terminals |  | 
[Beckhoff EM3701, 1Ch. differential pressure measuring terminal 100hPa (100mbar)](http://www.beckhoff.com/EM3701) | [em37xx](../src/devices/lcec_em37xx.c) | 0x00000002:0x0e753452 | Analog Input |  | 
[Beckhoff EM3702  2Ch. relative pressure measuring terminal 7,500 hPa (7.5 bar)](http://www.beckhoff.com/EM3702) | [em37xx](../src/devices/lcec_em37xx.c) | 0x00000002:0x0e763452 | Analog Input | Uncertain; @scottlaird has one | 
[Beckhoff EM3712  2Ch relative pressure measuring terminal -1,000...+1,000 hPa (-1...+1 bar)](http://www.beckhoff.com/EM3712) | [em37xx](../src/devices/lcec_em37xx.c) | 0x00000002:0x0e803452 | Analog Input |  | 
[Beckhoff EM7004 4-Axis Interface Unit](http://www.beckhoff.com/EM7004) | [em7004](../src/devices/lcec_em7004.c) | 0x00000002:0x1b5c3452 | Servo Drive |  | 
[Beckhoff EP1008-0001 8 Ch. Dig. Input 24V, 3ms, M8](https://www.beckhoff.com/EP1008-0001) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x03f04052 | Digital Input | Uncertain; @scottlaird has several | 
[Beckhoff EP1018-0001 8 Ch. Dig. Input 24V, 10µs, M8](https://www.beckhoff.com/EP1018-0001) | [el1xxx](../src/devices/lcec_el1xxx.c) | 0x00000002:0x03fa4052 | Digital Input | Uncertain; @scottlaird has several | 
[Beckhoff EP2008-0001 8 Ch. Dig. Output 24V, 0,5A, M8](https://www.beckhoff.com/EP2008-0001) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x07d84052 | Digital Output |  | 
[Beckhoff EP2028-0001 8 Ch. Dig. Output 24V, 2A, M8](https://www.beckhoff.com/EP2028-0001) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x07ec4052 | Digital Output |  | 
[Beckhoff EP2308-0000 4 Ch. Dig. In, 3ms, 4 Ch. Dig. Out 24V, 0,5A, S8](https://www.beckhoff.com/EP2308-0000) | [ep23xx](../src/devices/lcec_ep23xx.c) | 0x00000002:0x09044052 | Digital Input/Output |  | 
[Beckhoff EP2316-0003 8 Ch. Dig. In, 10µs, 8Ch. Dig. Out 24V, 0,5A, Diagnostic, 10-Pole Plug Connector](https://www.beckhoff.com/EP2316-0003) | [ep2316](../src/devices/lcec_ep2316.c) | 0x00000002:0x090c4052 | Digital Input/Output |  | 
[Beckhoff EP2318-0001 4 Ch. Dig. In, 10µs, 4 Ch. Dig. Out 24V, 0,5A, M8](https://www.beckhoff.com/EP2318-0001) | [ep23xx](../src/devices/lcec_ep23xx.c) | 0x00000002:0x090e4052 | Digital Input/Output |  | 
[Beckhoff EP2328-0001 4 CH. Dig. In, 3ms, 4 Ch. Dig. Out 24V, 2A, M8](https://www.beckhoff.com/EP2328-0001) | [ep23xx](../src/devices/lcec_ep23xx.c) | 0x00000002:0x09184052 | Digital Input/Output |  | 
[Beckhoff EP2338-0001 8 Ch. Dig. Input/Output 24V, 0,5A, M8](https://www.beckhoff.com/EP2338-0001) | [ep23xx](../src/devices/lcec_ep23xx.c) | 0x00000002:0x09224052 | Digital Input/Output | Uncertain; @scottlaird has several | 
[Beckhoff EP2349-0021 16 Ch. Dig. Input/Output 24V, 10µs, 0,5A, M8](https://www.beckhoff.com/EP2349-0021) | [ep23xx](../src/devices/lcec_ep23xx.c) | 0x00000002:0x092d4052 | Digital Input/Output |  | 
[Beckhoff EP2809-0021 16 Ch. Dig. Output 24V, 0,5A, M8](https://www.beckhoff.com/EP2809-0021) | [el2xxx](../src/devices/lcec_el2xxx.c) | 0x00000002:0x0af94052 | Digital Output |  | 
[Beckhoff EL7041 1Ch. Stepper motor output stage (50V, 5A)](http://www.beckhoff.com/EL7041) | [el7041](../src/devices/lcec_el7041.c) | 0x00000002:0x1b813052 | Stepper Drive | Uncertain; @scottlaird has several EP7041-0002 | 
[AB&T EpoCAT FR4000](https://www.bausano.net/en/hardware/epocat-fr-1000.html) | [epocat](../src/devices/lcec_epocat.c) | 0x0000079A:0x00decade | Stepper Drive | Merged 2023-12-31, untested | by @abausano
[Beckhoff EPX3158 8Ch. Ana. Input 4-20mA, Ex i](http://www.beckhoff.com/EPX3158) | [el3xxx](../src/devices/lcec_el3xxx.c) | 0x00000002:0x9809ab69 | Analog Input | New, untested. | 
[SMC EX260-SEC1](https://www.smcpneumatics.com/EX260-SEC1.html) | [ex260](../src/devices/lcec_ex260.c) | 0x00000114:0x01000001 | Valve Controller | Merged 2023-12-31, untested | by @satiowadahc
[SMC EX260-SEC1](https://www.smcpneumatics.com/EX260-SEC2.html) | [ex260](../src/devices/lcec_ex260.c) | 0x00000114:0x01000002 | Valve Controller | Merged 2023-12-31, untested | by @satiowadahc
[SMC EX260-SEC3](https://www.smcpneumatics.com/EX260-SEC3.html) | [ex260](../src/devices/lcec_ex260.c) | 0x00000114:0x01000003 | Valve Controller | Merged 2023-12-31, untested | by @satiowadahc
[SMC EX260-SEC4](https://www.smcpneumatics.com/EX260-SEC4.html) | [ex260](../src/devices/lcec_ex260.c) | 0x00000114:0x01000004 | Valve Controller | Merged 2023-12-31, untested | by @satiowadahc
[Omron R88D-KN01H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000005 | Servo Drive |  | 
[Omron R88D-KN01L-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000002 | Servo Drive |  | 
[Omron R88D-KN02H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000006 | Servo Drive |  | 
[Omron R88D-KN02L-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000003 | Servo Drive |  | 
[Omron R88D-KN04H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000007 | Servo Drive |  | 
[Omron R88D-KN04L-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000004 | Servo Drive |  | 
[Omron R88D-KN06F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x0000000b | Servo Drive |  | 
[Omron R88D-KN08H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000008 | Servo Drive |  | 
[Omron R88D-KN10F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x0000000c | Servo Drive |  | 
[Omron R88D-KN10H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000009 | Servo Drive |  | 
[Omron R88D-KN150F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x0000005f | Servo Drive |  | 
[Omron R88D-KN150H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x0000005a | Servo Drive |  | 
[Omron R88D-KN15F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x0000000d | Servo Drive |  | 
[Omron R88D-KN15H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x0000000a | Servo Drive |  | 
[Omron R88D-KN20F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x0000005b | Servo Drive |  | 
[Omron R88D-KN20H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000056 | Servo Drive |  | 
[Omron R88D-KN30F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x0000005c | Servo Drive |  | 
[Omron R88D-KN30H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000057 | Servo Drive |  | 
[Omron R88D-KN50F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x0000005d | Servo Drive |  | 
[Omron R88D-KN50H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000058 | Servo Drive |  | 
[Omron R88D-KN75F-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x0000005e | Servo Drive |  | 
[Omron R88D-KN75H-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000059 | Servo Drive |  | 
[Omron R88D-KNA5L-ECT G5 Series ServoDrive/Motor](http://www.ia.omron.com/) | [omrg5](../src/devices/lcec_omrg5.c) | 0x00000083:0x00000001 | Servo Drive |  | 
[Stoeber Posidrive MDS 5000](https://www.ethercat.org/en/products/CE4C65D2ED8B4F39A8166D6CEDB2C872.htm) | [stmds5k](../src/devices/lcec_stmds5k.c) | 0x000000b9:0x00001388 | Servo Drive |  | 

There are an additional 1 device(s) supported that do not have enough
documentation to display here.  Please look at the `documentation/devices/` files
and update them if you're able.
