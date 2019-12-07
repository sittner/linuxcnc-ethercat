//
//    Copyright (C) 2016 Sascha Ittner <sascha.ittner@modusoft.de>
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
#include "lcec_el3403.h"

typedef struct {
  hal_bit_t *sync_err;
  hal_float_t *current;
  hal_float_t *voltage;
  hal_float_t *power;
  unsigned int sync_err_pdo_os;
  unsigned int sync_err_pdo_bp;
  unsigned int curr_pdo_os;
  unsigned int curr_pdo_bp;
  unsigned int volt_pdo_os;
  unsigned int volt_pdo_bp;
  unsigned int pow_pdo_os;
  unsigned int pow_pdo_bp;
} lcec_el3403_chan_t;

typedef struct {
  lcec_el3403_chan_t chans[LCEC_EL3403_CHANS];
} lcec_el3403_data_t;

static const lcec_pindesc_t slave_pins[] = {
  { HAL_BIT, HAL_OUT, offsetof(lcec_el3403_chan_t, sync_err), "%s.%s.%s.pow-%d-sync-err" },
  { HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, current), "%s.%s.%s.pow-%d-current" },
  { HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, voltage), "%s.%s.%s.pow-%d-voltage" },
  { HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, power), "%s.%s.%s.pow-%d-power" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};


static ec_pdo_entry_info_t lcec_el3403_r1[] = {
  {0x7000, 0x01, 8}, /* Index */
};
  
static ec_pdo_entry_info_t lcec_el3403_r2[] = {
  {0x7010, 0x01, 8}, /* Index */
};
  
static ec_pdo_entry_info_t lcec_el3403_r3[] = {
  {0x7020, 0x01, 8}, /* Index */
};

static ec_pdo_entry_info_t lcec_el3403_channel1[] = {
  {0x0000, 0x00, 13}, /* Gap */
  {0x6000, 0x0e, 1}, /* Sync Error */
  {0x0000, 0x00, 1}, /* Gap */
  {0x1800, 0x09, 1},
  {0x6000, 0x11, 32}, /* Current */
  {0x6000, 0x12, 32}, /* Voltage */
  {0x6000, 0x13, 32}, /* Active power */
  {0x6000, 0x14, 8}, /* Index */
  {0x0000, 0x00, 8}, /* Gap */
  {0x6000, 0x1d, 32}, /* Variant value */
};

static ec_pdo_entry_info_t lcec_el3403_channel2[] = {
  {0x0000, 0x00, 13}, /* Gap */
  {0x6010, 0x0e, 1}, /* Sync Error */
  {0x0000, 0x00, 1}, /* Gap */
  {0x1801, 0x09, 1},
  {0x6010, 0x11, 32}, /* Current */
  {0x6010, 0x12, 32}, /* Voltage */
  {0x6010, 0x13, 32}, /* Active power */
  {0x6010, 0x14, 8}, /* Index */
  {0x0000, 0x00, 8}, /* Gap */
  {0x6010, 0x1d, 32}, /* Variant value */
};

static ec_pdo_entry_info_t lcec_el3403_channel3[] = {
  {0x0000, 0x00, 13}, /* Gap */
  {0x6020, 0x0e, 1}, /* Sync Error */
  {0x0000, 0x00, 1}, /* Gap */
  {0x1802, 0x09, 1},
  {0x6020, 0x11, 32}, /* Current */
  {0x6020, 0x12, 32}, /* Voltage */
  {0x6020, 0x13, 32}, /* Active power */
  {0x6020, 0x14, 8}, /* Index */
  {0x0000, 0x00, 8}, /* Gap */
  {0x6020, 0x1d, 32}, /* Variant value */
};

static ec_pdo_entry_info_t lcec_el3403_statusdata[] = {
    {0x0000, 0x00, 3}, /* Gap */
    {0xf100, 0x04, 1}, /* Missing zero crossing A */
    {0xf100, 0x05, 1}, /* Missing zero crossing B */
    {0xf100, 0x06, 1}, /* Missing zero crossing C */
    {0x0000, 0x00, 2}, /* Gap */
    {0xf100, 0x09, 1}, /* Phase sequence error */
    {0x0000, 0x00, 4}, /* Gap */
    {0xf100, 0x0e, 1}, /* Sync Error */
    {0x0000, 0x00, 2}, /* Gap */
};

static ec_pdo_info_t lcec_el3403_pdos_out[] = {
    {0x1600, 1, lcec_el3403_r1},
    {0x1601, 1, lcec_el3403_r2},
    {0x1602, 1, lcec_el3403_r3},
};
  
static ec_pdo_info_t lcec_el3403_pdos_in[] = {
    {0x1A00, 10, lcec_el3403_channel1},
    {0x1A01, 10, lcec_el3403_channel2},
    {0x1A02, 10, lcec_el3403_channel3},
    {0x1A03, 9, lcec_el3403_statusdata},
};

static ec_sync_info_t lcec_el3403_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL},
    {1, EC_DIR_INPUT,  0, NULL},
    {2, EC_DIR_OUTPUT, 3, lcec_el3403_pdos_out},
    {3, EC_DIR_INPUT,  4, lcec_el3403_pdos_in},
    {0xff}
};

void lcec_el3403_read(struct lcec_slave *slave, long period);

int lcec_el3403_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el3403_data_t *hal_data;
  lcec_el3403_chan_t *chan;
  int i;
  int err;

  // initialize callbacks
  slave->proc_read = lcec_el3403_read;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el3403_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el3403_data_t));
  slave->hal_data = hal_data;

  // initialize sync info
  slave->sync_info = lcec_el3403_syncs;

  // initialize pins
  for (i=0; i<LCEC_EL3403_CHANS; i++) {
    chan = &hal_data->chans[i];

    // initialize POD entries
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x0e, &chan->sync_err_pdo_os, &chan->sync_err_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x11, &chan->curr_pdo_os, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x12, &chan->volt_pdo_os, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x13, &chan->pow_pdo_os, NULL);

    // export pins
    if ((err = lcec_pin_newf_list(chan, slave_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      return err;
    }
  }

  return 0;
}

void lcec_el3403_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el3403_data_t *hal_data = (lcec_el3403_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  int i;
  lcec_el3403_chan_t *chan;
  int32_t current, voltage, power;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  // check inputs
  for (i=0; i<LCEC_EL3403_CHANS; i++) {
    chan = &hal_data->chans[i];

    // update state
    *(chan->sync_err) = EC_READ_BIT(&pd[chan->sync_err_pdo_os], chan->sync_err_pdo_bp);

    // update value
    current = EC_READ_S32(&pd[chan->curr_pdo_os]);
    voltage = EC_READ_S32(&pd[chan->volt_pdo_os]);
    power = EC_READ_S32(&pd[chan->pow_pdo_os]);

    *(chan->current) = current * 0.000001;  // 1 uA
    *(chan->voltage) = voltage * 0.0001; // 0.1 mV
    *(chan->power) = power * 0.01; // 0.01 W
  }
}

