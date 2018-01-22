//
//    Copyright (C) 2012 Sascha Ittner <sascha.ittner@modusoft.de>
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
#include "lcec_el5151.h"

typedef struct {
  hal_bit_t *ena_latch_c;
  hal_bit_t *ena_latch_ext_pos;
  hal_bit_t *ena_latch_ext_neg;
  hal_bit_t *reset;
  hal_bit_t *ina;
  hal_bit_t *inb;
  hal_bit_t *inc;
  hal_bit_t *inext;
  hal_bit_t *expol_stall;
  hal_bit_t *sync_err;
  hal_bit_t *latch_c_valid;
  hal_bit_t *latch_ext_valid;
  hal_bit_t *tx_toggle;
  hal_bit_t *set_raw_count;
  hal_s32_t *set_raw_count_val;
  hal_s32_t *raw_count;
  hal_s32_t *raw_latch;
  hal_u32_t *raw_period;
  hal_s32_t *count;
  hal_float_t *pos_scale;
  hal_float_t *pos;
  hal_float_t *period;

  unsigned int ena_latch_c_pdo_os;
  unsigned int ena_latch_c_pdo_bp;
  unsigned int ena_latch_ext_pos_pdo_os;
  unsigned int ena_latch_ext_pos_pdo_bp;
  unsigned int ena_latch_ext_neg_pdo_os;
  unsigned int ena_latch_ext_neg_pdo_bp;
  unsigned int set_count_pdo_os;
  unsigned int set_count_pdo_bp;
  unsigned int set_count_val_pdo_os;
  unsigned int set_count_done_pdo_os;
  unsigned int set_count_done_pdo_bp;
  unsigned int latch_c_valid_pdo_os;
  unsigned int latch_c_valid_pdo_bp;
  unsigned int latch_ext_valid_pdo_os;
  unsigned int latch_ext_valid_pdo_bp;
  unsigned int expol_stall_pdo_os;
  unsigned int expol_stall_pdo_bp;
  unsigned int ina_pdo_os;
  unsigned int ina_pdo_bp;
  unsigned int inb_pdo_os;
  unsigned int inb_pdo_bp;
  unsigned int inc_pdo_os;
  unsigned int inc_pdo_bp;
  unsigned int inext_pdo_os;
  unsigned int inext_pdo_bp;
  unsigned int sync_err_pdo_os;
  unsigned int sync_err_pdo_bp;
  unsigned int tx_toggle_pdo_os;
  unsigned int tx_toggle_pdo_bp;
  unsigned int count_pdo_os;
  unsigned int latch_pdo_os;
  unsigned int period_pdo_os;

  int do_init;
  int32_t last_count;
  double old_scale;
  double scale;

  int last_operational;
} lcec_el5151_data_t;

static ec_pdo_entry_info_t lcec_el5151_in[] = {
   {0x6000, 0x01,  1}, // Latch C valid
   {0x6000, 0x02,  1}, // Latch extern valid
   {0x6000, 0x03,  1}, // Set counter done
   {0x0000, 0x00,  4}, // Gap
   {0x6000, 0x08,  1}, // Extrapolation stall
   {0x6000, 0x09,  1}, // Status of input A
   {0x6000, 0x0a,  1}, // Status of input B
   {0x6000, 0x0b,  1}, // Status of input C
   {0x0000, 0x00,  1}, // Gap
   {0x6000, 0x0d,  1}, // Status of extern latch
   {0x1c32, 0x20,  1}, // Sync error
   {0x0000, 0x00,  1}, // Gap
   {0x1800, 0x09,  1}, // TxPDO Toggle
   {0x6000, 0x11, 32}, // Counter value
   {0x6000, 0x12, 32}  // Latch value
};

static ec_pdo_entry_info_t lcec_el5151_period[] = {
   {0x6000, 0x14, 32} // Period value
};

static ec_pdo_entry_info_t lcec_el5151_out[] = {
   {0x7000, 0x01,  1}, // Enable latch C
   {0x7000, 0x02,  1}, // Enable latch extern on positive edge
   {0x7000, 0x03,  1}, // Set counter
   {0x7000, 0x04,  1}, // Enable latch extern on negative edge
   {0x0000, 0x00,  4}, // Gap
   {0x0000, 0x00,  8}, // Gap
   {0x7000, 0x11, 32}  // Set counter value
};

static ec_pdo_info_t lcec_el5151_pdos_out[] = {
    {0x1600,  7, lcec_el5151_out}
};

static ec_pdo_info_t lcec_el5151_pdos_in[] = {
    {0x1A00, 15, lcec_el5151_in},
    {0x1A02,  1, lcec_el5151_period}
};

static ec_sync_info_t lcec_el5151_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL},
    {1, EC_DIR_INPUT,  0, NULL},
    {2, EC_DIR_OUTPUT, 1, lcec_el5151_pdos_out},
    {3, EC_DIR_INPUT,  2, lcec_el5151_pdos_in},
    {0xff}
};


void lcec_el5151_read(struct lcec_slave *slave, long period);
void lcec_el5151_write(struct lcec_slave *slave, long period);

int lcec_el5151_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el5151_data_t *hal_data;
  int err;

  // initialize callbacks
  slave->proc_read = lcec_el5151_read;
  slave->proc_write = lcec_el5151_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el5151_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el5151_data_t));
  slave->hal_data = hal_data;

  // initializer sync info
  slave->sync_info = lcec_el5151_syncs;

  // initialize global data
  hal_data->last_operational = 0;

  // initialize POD entries
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x01, &hal_data->latch_c_valid_pdo_os, &hal_data->latch_c_valid_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x02, &hal_data->latch_ext_valid_pdo_os, &hal_data->latch_ext_valid_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x03, &hal_data->set_count_done_pdo_os, &hal_data->set_count_done_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x08, &hal_data->expol_stall_pdo_os, &hal_data->expol_stall_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x09, &hal_data->ina_pdo_os, &hal_data->ina_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x0a, &hal_data->inb_pdo_os, &hal_data->inb_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x0b, &hal_data->inc_pdo_os, &hal_data->inc_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x0d, &hal_data->inext_pdo_os, &hal_data->inext_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x1c32, 0x20, &hal_data->sync_err_pdo_os, &hal_data->sync_err_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x1800, 0x09, &hal_data->tx_toggle_pdo_os, &hal_data->tx_toggle_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x11, &hal_data->count_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x12, &hal_data->latch_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x14, &hal_data->period_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000, 0x01, &hal_data->ena_latch_c_pdo_os, &hal_data->ena_latch_c_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000, 0x02, &hal_data->ena_latch_ext_pos_pdo_os, &hal_data->ena_latch_ext_pos_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000, 0x03, &hal_data->set_count_pdo_os, &hal_data->set_count_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000, 0x04, &hal_data->ena_latch_ext_neg_pdo_os, &hal_data->ena_latch_ext_neg_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000, 0x11, &hal_data->set_count_val_pdo_os, NULL);

  // export pins
  if ((err = hal_pin_bit_newf(HAL_IO, &(hal_data->ena_latch_c), comp_id, "%s.%s.%s.enc-index-c-enable", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-index-c-enable failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IO, &(hal_data->ena_latch_ext_pos), comp_id, "%s.%s.%s.enc-index-ext-pos-enable", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-index-ext-pos-enable failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IO, &(hal_data->ena_latch_ext_neg), comp_id, "%s.%s.%s.enc-index-ext-neg-enable", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-index-ext-neg-enable failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->reset), comp_id, "%s.%s.%s.enc-reset", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-reset failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->ina), comp_id, "%s.%s.%s.enc-ina", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-ina failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->inb), comp_id, "%s.%s.%s.enc-inb", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-inb failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->inc), comp_id, "%s.%s.%s.enc-inc", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-inc failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->inext), comp_id, "%s.%s.%s.enc-inext", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-inext failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->expol_stall), comp_id, "%s.%s.%s.enc-expol-stall", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-expol-stall failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->sync_err), comp_id, "%s.%s.%s.enc-sync-error", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-sync-error failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->latch_c_valid), comp_id, "%s.%s.%s.enc-latch-c-valid", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-latch-c-valid failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->latch_ext_valid), comp_id, "%s.%s.%s.enc-latch-ext-valid", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-latch-ext-valid failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->tx_toggle), comp_id, "%s.%s.%s.enc-tx-toggle", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-tx-toggle failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IO, &(hal_data->set_raw_count), comp_id, "%s.%s.%s.enc-set-raw-count", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-set-raw-count failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_s32_newf(HAL_IN, &(hal_data->set_raw_count_val), comp_id, "%s.%s.%s.enc-set-raw-count-val", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-set-raw-count-val failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_s32_newf(HAL_OUT, &(hal_data->raw_count), comp_id, "%s.%s.%s.enc-raw-count", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-raw-count failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_s32_newf(HAL_OUT, &(hal_data->count), comp_id, "%s.%s.%s.enc-count", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-count failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_s32_newf(HAL_OUT, &(hal_data->raw_latch), comp_id, "%s.%s.%s.enc-raw-latch", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-raw-latch failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_u32_newf(HAL_OUT, &(hal_data->raw_period), comp_id, "%s.%s.%s.enc-raw-period", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-raw-period failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->pos), comp_id, "%s.%s.%s.enc-pos", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-pos failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->period), comp_id, "%s.%s.%s.enc-period", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-period failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_IO, &(hal_data->pos_scale), comp_id, "%s.%s.%s.enc-pos-scale", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-scale failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }

  // initialize pins
  *(hal_data->ena_latch_c) = 0;
  *(hal_data->ena_latch_ext_pos) = 0;
  *(hal_data->ena_latch_ext_neg) = 0;
  *(hal_data->reset) = 0;
  *(hal_data->ina) = 0;
  *(hal_data->inb) = 0;
  *(hal_data->inc) = 0;
  *(hal_data->inext) = 0;
  *(hal_data->expol_stall) = 0;
  *(hal_data->sync_err) = 0;
  *(hal_data->latch_c_valid) = 0;
  *(hal_data->latch_ext_valid) = 0;
  *(hal_data->tx_toggle) = 0;
  *(hal_data->set_raw_count) = 0;
  *(hal_data->set_raw_count_val) = 0;
  *(hal_data->raw_count) = 0;
  *(hal_data->raw_latch) = 0;
  *(hal_data->raw_period) = 0;
  *(hal_data->count) = 0;
  *(hal_data->pos) = 0;
  *(hal_data->period) = 0;
  *(hal_data->pos_scale) = 1.0;

  // initialize variables
  hal_data->do_init = 1;
  hal_data->last_count = 0;
  hal_data->old_scale = *(hal_data->pos_scale) + 1.0;
  hal_data->scale = 1.0;

  return 0;
}

void lcec_el5151_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el5151_data_t *hal_data = (lcec_el5151_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  int32_t raw_count, raw_latch, raw_delta;
  uint32_t raw_period;

  // wait for slave to be operational
  if (!slave->state.operational) {
    hal_data->last_operational = 0;
    return;
  }

  // check for change in scale value
  if (*(hal_data->pos_scale) != hal_data->old_scale) {
    // scale value has changed, test and update it
    if ((*(hal_data->pos_scale) < 1e-20) && (*(hal_data->pos_scale) > -1e-20)) {
      // value too small, divide by zero is a bad thing
      *(hal_data->pos_scale) = 1.0;
    }
    // save new scale to detect future changes
    hal_data->old_scale = *(hal_data->pos_scale);
    // we actually want the reciprocal
    hal_data->scale = 1.0 / *(hal_data->pos_scale);
  }

  // get bit states
  *(hal_data->ina) = EC_READ_BIT(&pd[hal_data->ina_pdo_os], hal_data->ina_pdo_bp);
  *(hal_data->inb) = EC_READ_BIT(&pd[hal_data->inb_pdo_os], hal_data->inb_pdo_bp);
  *(hal_data->inc) = EC_READ_BIT(&pd[hal_data->inc_pdo_os], hal_data->inc_pdo_bp);
  *(hal_data->inext) = EC_READ_BIT(&pd[hal_data->inext_pdo_os], hal_data->inext_pdo_bp);
  *(hal_data->expol_stall) = EC_READ_BIT(&pd[hal_data->expol_stall_pdo_os], hal_data->expol_stall_pdo_bp);
  *(hal_data->sync_err) = EC_READ_BIT(&pd[hal_data->sync_err_pdo_os], hal_data->sync_err_pdo_bp);
  *(hal_data->latch_c_valid) = EC_READ_BIT(&pd[hal_data->latch_c_valid_pdo_os], hal_data->latch_c_valid_pdo_bp);
  *(hal_data->latch_ext_valid) = EC_READ_BIT(&pd[hal_data->latch_ext_valid_pdo_os], hal_data->latch_ext_valid_pdo_bp);
  *(hal_data->tx_toggle) = EC_READ_BIT(&pd[hal_data->tx_toggle_pdo_os], hal_data->tx_toggle_pdo_bp);

  // read raw values
  raw_count = EC_READ_S32(&pd[hal_data->count_pdo_os]);
  raw_latch = EC_READ_S32(&pd[hal_data->latch_pdo_os]);
  raw_period = EC_READ_U32(&pd[hal_data->period_pdo_os]);

  // check for operational change of slave
  if (!hal_data->last_operational) {
    hal_data->last_count = raw_count;
  }

  // check for counter set done
  if (EC_READ_BIT(&pd[hal_data->set_count_done_pdo_os], hal_data->set_count_done_pdo_bp)) {
    hal_data->last_count = raw_count;
    *(hal_data->set_raw_count) = 0;
  }

  // update raw values
  if (! *(hal_data->set_raw_count)) {
    *(hal_data->raw_count) = raw_count;
    *(hal_data->raw_period) = raw_period;
  }

  // handle initialization
  if (hal_data->do_init || *(hal_data->reset)) {
    hal_data->do_init = 0;
    hal_data->last_count = raw_count;
    *(hal_data->count) = 0;
  }

  // handle index
  if (*(hal_data->latch_ext_valid)) {
    *(hal_data->raw_latch) = raw_latch;
    hal_data->last_count = raw_latch;
    *(hal_data->count) = 0;
    *(hal_data->ena_latch_ext_pos) = 0;
    *(hal_data->ena_latch_ext_neg) = 0;
  }
  if (*(hal_data->latch_c_valid)) {
    *(hal_data->raw_latch) = raw_latch;
    hal_data->last_count = raw_latch;
    *(hal_data->count) = 0;
    *(hal_data->ena_latch_c) = 0;
  }

  // compute net counts
  raw_delta = raw_count - hal_data->last_count;
  hal_data->last_count = raw_count;
  *(hal_data->count) += raw_delta;

  // scale count to make floating point position
  *(hal_data->pos) = *(hal_data->count) * hal_data->scale;

  // scale period
  *(hal_data->period) = ((double) (*(hal_data->raw_period))) * LCEC_EL5151_PERIOD_SCALE;

  hal_data->last_operational = 1;
}

void lcec_el5151_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el5151_data_t *hal_data = (lcec_el5151_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;

  // set output data
  EC_WRITE_BIT(&pd[hal_data->set_count_pdo_os], hal_data->set_count_pdo_bp, *(hal_data->set_raw_count));
  EC_WRITE_BIT(&pd[hal_data->ena_latch_c_pdo_os], hal_data->ena_latch_c_pdo_bp, *(hal_data->ena_latch_c));
  EC_WRITE_BIT(&pd[hal_data->ena_latch_ext_pos_pdo_os], hal_data->ena_latch_ext_pos_pdo_bp, *(hal_data->ena_latch_ext_pos));
  EC_WRITE_BIT(&pd[hal_data->ena_latch_ext_neg_pdo_os], hal_data->ena_latch_ext_neg_pdo_bp, *(hal_data->ena_latch_ext_neg));
  EC_WRITE_S32(&pd[hal_data->set_count_val_pdo_os], *(hal_data->set_raw_count_val));
}

