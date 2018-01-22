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
#include "lcec_el5152.h"

typedef struct {
  hal_bit_t *index;
  hal_bit_t *index_ena;
  hal_bit_t *reset;
  hal_bit_t *ina;
  hal_bit_t *inb;
  hal_bit_t *expol_stall;
  hal_bit_t *tx_toggle;
  hal_bit_t *set_raw_count;
  hal_s32_t *set_raw_count_val;
  hal_s32_t *raw_count;
  hal_u32_t *raw_period;
  hal_s32_t *count;
  hal_float_t *pos_scale;
  hal_float_t *pos;
  hal_float_t *period;

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
  unsigned int tx_toggle_pdo_os;
  unsigned int tx_toggle_pdo_bp;
  unsigned int count_pdo_os;
  unsigned int period_pdo_os;

  int do_init;
  int32_t last_count;
  int last_index;
  double old_scale;
  double scale;
} lcec_el5152_chan_t;

typedef struct {
  lcec_el5152_chan_t chans[LCEC_EL5152_CHANS];
  int last_operational;
} lcec_el5152_data_t;

static ec_pdo_entry_info_t lcec_el5152_channel1_in[] = {
   {0x0000, 0x00, 2}, // Gap
   {0x6000, 0x03, 1}, // Set counter done
   {0x0000, 0x00, 4}, // Gap
   {0x6000, 0x08, 1}, // Extrapolation stall
   {0x6000, 0x09, 1}, // State input A
   {0x6000, 0x0a, 1}, // State input B
   {0x0000, 0x00, 3}, // Gap
   {0x1c32, 0x20, 1}, // Sync error
   {0x0000, 0x00, 1}, // Gap
   {0x1800, 0x09, 1}, // TxPDO toggle
   {0x6000, 0x11, 32} // Counter value
};

static ec_pdo_entry_info_t lcec_el5152_channel2_in[] = {
   {0x0000, 0x00, 2}, // Gap
   {0x6010, 0x03, 1}, // Set counter done
   {0x0000, 0x00, 4}, // Gap
   {0x6010, 0x08, 1}, // Extrapolation stall
   {0x6010, 0x09, 1}, // State input A
   {0x6010, 0x0a, 1}, // State input B
   {0x0000, 0x00, 3}, // Gap
   {0x1c32, 0x20, 1}, // Sync error
   {0x0000, 0x00, 1}, // Gap
   {0x1804, 0x09, 1}, // TxPDO toggle
   {0x6010, 0x11, 32} // Counter value
};

static ec_pdo_entry_info_t lcec_el5152_channel1_period[] = {
   {0x6000, 0x14, 32} // Period value
};

static ec_pdo_entry_info_t lcec_el5152_channel2_period[] = {
   {0x6010, 0x14, 32} // Period value
};

static ec_pdo_entry_info_t lcec_el5152_channel1_out[] = {
   {0x0000, 0x00, 1}, // Gap
   {0x0000, 0x00, 1}, // Gap
   {0x7000, 0x03, 1}, // Set counter
   {0x0000, 0x00, 1}, // Gap
   {0x0000, 0x00, 4}, // Gap
   {0x0000, 0x00, 8}, // Gap
   {0x7000, 0x11, 32} // Set counter value
};

static ec_pdo_entry_info_t lcec_el5152_channel2_out[] = {
   {0x0000, 0x00, 1}, // Gap
   {0x0000, 0x00, 1}, // Gap
   {0x7010, 0x03, 1}, // Set counter
   {0x0000, 0x00, 1}, // Gap
   {0x0000, 0x00, 4}, // Gap
   {0x0000, 0x00, 8}, // Gap
   {0x7010, 0x11, 32} // Set counter value
};

static ec_pdo_info_t lcec_el5152_pdos_out[] = {
    {0x1600,  7, lcec_el5152_channel1_out},
    {0x1602,  7, lcec_el5152_channel2_out}
};

static ec_pdo_info_t lcec_el5152_pdos_in[] = {
    {0x1A00, 11, lcec_el5152_channel1_in},
    {0x1A02,  1, lcec_el5152_channel1_period},
    {0x1A04, 11, lcec_el5152_channel2_in},
    {0x1A06,  1, lcec_el5152_channel2_period}
};

static ec_sync_info_t lcec_el5152_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL},
    {1, EC_DIR_INPUT,  0, NULL},
    {2, EC_DIR_OUTPUT, 2, lcec_el5152_pdos_out},
    {3, EC_DIR_INPUT,  4, lcec_el5152_pdos_in},
    {0xff}
};


void lcec_el5152_read(struct lcec_slave *slave, long period);
void lcec_el5152_write(struct lcec_slave *slave, long period);

int lcec_el5152_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el5152_data_t *hal_data;
  int i;
  lcec_el5152_chan_t *chan;
  int err;

  // initialize callbacks
  slave->proc_read = lcec_el5152_read;
  slave->proc_write = lcec_el5152_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el5152_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el5152_data_t));
  slave->hal_data = hal_data;

  // initializer sync info
  slave->sync_info = lcec_el5152_syncs;

  // initialize global data
  hal_data->last_operational = 0;

  // initialize pins
  for (i=0; i<LCEC_EL5152_CHANS; i++) {
    chan = &hal_data->chans[i];

    // initialize POD entries
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x03, &chan->set_count_done_pdo_os, &chan->set_count_done_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x08, &chan->expol_stall_pdo_os, &chan->expol_stall_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x09, &chan->ina_pdo_os, &chan->ina_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x0a, &chan->inb_pdo_os, &chan->inb_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x1800 + (i << 2), 0x09, &chan->tx_toggle_pdo_os, &chan->tx_toggle_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x11, &chan->count_pdo_os, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x14, &chan->period_pdo_os, NULL);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000 + (i << 4), 0x03, &chan->set_count_pdo_os, &chan->set_count_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000 + (i << 4), 0x11, &chan->set_count_val_pdo_os, NULL);

    // export pins
    if ((err = hal_pin_bit_newf(HAL_IN, &(chan->index), comp_id, "%s.%s.%s.enc-%d-index", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-index failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IO, &(chan->index_ena), comp_id, "%s.%s.%s.enc-%d-index-enable", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-index-enable failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
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
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->expol_stall), comp_id, "%s.%s.%s.enc-%d-expol-stall", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-expol-stall failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_OUT, &(chan->tx_toggle), comp_id, "%s.%s.%s.enc-%d-tx-toggle", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-tx-toggle failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_bit_newf(HAL_IO, &(chan->set_raw_count), comp_id, "%s.%s.%s.enc-%d-set-raw-count", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-set-raw-count failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
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
    if ((err = hal_pin_u32_newf(HAL_OUT, &(chan->raw_period), comp_id, "%s.%s.%s.enc-%d-raw-period", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-raw-period failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_float_newf(HAL_OUT, &(chan->pos), comp_id, "%s.%s.%s.enc-%d-pos", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-pos failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_float_newf(HAL_OUT, &(chan->period), comp_id, "%s.%s.%s.enc-%d-period", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-period failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }
    if ((err = hal_pin_float_newf(HAL_IO, &(chan->pos_scale), comp_id, "%s.%s.%s.enc-%d-pos-scale", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.enc-%d-scale failed\n", LCEC_MODULE_NAME, master->name, slave->name, i);
      return err;
    }

    // initialize pins
    *(chan->index) = 0;
    *(chan->index_ena) = 0;
    *(chan->reset) = 0;
    *(chan->ina) = 0;
    *(chan->inb) = 0;
    *(chan->expol_stall) = 0;
    *(chan->tx_toggle) = 0;
    *(chan->set_raw_count) = 0;
    *(chan->set_raw_count_val) = 0;
    *(chan->raw_count) = 0;
    *(chan->raw_period) = 0;
    *(chan->count) = 0;
    *(chan->pos) = 0;
    *(chan->period) = 0;
    *(chan->pos_scale) = 1.0;

    // initialize variables
    chan->do_init = 1;
    chan->last_count = 0;
    chan->last_index = 0;
    chan->old_scale = *(chan->pos_scale) + 1.0;
    chan->scale = 1.0;
  }

  return 0;
}

void lcec_el5152_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el5152_data_t *hal_data = (lcec_el5152_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  int i, idx_flag;
  lcec_el5152_chan_t *chan;
  int32_t idx_count, raw_count, raw_delta;
  uint32_t raw_period;

  // wait for slave to be operational
  if (!slave->state.operational) {
    hal_data->last_operational = 0;
    return;
  }

  // check inputs
  for (i=0; i<LCEC_EL5152_CHANS; i++) {
    chan = &hal_data->chans[i];

    // check for change in scale value
    if (*(chan->pos_scale) != chan->old_scale) {
      // scale value has changed, test and update it
      if ((*(chan->pos_scale) < 1e-20) && (*(chan->pos_scale) > -1e-20)) {
        // value too small, divide by zero is a bad thing
        *(chan->pos_scale) = 1.0;
      }
      // save new scale to detect future changes
      chan->old_scale = *(chan->pos_scale);
      // we actually want the reciprocal
      chan->scale = 1.0 / *(chan->pos_scale);
    }

    // get bit states
    *(chan->ina) = EC_READ_BIT(&pd[chan->ina_pdo_os], chan->ina_pdo_bp);
    *(chan->inb) = EC_READ_BIT(&pd[chan->inb_pdo_os], chan->inb_pdo_bp);
    *(chan->expol_stall) = EC_READ_BIT(&pd[chan->expol_stall_pdo_os], chan->expol_stall_pdo_bp);
    *(chan->tx_toggle) = EC_READ_BIT(&pd[chan->tx_toggle_pdo_os], chan->tx_toggle_pdo_bp);

    // read raw values
    raw_count = EC_READ_S32(&pd[chan->count_pdo_os]);
    raw_period = EC_READ_U32(&pd[chan->period_pdo_os]);

    // check for operational change of slave
    if (!hal_data->last_operational) {
      chan->last_count = raw_count;
    }

    // check for counter set done
    if (EC_READ_BIT(&pd[chan->set_count_done_pdo_os], chan->set_count_done_pdo_bp)) {
      chan->last_count = raw_count;
      *(chan->set_raw_count) = 0;
    }

    // update raw values
    if (! *(chan->set_raw_count)) {
      *(chan->raw_count) = raw_count;
      *(chan->raw_period) = raw_period;
    }

    // check for index edge
    idx_flag = 0;
    idx_count = 0;
    if (*(chan->index) && !chan->last_index) {
      idx_count = raw_count;
      idx_flag = 1;
    }
    chan->last_index = *(chan->index);

    // handle initialization
    if (chan->do_init || *(chan->reset)) {
      chan->do_init = 0;
      chan->last_count = raw_count;
      *(chan->count) = 0;
      idx_flag = 0;
    }

    // handle index
    if (idx_flag && *(chan->index_ena)) {
      chan->last_count = idx_count;
      *(chan->count) = 0;
      *(chan->index_ena) = 0;
    }

    // compute net counts
    raw_delta = raw_count - chan->last_count;
    chan->last_count = raw_count;
    *(chan->count) += raw_delta;

    // scale count to make floating point position
    *(chan->pos) = *(chan->count) * chan->scale;

    // scale period
    *(chan->period) = ((double) (*(chan->raw_period))) * LCEC_EL5152_PERIOD_SCALE;
  }

  hal_data->last_operational = 1;
}

void lcec_el5152_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el5152_data_t *hal_data = (lcec_el5152_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  int i;
  lcec_el5152_chan_t *chan;

  // set outputs
  for (i=0; i<LCEC_EL5152_CHANS; i++) {
    chan = &hal_data->chans[i];

    // set output data
    EC_WRITE_BIT(&pd[chan->set_count_pdo_os], chan->set_count_pdo_bp, *(chan->set_raw_count));
    EC_WRITE_S32(&pd[chan->set_count_val_pdo_os], *(chan->set_raw_count_val));
  }
}

