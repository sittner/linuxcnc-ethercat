//
//    Copyright (C) 2015 Claudio Lorini <claudio.lorini@iit.it>
//    Copyright (C) 2014 Sascha Ittner <sascha.ittner@modusoft.de>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

/**   
  \brief Linuxcnc and Machinekit HAL driver for IIT ECMS4, 2-channel EtherCat 
  Motion Supervisor with STM32F407.
  \details The ECMS4 board is used in conjunction with a 2FOC board to provide 
  a dual axis vector motor controller with Ethercat interface, inter-board 
  communication is performed by 2 CAN bus lines in a point to point configuration.
  CAN bus speed is @1Mbit in order to provide the highest bandwidth.
  Available control modes are CST and CSV (Cyclic Synchronous Torque and Velocity)

  HAL pins of this component are:
\code
    lcec_ecms4 component
    +---------------------------+
    |                           |
 ---|> EStop      Feedback[32] >|---
 ---|> DriveEn      Status[32] >|---
 ---|> SetPoint[32]  Error[32] >|---
    |                           |
    +---------------------------+

\details
  HAL Pins:
    BIT IN EStop
      When activated Disable the driver controller leaving the rotor free to spin
      (or in emergency brake depending on 2FOC configuration)

    BIT IN DriveEn
      When activated start control loop (Torque or speed). First activation after 
      power-on perform Initial Rotor Alignment for incremental encoders.

    S32 IN SetPoint
      Torque or Speed set point.

HAL Parameters:
    S32 OUT Feedback 
      Feedback from 2FOC driver, current position in CSV, current speed in CST.

    S32 OUT Status
      Status word of the 2FOC driver.
 
    S32 OUT Error
      Error flags of the 2FOC driver.

  Slave configuration seen by IgH master:
\code

> ethercat slaves -v
=== Master 0, Slave 0 ===
Alias: 1
Device: Main
State: PREOP
Flag: +
Identity:
  Vendor Id:       0x00000664
  Product code:    0x00001421
  Revision number: 0x00000001
  Serial number:   0x00000000
DL information:
  FMMU bit operation: no
  Distributed clocks: yes, 64 bit
  DC system time transmission delay: 0 ns
Port  Type  Link  Loop    Signal  NextSlave  RxTime [ns]  Diff [ns]   NextDc [ns]
   0  MII   up    open    yes             -   1790329218           0           0
   1  MII   down  closed  no              -            -           -           -
   2  N/A   down  closed  no              -            -           -           -
   3  N/A   down  closed  no              -            -           -           -
Mailboxes:
  Bootstrap RX: 0x0000/0, TX: 0x0000/0
  Standard  RX: 0x1000/128, TX: 0x1400/128
  Supported protocols: CoE
General:
  Group: Drive
  Image name: 
  Order number: ECMS4
  Device name: ECMS4
  CoE details:
    Enable SDO: yes
    Enable SDO Info: yes
    Enable PDO Assign: yes
    Enable PDO Configuration: no
    Enable Upload at startup: no
    Enable SDO complete access: yes
  Flags:
    Enable SafeOp: no
    Enable notLRW: no
  Current consumption: 0 mA

\details

  PDOs seen by IgH master:
\code

> ethercat pdos -v
SM0: PhysAddr 0x1000, DefaultSize  128, ControlRegister 0x26, Enable 1
SM1: PhysAddr 0x1400, DefaultSize  128, ControlRegister 0x22, Enable 1
SM2: PhysAddr 0x1800, DefaultSize    0, ControlRegister 0x64, Enable 1
  RxPDO 0x1603 "cst RxPDO "
    PDO entry 0x6040:00, 16 bit, "SubIndex 000"
    PDO entry 0x6071:00, 16 bit, "SubIndex 000"
    PDO entry 0x60b2:00, 16 bit, "SubIndex 000"
SM3: PhysAddr 0x1c00, DefaultSize    0, ControlRegister 0x20, Enable 1
  TxPDO 0x1a03 "cst TxPDO "
    PDO entry 0x6041:00, 16 bit, "SubIndex 000"
    PDO entry 0x6064:00, 32 bit, "SubIndex 000"
    PDO entry 0x606c:00, 32 bit, "SubIndex 000"
    PDO entry 0x6077:00, 16 bit, "SubIndex 000"

\details

  SDOs seen by IgH master:
\code

> ethercat sdos -v
SDO 0x1000, "Device type"
  0x1000:00, r-r-r-, uint32, 32 bit, "SubIndex 000"
SDO 0x1001, "Error register"
  0x1001:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
SDO 0x1008, "Device name"
  0x1008:00, r-r-r-, string, 40 bit, "SubIndex 000"
SDO 0x1009, "Hardware version"
  0x1009:00, r-r-r-, string, 16 bit, "SubIndex 000"
SDO 0x100a, "Software version"
  0x100a:00, r-r-r-, string, 32 bit, "SubIndex 000"
SDO 0x1018, "Identity"
  0x1018:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1018:01, r-r-r-, uint32, 32 bit, "Vendor ID"
  0x1018:02, r-r-r-, uint32, 32 bit, "Product code"
  0x1018:03, r-r-r-, uint32, 32 bit, "Revision"
  0x1018:04, r-r-r-, uint32, 32 bit, "Serial number"
SDO 0x10f1, "Error Settings"
  0x10f1:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x10f1:01, rwrwrw, uint32, 32 bit, "Local Error Reaction"
  0x10f1:02, rwrwrw, uint32, 32 bit, "Sync Error Counter Limit"
SDO 0x1600, "csp/csv RxPDO"
  0x1600:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1600:01, r-r-r-, uint32, 32 bit, "SubIndex 001"
  0x1600:02, r-r-r-, uint32, 32 bit, "SubIndex 002"
  0x1600:03, r-r-r-, uint32, 32 bit, "SubIndex 003"
  0x1600:04, r-r-r-, uint32, 32 bit, "SubIndex 004"
SDO 0x1601, "csp RxPDO"
  0x1601:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1601:01, r-r-r-, uint32, 32 bit, "SubIndex 001"
  0x1601:02, r-r-r-, uint32, 32 bit, "SubIndex 002"
SDO 0x1602, "csv RxPDO"
  0x1602:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1602:01, r-r-r-, uint32, 32 bit, "SubIndex 001"
  0x1602:02, r-r-r-, uint32, 32 bit, "SubIndex 002"
SDO 0x1603, "cst RxPDO "
  0x1603:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1603:01, r-r-r-, uint32, 32 bit, "Control Word "
  0x1603:02, r-r-r-, uint32, 32 bit, "Target Torque "
  0x1603:03, r-r-r-, uint32, 32 bit, "Torque offset "
SDO 0x1a00, "csp/csv TxPDO"
  0x1a00:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1a00:01, r-r-r-, uint32, 32 bit, "SubIndex 001"
  0x1a00:02, r-r-r-, uint32, 32 bit, "SubIndex 002"
  0x1a00:03, r-r-r-, uint32, 32 bit, "SubIndex 003"
  0x1a00:04, r-r-r-, uint32, 32 bit, "SubIndex 004"
SDO 0x1a01, "csp TxPDO"
  0x1a01:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1a01:01, r-r-r-, uint32, 32 bit, "SubIndex 001"
  0x1a01:02, r-r-r-, uint32, 32 bit, "SubIndex 002"
SDO 0x1a02, "csv TxPDO"
  0x1a02:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1a02:01, r-r-r-, uint32, 32 bit, "SubIndex 001"
  0x1a02:02, r-r-r-, uint32, 32 bit, "SubIndex 002"
SDO 0x1a03, "cst TxPDO "
  0x1a03:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1a03:01, r-r-r-, uint32, 32 bit, "Status Word "
  0x1a03:02, r-r-r-, uint32, 32 bit, "Actual Position "
  0x1a03:03, r-r-r-, uint32, 32 bit, "Actual velocity "
  0x1a03:04, r-r-r-, uint32, 32 bit, "Actual Torque "
SDO 0x1c00, "Sync manager type"
  0x1c00:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1c00:01, r-r-r-, uint8, 8 bit, "SubIndex 001"
  0x1c00:02, r-r-r-, uint8, 8 bit, "SubIndex 002"
  0x1c00:03, r-r-r-, uint8, 8 bit, "SubIndex 003"
  0x1c00:04, r-r-r-, uint8, 8 bit, "SubIndex 004"
SDO 0x1c12, "RxPDO assign"
  0x1c12:00, rwr-r-, uint8, 8 bit, "SubIndex 000"
  0x1c12:01, rwr-r-, uint16, 16 bit, "SubIndex 001"
  0x1c12:02, rwr-r-, uint16, 16 bit, "SubIndex 002"
SDO 0x1c13, "TxPDO assign"
  0x1c13:00, rwr-r-, uint8, 8 bit, "SubIndex 000"
  0x1c13:01, rwr-r-, uint16, 16 bit, "SubIndex 001"
  0x1c13:02, rwr-r-, uint16, 16 bit, "SubIndex 002"
SDO 0x1c32, "SM output parameter"
  0x1c32:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1c32:01, rwr-r-, uint16, 16 bit, "Synchronization Type"
  0x1c32:02, r-r-r-, uint32, 32 bit, "Cycle Time"
  0x1c32:03, ------, type 0000, 32 bit, "SubIndex 003"
  0x1c32:04, r-r-r-, uint16, 16 bit, "Synchronization Types supported"
  0x1c32:05, r-r-r-, uint32, 32 bit, "Minimum Cycle Time"
  0x1c32:06, r-r-r-, uint32, 32 bit, "Calc and Copy Time"
  0x1c32:07, ------, type 0000, 32 bit, "SubIndex 007"
  0x1c32:08, rwrwrw, uint16, 16 bit, "Get Cycle Time"
  0x1c32:09, r-r-r-, uint32, 32 bit, "Delay Time"
  0x1c32:0a, rwrwrw, uint32, 32 bit, "Sync0 Cycle Time"
  0x1c32:0b, r-r-r-, uint16, 16 bit, "SM-Event Missed"
  0x1c32:0c, r-r-r-, uint16, 16 bit, "Cycle Time Too Small"
  0x1c32:0d, ------, type 0000, 16 bit, "Shift Time Too Short"
  0x1c32:0e, ------, type 0000, 16 bit, "SubIndex 014"
  0x1c32:0f, ------, type 0000, 32 bit, "SubIndex 015"
  0x1c32:10, ------, type 0000, 32 bit, "SubIndex 016"
  0x1c32:11, ------, type 0000, 32 bit, "SubIndex 017"
  0x1c32:12, ------, type 0000, 32 bit, "SubIndex 018"
  0x1c32:13, ------, type 0000, 0 bit, "SubIndex 019"
  0x1c32:14, ------, type 0000, 0 bit, "SubIndex 020"
  0x1c32:15, ------, type 0000, 0 bit, "SubIndex 021"
  0x1c32:16, ------, type 0000, 0 bit, "SubIndex 022"
  0x1c32:17, ------, type 0000, 0 bit, "SubIndex 023"
  0x1c32:18, ------, type 0000, 0 bit, "SubIndex 024"
  0x1c32:19, ------, type 0000, 0 bit, "SubIndex 025"
  0x1c32:1a, ------, type 0000, 0 bit, "SubIndex 026"
  0x1c32:1b, ------, type 0000, 0 bit, "SubIndex 027"
  0x1c32:1c, ------, type 0000, 0 bit, "SubIndex 028"
  0x1c32:1d, ------, type 0000, 0 bit, "SubIndex 029"
  0x1c32:1e, ------, type 0000, 0 bit, "SubIndex 030"
  0x1c32:1f, ------, type 0000, 0 bit, "SubIndex 031"
  0x1c32:20, r-r-r-, bool, 1 bit, "Sync Error"
SDO 0x1c33, "SM input parameter"
  0x1c33:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x1c33:01, rwr-r-, uint16, 16 bit, "Synchronization Type"
  0x1c33:02, r-r-r-, uint32, 32 bit, "Cycle Time"
  0x1c33:03, ------, type 0000, 32 bit, "SubIndex 003"
  0x1c33:04, r-r-r-, uint16, 16 bit, "Synchronization Types supported"
  0x1c33:05, r-r-r-, uint32, 32 bit, "Minimum Cycle Time"
  0x1c33:06, r-r-r-, uint32, 32 bit, "Calc and Copy Time"
  0x1c33:07, ------, type 0000, 32 bit, "SubIndex 007"
  0x1c33:08, rwrwrw, uint16, 16 bit, "Get Cycle Time"
  0x1c33:09, r-r-r-, uint32, 32 bit, "Delay Time"
  0x1c33:0a, rwrwrw, uint32, 32 bit, "Sync0 Cycle Time"
  0x1c33:0b, r-r-r-, uint16, 16 bit, "SM-Event Missed"
  0x1c33:0c, r-r-r-, uint16, 16 bit, "Cycle Time Too Small"
  0x1c33:0d, ------, type 0000, 16 bit, "Shift Time Too Short"
  0x1c33:0e, ------, type 0000, 16 bit, "SubIndex 014"
  0x1c33:0f, ------, type 0000, 32 bit, "SubIndex 015"
  0x1c33:10, ------, type 0000, 32 bit, "SubIndex 016"
  0x1c33:11, ------, type 0000, 32 bit, "SubIndex 017"
  0x1c33:12, ------, type 0000, 32 bit, "SubIndex 018"
  0x1c33:13, ------, type 0000, 0 bit, "SubIndex 019"
  0x1c33:14, ------, type 0000, 0 bit, "SubIndex 020"
  0x1c33:15, ------, type 0000, 0 bit, "SubIndex 021"
  0x1c33:16, ------, type 0000, 0 bit, "SubIndex 022"
  0x1c33:17, ------, type 0000, 0 bit, "SubIndex 023"
  0x1c33:18, ------, type 0000, 0 bit, "SubIndex 024"
  0x1c33:19, ------, type 0000, 0 bit, "SubIndex 025"
  0x1c33:1a, ------, type 0000, 0 bit, "SubIndex 026"
  0x1c33:1b, ------, type 0000, 0 bit, "SubIndex 027"
  0x1c33:1c, ------, type 0000, 0 bit, "SubIndex 028"
  0x1c33:1d, ------, type 0000, 0 bit, "SubIndex 029"
  0x1c33:1e, ------, type 0000, 0 bit, "SubIndex 030"
  0x1c33:1f, ------, type 0000, 0 bit, "SubIndex 031"
  0x1c33:20, r-r-r-, bool, 1 bit, "Sync Error"
SDO 0x603f, "Error Code"
  0x603f:00, r-r-r-, uint16, 16 bit, "SubIndex 000"
SDO 0x6040, "Control Word"
  0x6040:00, rwrwrw, uint16, 16 bit, "SubIndex 000"
SDO 0x6041, "Status Word"
  0x6041:00, r-r-r-, uint16, 16 bit, "SubIndex 000"
SDO 0x605a, "Quickstop Option Code"
  0x605a:00, rwrwrw, int16, 16 bit, "SubIndex 000"
SDO 0x605b, "Shutdown Option Code"
  0x605b:00, rwrwrw, int16, 16 bit, "SubIndex 000"
SDO 0x605c, "Disable Operation Option Code"
  0x605c:00, rwrwrw, int16, 16 bit, "SubIndex 000"
SDO 0x605e, "Fault Reaction Code"
  0x605e:00, rwrwrw, int16, 16 bit, "SubIndex 000"
SDO 0x6060, "Modes of Operation"
  0x6060:00, rwrwrw, int8, 8 bit, "SubIndex 000"
SDO 0x6061, "Modes of Operation Display"
  0x6061:00, r-r-r-, int8, 8 bit, "SubIndex 000"
SDO 0x6064, "Position Actual Value"
  0x6064:00, r-r-r-, int32, 32 bit, "SubIndex 000"
SDO 0x606c, "Velocity Actual Value"
  0x606c:00, r-r-r-, int32, 32 bit, "SubIndex 000"
SDO 0x6071, "Target Torque"
  0x6071:00, rwrwrw, int16, 16 bit, "SubIndex 000"
SDO 0x6077, "Torque Actual Value"
  0x6077:00, r-r-r-, int16, 16 bit, "SubIndex 000"
SDO 0x607a, "Target Position"
  0x607a:00, rwrwrw, int32, 32 bit, "SubIndex 000"
SDO 0x607d, "Software Position Limit"
  0x607d:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x607d:01, rwrwrw, int32, 32 bit, "SubIndex 001"
  0x607d:02, rwrwrw, int32, 32 bit, "SubIndex 002"
SDO 0x6085, "Quickstop Declaration"
  0x6085:00, rwrwrw, int32, 32 bit, "SubIndex 000"
SDO 0x60b2, "Torque Offset"
  0x60b2:00, rwrwrw, int16, 16 bit, "SubIndex 000"
SDO 0x60c2, "Interpolation Time Period"
  0x60c2:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0x60c2:01, rwrwrw, uint8, 8 bit, "Interpolation period "
  0x60c2:02, rwrwrw, int8, 8 bit, "Interpolation Index"
SDO 0x60ff, "Target Velocity"
  0x60ff:00, rwrwrw, int32, 32 bit, "SubIndex 000"
SDO 0x6502, "Supported Drive Modes"
  0x6502:00, r-r-r-, uint32, 32 bit, "SubIndex 000"
SDO 0xf000, "Modular device profile"
  0xf000:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0xf000:01, r-r-r-, uint16, 16 bit, "Module index distance"
  0xf000:02, r-r-r-, uint16, 16 bit, "Maximum number of modules"
SDO 0xf010, "Module profile list"
  0xf010:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0xf010:01, r-r-r-, uint32, 32 bit, "SubIndex 001"
  0xf010:02, r-r-r-, uint32, 32 bit, "SubIndex 002"
SDO 0xf030, "Configured module Ident list"
  0xf030:00, rwr-r-, uint8, 8 bit, "SubIndex 000"
  0xf030:01, rwr-r-, uint32, 32 bit, "SubIndex 001"
  0xf030:02, rwr-r-, uint32, 32 bit, "SubIndex 002"
SDO 0xf050, "Module detected list"
  0xf050:00, r-r-r-, uint8, 8 bit, "SubIndex 000"
  0xf050:01, r-r-r-, uint32, 32 bit, "SubIndex 001"
  0xf050:02, r-r-r-, uint32, 32 bit, "SubIndex 002"

\details

*/

#include "lcec.h"
#include "lcec_ecms4.h"

void lcec_ecms4_read(struct lcec_slave *slave, long period);
void lcec_ecms4_write(struct lcec_slave *slave, long period);

/** \todo manage CST/CSV configuration */

// CST RxPDOs
// index, sub-index, size
ec_pdo_entry_info_t lcec_ecms4_rxpdo_entries[] = {
    {0x6040, 0x00, 16}, /* SubIndex 000 */ // Control Word
    {0x6071, 0x00, 16}, /* SubIndex 000 */ // Target torque
    {0x60b2, 0x00, 16}  /* SubIndex 000 */ // Torque offset
};
// CST TxPDOs
ec_pdo_entry_info_t lcec_ecms4_txpdo_entries[] = {
    {0x6041, 0x00, 16}, /* SubIndex 000 */ // Status word
    {0x6064, 0x00, 32}, /* SubIndex 000 */ // Position actual value
    {0x606c, 0x00, 32}, /* SubIndex 000 */ // Velocity actual value
    {0x6077, 0x00, 16}  /* SubIndex 000 */ // Torque   actual value
};

//  Index,  n.PDOs, array of PDO entries
ec_pdo_info_t lcec_ecms4_rxpdos[] = {
    {0x1603, 3, lcec_ecms4_rxpdo_entries} /* cst RxPDO  */
};
ec_pdo_info_t lcec_ecms4_txpdos[] = {
    {0x1a03, 4, lcec_ecms4_txpdo_entries}  /* cst TxPDO  */
};

//  Indx, SM direction, n.PDOs, array of PDOs, WD mode
ec_sync_info_t lcec_ecms4_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL,                EC_WD_DISABLE},
    {1, EC_DIR_INPUT,  0, NULL,                EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 1, lcec_ecms4_rxpdos,   EC_WD_ENABLE},
    {3, EC_DIR_INPUT,  1, lcec_ecms4_txpdos,   EC_WD_DISABLE},
    {0xff}
};

// data structure of a ECMS4 channel
typedef struct {
  // inputs
  hal_bit_t *EStop;
  hal_bit_t *DriveEn;
  hal_s32_t *SetPoint;
  // outputs
  hal_s32_t *Feedback;
  hal_u32_t *Status;
  hal_u32_t *Error;

  // Ethercat control word
  unsigned int ec_control;
  // torque set point and offset
  int ec_target_torque;
  int ec_torque_offset;

  // Ethercat status word
  unsigned int ec_status;
  // 2FOC current process data values 
  int ec_actual_position;
  int ec_actual_velocity;
  int ec_actual_torque;

  // OffSets and BitPositions used to access data in PDOs
  unsigned int EStop_offs;
  unsigned int DriveEn_offs;
  unsigned int SetPoint_offs;
  unsigned int Feedback_offs;
  unsigned int Status_offs;
  unsigned int Error_offs;

  unsigned int ec_control_offs;
  unsigned int ec_target_torque_offs;
  unsigned int ec_torque_offset_offs;

  unsigned int ec_status_offs;
  unsigned int ec_actual_position_offs;
  unsigned int ec_actual_velocity_offs;
  unsigned int ec_actual_torque_offs;
  
} lcec_ecms4_chan_t;

/** \brief complete data structure for ECMS4 */
typedef struct {
  lcec_ecms4_chan_t chans[LCEC_ECMS4_CHANS];
} lcec_ecms4_data_t;

int lcec_ecms4_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_ecms4_data_t *hal_data;
  int err, i;

  // initialize callbacks
  slave->proc_read = lcec_ecms4_read;
  slave->proc_write = lcec_ecms4_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_ecms4_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_ecms4_data_t));
  slave->hal_data = hal_data;

  // set Cyclic Synchronous Velocity mode
  // if (ecrt_slave_config_sdo8(slave->config, 0x6060, 0x00, 0x09) != 0) {
  // set to Cyclic Synchronous Torque mode for channel 1
  if (ecrt_slave_config_sdo8(slave->config, 0x6060, 0x00, 0x0A) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s SDO CSV mode\n", master->name, slave->name);
  }
  // set to Cyclic Synchronous Torque mode for channel 2
  if (ecrt_slave_config_sdo8(slave->config, 0x6860, 0x00, 0x0A) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s SDO CSV mode\n", master->name, slave->name);
  }

  // SDO 0x1C12: assign TxPDO to 0x1602 (ch.1) 0x1612 (ch.2) for CSV
  //                          or 0x1603 (ch.1) 0x1613 (ch.2) for CST
  // set number of TxPDOs
  if (ecrt_slave_config_sdo8(slave->config, 0x1c12, 0x00, 0x02) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s SDO 0x1C12:00\n", master->name, slave->name);
  }
  //if (ecrt_slave_config_sdo16(slave->config, 0x1c12, 0x01, 0x1602) != 0) {
  if (ecrt_slave_config_sdo16(slave->config, 0x1c12, 0x01, 0x1603) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s SDO 0x1C12:01\n", master->name, slave->name);
  }
  //if (ecrt_slave_config_sdo16(slave->config, 0x1c12, 0x02, 0x1612) != 0) {
  if (ecrt_slave_config_sdo16(slave->config, 0x1c12, 0x02, 0x1613) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s SDO 0x1C12:01\n", master->name, slave->name);
  }

  // SDO 0x1C13: assign RxPDO to 0x1A02 (ch.1) 0x1A12 (ch.2) for CSV
  //                          or 0x1A03 (ch.1) 0x1A13 (ch.2) for CST
  // set number of RxPDOs
  if (ecrt_slave_config_sdo8(slave->config, 0x1c13, 0x00, 0x02) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s SDO 0x1C13:00\n", master->name, slave->name);
  }
  //if (ecrt_slave_config_sdo16(slave->config, 0x1c13, 0x01, 0x1A02) != 0) {
  if (ecrt_slave_config_sdo16(slave->config, 0x1c13, 0x01, 0x1A03) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s SDO 0x1C13:01\n", master->name, slave->name);
  } 
  //if (ecrt_slave_config_sdo16(slave->config, 0x1c13, 0x02, 0x1A12) != 0) {
  if (ecrt_slave_config_sdo16(slave->config, 0x1c13, 0x02, 0x1A13) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s SDO 0x1C13:01\n", master->name, slave->name);
  } 

  // initialize sync info
  slave->sync_info = lcec_ecms4_syncs;

  // init PDOs for each channel of ECMS4
  for (i=0; i<LCEC_ECMS4_CHANS ;i++) {
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6040, 0x00, &hal_data->chans[i].ec_control_offs, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6071, 0x00, &hal_data->chans[i].ec_target_torque_offs, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x60b2, 0x00, &hal_data->chans[i].ec_torque_offset_offs, NULL);

    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6041, 0x00, &hal_data->chans[i].ec_status_offs, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6064, 0x00, &hal_data->chans[i].ec_actual_position_offs, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x606c, 0x00, &hal_data->chans[i].ec_actual_velocity_offs, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6077, 0x00, &hal_data->chans[i].ec_actual_torque_offs, NULL);

    // export (component) input pins
    if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->chans[i].EStop), comp_id, "%s.%s.%s.estop.%d", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.estop on channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->chans[i].DriveEn), comp_id, "%s.%s.%s.driveen.%d", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.driveen on channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_s32_newf(HAL_IN, &(hal_data->chans[i].SetPoint), comp_id, "%s.%s.%s.setpoint.%d", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.setpoint in channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    // export (component) output pins
    if ((err = hal_pin_s32_newf(HAL_OUT, &(hal_data->chans[i].Feedback), comp_id, "%s.%s.%s.feedback.%d", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.feedback in channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_u32_newf(HAL_OUT, &(hal_data->chans[i].Status), comp_id, "%s.%s.%s.status.%d", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.status in channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_u32_newf(HAL_OUT, &(hal_data->chans[i].Error), comp_id, "%s.%s.%s.error.%d", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.error in channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }

    // set default pin values
    *(hal_data->chans[i].EStop) = 1;
    *(hal_data->chans[i].DriveEn) = 0;
    *(hal_data->chans[i].SetPoint) = 0;

    *(hal_data->chans[i].Feedback) = 0;
    *(hal_data->chans[i].Status) = 0;
    *(hal_data->chans[i].Error) = 0;
  }

  return 0;
}

void lcec_ecms4_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  uint8_t *pd = master->process_data;
  lcec_ecms4_data_t *hal_data = (lcec_ecms4_data_t *) slave->hal_data;
  int i;

  for (i=0; i<LCEC_ECMS4_CHANS ;i++) {
    hal_data->chans[i].ec_status          = EC_READ_U16(&pd[hal_data->chans[i].ec_status_offs]);
    hal_data->chans[i].ec_actual_position = EC_READ_S32(&pd[hal_data->chans[i].ec_actual_position_offs]);
    hal_data->chans[i].ec_actual_velocity = EC_READ_S32(&pd[hal_data->chans[i].ec_actual_velocity_offs]);
    hal_data->chans[i].ec_actual_torque   = EC_READ_S16(&pd[hal_data->chans[i].ec_actual_torque_offs]);

    // update HAL pins
    *(hal_data->chans[i].Feedback) = hal_data->chans[i].ec_actual_position;
  }
}

void lcec_ecms4_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  uint8_t *pd = master->process_data;
  lcec_ecms4_data_t *hal_data = (lcec_ecms4_data_t *) slave->hal_data;

  int i;

  for (i=0; i<LCEC_ECMS4_CHANS ;i++) {
    // set process data to the HAL pin value
    hal_data->chans[i].ec_target_torque = *(hal_data->chans[i].SetPoint);
    
    EC_WRITE_U16(&pd[hal_data->chans[i].ec_target_torque_offs], hal_data->chans[i].ec_target_torque);
  }
}

