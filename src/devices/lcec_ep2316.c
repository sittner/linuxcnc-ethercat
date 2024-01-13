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

#include "../lcec.h"
#include "lcec_ep2316.h"

static int lcec_ep2316_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[]={
  { "EP2316", LCEC_BECKHOFF_VID, 0x090C4052, LCEC_EP2316_PDOS, 0, NULL, lcec_ep2316_init},
  { NULL },
};
ADD_TYPES(types);

typedef struct {
  hal_bit_t *in;
  hal_bit_t *in_not;
  hal_bit_t *in_diag;
  hal_bit_t *out;
  hal_bit_t invert;
  unsigned int in_pdo_os;
  unsigned int in_pdo_bp;
  unsigned int in_diag_pdo_os;
  unsigned int in_diag_pdo_bp;
  unsigned int out_pdo_os;
  unsigned int out_pdo_bp;
} lcec_ep2316_pin_t;

typedef struct {
  lcec_ep2316_pin_t io[8];
  hal_bit_t *sync_error;
  hal_bit_t *safe_state_active;
  hal_bit_t *error_chan_1;
  hal_bit_t *error_chan_2;
  hal_bit_t *txpdo_toggle;
  hal_bit_t *set_safe_state;
  hal_bit_t *reset_outputs;
  unsigned int sync_error_pdo_os;
  unsigned int sync_error_pdo_bp;
  unsigned int safe_state_active_pdo_os;
  unsigned int safe_state_active_pdo_bp;
  unsigned int error_chan_1_pdo_os;
  unsigned int error_chan_1_pdo_bp;
  unsigned int error_chan_2_pdo_os;
  unsigned int error_chan_2_pdo_bp;
  unsigned int txpdo_toggle_pdo_os;
  unsigned int txpdo_toggle_pdo_bp;
  unsigned int set_safe_state_pdo_os;
  unsigned int set_safe_state_pdo_bp;
  unsigned int reset_outputs_pdo_os;
  unsigned int reset_outputs_pdo_bp;
} lcec_ep2316_data_t;

static const lcec_pindesc_t ep2316_slave_pins[] = {
  { HAL_BIT, HAL_OUT, offsetof(lcec_ep2316_pin_t, in), "%s.%s.%s.din-%d" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_ep2316_pin_t, in_not), "%s.%s.%s.din-%d-not" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_ep2316_pin_t, in_diag), "%s.%s.%s.diag-in-%d" },
  { HAL_BIT, HAL_IN, offsetof(lcec_ep2316_pin_t, out), "%s.%s.%s.dout-%d"},
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static const lcec_pindesc_t ep2316_single_slave_pins[] = {
  { HAL_BIT, HAL_OUT, offsetof(lcec_ep2316_data_t, sync_error), "%s.%s.%s.sync-error" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_ep2316_data_t, safe_state_active), "%s.%s.%s.safe-state-active" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_ep2316_data_t, error_chan_1), "%s.%s.%s.error-channel-1" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_ep2316_data_t, error_chan_2), "%s.%s.%s.error-channel-2" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_ep2316_data_t, txpdo_toggle), "%s.%s.%s.txpdo-toggle" },
  { HAL_BIT, HAL_IN, offsetof(lcec_ep2316_data_t, set_safe_state), "%s.%s.%s.set-safe-state" },
  { HAL_BIT, HAL_IN, offsetof(lcec_ep2316_data_t, reset_outputs), "%s.%s.%s.reset-outputs" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static void lcec_ep2316_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_ep2316_data_t *hal_data = (lcec_ep2316_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  lcec_ep2316_pin_t *pin;
  int i, s;
  int pin_count = 8;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  // read status
  *(hal_data->sync_error) = EC_READ_BIT(&pd[hal_data->sync_error_pdo_os], hal_data->sync_error_pdo_bp);
  *(hal_data->safe_state_active) = EC_READ_BIT(&pd[hal_data->safe_state_active_pdo_os], hal_data->safe_state_active_pdo_bp);
  *(hal_data->error_chan_1) = EC_READ_BIT(&pd[hal_data->error_chan_1_pdo_os], hal_data->error_chan_1_pdo_bp);
  *(hal_data->error_chan_2) = EC_READ_BIT(&pd[hal_data->error_chan_2_pdo_os], hal_data->error_chan_2_pdo_bp);
  *(hal_data->txpdo_toggle) = EC_READ_BIT(&pd[hal_data->txpdo_toggle_pdo_os], hal_data->txpdo_toggle_pdo_bp);

  // check inputs
  for (i=0; i<pin_count; i++) {
    pin = &hal_data->io[i];
    s = EC_READ_BIT(&pd[pin->in_pdo_os], pin->in_pdo_bp);
    *(pin->in) = s;
    *(pin->in_not) = !s;
    s = EC_READ_BIT(&pd[pin->in_diag_pdo_os], pin->in_diag_pdo_bp);
    *(pin->in_diag) = s;
  }
}

static void lcec_ep2316_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_ep2316_data_t *hal_data = (lcec_ep2316_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  lcec_ep2316_pin_t *pin;
  int i, s;
  int pin_count = 8;

  // set auxiliary outputs
  EC_WRITE_BIT(&pd[hal_data->set_safe_state_pdo_os], hal_data->set_safe_state_pdo_bp, hal_data->set_safe_state);
  EC_WRITE_BIT(&pd[hal_data->reset_outputs_pdo_os], hal_data->reset_outputs_pdo_bp, hal_data->reset_outputs);

  // set outputs
  for (i=0; i<pin_count; i++) {
    pin = &hal_data->io[i];
    s = *(pin->out);
    if (pin->invert) {
      s = !s;
    }
    EC_WRITE_BIT(&pd[pin->out_pdo_os], pin->out_pdo_bp, s);
  }
}

static int lcec_ep2316_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_ep2316_data_t *hal_data;
  lcec_ep2316_pin_t *pin;
  int i, err;
  int pin_count = 8;

  ec_pdo_entry_reg_t *initial_pdo = pdo_entry_regs;

  // initialize callbacks
  slave->proc_read = lcec_ep2316_read;
  slave->proc_write = lcec_ep2316_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_ep2316_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_ep2316_data_t));
  slave->hal_data = hal_data;

  // initialize status
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x1c32, 0x20, &hal_data->sync_error_pdo_os, &hal_data->sync_error_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xf600, 0x01, &hal_data->safe_state_active_pdo_os, &hal_data->safe_state_active_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xf600, 0x02, &hal_data->error_chan_1_pdo_os, &hal_data->error_chan_1_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xf600, 0x03, &hal_data->error_chan_2_pdo_os, &hal_data->error_chan_2_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x1800, 0x09, &hal_data->txpdo_toggle_pdo_os, &hal_data->txpdo_toggle_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xf700, 0x01, &hal_data->set_safe_state_pdo_os, &hal_data->set_safe_state_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xf700, 0x02, &hal_data->reset_outputs_pdo_os, &hal_data->reset_outputs_pdo_bp);

  // set initial values
  hal_data->sync_error = 0;
  hal_data->safe_state_active = 0;
  hal_data->error_chan_1 = 0;
  hal_data->error_chan_2 = 0;
  hal_data->txpdo_toggle = 0;
  hal_data->set_safe_state = 0;
  hal_data->reset_outputs = 0;

  // export pins
  if ((err=lcec_pin_newf_list(hal_data, ep2316_single_slave_pins, LCEC_MODULE_NAME, master->name, slave->name)) !=0) {
    return err;
  }

  // initialize pins
  for (i=0; i<pin_count; i++) {
    // initialize POD entry
    pin = &hal_data->io[i];

    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x01 + i, &pin->in_pdo_os, &pin->in_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6001, 0x01 + i, &pin->in_diag_pdo_os, &pin->in_diag_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000, 0x01 + i, &pin->out_pdo_os, &pin->out_pdo_bp);

    // export pins
    if ((err = lcec_pin_newf_list(pin, ep2316_slave_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      return err;
    }

    // initialize pins
    *(pin->out) = 0;
    pin->invert = 0;
  }

  return 0;
}
