//
//    Copyright (C) 2018 Sascha Ittner <sascha.ittner@modusoft.de>
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
#include "lcec_ax5200.h"

typedef struct {
  lcec_class_ax5_chan_t chans[LCEC_AX5200_CHANS];
} lcec_ax5200_data_t;

static ec_pdo_entry_info_t lcec_ax5200_in_a[] = {
   {0x0087, 0x01, 16}, // status word
   {0x0033, 0x01, 32}, // position feedback
   {0x0054, 0x01, 16}  // torque feedback
};

static ec_pdo_entry_info_t lcec_ax5200_in_b[] = {
   {0x0087, 0x02, 16}, // status word
   {0x0033, 0x02, 32}, // position feedback
   {0x0054, 0x02, 16}  // torque feedback
};

static ec_pdo_entry_info_t lcec_ax5200_out_a[] = {
   {0x0086, 0x01,  16}, // control-word
   {0x0018, 0x01,  32}, // velo-command
};

static ec_pdo_entry_info_t lcec_ax5200_out_b[] = {
   {0x0086, 0x02,  16}, // control word
   {0x0018, 0x02,  32}, // velo command
};

static ec_pdo_info_t lcec_ax5200_pdos_out[] = {
    {0x0018,  2, lcec_ax5200_out_a},
    {0x1018,  2, lcec_ax5200_out_b}
};

static ec_pdo_info_t lcec_ax5200_pdos_in[] = {
    {0x0010,  3, lcec_ax5200_in_a},
    {0x1010,  3, lcec_ax5200_in_b}
};

static ec_sync_info_t lcec_ax5200_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL},
    {1, EC_DIR_INPUT,  0, NULL},
    {2, EC_DIR_OUTPUT, 2, lcec_ax5200_pdos_out},
    {3, EC_DIR_INPUT,  2, lcec_ax5200_pdos_in},
    {0xff}
};

void lcec_ax5200_read(struct lcec_slave *slave, long period);
void lcec_ax5200_write(struct lcec_slave *slave, long period);

int lcec_ax5200_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_ax5200_data_t *hal_data;
  int i;
  lcec_class_ax5_chan_t *chan;
  int err;
  char pfx[HAL_NAME_LEN];

  // initialize callbacks
  slave->proc_read = lcec_ax5200_read;
  slave->proc_write = lcec_ax5200_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_ax5200_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_ax5200_data_t));
  slave->hal_data = hal_data;

  // initialize sync info
  slave->sync_info = lcec_ax5200_syncs;

  // initialize pins
  for (i=0; i<LCEC_AX5200_CHANS; i++, pdo_entry_regs += LCEC_CLASS_AX5_PDOS) {
    chan = &hal_data->chans[i];

    // init subclasses
    rtapi_snprintf(pfx, HAL_NAME_LEN, "ch%d.srv", i);
    if ((err = lcec_class_ax5_init(slave, pdo_entry_regs, chan, i, pfx)) != 0) {
      return err;
    }
    rtapi_snprintf(pfx, HAL_NAME_LEN, "ch%d.enc", i);
    if ((err = class_enc_init(slave, &chan->enc, 32, pfx)) != 0) {
      return err;
    }
  }

  return 0;
}

void lcec_ax5200_read(struct lcec_slave *slave, long period) {
  lcec_ax5200_data_t *hal_data = (lcec_ax5200_data_t *) slave->hal_data;
  int i;
  lcec_class_ax5_chan_t *chan;

  // check inputs
  for (i=0; i<LCEC_AX5200_CHANS; i++) {
    chan = &hal_data->chans[i];
    lcec_class_ax5_read(slave, chan);
  }
}

void lcec_ax5200_write(struct lcec_slave *slave, long period) {
  lcec_ax5200_data_t *hal_data = (lcec_ax5200_data_t *) slave->hal_data;
  int i;
  lcec_class_ax5_chan_t *chan;

  // write outputs
  for (i=0; i<LCEC_AX5200_CHANS; i++) {
    chan = &hal_data->chans[i];
    lcec_class_ax5_write(slave, chan);
  }
}

