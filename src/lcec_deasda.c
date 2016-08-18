//
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

#include "lcec.h"
#include "lcec_deasda.h"
#include "math.h"

#define DEASDA_PULSES_PER_REV_DEFLT (1280000)
#define DEASDA_RPM_FACTOR           (0.1)
#define DEASDA_RPM_RCPT             (1.0 / DEASDA_RPM_FACTOR)
#define DEASDA_RPM_MUL              (60.0)
#define DEASDA_RPM_DIV              (1.0 / 60.0)

#define DEASDA_FAULT_AUTORESET_CYCLES  100
#define DEASDA_FAULT_AUTORESET_RETRIES 3

typedef struct {
  int do_init;

  long long pos_cnt;
  int32_t last_pos_cnt;

  long long extenc_cnt;
  int32_t last_extenc_cnt;

  hal_float_t *vel_fb;
  hal_float_t *vel_fb_rpm;
  hal_float_t *vel_fb_rpm_abs;
  hal_float_t *vel_rpm;
  hal_bit_t *ready;
  hal_bit_t *switched_on;
  hal_bit_t *oper_enabled;
  hal_bit_t *fault;
  hal_bit_t *volt_enabled;
  hal_bit_t *quick_stoped;
  hal_bit_t *on_disabled;
  hal_bit_t *warning;
  hal_bit_t *remote;
  hal_bit_t *at_speed;
  hal_bit_t *limit_active;
  hal_bit_t *zero_speed;
  hal_s32_t *enc_raw;
  hal_s32_t *extenc_raw;
  hal_u32_t *pos_raw_hi;
  hal_u32_t *pos_raw_lo;
  hal_u32_t *extenc_raw_hi;
  hal_u32_t *extenc_raw_lo;
  hal_float_t *pos_fb;
  hal_float_t *pos_extenc;
  hal_bit_t *on_home_neg;
  hal_bit_t *on_home_pos;
  hal_bit_t *pos_reset;
  hal_bit_t *switch_on;
  hal_bit_t *enable_volt;
  hal_bit_t *quick_stop;
  hal_bit_t *enable;
  hal_bit_t *fault_reset;
  hal_bit_t *halt;
  hal_float_t *vel_cmd;

  hal_float_t pos_scale;
  hal_float_t extenc_scale;
  hal_u32_t pprev;
  hal_s32_t home_raw;
  hal_u32_t fault_autoreset_cycles;
  hal_u32_t fault_autoreset_retries;

  hal_float_t pos_scale_old;
  hal_u32_t pprev_old;
  double pos_scale_rcpt;
  double pos_scale_cnt;

  unsigned int status_pdo_os;
  unsigned int currpos_pdo_os;
  unsigned int currvel_pdo_os;
  unsigned int extenc_pdo_os;
  unsigned int control_pdo_os;
  unsigned int cmdvel_pdo_os;

  hal_bit_t last_switch_on;
  hal_bit_t internal_fault;

  hal_u32_t fault_reset_retry;
  hal_u32_t fault_reset_state;
  hal_u32_t fault_reset_cycle;

} lcec_deasda_data_t;


static ec_pdo_entry_info_t lcec_deasda_in[] = {
   {0x6041, 0x00, 16}, // Status Word
   {0x606C, 0x00, 32}, // Current Velocity
   {0x6064, 0x00, 32}, // Current Position
   {0x2511, 0x00, 32}  // external encoder
};

static ec_pdo_entry_info_t lcec_deasda_out[] = {
   {0x6040, 0x00, 16}, // Control Word
   {0x60FF, 0x00, 32}  // Target Velocity
};

static ec_pdo_info_t lcec_deasda_pdos_out[] = {
    {0x1602,  2, lcec_deasda_out}
};

static ec_pdo_info_t lcec_deasda_pdos_in[] = {
    {0x1a02, 4, lcec_deasda_in}
};

static ec_sync_info_t lcec_deasda_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL},
    {1, EC_DIR_INPUT,  0, NULL},
    {2, EC_DIR_OUTPUT, 1, lcec_deasda_pdos_out},
    {3, EC_DIR_INPUT,  1, lcec_deasda_pdos_in},
    {0xff}
};

void lcec_deasda_check_scales(lcec_deasda_data_t *hal_data);

void lcec_deasda_read(struct lcec_slave *slave, long period);
void lcec_deasda_write(struct lcec_slave *slave, long period);

int lcec_deasda_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_deasda_data_t *hal_data;
  int err;
  uint32_t tu;
  int8_t ti;

  // initialize callbacks
  slave->proc_read = lcec_deasda_read;
  slave->proc_write = lcec_deasda_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_deasda_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_deasda_data_t));
  slave->hal_data = hal_data;

  // set to cyclic synchronous velocity mode
  if (ecrt_slave_config_sdo8(slave->config, 0x6060, 0x00, 9) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s sdo velo mode\n", master->name, slave->name);
  }

  // set interpolation time period
  tu = master->app_time_period;
  ti = -9;
  while ((tu % 10) == 0 || tu > 255) { tu /=  10; ti++; }
  if (ecrt_slave_config_sdo8(slave->config, 0x60C2, 0x01, (uint8_t)tu) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s sdo ipol time period units\n", master->name, slave->name);
  }
  if (ecrt_slave_config_sdo8(slave->config, 0x60C2, 0x02, ti) != 0) {
    rtapi_print_msg (RTAPI_MSG_ERR, LCEC_MSG_PFX "fail to configure slave %s.%s sdo ipol time period index\n", master->name, slave->name);
  }

  // initialize sync info
  slave->sync_info = lcec_deasda_syncs;

  // initialize POD entries
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6041, 0x00, &hal_data->status_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x606C, 0x00, &hal_data->currvel_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6064, 0x00, &hal_data->currpos_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x2511, 0x00, &hal_data->extenc_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6040, 0x00, &hal_data->control_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x60FF, 0x00, &hal_data->cmdvel_pdo_os, NULL);

  // export pins
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
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->ready), comp_id, "%s.%s.%s.srv-ready", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-ready failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->switched_on), comp_id, "%s.%s.%s.srv-switched-on", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-switched-on failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->oper_enabled), comp_id, "%s.%s.%s.srv-oper-enabled", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-oper-enabled failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->fault), comp_id, "%s.%s.%s.srv-fault", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-fault failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->volt_enabled), comp_id, "%s.%s.%s.srv-volt-enabled", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-volt-enabled failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->quick_stoped), comp_id, "%s.%s.%s.srv-quick-stoped", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-quick-stoped failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->on_disabled), comp_id, "%s.%s.%s.srv-on-disabled", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-on-disabled failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->warning), comp_id, "%s.%s.%s.srv-warning", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-warning failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->remote), comp_id, "%s.%s.%s.srv-remote", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-remote failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->at_speed), comp_id, "%s.%s.%s.srv-at-speed", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-at-speed failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->limit_active), comp_id, "%s.%s.%s.srv-limit-active", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-limit-active failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_OUT, &(hal_data->zero_speed), comp_id, "%s.%s.%s.srv-zero-speed", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-zero-speed failed\n", LCEC_MODULE_NAME, master->name, slave->name);
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
  if ((err = hal_pin_s32_newf(HAL_OUT, &(hal_data->extenc_raw), comp_id, "%s.%s.%s.srv-extenc-raw", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-extenc-raw failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_u32_newf(HAL_OUT, &(hal_data->extenc_raw_hi), comp_id, "%s.%s.%s.srv-extenc-raw-hi", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-extenc-raw-hi failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_u32_newf(HAL_OUT, &(hal_data->extenc_raw_lo), comp_id, "%s.%s.%s.srv-extenc-raw-lo", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-extenc-raw-lo failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->pos_fb), comp_id, "%s.%s.%s.srv-pos-fb", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-pos-fb failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_OUT, &(hal_data->pos_extenc), comp_id, "%s.%s.%s.srv-pos-extenc", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-pos-extenc failed\n", LCEC_MODULE_NAME, master->name, slave->name);
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
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->pos_reset), comp_id, "%s.%s.%s.srv-pos-reset", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-pos-reset failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->switch_on), comp_id, "%s.%s.%s.srv-switch-on", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-switch-on failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->enable_volt), comp_id, "%s.%s.%s.srv-enable-volt", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-enable-volt failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->quick_stop), comp_id, "%s.%s.%s.srv-quick-stop", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-quick-stop failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->enable), comp_id, "%s.%s.%s.srv-enable", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-enable failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->fault_reset), comp_id, "%s.%s.%s.srv-fault-reset", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-fault-reset failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_bit_newf(HAL_IN, &(hal_data->halt), comp_id, "%s.%s.%s.srv-halt", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-halt failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_pin_float_newf(HAL_IN, &(hal_data->vel_cmd), comp_id, "%s.%s.%s.srv-vel-cmd", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s.%s.%s.srv-vel-cmd failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }

  // export parameters
  if ((err = hal_param_float_newf(HAL_RW, &(hal_data->pos_scale), comp_id, "%s.%s.%s.srv-pos-scale", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting param %s.%s.%s.srv-pos-scale failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_param_float_newf(HAL_RW, &(hal_data->extenc_scale), comp_id, "%s.%s.%s.srv-extenc-scale", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting param %s.%s.%s.srv-extenc-scale failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_param_u32_newf(HAL_RW, &(hal_data->pprev), comp_id, "%s.%s.%s.srv-pulses-per-rev", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting param %s.%s.%s.srv-pulses-per-rev failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_param_s32_newf(HAL_RW, &(hal_data->home_raw), comp_id, "%s.%s.%s.srv-home-raw", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting param %s.%s.%s.srv-home-raw failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_param_u32_newf(HAL_RW, &(hal_data->fault_autoreset_cycles), comp_id, "%s.%s.%s.srv-fault-autoreset-cycles", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting param %s.%s.%s.srv-fault-autoreset-cycles failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }
  if ((err = hal_param_u32_newf(HAL_RW, &(hal_data->fault_autoreset_retries), comp_id, "%s.%s.%s.srv-fault-autoreset-retries", LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting param %s.%s.%s.srv-fault-autoreset-retries failed\n", LCEC_MODULE_NAME, master->name, slave->name);
    return err;
  }

  // set default pin values
  *(hal_data->vel_fb) = 0.0;
  *(hal_data->vel_fb_rpm) = 0.0;
  *(hal_data->vel_fb_rpm_abs) = 0.0;
  *(hal_data->vel_rpm) = 0.0;
  *(hal_data->ready) = 0;
  *(hal_data->switched_on) = 0;
  *(hal_data->oper_enabled) = 0;
  *(hal_data->fault) = 0;
  *(hal_data->volt_enabled) = 0;
  *(hal_data->quick_stoped) = 0;
  *(hal_data->on_disabled) = 0;
  *(hal_data->warning) = 0;
  *(hal_data->remote) = 0;
  *(hal_data->at_speed) = 0;
  *(hal_data->limit_active) = 0;
  *(hal_data->zero_speed) = 0;
  *(hal_data->enc_raw) = 0;
  *(hal_data->pos_raw_hi) = 0;
  *(hal_data->pos_raw_lo) = 0;
  *(hal_data->extenc_raw) = 0;
  *(hal_data->extenc_raw_hi) = 0;
  *(hal_data->extenc_raw_lo) = 0;
  *(hal_data->pos_fb) = 0.0;
  *(hal_data->pos_extenc) = 0.0;
  *(hal_data->on_home_neg) = 0;
  *(hal_data->on_home_pos) = 0;
  *(hal_data->pos_reset) = 0;
  *(hal_data->switch_on) = 0;
  *(hal_data->enable_volt) = 0;
  *(hal_data->quick_stop) = 0;
  *(hal_data->enable) = 0;
  *(hal_data->fault_reset) = 0;
  *(hal_data->halt) = 0;
  *(hal_data->vel_cmd) = 0.0;

  // initialize variables
  hal_data->do_init = 1;
  hal_data->pos_scale = 1.0;
  hal_data->extenc_scale = 1.0;
  hal_data->home_raw = 0;
  hal_data->fault_autoreset_cycles = DEASDA_FAULT_AUTORESET_CYCLES;
  hal_data->fault_autoreset_retries = DEASDA_FAULT_AUTORESET_RETRIES;
  hal_data->pos_cnt = 0;
  hal_data->last_pos_cnt = 0;
  hal_data->extenc_cnt = 0;
  hal_data->last_extenc_cnt = 0;
  hal_data->pos_scale_old = hal_data->pos_scale + 1.0;
  hal_data->pos_scale_rcpt = 1.0;
  hal_data->pos_scale_cnt = 1.0;
  hal_data->pprev = DEASDA_PULSES_PER_REV_DEFLT;
  hal_data->pprev_old = hal_data->pprev + 1;
  hal_data->last_switch_on = 0;
  hal_data->internal_fault = 0;

  hal_data->fault_reset_retry = 0;
  hal_data->fault_reset_state = 0;
  hal_data->fault_reset_cycle = 0;

  return 0;
}

void lcec_deasda_check_scales(lcec_deasda_data_t *hal_data) {
  // check for change in scale value
  if (hal_data->pos_scale != hal_data->pos_scale_old || hal_data->pprev != hal_data->pprev_old) {

    // scale value has changed, test and update it
    if ((hal_data->pos_scale < 1e-20) && (hal_data->pos_scale > -1e-20)) {
      // value too small, divide by zero is a bad thing
      hal_data->pos_scale = 1.0;
    }

    // save new scale to detect future changes
    hal_data->pos_scale_old = hal_data->pos_scale;
    hal_data->pprev_old = hal_data->pprev;

    // we actually want the reciprocal
    hal_data->pos_scale_rcpt = 1.0 / hal_data->pos_scale;

    // scale for counter
    hal_data->pos_scale_cnt = hal_data->pos_scale / ((double) hal_data->pprev);
  }
}

void lcec_deasda_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_deasda_data_t *hal_data = (lcec_deasda_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  uint16_t status;
  int32_t speed_raw;
  double rpm;
  int32_t pos_cnt, pos_cnt_diff;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  // check for change in scale value
  lcec_deasda_check_scales(hal_data);

  // read status word
  status = EC_READ_U16(&pd[hal_data->status_pdo_os]);
  *(hal_data->ready)        = (status >> 0) & 0x01;
  *(hal_data->switched_on)  = (status >> 1) & 0x01;
  *(hal_data->oper_enabled) = (status >> 2) & 0x01;
  hal_data->internal_fault  = (status >> 3) & 0x01;
  *(hal_data->volt_enabled) = (status >> 4) & 0x01;
  *(hal_data->quick_stoped) = !((status >> 5) & 0x01);
  *(hal_data->on_disabled)  = (status >> 6) & 0x01;
  *(hal_data->warning)      = (status >> 7) & 0x01;
  *(hal_data->remote)       = (status >> 9) & 0x01;
  *(hal_data->at_speed)     = (status >> 10) & 0x01;
  *(hal_data->limit_active) = (status >> 11) & 0x01;
  *(hal_data->zero_speed)   = (status >> 12) & 0x01;

  // clear pending fault reset if no fault
  if (!hal_data->internal_fault) {
    hal_data->fault_reset_retry = 0;
  }

  // generate gated fault
  if (hal_data->fault_reset_retry > 0) {
    if (hal_data->fault_reset_cycle < hal_data->fault_autoreset_cycles) {
      hal_data->fault_reset_cycle++;
    } else {
      hal_data->fault_reset_cycle = 0;
      hal_data->fault_reset_state = !hal_data->fault_reset_state;
      if (hal_data->fault_reset_state) {
        hal_data->fault_reset_retry--;
      }
    }
    *(hal_data->fault) = 0;
  } else {
    *(hal_data->fault) = hal_data->internal_fault;
  }

  // read current speed
  speed_raw = EC_READ_S32(&pd[hal_data->currvel_pdo_os]);
  rpm = (double)speed_raw * DEASDA_RPM_FACTOR;
  *(hal_data->vel_fb_rpm) = rpm;
  *(hal_data->vel_fb_rpm_abs) = fabs(rpm);
  *(hal_data->vel_fb) = rpm * DEASDA_RPM_DIV * hal_data->pos_scale;

  // update raw position counter
  pos_cnt = EC_READ_S32(&pd[hal_data->currpos_pdo_os]);
  *(hal_data->enc_raw) = pos_cnt;
  *(hal_data->on_home_neg) = (pos_cnt <= hal_data->home_raw);
  *(hal_data->on_home_pos) = (pos_cnt >= hal_data->home_raw);
  pos_cnt_diff = pos_cnt - hal_data->last_pos_cnt;
  hal_data->last_pos_cnt = pos_cnt;
  hal_data->pos_cnt += pos_cnt_diff;

  // update external encoder counter
  pos_cnt = EC_READ_S32(&pd[hal_data->extenc_pdo_os]);
  *(hal_data->extenc_raw) = pos_cnt;
  pos_cnt_diff = pos_cnt - hal_data->last_extenc_cnt;
  hal_data->last_extenc_cnt = pos_cnt;
  hal_data->extenc_cnt += pos_cnt_diff;

  // handle initialization
  if (hal_data->do_init || *(hal_data->pos_reset)) {
    hal_data->do_init = 0;
    hal_data->pos_cnt = 0;
    hal_data->extenc_cnt = 0;
  }

  // update raw counter pins
  *(hal_data->pos_raw_hi) = (hal_data->pos_cnt >> 32) & 0xffffffff;
  *(hal_data->pos_raw_lo) = (hal_data->pos_cnt) & 0xffffffff;
  *(hal_data->extenc_raw_hi) = (hal_data->extenc_cnt >> 32) & 0xffffffff;
  *(hal_data->extenc_raw_lo) = (hal_data->extenc_cnt) & 0xffffffff;

  // scale count to make floating point position
  *(hal_data->pos_fb) = (double)(hal_data->pos_cnt) * hal_data->pos_scale_cnt;
  *(hal_data->pos_extenc) = (double)(hal_data->extenc_cnt) * hal_data->extenc_scale;
}

void lcec_deasda_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_deasda_data_t *hal_data = (lcec_deasda_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  uint16_t control;
  double speed_raw;
  int switch_on_edge;

  // check for enable edge
  switch_on_edge = *(hal_data->switch_on) && !hal_data->last_switch_on;
  hal_data->last_switch_on = *(hal_data->switch_on);

  // check for autoreset
  if (hal_data->fault_autoreset_retries > 0 && hal_data->fault_autoreset_cycles > 0 && switch_on_edge && hal_data->internal_fault) {
    hal_data->fault_reset_retry = hal_data->fault_autoreset_retries;
    hal_data->fault_reset_state = 1;
    hal_data->fault_reset_cycle = 0;
  }

  // check for change in scale value
  lcec_deasda_check_scales(hal_data);

  // write dev ctrl
  control = 0;
  if (*(hal_data->enable_volt)) {
    control |= (1 << 1);
  }
  if (! *(hal_data->quick_stop)) {
    control |= (1 << 2);
  }
  if (*(hal_data->fault_reset)) {
    control |= (1 << 7);
  }
  if (*(hal_data->halt)) {
    control |= (1 << 8);
  }
  if (hal_data->fault_reset_retry > 0) {
      if (hal_data->fault_reset_state) {
        control |= (1 << 7);
      }
  } else {
    if (*(hal_data->switch_on)) {
      control |= (1 << 0);
    }
    if (*(hal_data->enable) && *(hal_data->switched_on)) {
      control |= (1 << 3);
    }
  }
  EC_WRITE_U16(&pd[hal_data->control_pdo_os], control);

  // calculate rpm command
  *(hal_data->vel_rpm) = *(hal_data->vel_cmd) * hal_data->pos_scale_rcpt * DEASDA_RPM_MUL;

  // set RPM
  speed_raw = *(hal_data->vel_rpm) * DEASDA_RPM_RCPT;
  if (speed_raw > (double)0x7fffffff) {
    speed_raw = (double)0x7fffffff;
  }
  if (speed_raw < (double)-0x7fffffff) {
    speed_raw = (double)-0x7fffffff;
  }
  EC_WRITE_S32(&pd[hal_data->cmdvel_pdo_os], (int32_t)speed_raw);
}

