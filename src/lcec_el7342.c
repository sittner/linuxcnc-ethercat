//
//    Copyright (C) 2011 Sascha Ittner <sascha.ittner@modusoft.de>
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

#include "lcec.h"
#include "lcec_el7342.h"

#define INFO_SEL_STATUS_WORD   0
#define INFO_SEL_MOTOR_VOLT    1
#define INFO_SEL_MOTOR_CURR    2
#define INFO_SEL_CURR_LIMIT    3
#define INFO_SEL_CTRL_ERR      4
#define INFO_SEL_DUTY_CYCLE    5
#define INFO_SEL_MOTOR_VELO    7
#define INFO_SEL_OVERLOAD_TIME 8
#define INFO_SEL_INT_TEMP      101
#define INFO_SEL_CTRL_VOLT     103
#define INFO_SEL_SUPP_VOLT     104
#define INFO_SEL_DCM_SWORD     150
#define INFO_SEL_DCM_STATE     151

typedef struct {
  hal_bit_t *reset;
  hal_bit_t *ina;
  hal_bit_t *inb;
  hal_bit_t *inext;
  hal_bit_t *sync_err;
  hal_bit_t *expol_stall;
  hal_bit_t *count_overflow;
  hal_bit_t *count_underflow;
  hal_bit_t *tx_toggle;
  hal_bit_t *set_raw_count;
  hal_s32_t *set_raw_count_val;
  hal_bit_t *latch_ext_valid;
  hal_bit_t *ena_latch_ext_pos;
  hal_bit_t *ena_latch_ext_neg;
  hal_s32_t *raw_count;
  hal_s32_t *raw_latch;
  hal_s32_t *count;
  hal_float_t *pos_scale;
  hal_float_t *pos;

  hal_bit_t *dcm_reset;
  hal_bit_t *dcm_reduce_torque;
  hal_bit_t *dcm_enable;
  hal_bit_t *dcm_absmode;
  hal_float_t *dcm_value;
  hal_float_t *dcm_scale;
  hal_float_t *dcm_offset;
  hal_float_t *dcm_min_dc;
  hal_float_t *dcm_max_dc;
  hal_float_t *dcm_curr_dc;
  hal_s32_t *dcm_raw_val;
  hal_bit_t *dcm_ready_to_enable;
  hal_bit_t *dcm_ready;
  hal_bit_t *dcm_warning;
  hal_bit_t *dcm_error;
  hal_bit_t *dcm_move_pos;
  hal_bit_t *dcm_move_neg;
  hal_bit_t *dcm_torque_reduced;
  hal_bit_t *dcm_din1;
  hal_bit_t *dcm_din2;
  hal_bit_t *dcm_sync_err;
  hal_bit_t *dcm_tx_toggle;
  hal_s32_t *dcm_raw_info1;
  hal_s32_t *dcm_raw_info2;
  hal_u32_t *dcm_sel_info1;
  hal_u32_t *dcm_sel_info2;
  hal_float_t *dcm_velo_fb;
  hal_float_t *dcm_current_fb;

  unsigned int set_count_pdo_os;
  unsigned int set_count_pdo_bp;
  unsigned int set_count_val_pdo_os;
  unsigned int set_count_done_pdo_os;
  unsigned int set_count_done_pdo_bp;
  unsigned int expol_stall_pdo_os;
  unsigned int expol_stall_pdo_bp;
  unsigned int ina_pdo_os;
  unsigned int ina_pdo_bp;
  unsigned int inb_pdo_os;
  unsigned int inb_pdo_bp;
  unsigned int inext_pdo_os;
  unsigned int inext_pdo_bp;
  unsigned int sync_err_pdo_os;
  unsigned int sync_err_pdo_bp;
  unsigned int tx_toggle_pdo_os;
  unsigned int tx_toggle_pdo_bp;
  unsigned int count_overflow_pdo_os;
  unsigned int count_overflow_pdo_bp;
  unsigned int count_underflow_pdo_os;
  unsigned int count_underflow_pdo_bp;
  unsigned int latch_ext_valid_pdo_os;
  unsigned int latch_ext_valid_pdo_bp;
  unsigned int ena_latch_ext_pos_pdo_os;
  unsigned int ena_latch_ext_pos_pdo_bp;
  unsigned int ena_latch_ext_neg_pdo_os;
  unsigned int ena_latch_ext_neg_pdo_bp;
  unsigned int count_pdo_os;
  unsigned int latch_pdo_os;

  unsigned int dcm_ena_pdo_os;
  unsigned int dcm_ena_pdo_bp;
  unsigned int dcm_reset_pdo_os;
  unsigned int dcm_reset_pdo_bp;
  unsigned int dcm_reduce_torque_pdo_os;
  unsigned int dcm_reduce_torque_pdo_bp;
  unsigned int dcm_velo_pdo_os;
  unsigned int dcm_ready_to_enable_pdo_os;
  unsigned int dcm_ready_to_enable_pdo_bp;
  unsigned int dcm_ready_pdo_os;
  unsigned int dcm_ready_pdo_bp;
  unsigned int dcm_warning_pdo_os;
  unsigned int dcm_warning_pdo_bp;
  unsigned int dcm_error_pdo_os;
  unsigned int dcm_error_pdo_bp;
  unsigned int dcm_move_pos_pdo_os;
  unsigned int dcm_move_pos_pdo_bp;
  unsigned int dcm_move_neg_pdo_os;
  unsigned int dcm_move_neg_pdo_bp;
  unsigned int dcm_torque_reduced_pdo_os;
  unsigned int dcm_torque_reduced_pdo_bp;
  unsigned int dcm_din1_pdo_os;
  unsigned int dcm_din1_pdo_bp;
  unsigned int dcm_din2_pdo_os;
  unsigned int dcm_din2_pdo_bp;
  unsigned int dcm_sync_err_pdo_os;
  unsigned int dcm_sync_err_pdo_bp;
  unsigned int dcm_tx_toggle_pdo_os;
  unsigned int dcm_tx_toggle_pdo_bp;
  unsigned int dcm_info1_pdo_os;
  unsigned int dcm_info2_pdo_os;

  int enc_do_init;
  int16_t enc_last_count;
  double enc_old_scale;
  double enc_scale_recip;
  double dcm_old_scale;
  double dcm_scale_recip;

  ec_sdo_request_t *sdo_info1_select;
  ec_sdo_request_t *sdo_info2_select;

} lcec_el7342_chan_t;

typedef struct {
  lcec_el7342_chan_t chans[LCEC_EL7342_CHANS];
  int last_operational;
} lcec_el7342_data_t;

static ec_pdo_entry_info_t lcec_el7342_channel1_enc_out[] = {
    {0x0000, 0x00,  1}, // Gap
    {0x7000, 0x02,  1}, // Enable latch extern on positive edge
    {0x7000, 0x03,  1}, // Set counter
    {0x7000, 0x04,  1}, // Enable latch extern on negative edge
    {0x0000, 0x00,  4}, // Gap
    {0x0000, 0x00,  8}, // Gap
    {0x7000, 0x11, 16}  // Set counter value
};

static ec_pdo_entry_info_t lcec_el7342_channel2_enc_out[] = {
    {0x0000, 0x00,  1}, // Gap
    {0x7010, 0x02,  1}, // Enable latch extern on positive edge
    {0x7010, 0x03,  1}, // Set counter
    {0x7010, 0x04,  1}, // Enable latch extern on negative edge
    {0x0000, 0x00,  4}, // Gap
    {0x0000, 0x00,  8}, // Gap
    {0x7010, 0x11, 16}  // Set counter value
};

static ec_pdo_entry_info_t lcec_el7342_channel1_dcm_out[] = {
    {0x7020, 0x01,  1}, // Enable
    {0x7020, 0x02,  1}, // Reset
    {0x7020, 0x03,  1}, // Reduce torque
    {0x0000, 0x00,  5}, // Gap
    {0x0000, 0x00,  8}  // Gap
};

static ec_pdo_entry_info_t lcec_el7342_channel1_vel_out[] = {
    {0x7020, 0x21, 16}, // Velocity
};

static ec_pdo_entry_info_t lcec_el7342_channel2_dcm_out[] = {
    {0x7030, 0x01,  1}, // Enable
    {0x7030, 0x02,  1}, // Reset
    {0x7030, 0x03,  1}, // Reduce torque
    {0x0000, 0x00,  5}, // Gap
    {0x0000, 0x00,  8}  // Gap
};

static ec_pdo_entry_info_t lcec_el7342_channel2_vel_out[] = {
    {0x7030, 0x21, 16}  // Velocity
};

static ec_pdo_entry_info_t lcec_el7342_channel1_enc_in[] = {
    {0x0000, 0x00,  1}, // Gap
    {0x6000, 0x02,  1}, // Latch extern valid
    {0x6000, 0x03,  1}, // Set counter done
    {0x6000, 0x04,  1}, // Counter underflow
    {0x6000, 0x05,  1}, // Counter overflow
    {0x0000, 0x00,  2}, // Gap
    {0x6000, 0x08,  1}, // Extrapolation stall
    {0x6000, 0x09,  1}, // Status of input A
    {0x6000, 0x0a,  1}, // Status of input B
    {0x0000, 0x00,  1}, // Gap
    {0x0000, 0x00,  1}, // Gap
    {0x6000, 0x0d,  1}, // Status of extern latch
    {0x1c32, 0x20,  1}, // Sync error
    {0x0000, 0x00,  1}, // Gap
    {0x1800, 0x09,  1}, // TxPDO Toggle
    {0x6000, 0x11, 16}, // Counter value
    {0x6000, 0x12, 16}  // Latch value
};

static ec_pdo_entry_info_t lcec_el7342_channel2_enc_in[] = {
    {0x0000, 0x00,  1}, // Gap
    {0x6010, 0x02,  1}, // Latch extern valid
    {0x6010, 0x03,  1}, // Set counter done
    {0x6010, 0x04,  1}, // Counter underflow
    {0x6010, 0x05,  1}, // Counter overflow
    {0x0000, 0x00,  2}, // Gap
    {0x6010, 0x08,  1}, // Extrapolation stall
    {0x6010, 0x09,  1}, // Status of input A
    {0x6010, 0x0a,  1}, // Status of input B
    {0x0000, 0x00,  1}, // Gap
    {0x0000, 0x00,  1}, // Gap
    {0x6010, 0x0d,  1}, // Status of extern latch
    {0x1c32, 0x20,  1}, // Sync error
    {0x0000, 0x00,  1}, // Gap
    {0x1803, 0x09,  1}, // TxPDO Toggle
    {0x6010, 0x11, 16}, // Counter value
    {0x6010, 0x12, 16}  // Latch value
};

static ec_pdo_entry_info_t lcec_el7342_channel1_dcm_in[] = {
    {0x6020, 0x01,  1}, // Ready to enable
    {0x6020, 0x02,  1}, // Ready
    {0x6020, 0x03,  1}, // Warning
    {0x6020, 0x04,  1}, // Error
    {0x6020, 0x05,  1}, // Moving positive
    {0x6020, 0x06,  1}, // Moving negative
    {0x6020, 0x07,  1}, // Torque reduced
    {0x0000, 0x00,  1}, // Gap
    {0x0000, 0x00,  3}, // Gap
    {0x6020, 0x0c,  1}, // Digital input 1
    {0x6020, 0x0d,  1}, // Digital input 2
    {0x1c32, 0x20,  1}, // Sync error
    {0x0000, 0x00,  1}, // Gap
    {0x1806, 0x09,  1}  // TxPDO Toggle
};

static ec_pdo_entry_info_t lcec_el7342_channel1_dcm_sync_info[] = {
    {0x6020, 0x11,  16}, // Synchronous information 1
    {0x6020, 0x12,  16}  // Synchronous information 2
};

static ec_pdo_entry_info_t lcec_el7342_channel2_dcm_in[] = {
    {0x6030, 0x01,  1}, // Ready to enable
    {0x6030, 0x02,  1}, // Ready
    {0x6030, 0x03,  1}, // Warning
    {0x6030, 0x04,  1}, // Error
    {0x6030, 0x05,  1}, // Moving positive
    {0x6030, 0x06,  1}, // Moving negative
    {0x6030, 0x07,  1}, // Torque reduced
    {0x0000, 0x00,  1}, // Gap
    {0x0000, 0x00,  3}, // Gap
    {0x6030, 0x0c,  1}, // Digital input 1
    {0x6030, 0x0d,  1}, // Digital input 2
    {0x1c32, 0x20,  1}, // Sync error
    {0x0000, 0x00,  1}, // Gap
    {0x1808, 0x09,  1}  // TxPDO Toggle
};

static ec_pdo_entry_info_t lcec_el7342_channel2_dcm_sync_info[] = {
    {0x6030, 0x11,  16}, // Synchronous information 1
    {0x6030, 0x12,  16}  // Synchronous information 2
};

static ec_pdo_info_t lcec_el7342_pdos_out[] = {
    {0x1600,  7, lcec_el7342_channel1_enc_out},
    {0x1602,  7, lcec_el7342_channel2_enc_out},
    {0x1604,  5, lcec_el7342_channel1_dcm_out},
    {0x1606,  1, lcec_el7342_channel1_vel_out},
    {0x1607,  5, lcec_el7342_channel2_dcm_out},
    {0x1609,  1, lcec_el7342_channel2_vel_out}
};

static ec_pdo_info_t lcec_el7342_pdos_in[] = {
    {0x1a00, 17, lcec_el7342_channel1_enc_in},
    {0x1a03, 17, lcec_el7342_channel2_enc_in},
    {0x1a06, 14, lcec_el7342_channel1_dcm_in},
    {0x1a07,  2, lcec_el7342_channel1_dcm_sync_info},
    {0x1a08, 14, lcec_el7342_channel2_dcm_in},
    {0x1a09,  2, lcec_el7342_channel2_dcm_sync_info},
};

static ec_sync_info_t lcec_el7342_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL},
    {1, EC_DIR_INPUT,  0, NULL},
    {2, EC_DIR_OUTPUT, 6, lcec_el7342_pdos_out},
    {3, EC_DIR_INPUT,  6, lcec_el7342_pdos_in},
    {0xff}
};

void lcec_el7342_read(struct lcec_slave *slave, long period);
void lcec_el7342_write(struct lcec_slave *slave, long period);

void lcec_el7342_set_info(lcec_el7342_chan_t *chan, hal_s32_t *raw_info, hal_u32_t *sel_info);

int lcec_el7342_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el7342_data_t *hal_data;
  int i;
  lcec_el7342_chan_t *chan;
  uint8_t info1_select, info2_select;
  int err;

  // initialize callbacks
  slave->proc_read = lcec_el7342_read;
  slave->proc_write = lcec_el7342_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el7342_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el7342_data_t));
  slave->hal_data = hal_data;

  // initialize sync info
  slave->sync_info = lcec_el7342_syncs;

  // initialize global data
  hal_data->last_operational = 0;

  // initialize pins
  for (i=0; i<LCEC_EL7342_CHANS; i++) {
    chan = &hal_data->chans[i];

    // read sdos
    // Info1 selector
    if ((chan->sdo_info1_select = lcec_read_sdo(slave, 0x8022 + (i << 4), 0x11, 1)) == NULL) {
      return -EIO;
    }
    info1_select = EC_READ_U8(ecrt_sdo_request_data(chan->sdo_info1_select));
    // Info2 selector
    if ((chan->sdo_info2_select = lcec_read_sdo(slave, 0x8022 + (i << 4), 0x19, 1)) == NULL) {
      return -EIO;
    }
    info2_select = EC_READ_U8(ecrt_sdo_request_data(chan->sdo_info2_select));

    // initialize POD entries
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x02, &chan->latch_ext_valid_pdo_os, &chan->latch_ext_valid_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x03, &chan->set_count_done_pdo_os, &chan->set_count_done_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x04, &chan->count_overflow_pdo_os, &chan->count_overflow_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x05, &chan->count_underflow_pdo_os, &chan->count_underflow_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x08, &chan->expol_stall_pdo_os, &chan->expol_stall_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x09, &chan->ina_pdo_os, &chan->ina_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x0a, &chan->inb_pdo_os, &chan->inb_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x0d, &chan->inext_pdo_os, &chan->inext_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x1c32           , 0x20, &chan->sync_err_pdo_os, &chan->sync_err_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x1800 + (i *  3), 0x09, &chan->tx_toggle_pdo_os, &chan->tx_toggle_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x11, &chan->count_pdo_os, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x12, &chan->latch_pdo_os, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000 + (i << 4), 0x02, &chan->ena_latch_ext_pos_pdo_os, &chan->ena_latch_ext_pos_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000 + (i << 4), 0x03, &chan->set_count_pdo_os, &chan->set_count_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000 + (i << 4), 0x04, &chan->ena_latch_ext_neg_pdo_os, &chan->ena_latch_ext_neg_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000 + (i << 4), 0x11, &chan->set_count_val_pdo_os, NULL);

    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x01, &chan->dcm_ready_to_enable_pdo_os, &chan->dcm_ready_to_enable_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x02, &chan->dcm_ready_pdo_os, &chan->dcm_ready_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x03, &chan->dcm_warning_pdo_os, &chan->dcm_warning_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x04, &chan->dcm_error_pdo_os, &chan->dcm_error_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x05, &chan->dcm_move_pos_pdo_os, &chan->dcm_move_pos_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x06, &chan->dcm_move_neg_pdo_os, &chan->dcm_move_neg_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x07, &chan->dcm_torque_reduced_pdo_os, &chan->dcm_torque_reduced_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x0c, &chan->dcm_din1_pdo_os, &chan->dcm_din1_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x0d, &chan->dcm_din2_pdo_os, &chan->dcm_din2_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x1c32           , 0x20, &chan->dcm_sync_err_pdo_os, &chan->dcm_sync_err_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x1806 + (i << 1), 0x09, &chan->dcm_tx_toggle_pdo_os, &chan->dcm_tx_toggle_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x11, &chan->dcm_info1_pdo_os, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6020 + (i << 4), 0x12, &chan->dcm_info2_pdo_os, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7020 + (i << 4), 0x01, &chan->dcm_ena_pdo_os, &chan->dcm_ena_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7020 + (i << 4), 0x02, &chan->dcm_reset_pdo_os, &chan->dcm_reset_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7020 + (i << 4), 0x03, &chan->dcm_reduce_torque_pdo_os, &chan->dcm_reduce_torque_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7020 + (i << 4), 0x21, &chan->dcm_velo_pdo_os, NULL);

    // export encoder pins
    if ((err = hal_pin_bit_newf(HAL_IN, &(chan->reset), comp_id, "%s.%s.%s.enc-%d-reset", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-reset failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->ina), comp_id, "%s.%s.%s.enc-%d-ina", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-ina failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->inb), comp_id, "%s.%s.%s.enc-%d-inb", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-inb failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->inext), comp_id, "%s.%s.%s.enc-%d-inext", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-inext failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->sync_err), comp_id, "%s.%s.%s.enc-%d-sync-error", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-sync-error failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->expol_stall), comp_id, "%s.%s.%s.enc-%d-expol-stall", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-expol-stall failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->tx_toggle), comp_id, "%s.%s.%s.enc-%d-tx-toggle", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-tx-toggle failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->count_overflow), comp_id, "%s.%s.%s.enc-%d-count-overflow", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-count-overflow failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->count_underflow), comp_id, "%s.%s.%s.enc-%d-count-underflow", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-count-underflow failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->latch_ext_valid), comp_id, "%s.%s.%s.enc-%d-latch-ext-valid", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-latch-ext-valid failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IO, &(chan->set_raw_count), comp_id, "%s.%s.%s.enc-%d-set-raw-count", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-set-raw-count failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IO, &(chan->ena_latch_ext_pos), comp_id, "%s.%s.%s.enc-%d-index-ext-pos-enable", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-ndex-ext-pos-enable failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IO, &(chan->ena_latch_ext_neg), comp_id, "%s.%s.%s.enc-%d-index-ext-neg-enable", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-ndex-ext-neg-enable failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_s32_newf(HAL_IN, &(chan->set_raw_count_val), comp_id, "%s.%s.%s.enc-%d-set-raw-count-val", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-set-raw-count-val failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_s32_newf(HAL_OUT, &(chan->raw_count), comp_id, "%s.%s.%s.enc-%d-raw-count", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-raw-count failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_s32_newf(HAL_OUT, &(chan->count), comp_id, "%s.%s.%s.enc-%d-count", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-count failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_s32_newf(HAL_OUT, &(chan->raw_latch), comp_id, "%s.%s.%s.enc-%d-raw-latch", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-raw-latch failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_float_newf(HAL_OUT, &(chan->pos), comp_id, "%s.%s.%s.enc-%d-pos", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-pos failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_float_newf(HAL_IO, &(chan->pos_scale), comp_id, "%s.%s.%s.enc-%d-pos-scale", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-scale failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }

    // export servo pins
    if ((err = hal_pin_float_newf(HAL_IO, &(chan->dcm_scale), comp_id, "%s.%s.%s.srv-%d-scale", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-scale failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_float_newf(HAL_IO, &(chan->dcm_offset), comp_id, "%s.%s.%s.srv-%d-offset", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-offset failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_float_newf(HAL_IO, &(chan->dcm_min_dc), comp_id, "%s.%s.%s.srv-%d-min-dc", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-min-dc failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_float_newf(HAL_IO, &(chan->dcm_max_dc), comp_id, "%s.%s.%s.srv-%d-max-dc", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-max-dc failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_float_newf(HAL_OUT, &(chan->dcm_curr_dc), comp_id, "%s.%s.%s.srv-%d-curr-dc", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-curr-dc failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IN, &(chan->dcm_enable), comp_id, "%s.%s.%s.srv-%d-enable", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-enable failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IN, &(chan->dcm_absmode), comp_id, "%s.%s.%s.srv-%d-absmode", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-absmode failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_float_newf(HAL_IN, &(chan->dcm_value), comp_id, "%s.%s.%s.srv-%d-cmd", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-value failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_s32_newf(HAL_OUT, &(chan->dcm_raw_val), comp_id, "%s.%s.%s.srv-%d-raw-cmd", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-raw-cmd failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IN, &(chan->dcm_reset), comp_id, "%s.%s.%s.srv-%d-reset", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-reset failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IN, &(chan->dcm_reduce_torque), comp_id, "%s.%s.%s.srv-%d-reduce-torque", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-reduce-torque failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_ready_to_enable), comp_id, "%s.%s.%s.srv-%d-ready-to-enable", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-ready-to-enable%d- failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_ready), comp_id, "%s.%s.%s.srv-%d-ready", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-ready%d- failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_warning), comp_id, "%s.%s.%s.srv-%d-warning", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-warning failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_error), comp_id, "%s.%s.%s.srv-%d-error", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-error failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_move_pos), comp_id, "%s.%s.%s.srv-%d-move-pos", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-move-pos failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_move_neg), comp_id, "%s.%s.%s.srv-%d-move-neg", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-move-neg failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_torque_reduced), comp_id, "%s.%s.%s.srv-%d-torque-reduced", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-torque-reduced failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_din1), comp_id, "%s.%s.%s.srv-%d-din1", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-din1 failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_din2), comp_id, "%s.%s.%s.srv-%d-din2", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-din2 failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_sync_err), comp_id, "%s.%s.%s.srv-%d-sync-error", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-sync-error failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->dcm_tx_toggle), comp_id, "%s.%s.%s.srv-%d-tx-toggle", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-tx-toggle failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_s32_newf(HAL_OUT, &(chan->dcm_raw_info1), comp_id, "%s.%s.%s.srv-%d-raw-info1", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-raw-info1 failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_s32_newf(HAL_OUT, &(chan->dcm_raw_info2), comp_id, "%s.%s.%s.srv-%d-raw-info2", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-raw-info2 failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_u32_newf(HAL_OUT, &(chan->dcm_sel_info1), comp_id, "%s.%s.%s.srv-%d-sel-info1", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-sel-info1 failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_u32_newf(HAL_OUT, &(chan->dcm_sel_info2), comp_id, "%s.%s.%s.srv-%d-sel-info2", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-%d-sel-info2 failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if (info1_select == INFO_SEL_MOTOR_VELO || info2_select == INFO_SEL_MOTOR_VELO) {
      if ((err = hal_pin_float_newf(HAL_OUT, &(chan->dcm_velo_fb), comp_id, "%s.%s.%s.srv-%d-velo-fb", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
        rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-velo-fb failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
        return err;
      }
    }
    if (info1_select == INFO_SEL_MOTOR_CURR || info2_select == INFO_SEL_MOTOR_CURR) {
      if ((err = hal_pin_float_newf(HAL_OUT, &(chan->dcm_current_fb), comp_id, "%s.%s.%s.srv-%d-current-fb", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
        rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-current-fb failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
        return err;
      }
    }

    // initialize pins
    *(chan->reset) = 0;
    *(chan->ina) = 0;
    *(chan->inb) = 0;
    *(chan->inext) = 0;
    *(chan->sync_err) = 0;
    *(chan->expol_stall) = 0;
    *(chan->tx_toggle) = 0;
    *(chan->count_overflow) = 0;
    *(chan->count_underflow) = 0;
    *(chan->latch_ext_valid) = 0;
    *(chan->ena_latch_ext_pos) = 0;
    *(chan->ena_latch_ext_neg) = 0;
    *(chan->set_raw_count) = 0;
    *(chan->set_raw_count_val) = 0;
    *(chan->raw_count) = 0;
    *(chan->raw_latch) = 0;
    *(chan->count) = 0;
    *(chan->pos) = 0;
    *(chan->pos_scale) = 1.0;

    *(chan->dcm_scale) = 1.0;
    *(chan->dcm_offset) = 0.0;
    *(chan->dcm_min_dc) = -1.0;
    *(chan->dcm_max_dc) = 1.0;
    *(chan->dcm_curr_dc) = 0.0;
    *(chan->dcm_enable) = 0;
    *(chan->dcm_absmode) = 0;
    *(chan->dcm_value) = 0.0;
    *(chan->dcm_raw_val) = 0;
    *(chan->dcm_reset) = 0;
    *(chan->dcm_reduce_torque) = 0;
    *(chan->dcm_ready_to_enable) = 0;
    *(chan->dcm_ready) = 0;
    *(chan->dcm_warning) = 0;
    *(chan->dcm_error) = 0;
    *(chan->dcm_move_pos) = 0;
    *(chan->dcm_move_neg) = 0;
    *(chan->dcm_torque_reduced) = 0;
    *(chan->dcm_din1) = 0;
    *(chan->dcm_din2) = 0;
    *(chan->dcm_sync_err) = 0;
    *(chan->dcm_tx_toggle) = 0;
    *(chan->dcm_raw_info1) = 0;
    *(chan->dcm_raw_info2) = 0;
    *(chan->dcm_sel_info1) = info1_select;
    *(chan->dcm_sel_info2) = info2_select;
    if (chan->dcm_velo_fb != NULL) {
      *(chan->dcm_velo_fb) = 0.0;
    }
    if (chan->dcm_current_fb != NULL) {
      *(chan->dcm_current_fb) = 0.0;
    }

    // initialize variables
    chan->enc_do_init = 1;
    chan->enc_last_count = 0;
    chan->enc_old_scale = *(chan->pos_scale) + 1.0;
    chan->enc_scale_recip = 1.0;

    chan->dcm_old_scale = *(chan->dcm_scale) + 1.0;
    chan->dcm_scale_recip = 1.0;
  }

  return 0;
}

void lcec_el7342_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el7342_data_t *hal_data = (lcec_el7342_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  int i;
  lcec_el7342_chan_t *chan;
  int16_t raw_count, raw_latch, raw_delta;

  // wait for slave to be operational
  if (!slave->state.operational) {
    hal_data->last_operational = 0;
    return;
  }

  // check inputs
  for (i=0; i<LCEC_EL7342_CHANS; i++) {
    chan = &hal_data->chans[i];

    // check for change in scale value
    if (*(chan->pos_scale) != chan->enc_old_scale) {
      // scale value has changed, test and update it
      if ((*(chan->pos_scale) < 1e-20) && (*(chan->pos_scale) > -1e-20)) {
        // value too small, divide by zero is a bad thing
        *(chan->pos_scale) = 1.0;
      }
      // save new scale to detect future changes
      chan->enc_old_scale = *(chan->pos_scale);
      // we actually want the reciprocal
      chan->enc_scale_recip = 1.0 / *(chan->pos_scale);
    }

    // get bit states
    *(chan->ina) = EC_READ_BIT(&pd[chan->ina_pdo_os], chan->ina_pdo_bp);
    *(chan->inb) = EC_READ_BIT(&pd[chan->inb_pdo_os], chan->inb_pdo_bp);
    *(chan->inext) = EC_READ_BIT(&pd[chan->inext_pdo_os], chan->inext_pdo_bp);
    *(chan->sync_err) = EC_READ_BIT(&pd[chan->sync_err_pdo_os], chan->sync_err_pdo_bp);
    *(chan->expol_stall) = EC_READ_BIT(&pd[chan->expol_stall_pdo_os], chan->expol_stall_pdo_bp);
    *(chan->tx_toggle) = EC_READ_BIT(&pd[chan->tx_toggle_pdo_os], chan->tx_toggle_pdo_bp);
    *(chan->count_overflow) = EC_READ_BIT(&pd[chan->count_overflow_pdo_os], chan->count_overflow_pdo_bp);
    *(chan->count_underflow) = EC_READ_BIT(&pd[chan->count_underflow_pdo_os], chan->count_underflow_pdo_bp);
    *(chan->latch_ext_valid) = EC_READ_BIT(&pd[chan->latch_ext_valid_pdo_os], chan->latch_ext_valid_pdo_bp);

    *(chan->dcm_ready_to_enable) = EC_READ_BIT(&pd[chan->dcm_ready_to_enable_pdo_os], chan->dcm_ready_to_enable_pdo_bp);
    *(chan->dcm_ready) = EC_READ_BIT(&pd[chan->dcm_ready_pdo_os], chan->dcm_ready_pdo_bp);
    *(chan->dcm_warning) = EC_READ_BIT(&pd[chan->dcm_warning_pdo_os], chan->dcm_warning_pdo_bp);
    *(chan->dcm_error) = EC_READ_BIT(&pd[chan->dcm_error_pdo_os], chan->dcm_error_pdo_bp);
    *(chan->dcm_move_pos) = EC_READ_BIT(&pd[chan->dcm_move_pos_pdo_os], chan->dcm_move_pos_pdo_bp);
    *(chan->dcm_move_neg) = EC_READ_BIT(&pd[chan->dcm_move_neg_pdo_os], chan->dcm_move_neg_pdo_bp);
    *(chan->dcm_torque_reduced) = EC_READ_BIT(&pd[chan->dcm_torque_reduced_pdo_os], chan->dcm_torque_reduced_pdo_bp);
    *(chan->dcm_din1) = EC_READ_BIT(&pd[chan->dcm_din1_pdo_os], chan->dcm_din1_pdo_bp);
    *(chan->dcm_din2) = EC_READ_BIT(&pd[chan->dcm_din2_pdo_os], chan->dcm_din2_pdo_bp);
    *(chan->dcm_sync_err) = EC_READ_BIT(&pd[chan->dcm_sync_err_pdo_os], chan->dcm_sync_err_pdo_bp);
    *(chan->dcm_tx_toggle) = EC_READ_BIT(&pd[chan->dcm_tx_toggle_pdo_os], chan->dcm_tx_toggle_pdo_bp);

    // read raw values
    raw_count = EC_READ_S16(&pd[chan->count_pdo_os]);
    raw_latch = EC_READ_S16(&pd[chan->latch_pdo_os]);

    // read raw info values
    *(chan->dcm_raw_info1) = EC_READ_S16(&pd[chan->dcm_info1_pdo_os]);
    *(chan->dcm_raw_info2) = EC_READ_S16(&pd[chan->dcm_info2_pdo_os]);

    // dispatch info values
    lcec_el7342_set_info(chan, chan->dcm_raw_info1, chan->dcm_sel_info1);
    lcec_el7342_set_info(chan, chan->dcm_raw_info2, chan->dcm_sel_info2);

    // check for operational change of slave
    if (!hal_data->last_operational) {
      chan->enc_last_count = raw_count;
    }

    // check for counter set done
    if (EC_READ_BIT(&pd[chan->set_count_done_pdo_os], chan->set_count_done_pdo_bp)) {
      chan->enc_last_count = raw_count;
      *(chan->set_raw_count) = 0;
    }

    // update raw values
    if (! *(chan->set_raw_count)) {
      *(chan->raw_count) = raw_count;
    }

    // handle initialization
    if (chan->enc_do_init || *(chan->reset)) {
      chan->enc_do_init = 0;
      chan->enc_last_count = raw_count;
      *(chan->count) = 0;
    }

    // handle index
    if (*(chan->latch_ext_valid)) {
      *(chan->raw_latch) = raw_latch;
      chan->enc_last_count = raw_latch;
      *(chan->count) = 0;
      *(chan->ena_latch_ext_pos) = 0;
      *(chan->ena_latch_ext_neg) = 0;
    }

    // compute net counts
    raw_delta = raw_count - chan->enc_last_count;
    chan->enc_last_count = raw_count;
    *(chan->count) += raw_delta;

    // scale count to make floating point position
    *(chan->pos) = *(chan->count) * chan->enc_scale_recip;
  }

  hal_data->last_operational = 1;
}

void lcec_el7342_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el7342_data_t *hal_data = (lcec_el7342_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  int i;
  lcec_el7342_chan_t *chan;
  double tmpval, tmpdc, raw_val;

  // set outputs
  for (i=0; i<LCEC_EL7342_CHANS; i++) {
    chan = &hal_data->chans[i];

    // validate duty cycle limits, both limits must be between
    // 0.0 and 1.0 (inclusive) and max must be greater then min
    if (*(chan->dcm_max_dc) > 1.0) {
      *(chan->dcm_max_dc) = 1.0;
    }
    if (*(chan->dcm_min_dc) > *(chan->dcm_max_dc)) {
      *(chan->dcm_min_dc) = *(chan->dcm_max_dc);
    }
    if (*(chan->dcm_min_dc) < -1.0) {
      *(chan->dcm_min_dc) = -1.0;
    }
    if (*(chan->dcm_max_dc) < *(chan->dcm_min_dc)) {
      *(chan->dcm_max_dc) = *(chan->dcm_min_dc);
    }

    // do scale calcs only when scale changes
    if (*(chan->dcm_scale) != chan->dcm_old_scale) {
      // validate the new scale value
      if ((*(chan->dcm_scale) < 1e-20) && (*(chan->dcm_scale) > -1e-20)) {
        // value too small, divide by zero is a bad thing
        *(chan->dcm_scale) = 1.0;
      }
      // get ready to detect future scale changes
      chan->dcm_old_scale = *(chan->dcm_scale);
      // we will need the reciprocal
      chan->dcm_scale_recip = 1.0 / *(chan->dcm_scale);
    }

    // get command
    tmpval = *(chan->dcm_value);
    if (*(chan->dcm_absmode) && (tmpval < 0)) {
      tmpval = -tmpval;
    }

    // convert value command to duty cycle
    tmpdc = tmpval * chan->dcm_scale_recip + *(chan->dcm_offset);
    if (tmpdc < *(chan->dcm_min_dc)) {
      tmpdc = *(chan->dcm_min_dc);
    }
    if (tmpdc > *(chan->dcm_max_dc)) {
      tmpdc = *(chan->dcm_max_dc);
    }

    // set output values
    if (*(chan->dcm_enable) == 0) {
      raw_val = 0;
      *(chan->dcm_curr_dc) = 0;
    } else {
      raw_val = (double)0x7fff * tmpdc;
      if (raw_val > (double)0x7fff) {
        raw_val = (double)0x7fff;
      }
      if (raw_val < (double)-0x7fff) {
        raw_val = (double)-0x7fff;
      }
      *(chan->dcm_curr_dc) = tmpdc;
    }

    // update value
    *(chan->dcm_raw_val) = (int32_t)raw_val;

    // set output data
    EC_WRITE_BIT(&pd[chan->set_count_pdo_os], chan->set_count_pdo_bp, *(chan->set_raw_count));
    EC_WRITE_BIT(&pd[chan->ena_latch_ext_pos_pdo_os], chan->ena_latch_ext_pos_pdo_bp, *(chan->ena_latch_ext_pos));
    EC_WRITE_BIT(&pd[chan->ena_latch_ext_neg_pdo_os], chan->ena_latch_ext_neg_pdo_bp, *(chan->ena_latch_ext_neg));
    EC_WRITE_S16(&pd[chan->set_count_val_pdo_os], *(chan->set_raw_count_val));

    EC_WRITE_BIT(&pd[chan->dcm_ena_pdo_os], chan->dcm_ena_pdo_bp, *(chan->dcm_enable));
    EC_WRITE_BIT(&pd[chan->dcm_reset_pdo_os], chan->dcm_reset_pdo_bp, *(chan->dcm_reset));
    EC_WRITE_BIT(&pd[chan->dcm_reduce_torque_pdo_os], chan->dcm_reduce_torque_pdo_bp, *(chan->dcm_reduce_torque));
    EC_WRITE_S16(&pd[chan->dcm_velo_pdo_os], (int16_t)raw_val);
  }
}

void lcec_el7342_set_info(lcec_el7342_chan_t *chan, hal_s32_t *raw_info, hal_u32_t *sel_info) {
  switch(*sel_info) {
    case INFO_SEL_MOTOR_VELO:
      *(chan->dcm_velo_fb) = (double) *raw_info * 0.0001 * chan->dcm_old_scale;
      break;

    case INFO_SEL_MOTOR_CURR:
      *(chan->dcm_current_fb) = (double) *raw_info * 0.001;
      break;
  }
}

