# Devices Supported by LinuxCNC-Ethercat

*This is a work in progress, listing all of the devices that LinuxCNC-Ethercat
has code to support today.  Not all of these are well-tested.*

Description | Name in Source Code | EtherCAT VID:PID | Device Type | Channels | Notes
----------- | ------------------- | ---------------- | ----------- | -------: | ------
[Beckhoff AX5101 Digital Compact Servo Drive, 1-channel](https://www.beckhoff.com/en-us/products/motion/servo-drives/ax5000-digital-compact-servo-drives/ax5101.html) | AX5101 | 0x00000002:0x13ed6012 | Servo Controller | 1 | 1.5A current limit
[Beckhoff AX5103 Digital Compact Servo Drive, 1-channel](https://www.beckhoff.com/en-us/products/motion/servo-drives/ax5000-digital-compact-servo-drives/ax5103.html) | AX5103 | 0x00000002:0x13ef6012 | Servo Controller | 1 | 3A current limit
[Beckhoff AX5106 Digital Compact Servo Drive, 1-channel](https://www.beckhoff.com/en-us/products/motion/servo-drives/ax5000-digital-compact-servo-drives/ax5106.html) | AX5106 | 0x00000002:0x13f26012 | Servo Controller | 1 | 6A current limit
[Beckhoff AX5112 Digital Compact Servo Drive, 1-channel](https://www.beckhoff.com/en-us/products/motion/servo-drives/ax5000-digital-compact-servo-drives/ax5112.html) | AX5112 | 0x00000002:0x13f86012 | Servo Controller | 1 | 12A current limit
[Beckhoff AX5118 Digital Compact Servo Drives, 1-channel](https://www.beckhoff.com/en-us/products/motion/servo-drives/ax5000-digital-compact-servo-drives/ax5118.html) | AX5118 | 0x00000002:0x13fe6012 | Servo Controller | 1 | 18A current limit
[Beckhoff AX5203 Digial Compact Servo Drives, 2-channel](https://www.beckhoff.com/en-us/products/motion/servo-drives/ax5000-digital-compact-servo-drives/ax5203.html) | AX5203 | 0x00000002:0x14536012 | Servo Controller | 2 | 3A CURRENT LIMIT
[Beckhoff AX5206 Digital Compact Servo Drive, 2-channel](https://www.beckhoff.com/en-us/products/motion/servo-drives/ax5000-digital-compact-servo-drives/ax5206.html) | AX5206 | 0x00000002:0x14566012 | Servo Controller | 2 | 6A current limit
[Beckhoff AX5805 TwinSAFE drive option card](https://www.beckhoff.com/en-us/products/motion/servo-drives/ax5000-digital-compact-servo-drives/ax5805.html) | AX5805 | 0x00000002:0x16AD6012 | Servo Accessory |  | 
[Delta ASDA-A2-E](https://www.deltaww.com/en-us/products/Servo-Systems-AC-Servo-Motors-and-Drives/23) | DeASDA | 0x000001dd:0x10305070 | Servo Controller | 1 | Includes 2 digital inputs
[Delta MS300 AC Motor Controller](https://www.deltaww.com/en-us/products/AC-Motor-Drives/3449) | DeMS300 | 0x000001dd:0x10400200 | AC Controller |  | 
[Beckhoff EK1100 EtherCAT Coupler](https://www.beckhoff.com/en-us/products/i-o/ethercat-terminals/ek1xxx-bk1xx0-ethercat-coupler/ek1100.html) | EK1100 | 0x00000002:0x044C2C52 | Coupler |  | Connects Beckhoff EL* modules to 100base-TX networks, and injects DC power into the EL bus.
[Beckhoff EK1101 EtherCAT Coupler with ID switch](https://www.beckhoff.com/en-us/products/i-o/ethercat-terminals/ek1xxx-bk1xx0-ethercat-coupler/ek1101.html) | EK1101 | 0x00000002:0x044D2C52 | Coupler |  | Connects Beckhoff EL* modules to 100base-TX networks, and injects DC power into the EL bus.
[Beckhoff EK1101 EtherCAT extension](https://www.beckhoff.com/en-us/products/i-o/ethercat-terminals/ek1xxx-bk1xx0-ethercat-coupler/ek1110.html) | EK1110 | 0x00000002:0x04562C52 | Coupler |  | Connects Beckhoff EL* modules to 100base-TX networks
[Beckhoff EK1122 2-port EtherCAT junction](https://www.beckhoff.com/en-us/products/i-o/ethercat-terminals/ek1xxx-bk1xx0-ethercat-coupler/ek1122.html) | EK1122 | 0x00000002:0x04622C52 | Coupler |  | Adds 2 RJ45 Ethernet ports in-line, allowing star topologies.
[Beckhoff EL1002 2-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1002 | 0x00000002:0x03EA3052 | Digital Input | 2 | 24 VDC, 3 ms input filter
[Beckhoff EL1004 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1004 | 0x00000002:0x03EC3052 | Digital Input | 4 | 24 VDC, 3 ms input filter
[Beckhoff EL1008 8-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1008 | 0x00000002:0x03F03052 | Digital Input | 8 | 24 VDC, 3 ms input filter
[Beckhoff EL1012 2-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1012 | 0x00000002:0x03F43052 | Digital Input | 2 | 24 VDC, 10 µs input filter
[Beckhoff EL1014 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1014 | 0x00000002:0x03F63052 | Digital Input | 4 | 24 VDC, 10 µs input filter
[Beckhoff EL1018 8-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1018 | 0x00000002:0x03FA3052 | Digital Input | 8 | 24 VDC, 10 µs input filter
[Beckhoff EL1024 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1024 | 0x00000002:0x04003052 | Digital Input | 4 | 24 VDC, for type 2 sensors
[Beckhoff EL1034 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1034 | 0x00000002:0x040A3052 | Digital Input | 4 | 24 VDC, potential-free inputs
[Beckhoff EL1084 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1084 | 0x00000002:0x043C3052 | Digital Input | 4 | 24 VDC, switching to negative potential, 3 ms
[Beckhoff EL1088 8-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1088 | 0x00000002:0x04403052 | Digital Input | 8 | 24 VDC, switching to negative potential, 3 ms
[Beckhoff EL1094 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1094 | 0x00000002:0x04463052 | Digital Input | 4 | 24 VDC, switching to negative potential, 10 µs
[Beckhoff EL1098 8-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1098 | 0x00000002:0x044A3052 | Digital Input | 8 | 24 VDC, switching to negative potential, 10 µs
[Beckhoff EL1104 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623216395.html&id=) | EL1104 | 0x00000002:0x04503052 | Digital Input | 4 | 24 VDC with sensor supply, 3 ms
[Beckhoff EL1114 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623216395.html&id=) | EL1114 | 0x00000002:0x045A3052 | Digital Input | 4 | 24 VDC with sensor supply, 10 µs
[Beckhoff EL1124 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1124 | 0x00000002:0x04643052 | Digital Input | 4 | 5 VDC
[Beckhoff EL1134 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1134 | 0x00000002:0x046E3052 | Digital Input | 4 | 12 VDC
[Beckhoff EL1144 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1144 | 0x00000002:0x04783052 | Digital Input | 4 | 48 VDC
[Beckhoff EL1252 2-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1252 | 0x00000002:0x04E43052 | Digital Input | 2 | 24 VDC, with timestamp
[Beckhoff EL1804 4-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1804 | 0x00000002:0x070C3052 | Digital Input | 4 | 24 VDC, high density, 3 ms
[Beckhoff EL1808 8-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623205643.html&id=) | EL1808 | 0x00000002:0x07103052 | Digital Input | 8 | 24 VDC, high density, 3 ms
[Beckhoff EL1809 16-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623216395.html&id=) | EL1809 | 0x00000002:0x07113052 | Digital Input | 16 | 24 VDC, high density, 3 ms
[Beckhoff EL1819 16-port digital input](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623216395.html&id=) | EL1819 | 0x00000002:0x071B3052 | Digital Input | 16 | 24 VDC, high density, 10 µs
[Beckhoff EL1859 8-port 1-wire digital interface](https://infosys.beckhoff.com/english.php?content=../content/1033/el10xx_el11xx/1623216395.html&id=) | EL1859 | 0x00000002:0x07433052 | Digital Input | 8 | 24 VDC, 1-wire, 3 ms
[Beckhoff EL1904 4-port digital input w/ TwinSAFE](https://www.beckhoff.com/en-us/products/automation/twinsafe/twinsafe-hardware/el1904.html) | EL1904 | 0x00000002:0x07703052 | Digital Input | 4 | 24 VDC, with TwinSAFE safety hardware
[Beckhoff EL1918 8-port digital input w/ TwinSAFE Logic](https://www.beckhoff.com/en-us/products/automation/twinsafe/twinsafe-hardware/el1918.html) | EL1918_LOGIC | 0x00000002:0x077e3052 | Digital Input | 8 | 24 VDC, with TwinSAFE safety hardware and TwinSAFE logic
[Modusoft 3lm2rm](https://www.modusoft.de/linux-cnc/) | Ph3LM2RM | 0x00000907:0x10000001 | Unknown |  | 
[Stober POSIDRIVE MDS 5000 Servo Inverter](https://www.stoeber.de/en/solutions/inverter/) | StMDS5k | 0x000000b9:0x00001388 | Servo Controller |  | Discontinued, finding Ethercat parameter documentation is difficult.

There are an additional 113 devices supported that do not have enough
documentation to display here.  Please look at the `documentation/devices/` files
and update them if you're able.
