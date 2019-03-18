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

#include "hal.h"

#include "lcec.h"
#include "lcec_el7211.h"

#include "lcec_class_enc.h"

#define FAULT_RESET_PERIOD_NS  100000000

typedef struct {
  hal_bit_t *enable;
  hal_bit_t *enabled;
  hal_bit_t *fault;

  hal_bit_t *status_ready;
  hal_bit_t *status_switched_on;
  hal_bit_t *status_operation;
  hal_bit_t *status_fault;
  hal_bit_t *status_disabled;
  hal_bit_t *status_warning;
  hal_bit_t *status_limit_active;

  hal_float_t *vel_cmd;
  hal_float_t *vel_fb;
  hal_float_t *vel_fb_rpm;
  hal_float_t *vel_fb_rpm_abs;

  hal_s32_t *vel_fb_raw;

  hal_float_t scale;

  hal_u32_t vel_resolution;
  hal_u32_t pos_resolution;

  lcec_class_enc_data_t enc;

  unsigned int pos_fb_pdo_os;
  unsigned int status_pdo_os;
  unsigned int vel_fb_pdo_os;
  unsigned int ctrl_pdo_os;
  unsigned int vel_cmd_pdo_os;

  double vel_scale;
  double vel_rcpt;

  double scale_old;
  double scale_rcpt;
  double vel_out_scale;

  long fault_reset_timer;

} lcec_el7211_data_t;

static const lcec_pindesc_t slave_pins[] = {
  { HAL_BIT, HAL_IN, offsetof(lcec_el7211_data_t, enable), "%s.%s.%s.enable" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el7211_data_t, enabled), "%s.%s.%s.enabled" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el7211_data_t, fault), "%s.%s.%s.fault" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el7211_data_t, status_ready), "%s.%s.%s.status-ready" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el7211_data_t, status_switched_on), "%s.%s.%s.status-switched-on" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el7211_data_t, status_operation), "%s.%s.%s.status-operation" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el7211_data_t, status_fault), "%s.%s.%s.status-fault" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el7211_data_t, status_disabled), "%s.%s.%s.status-disabled" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el7211_data_t, status_warning), "%s.%s.%s.status-warning" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el7211_data_t, status_limit_active), "%s.%s.%s.status-limit-active" },
  { HAL_FLOAT, HAL_IN, offsetof(lcec_el7211_data_t, vel_cmd), "%s.%s.%s.velo-cmd" },
  { HAL_FLOAT, HAL_OUT, offsetof(lcec_el7211_data_t, vel_fb), "%s.%s.%s.velo-fb" },
  { HAL_FLOAT, HAL_OUT, offsetof(lcec_el7211_data_t, vel_fb_rpm), "%s.%s.%s.velo-fb-rpm" },
  { HAL_FLOAT, HAL_OUT, offsetof(lcec_el7211_data_t, vel_fb_rpm_abs), "%s.%s.%s.velo-fb-rpm-abs" },
  { HAL_S32, HAL_OUT, offsetof(lcec_el7211_data_t, vel_fb_raw), "%s.%s.%s.velo-fb-raw" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static const lcec_pindesc_t slave_params[] = {
  { HAL_FLOAT, HAL_RW, offsetof(lcec_el7211_data_t, scale), "%s.%s.%s.scale" },
  { HAL_U32, HAL_RO, offsetof(lcec_el7211_data_t, vel_resolution), "%s.%s.%s.vel-resolution" },
  { HAL_U32, HAL_RO, offsetof(lcec_el7211_data_t, pos_resolution), "%s.%s.%s.pos-resolution" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static ec_pdo_entry_info_t lcec_el7211_in_pos[] = {
   {0x6000, 0x11, 32}  // actual position
};

static ec_pdo_entry_info_t lcec_el7211_in_status[] = {
   {0x6010, 0x01, 16}  // status word
};

static ec_pdo_entry_info_t lcec_el7211_in_vel[] = {
   {0x6010, 0x07, 32}  // actual velocity
};

static ec_pdo_entry_info_t lcec_el7211_out_ctrl[] = {
   {0x7010, 0x01, 16}  // control word
};

static ec_pdo_entry_info_t lcec_el7211_out_cmd[] = {
   {0x7010, 0x06, 32}  // velocity command
};

static ec_pdo_info_t lcec_el7211_pdos_in[] = {
    {0x1A00, 1, lcec_el7211_in_pos},
    {0x1A01, 1, lcec_el7211_in_status},
    {0x1A02, 1, lcec_el7211_in_vel},
};

static ec_pdo_info_t lcec_el7211_pdos_out[] = {
    {0x1600, 1, lcec_el7211_out_ctrl},
    {0x1601, 1, lcec_el7211_out_cmd},
};

static ec_sync_info_t lcec_el7211_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL},
    {1, EC_DIR_INPUT,  0, NULL},
    {2, EC_DIR_OUTPUT, 2, lcec_el7211_pdos_out},
    {3, EC_DIR_INPUT,  3, lcec_el7211_pdos_in},
    {0xff}
};


void lcec_el7211_check_scales(lcec_el7211_data_t *hal_data);
void lcec_el7211_read(struct lcec_slave *slave, long period);
void lcec_el7211_write(struct lcec_slave *slave, long period);

int lcec_el7211_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el7211_data_t *hal_data;
  int err;
  uint8_t sdo_buf[4];
  uint32_t sdo_vel_resolution;
  uint32_t sdo_pos_resolution;

  // initialize callbacks
  slave->proc_read = lcec_el7211_read;
  slave->proc_write = lcec_el7211_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el7211_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el7211_data_t));
  slave->hal_data = hal_data;

  // read sdos
  if (lcec_read_sdo(slave, 0x9010, 0x14, sdo_buf, 4)) {
    return -EIO;
  }
  sdo_vel_resolution = EC_READ_U32(sdo_buf);
  if (lcec_read_sdo(slave, 0x9010, 0x15, sdo_buf, 4)) {
    return -EIO;
  }
  sdo_pos_resolution = EC_READ_U32(sdo_buf);

  // initialize sync info
  slave->sync_info = lcec_el7211_syncs;

  // initialize POD entries
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x11, &hal_data->pos_fb_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6010, 0x01, &hal_data->status_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6010, 0x07, &hal_data->vel_fb_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7010, 0x01, &hal_data->ctrl_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7010, 0x06, &hal_data->vel_cmd_pdo_os, NULL);

  // export pins
  if ((err = lcec_pin_newf_list(hal_data, slave_pins, LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    return err;
  }

  // export parameters
  if ((err = lcec_param_newf_list(hal_data, slave_params, LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    return err;
  }

  // init subclasses
  if ((err = class_enc_init(slave, &hal_data->enc, 32, "enc")) != 0) {
    return err;
  }

  // init parameters
  hal_data->scale = 1.0;
  hal_data->vel_resolution = sdo_vel_resolution;
  hal_data->pos_resolution = sdo_pos_resolution;

  // initialize variables
  if (sdo_vel_resolution > 0) {
    hal_data->vel_scale = (double) sdo_vel_resolution;
    hal_data->vel_rcpt = 1.0 / ((double) sdo_vel_resolution);
  } else {
    hal_data->vel_scale = 0.0;
    hal_data->vel_rcpt = 0.0;
  }
  hal_data->scale_old = hal_data->scale + 1.0;
  hal_data->scale_rcpt = 0.0;
  hal_data->vel_out_scale = 0.0;
  hal_data->fault_reset_timer = 0;

  return 0;
}

void lcec_el7211_check_scales(lcec_el7211_data_t *hal_data) {
  // check for change in scale value
  if (hal_data->scale != hal_data->scale_old) {
    // scale value has changed, test and update it
    if ((hal_data->scale < 1e-20) && (hal_data->scale > -1e-20)) {
      // value too small, divide by zero is a bad thing
      hal_data->scale = 1.0;
    }
    // save new scale to detect future changes
    hal_data->scale_old = hal_data->scale;
    // we actually want the reciprocal
    hal_data->scale_rcpt = 1.0 / hal_data->scale;
    // calculate velo output scale
    hal_data->vel_out_scale = hal_data->vel_scale * hal_data->scale;
  }
}

void lcec_el7211_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el7211_data_t *hal_data = (lcec_el7211_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  uint16_t status;
  int32_t vel_raw;
  double vel;
  uint32_t pos_cnt;

  // wait for slave to be operational
  if (!slave->state.operational) {
    *(hal_data->status_ready) = 0;
    *(hal_data->status_switched_on) = 0;
    *(hal_data->status_operation) = 0;
    *(hal_data->status_fault) = 1;
    *(hal_data->status_disabled) = 0;
    *(hal_data->status_warning) = 0;
    *(hal_data->status_limit_active) = 0;
    *(hal_data->enabled) = 0;
    return;
  }

  // check for change in scale value
  lcec_el7211_check_scales(hal_data);

  // read status word
  status = EC_READ_U16(&pd[hal_data->status_pdo_os]);
  *(hal_data->status_ready) = (status >> 0) & 0x01;
  *(hal_data->status_switched_on) = (status >> 1) & 0x01;
  *(hal_data->status_operation) = (status >> 2) & 0x01;
  *(hal_data->status_fault) = (status >> 3) & 0x01;
  *(hal_data->status_disabled) = (status >> 6) & 0x01;
  *(hal_data->status_warning) = (status >> 7) & 0x01;
  *(hal_data->status_limit_active) = (status >> 11) & 0x01;

  *(hal_data->enabled) = *(hal_data->status_ready) && *(hal_data->status_switched_on) && *(hal_data->status_operation);
  *(hal_data->fault) = 0;
  if (*(hal_data->enable) && *(hal_data->status_fault)) {
    if (hal_data->fault_reset_timer > 0) {
      hal_data->fault_reset_timer -= period;
    } else {
      *(hal_data->fault) = 1;
    }
  } else {
    hal_data->fault_reset_timer = FAULT_RESET_PERIOD_NS;
  }

  // read velocity
  vel_raw = EC_READ_S32(&pd[hal_data->vel_fb_pdo_os]);
  *(hal_data->vel_fb_raw) = vel_raw;
  vel = ((double) vel_raw) * hal_data->vel_rcpt;
  *(hal_data->vel_fb) = vel * hal_data->scale_rcpt;
  vel = vel * 60.0;
  *(hal_data->vel_fb_rpm) = vel;
  *(hal_data->vel_fb_rpm_abs) = fabs(vel);

  // update position feedback
  pos_cnt = EC_READ_U32(&pd[hal_data->pos_fb_pdo_os]);
  class_enc_update(&hal_data->enc, hal_data->pos_resolution, hal_data->scale_rcpt, pos_cnt, 0, 0);
}

void lcec_el7211_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el7211_data_t *hal_data = (lcec_el7211_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  uint16_t control;
  double velo_raw;

  // check for change in scale value
  lcec_el7211_check_scales(hal_data);

  control = 0;
  if (*(hal_data->enable)) {
    if (*(hal_data->status_fault)) {
      control = 0x80;
    } else if (*(hal_data->status_disabled)) {
      control = 0x06;
    } else if (*(hal_data->status_ready)) {
      control = 0x07;
      if (*(hal_data->status_switched_on)) {
        control = 0x0f;
      }
    }
  }
  EC_WRITE_U16(&pd[hal_data->ctrl_pdo_os], control);

  // set velocity
  velo_raw = *(hal_data->vel_cmd) * hal_data->vel_out_scale;
  if (velo_raw > (double)0x7fffffff) {
    velo_raw = (double)0x7fffffff;
  }
  if (velo_raw < (double)-0x7fffffff) {
    velo_raw = (double)-0x7fffffff;
  }
  EC_WRITE_S32(&pd[hal_data->vel_cmd_pdo_os], (int32_t)velo_raw);

}

