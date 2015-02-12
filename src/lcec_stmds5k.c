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
#include "lcec_stmds5k.h"

#define STMDS5K_PCT_REG_FACTOR (0.5 * (double)0x7fff)
#define STMDS5K_PCT_REG_DIV    (2.0 / (double)0x7fff)
#define STMDS5K_TORQUE_DIV     (8.0 / (double)0x7fff)
#define STMDS5K_TORQUE_REF_DIV (0.01)
#define STMDS5K_RPM_FACTOR     (60.0)
#define STMDS5K_RPM_DIV        (1.0 / 60.0)

typedef struct {
  int do_init;

  long long pos_cnt;
  long long index_cnt;
  int32_t last_pos_cnt;

  hal_float_t *vel_cmd;
  hal_float_t *vel_fb;
  hal_float_t *vel_fb_rpm;
  hal_float_t *vel_fb_rpm_abs;
  hal_float_t *vel_rpm;
  hal_u32_t *pos_raw_hi;
  hal_u32_t *pos_raw_lo;
  hal_float_t *pos_fb;
  hal_float_t *torque_fb;
  hal_float_t *torque_fb_abs;
  hal_float_t *torque_fb_pct;
  hal_float_t *torque_lim;
  hal_bit_t *stopped;
  hal_bit_t *at_speed;
  hal_bit_t *overload;
  hal_bit_t *ready;
  hal_bit_t *error;
  hal_bit_t *toggle;
  hal_bit_t *loc_ena;
  hal_bit_t *enable;
  hal_bit_t *err_reset;
  hal_bit_t *fast_ramp;
  hal_bit_t *brake;
  hal_bit_t *index_ena;
  hal_bit_t *pos_reset;
  hal_s32_t *enc_raw;
  hal_bit_t *on_home_neg;
  hal_bit_t *on_home_pos;

  hal_float_t speed_max_rpm;
  hal_float_t speed_max_rpm_sp;
  hal_float_t torque_reference;
  hal_float_t pos_scale;
  hal_s32_t home_raw;
  double speed_max_rpm_sp_rcpt;

  double pos_scale_old;
  double pos_scale_rcpt;
  double pos_scale_cnt;

  int last_index_ena;
  int32_t index_ref;

  unsigned int dev_state_pdo_os;
  unsigned int speed_mot_pdo_os;
  unsigned int torque_mot_pdo_os;
  unsigned int speed_state_pdo_os;
  unsigned int pos_mot_pdo_os;
  unsigned int dev_ctrl_pdo_os;
  unsigned int speed_sp_rel_pdo_os;
  unsigned int torque_max_pdo_os;

  ec_sdo_request_t *sdo_torque_reference;
  ec_sdo_request_t *sdo_speed_max_rpm;
  ec_sdo_request_t *sdo_speed_max_rpm_sp;

} lcec_stmds5k_data_t;

void lcec_stmds5k_check_scales(lcec_stmds5k_data_t *hal_data);

void lcec_stmds5k_read(struct lcec_slave *slave, long period);
void lcec_stmds5k_write(struct lcec_slave *slave, long period);

int lcec_stmds5k_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_stmds5k_data_t *hal_data;
  int err;

  // initialize callbacks
  slave->proc_read = lcec_stmds5k_read;
  slave->proc_write = lcec_stmds5k_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_stmds5k_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_stmds5k_data_t));
  slave->hal_data = hal_data;

  // read sdos
  // B18 : torque reference
  if ((hal_data->sdo_torque_reference = lcec_read_sdo(slave, 0x2212, 0x00, 4)) == NULL) {
    return -EIO;
  }
  hal_data->torque_reference = (double)EC_READ_S32(ecrt_sdo_request_data(hal_data->sdo_torque_reference)) * STMDS5K_TORQUE_REF_DIV;
  // C01 : max rpm
  if ((hal_data->sdo_speed_max_rpm = lcec_read_sdo(slave, 0x2401, 0x00, 4)) == NULL) {
    return -EIO;
  }
  hal_data->speed_max_rpm = (double)EC_READ_S32(ecrt_sdo_request_data(hal_data->sdo_speed_max_rpm));
  // D02 : setpoint max rpm
  if ((hal_data->sdo_speed_max_rpm_sp = lcec_read_sdo(slave, 0x2602, 0x00, 4)) == NULL) {
    return -EIO;
  }
  hal_data->speed_max_rpm_sp = (double)EC_READ_S32(ecrt_sdo_request_data(hal_data->sdo_speed_max_rpm_sp));
  if (hal_data->speed_max_rpm_sp > 1e-20 || hal_data->speed_max_rpm_sp < -1e-20) {
    hal_data->speed_max_rpm_sp_rcpt = 1.0 / hal_data->speed_max_rpm_sp;
  } else {
    hal_data->speed_max_rpm_sp_rcpt = 0.0;
  }

  // initialize POD entries
  // E200 : device state byte
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x28c8, 0x00, &hal_data->dev_state_pdo_os, NULL);
  // E100 : speed motor (x 0.1% relative to C01)
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x2864, 0x00, &hal_data->speed_mot_pdo_os, NULL);
  // E02 : torque motor filterd (x 0,1 Nm)
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x2802, 0x00, &hal_data->torque_mot_pdo_os, NULL);
  // D200 : speed state word
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x26c8, 0x00, &hal_data->speed_state_pdo_os, NULL);
  // E09 : rotor position
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x2809, 0x00, &hal_data->pos_mot_pdo_os, NULL);
  // A180 : Device Control Byte
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x20b4, 0x00, &hal_data->dev_ctrl_pdo_os, NULL);
  // D230 : speed setpoint (x 0.1 % relative to D02, -200.0% .. 200.0%)
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x26e6, 0x00, &hal_data->speed_sp_rel_pdo_os, NULL);
  // C230 : maximum torque (x 1%, 0% .. 200%)
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x24e6, 0x00, &hal_data->torque_max_pdo_os, NULL);

  // export pins
  if ((err = hal_pin_float_newf(HAL_IN, &(hal_data->vel_cmd), comp_id, "%s.%s.%s.srv-vel-cmd", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-vel-cmd failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->vel_fb), comp_id, "%s.%s.%s.srv-vel-fb", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-vel-fb failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->vel_fb_rpm), comp_id, "%s.%s.%s.srv-vel-fb-rpm", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-vel-fb-rpm failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->vel_fb_rpm_abs), comp_id, "%s.%s.%s.srv-vel-fb-rpm-abs", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-vel-fb-rpm-abs failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->vel_rpm), comp_id, "%s.%s.%s.srv-vel-rpm", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-vel-rpm failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_s32_newf(HAL_OUT, &(hal_data->enc_raw), comp_id, "%s.%s.%s.srv-enc-raw", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-enc-raw failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_u32_newf(HAL_OUT, &(hal_data->pos_raw_hi), comp_id, "%s.%s.%s.srv-pos-raw-hi", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-pos-raw-hi failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_u32_newf(HAL_OUT, &(hal_data->pos_raw_lo), comp_id, "%s.%s.%s.srv-pos-raw-lo", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-pos-raw-lo failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->pos_fb), comp_id, "%s.%s.%s.srv-pos-fb", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-pos-fb failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->torque_fb), comp_id, "%s.%s.%s.srv-torque-fb", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-torque-fb failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->torque_fb_abs), comp_id, "%s.%s.%s.srv-torque-fb-abs", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-torque-fb-abs failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->torque_fb_pct), comp_id, "%s.%s.%s.srv-torque-fb-pct", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-torque-fb-pct failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_IN, &(hal_data->torque_lim), comp_id, "%s.%s.%s.srv-torque-lim", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-torque-lim failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->stopped), comp_id, "%s.%s.%s.srv-stopped", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-stopped failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->at_speed), comp_id, "%s.%s.%s.srv-at-speed", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-at-speed failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->overload), comp_id, "%s.%s.%s.srv-overload", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-overload failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->ready), comp_id, "%s.%s.%s.srv-ready", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-ready failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->error), comp_id, "%s.%s.%s.srv-error", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-error failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->toggle), comp_id, "%s.%s.%s.srv-toggle", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-toggle failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->loc_ena), comp_id, "%s.%s.%s.srv-loc-ena", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-loc-ena failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->enable), comp_id, "%s.%s.%s.srv-enable", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-enable failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->err_reset), comp_id, "%s.%s.%s.srv-err-reset", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-err-reset failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->fast_ramp), comp_id, "%s.%s.%s.srv-fast-ramp", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-fast-ramp failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->brake), comp_id, "%s.%s.%s.srv-brake", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-brake failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IO, &(hal_data->index_ena), comp_id, "%s.%s.%s.srv-index-ena", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-index-ena failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->pos_reset), comp_id, "%s.%s.%s.srv-pos-reset", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-pos-reset failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->on_home_neg), comp_id, "%s.%s.%s.srv-on-home-neg", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-on-home-neg failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->on_home_pos), comp_id, "%s.%s.%s.srv-on-home-pos", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-on-home-pos failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }

  // export parameters
  if ((err = hal_param_float_newf(HAL_RW, &(hal_data->pos_scale), comp_id, "%s.%s.%s.srv-pos-scale", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-pos-scale failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_param_float_newf(HAL_RO, &(hal_data->torque_reference), comp_id, "%s.%s.%s.srv-torque-ref", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-torque-ref failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_param_float_newf(HAL_RO, &(hal_data->speed_max_rpm), comp_id, "%s.%s.%s.srv-max-rpm", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-max-rpm failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_param_float_newf(HAL_RO, &(hal_data->speed_max_rpm_sp), comp_id, "%s.%s.%s.srv-max-rpm-sp", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-max-rpm-sp failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_param_s32_newf(HAL_RW, &(hal_data->home_raw), comp_id, "%s.%s.%s.srv-home-raw", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-home-raw failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }

  // set default pin values
  *(hal_data->vel_cmd) = 0.0;
  *(hal_data->vel_fb) = 0.0;
  *(hal_data->vel_fb_rpm) = 0.0;
  *(hal_data->vel_fb_rpm_abs) = 0.0;
  *(hal_data->vel_rpm) = 0.0;
  *(hal_data->pos_raw_hi) = 0;
  *(hal_data->pos_raw_lo) = 0;
  *(hal_data->pos_fb) = 0.0;
  *(hal_data->torque_fb) = 0.0;
  *(hal_data->torque_fb_abs) = 0.0;
  *(hal_data->torque_fb_pct) = 0.0;
  *(hal_data->torque_lim) = 1.0;
  *(hal_data->stopped) = 0;
  *(hal_data->at_speed) = 0;
  *(hal_data->overload) = 0;
  *(hal_data->ready) = 0;
  *(hal_data->error) = 0;
  *(hal_data->toggle) = 0;
  *(hal_data->loc_ena) = 0;
  *(hal_data->enable) = 0;
  *(hal_data->err_reset) = 0;
  *(hal_data->fast_ramp) = 0;
  *(hal_data->brake) = 0;
  *(hal_data->index_ena) = 0;
  *(hal_data->pos_reset) = 0;
  *(hal_data->enc_raw) = 0;
  *(hal_data->on_home_neg) = 0;
  *(hal_data->on_home_pos) = 0;

  // initialize variables
  hal_data->pos_scale = 1.0;
  hal_data->do_init = 1;
  hal_data->pos_cnt = 0;
  hal_data->index_cnt = 0;
  hal_data->last_pos_cnt = 0;
  hal_data->pos_scale_old = hal_data->pos_scale + 1.0;
  hal_data->pos_scale_rcpt = 1.0;
  hal_data->pos_scale_cnt = 1.0;
  hal_data->last_index_ena = 0;
  hal_data->index_ref = 0;
  hal_data->home_raw = 0;

  return 0;
}

void lcec_stmds5k_check_scales(lcec_stmds5k_data_t *hal_data) {
  // check for change in scale value
  if (hal_data->pos_scale != hal_data->pos_scale_old) {
    // scale value has changed, test and update it
    if ((hal_data->pos_scale < 1e-20) && (hal_data->pos_scale > -1e-20)) {
      // value too small, divide by zero is a bad thing
      hal_data->pos_scale = 1.0;
    }
    // save new scale to detect future changes
    hal_data->pos_scale_old = hal_data->pos_scale;
    // we actually want the reciprocal
    hal_data->pos_scale_rcpt = 1.0 / hal_data->pos_scale;
    // scale for counter
    hal_data->pos_scale_cnt = hal_data->pos_scale_rcpt / (double)(0x100000000LL);
  }
}

void lcec_stmds5k_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_stmds5k_data_t *hal_data = (lcec_stmds5k_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  int32_t index_tmp;
  int32_t pos_cnt, pos_cnt_diff;
  long long net_count;
  uint8_t dev_state;
  uint16_t speed_state;
  int16_t speed_raw, torque_raw;
  double rpm, torque;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  // check for change in scale value
  lcec_stmds5k_check_scales(hal_data);

  // read device state
  dev_state = EC_READ_U8(&pd[hal_data->dev_state_pdo_os]);
  *(hal_data->ready) = (dev_state >> 0) & 0x01;
  *(hal_data->error) = (dev_state >> 1) & 0x01;
  *(hal_data->loc_ena) = (dev_state >> 6) & 0x01;
  *(hal_data->toggle) = (dev_state >> 7) & 0x01;

  // read speed state
  speed_state = EC_READ_U16(&pd[hal_data->speed_state_pdo_os]);
  *(hal_data->stopped) = (speed_state >> 0) & 0x01;
  *(hal_data->at_speed) = (speed_state >> 1) & 0x01;
  *(hal_data->overload) = (speed_state >> 2) & 0x01;

  // read current speed
  speed_raw = EC_READ_S16(&pd[hal_data->speed_mot_pdo_os]);
  rpm = hal_data->speed_max_rpm * (double)speed_raw * STMDS5K_PCT_REG_DIV;
  *(hal_data->vel_fb_rpm) = rpm;
  *(hal_data->vel_fb_rpm_abs) = fabs(rpm);
  *(hal_data->vel_fb) = rpm * STMDS5K_RPM_DIV * hal_data->pos_scale_rcpt;

  // read torque
  // E02 : torque motor filterd (x 0,1 Nm)
  torque_raw = EC_READ_S16(&pd[hal_data->torque_mot_pdo_os]);
  torque = (double)torque_raw * STMDS5K_TORQUE_DIV;
  *(hal_data->torque_fb_pct) = fabs(torque * 100.0);
  torque = torque * hal_data->torque_reference;
  *(hal_data->torque_fb) = torque;
  *(hal_data->torque_fb_abs) = fabs(torque);

  // update raw position counter
  pos_cnt = EC_READ_S32(&pd[hal_data->pos_mot_pdo_os]);
  *(hal_data->enc_raw) = pos_cnt;
  *(hal_data->on_home_neg) = ((pos_cnt - hal_data->home_raw) <= 0);
  *(hal_data->on_home_pos) = ((pos_cnt - hal_data->home_raw) >= 0);
  pos_cnt <<= 8;
  pos_cnt_diff = pos_cnt - hal_data->last_pos_cnt;
  hal_data->last_pos_cnt = pos_cnt;
  hal_data->pos_cnt += pos_cnt_diff;

  // check for index edge
  if (*(hal_data->index_ena)) {
    index_tmp = (hal_data->pos_cnt >> 32) & 0xffffffff;
    if (hal_data->do_init || !hal_data->last_index_ena) {
      hal_data->index_ref = index_tmp;
    } else if (index_tmp > hal_data->index_ref) {
      hal_data->index_cnt = (long long)index_tmp << 32;
      *(hal_data->index_ena) = 0;
    } else if (index_tmp  < hal_data->index_ref) {
      hal_data->index_cnt = (long long)hal_data->index_ref << 32;
      *(hal_data->index_ena) = 0;
    }
  }
  hal_data->last_index_ena = *(hal_data->index_ena);

  // handle initialization
  if (hal_data->do_init || *(hal_data->pos_reset)) {
    hal_data->do_init = 0;
    hal_data->index_cnt = hal_data->pos_cnt;
  }

  // compute net counts
  net_count = hal_data->pos_cnt - hal_data->index_cnt;

  // update raw counter pins
  *(hal_data->pos_raw_hi) = (net_count >> 32) & 0xffffffff;
  *(hal_data->pos_raw_lo) = net_count & 0xffffffff;

  // scale count to make floating point position
  *(hal_data->pos_fb) = net_count * hal_data->pos_scale_cnt;
}

void lcec_stmds5k_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_stmds5k_data_t *hal_data = (lcec_stmds5k_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  uint8_t dev_ctrl;
  double speed_raw, torque_raw;

  // check for change in scale value
  lcec_stmds5k_check_scales(hal_data);

  // write dev ctrl
  dev_ctrl = 0;
  if (*(hal_data->enable)) {
    dev_ctrl |= (1 << 0);
  }
  if (*(hal_data->err_reset)) {
    dev_ctrl |= (1 << 1);
  }
  if (*(hal_data->fast_ramp)) {
    dev_ctrl |= (1 << 2);
  }
  if (*(hal_data->brake)) {
    dev_ctrl |= (1 << 6);
  }
  if (! *(hal_data->toggle)) {
    dev_ctrl |= (1 << 7);
  }
  EC_WRITE_U8(&pd[hal_data->dev_ctrl_pdo_os], dev_ctrl);

  // set maximum torque
  if (*(hal_data->torque_lim) > 2.0) {
    *(hal_data->torque_lim) = 2.0;
  }
  if (*(hal_data->torque_lim) < -2.0) {
    *(hal_data->torque_lim) = -2.0;
  }
  torque_raw = *(hal_data->torque_lim) * STMDS5K_PCT_REG_FACTOR;
  if (torque_raw > (double)0x7fff) {
    torque_raw = (double)0x7fff;
  }
  if (torque_raw < (double)-0x7fff) {
    torque_raw = (double)-0x7fff;
  }
  EC_WRITE_S16(&pd[hal_data->torque_max_pdo_os], (int16_t)torque_raw);

  // calculate rpm command
  *(hal_data->vel_rpm) = *(hal_data->vel_cmd) * hal_data->pos_scale * STMDS5K_RPM_FACTOR;

  // set RPM
  if (*(hal_data->vel_rpm) > hal_data->speed_max_rpm) {
    *(hal_data->vel_rpm) = hal_data->speed_max_rpm;
  }
  if (*(hal_data->vel_rpm) < -hal_data->speed_max_rpm) {
    *(hal_data->vel_rpm) = -hal_data->speed_max_rpm;
  }
  speed_raw = *(hal_data->vel_rpm) * hal_data->speed_max_rpm_sp_rcpt * STMDS5K_PCT_REG_FACTOR;
  if (speed_raw > (double)0x7fff) {
    speed_raw = (double)0x7fff;
  }
  if (speed_raw < (double)-0x7fff) {
    speed_raw = (double)-0x7fff;
  }
  if (! *(hal_data->enable)) {
    speed_raw = 0.0;
  }
  EC_WRITE_S16(&pd[hal_data->speed_sp_rel_pdo_os], (int16_t)speed_raw);
}

