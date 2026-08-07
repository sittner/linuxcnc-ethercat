# linuxcnc-ethercat

LinuxCNC EtherCAT HAL driver

---

## 📦 This Repository Is Archived

This repository is **archived and no longer actively developed** by its author. It remains available in read-only form for historical reference. This is the logical next step of this repo's long-standing position — the author's *personal* repository, never intended to compete with or replace the official community fork. Its archival **does not affect** the community fork or the author's ongoing work in any way.

### 👉 LinuxCNC users: use the community fork

For LinuxCNC users seeking **stability**, broader device support, and active community maintenance, the actively maintained fork lives here and is **not affected by this archive in any way**:

> **https://github.com/linuxcnc-ethercat/linuxcnc-ethercat**

This is the recommended home for production LinuxCNC EtherCAT users going forward.

### 👉 Original author's continued work

The original author (Sascha Ittner) now develops **StratuMAK**, a toolkit for building machine controls. The EtherCAT stack from this repository lives on there — the LinuxCNC-EtherCAT **`lcec` XML configuration format is deliberately carried forward** — re-homed into a single-process Go/C architecture with the IgH master built in:

> **https://github.com/stratuMAK/stratumak**

Note that StratuMAK is at **lab-prototype stage** and is *not yet suitable for unattended production use* — if you need a stable, production-ready EtherCAT driver today, use the community fork above. If you are following the author's development, head to StratuMAK.

### 🙏 Thanks

Sincere thanks to the LinuxCNC community and to everyone who contributed to, tested, and relied on this driver over the years.

---

## Description

`linuxcnc-ethercat` provides a LinuxCNC HAL (Hardware Abstraction Layer) driver for EtherCAT fieldbus devices. It enables real-time communication with EtherCAT slaves — including I/O terminals, servo drives, encoders, VFDs, and more — directly from within LinuxCNC.

The project consists of two main components:

- **`lcec`** — the realtime HAL component that communicates with the EtherCAT master and exposes device I/O as HAL pins
- **`lcec_conf`** — the configuration tool that reads an XML bus description and sets up the EtherCAT master, slaves, PDO mappings, DC sync settings, and CoE/SoE init commands

---

## Supported Devices

| Category | Devices |
|---|---|
| **Beckhoff EtherCAT Couplers** | EK1100 |
| **Beckhoff Digital Input** | EL1xxx (EL1002–EL1809), EL1252, EL1859 |
| **Beckhoff Digital Output** | EL2xxx (EL2002–EL2809), EL2202, EL2521 |
| **Beckhoff Analog Input** | EL31x2, EL31x4, EL3255, EL32x4, EL3314, EM3712 |
| **Beckhoff Analog Output** | EL40x1, EL40x2, EL40x8, EL41x2, EL41x4 |
| **Beckhoff Encoder Terminals** | EL5002, EL5021, EL5032, EL5101, EL5122, EL5151, EL5152 |
| **Beckhoff Motor Terminals** | EL7041-1000, EL70x1, EL7211, EL7342, EL7411, EM7004 |
| **Beckhoff Safety (TwinSAFE)** | EL1904, EL1918, EL2904, EL6900 |
| **Beckhoff Power Supply** | EL95xx (EL9505–EL9515) |
| **Beckhoff Servo Drives** | AX5100, AX5200, AX5805 |
| **Third-Party Servo Drives** | Stoeber MDS5000 (`stmds5k`), Delta ASDA (`deasda`), Omron G5 (`omrg5`) |
| **Third-Party VFDs** | Delta MS300 (`dems300`) |
| **Third-Party Measuring System** | modusoft Measurement Card for Phillips 3LM/2RM (`ph3lm2rm`) |
| **Generic** | Passthrough driver for arbitrary PDO mapping |

---

## Prerequisites

- **LinuxCNC** with HAL development headers (`halcompile` / `comp`)
- **IgH EtherCAT Master** — for userspace operation, the **uspace branch** from https://github.com/sittner/ethercat/ is required (this is an unofficial branch; the official IgH repository does not support userspace operation)
- **Build essentials** — gcc, make, etc.
- **libxml2** development headers (required for `lcec_conf`)

---

## Building & Installation

```bash
make
sudo make install
```

To inspect build system configuration:

```bash
make configure
```

---

## Configuration

EtherCAT bus configuration is done via XML files that are processed by `lcec_conf` at runtime. The XML config describes:

- EtherCAT masters and their network interfaces
- Slave devices and their vendor/product IDs
- PDO mappings (process data object assignments)
- DC sync settings (distributed clocks)
- Init commands (CoE SDO / SoE IDN writes executed at startup)

See the [`examples/`](examples/) directory for sample configurations.

---

## EtherCAT Master Support

| Mode | Status |
|---|---|
| **Userspace (uspace)** | Primary focus — actively used and tested. Requires the uspace branch of IgH EtherCAT Master from https://github.com/sittner/ethercat/ |
| **Kernel module (kmod)** | Intended to be supported, but **not really tested** |
| **non RT_PREEMPT** | Intended to be supported, but **not really tested** |

> **Note:** Kernel-based EtherCAT master support is included in principle but receives little to no testing. The primary development and testing target is userspace (uspace) operation.

---

## License

This project is licensed under the **GNU General Public License v2.0**. See the [LICENSE](LICENSE) file for details.

---

## Credits & Acknowledgments

- **[LinuxCNC Community](https://linuxcnc.org/)** — for the excellent open-source CNC control software and ecosystem
- **[IgH (Ingenieurgemeinschaft IgH)](https://etherlab.org/)** — for their excellent EtherCAT Master implementation
- **[Beckhoff Automation](https://www.beckhoff.com/)** — for inventing the EtherCAT technology
