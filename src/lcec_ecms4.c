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
*/

#include "lcec.h"
#include "lcec_ecms4.h"

void lcec_ecms4_read(struct lcec_slave *slave, long period);
void lcec_ecms4_write(struct lcec_slave *slave, long period);

/* Master 0, Slave 8, "ECMS4"
 * Vendor ID:       0x00000664
 * Product code:    0x00001421
 * Revision number: 0x00000001
 */

/** \todo manage CST/CSV configuration */

ec_pdo_entry_info_t lcec_ecms4_pdo_entries[] = {
    // CST RxPDOs
    {0x6040, 0x00, 16}, /* SubIndex 000 */ // Control Word
    {0x6071, 0x00, 16}, /* SubIndex 000 */ // Target torque
    {0x60b2, 0x00, 16}, /* SubIndex 000 */ // Torque offset
    // CST TxPDOs
    {0x6041, 0x00, 16}, /* SubIndex 000 */ // Status word
    {0x6064, 0x00, 32}, /* SubIndex 000 */ // Position actual value
    {0x606c, 0x00, 32}, /* SubIndex 000 */ // Velocity actual value
    {0x6077, 0x00, 16}, /* SubIndex 000 */ // Torque   actual value
};

ec_pdo_info_t lcec_ecms4_pdos[] = {
    {0x1603, 3, lcec_ecms4_pdo_entries + 0}, /* cst RxPDO  */
    {0x1a03, 4, lcec_ecms4_pdo_entries + 3}, /* cst TxPDO  */
};

ec_sync_info_t lcec_ecms4_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 1, lcec_ecms4_pdos + 0, EC_WD_ENABLE},
    {3, EC_DIR_INPUT, 1, lcec_ecms4_pdos + 1, EC_WD_DISABLE},
    {0xff}
};

// data structure of a ECMS4 channel
typedef struct {
  // inputs
  hal_bit_t *EStop;
  hal_bit_t *DriveEn;
  hal_s32_t *SetPoint;
  // outputs
  hal_s32_t *Feedback
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
  int ec_actual_Position;
  int ec_actual_Velocity;
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
  unsigned int ec_actual_Position_offs;
  unsigned int ec_actual_Velocity_offs;
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


  // set to cyclic synchronous velocity mode
  //if (ecrt_slave_config_sdo8(slave->config, 0x6060, 0x00, 9) != 0) {
  //  rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s sdo velo mode\n", master->name, slave->name);
  //}

  // initialize sync info
  slave->sync_info = lcec_ecms4_syncs;

  // init PDOs for each channel of ECMS4
  for (i=0; i<LCEC_ECMS4_CHANS ;i++) {
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6040, 0x00, hal_data->chans[i].ec_control_offs, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6071, 0x00, hal_data->chans[i].ec_target_torque_offs, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x60b2, 0x00, hal_data->chans[i].ec_torque_offset_offs, NULL);

    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6041, 0x00, hal_data->chans[i].ec_status_offs, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6064, 0x00, hal_data->chans[i].ec_actual_Position_offs, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x606c, 0x00, hal_data->chans[i].ec_actual_Velocity_offs, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6077, 0x00, hal_data->chans[i].ec_actual_torque_offs, NULL);

    // export (component) input pins
    if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->chans[i].EStop), comp_id, "%s.%s.%s.estop", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.estop on channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->chans[i].DriveEn), comp_id, "%s.%s.%s.driveen", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.driveen on channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_s32_newf(HAL_in, &(hal_data->chans[i].SetPoint), comp_id, "%s.%s.%s.setpoint", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.setpoint in channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    // export (component) output pins
    if ((err = hal_pin_s32_newf(HAL_OUT, &(hal_data->chans[i].Feedback), comp_id, "%s.%s.%s.feedback", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.feedback in channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_u32_newf(HAL_OUT, &(hal_data->chans[i].Status), comp_id, "%s.%s.%s.status", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.status in channel %d failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_u32_newf(HAL_OUT, &(hal_data->chans[i].Error), comp_id, "%s.%s.%s.error", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
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

void lcec_deasda_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_deasda_data_t *hal_data = (lcec_deasda_data_t *) slave->hal_data;
}

void lcec_deasda_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_deasda_data_t *hal_data = (lcec_deasda_data_t *) slave->hal_data;

}

