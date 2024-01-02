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

#include "lcec_el3xxx.h"

#include "../lcec.h"

// There's no simple way to pass parameters into _init, so we have a
// couple variants here that set up a few different minor variants.
//
// Right now, the options are:
//
// - lcec_el3xxx_init_16_sync: for EL31xx devices.  Expects 16-bit
//   values and supports the `-sync-err` PDO.
//
// - lcec_el3xxx_init_12_nosync: for EL30xx devices.  Expects 12-bit
//   values and does not support the `-sync-err` PDO.
//
// TODO(scottlaird): add support for additional EL31xx operating modes, as per
//   https://download.beckhoff.com/download/Document/io/ethercat-terminals/el31xxen.pdf#page=251
//   Specifically, support disabling the filter (0x8000:06) and DC mode.
//
// TODO(scottlaird): add support framework for almost-but-not-quite analog
//   devices with different pin names and read() functions.

// TODO(scottlaird): using generic support, add EL32xx temperature
//   sensors family.  The EL32xxs seem to be 16-bit devices with no
//   sync.  The existing EL3202 driver has slightly different pin
//   names, and the read() function is calibrated slightly
//   differently.
//
// TODO(scottlaird): using generic support, add EL3255 pot input.
//
// TODO(scottlaird): using generic support, add EM37xx pressure sensor input.
//
// TODO(scottlaird): support weird multifunction analog devices like
//   the ELM3246 and EL3751?  This are weird enough that they may make
//   sense as an indpendent driver, if anyone ever actually turns up
//   with hardware.
static int lcec_el3xxx_init_16_sync(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);
static int lcec_el3xxx_init_12_nosync(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[] = {
    {"EL3001", LCEC_EL3XXX_VID, LCEC_EL3001_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3002", LCEC_EL3XXX_VID, LCEC_EL3002_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    //  { "EL3004", LCEC_EL3XXX_VID, LCEC_EL3004_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3011", LCEC_EL3XXX_VID, LCEC_EL3011_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3012", LCEC_EL3XXX_VID, LCEC_EL3012_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3014", LCEC_EL3XXX_VID, LCEC_EL3014_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3021", LCEC_EL3XXX_VID, LCEC_EL3021_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3022", LCEC_EL3XXX_VID, LCEC_EL3022_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3024", LCEC_EL3XXX_VID, LCEC_EL3024_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3041", LCEC_EL3XXX_VID, LCEC_EL3041_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3042", LCEC_EL3XXX_VID, LCEC_EL3042_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    //  { "EL3044", LCEC_EL3XXX_VID, LCEC_EL3044_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3048", LCEC_EL3XXX_VID, LCEC_EL3048_PID, LCEC_EL3XX8_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3051", LCEC_EL3XXX_VID, LCEC_EL3051_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3052", LCEC_EL3XXX_VID, LCEC_EL3052_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    //  { "EL3054", LCEC_EL3XXX_VID, LCEC_EL3054_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3058", LCEC_EL3XXX_VID, LCEC_EL3058_PID, LCEC_EL3XX8_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3061", LCEC_EL3XXX_VID, LCEC_EL3061_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3062", LCEC_EL3XXX_VID, LCEC_EL3062_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    //  { "EL3064", LCEC_EL3XXX_VID, LCEC_EL3064_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},
    {"EL3068", LCEC_EL3XXX_VID, LCEC_EL3068_PID, LCEC_EL3XX8_PDOS, 0, NULL, lcec_el3xxx_init_12_nosync},

    {"EL3101", LCEC_EL3XXX_VID, LCEC_EL3101_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    //  { "EL3102", LCEC_EL3XXX_VID, LCEC_EL3102_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EL3104", LCEC_EL3XXX_VID, LCEC_EL3104_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EL3111", LCEC_EL3XXX_VID, LCEC_EL3111_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    //  { "EL3112", LCEC_EL3XXX_VID, LCEC_EL3112_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EL3114", LCEC_EL3XXX_VID, LCEC_EL3114_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EL3121", LCEC_EL3XXX_VID, LCEC_EL3121_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    //  { "EL3122", LCEC_EL3XXX_VID, LCEC_EL3122_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    //  { "EL3124", LCEC_EL3XXX_VID, LCEC_EL3124_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EL3141", LCEC_EL3XXX_VID, LCEC_EL3141_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    //  { "EL3142", LCEC_EL3XXX_VID, LCEC_EL3142_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EL3144", LCEC_EL3XXX_VID, LCEC_EL3144_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EL3151", LCEC_EL3XXX_VID, LCEC_EL3151_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    //  { "EL3152", LCEC_EL3XXX_VID, LCEC_EL3152_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EL3154", LCEC_EL3XXX_VID, LCEC_EL3154_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EL3161", LCEC_EL3XXX_VID, LCEC_EL3161_PID, LCEC_EL3XX1_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    //  { "EL3162", LCEC_EL3XXX_VID, LCEC_EL3162_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    //  { "EL3164", LCEC_EL3XXX_VID, LCEC_EL3164_PID, LCEC_EL3XX4_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EL3182", LCEC_EL3XXX_VID, LCEC_EL3182_PID, LCEC_EL3XX2_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {"EPX3158", LCEC_EL3XXX_VID, LCEC_EPX3158_PID, LCEC_EL3XX8_PDOS, 0, NULL, lcec_el3xxx_init_16_sync},
    {NULL},
};
ADD_TYPES(types);

typedef struct {
  hal_bit_t *overrange;
  hal_bit_t *underrange;
  hal_bit_t *error;
  hal_bit_t *sync_err;
  hal_s32_t *raw_val;
  hal_float_t *scale;
  hal_float_t *bias;
  hal_float_t *val;
  unsigned int ovr_pdo_os;
  unsigned int ovr_pdo_bp;
  unsigned int udr_pdo_os;
  unsigned int udr_pdo_bp;
  unsigned int error_pdo_os;
  unsigned int error_pdo_bp;
  unsigned int sync_err_pdo_os;
  unsigned int sync_err_pdo_bp;
  unsigned int val_pdo_os;
} lcec_el3xxx_chan_t;

static const lcec_pindesc_t slave_pins_sync[] = {
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, error), "%s.%s.%s.ain-%d-error"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, sync_err), "%s.%s.%s.ain-%d-sync-err"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, overrange), "%s.%s.%s.ain-%d-overrange"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, underrange), "%s.%s.%s.ain-%d-underrange"},
    {HAL_S32, HAL_OUT, offsetof(lcec_el3xxx_chan_t, raw_val), "%s.%s.%s.ain-%d-raw"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, val), "%s.%s.%s.ain-%d-val"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, scale), "%s.%s.%s.ain-%d-scale"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, bias), "%s.%s.%s.ain-%d-bias"},
    {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL}};

static const lcec_pindesc_t slave_pins_nosync[] = {
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, error), "%s.%s.%s.ain-%d-error"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, overrange), "%s.%s.%s.ain-%d-overrange"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, underrange), "%s.%s.%s.ain-%d-underrange"},
    {HAL_S32, HAL_OUT, offsetof(lcec_el3xxx_chan_t, raw_val), "%s.%s.%s.ain-%d-raw"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, val), "%s.%s.%s.ain-%d-val"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, scale), "%s.%s.%s.ain-%d-scale"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, bias), "%s.%s.%s.ain-%d-bias"},
    {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL}};

typedef struct {
  lcec_el3xxx_chan_t chans[LCEC_EL3XXX_MAXCHANS];
} lcec_el3xxx_data_t;

static void lcec_el3xxx_read_16(struct lcec_slave *slave, long period);
static void lcec_el3xxx_read_12(struct lcec_slave *slave, long period);

static int lcec_el3xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs, int bitdepth,
                            int has_sync) {
  lcec_master_t *master = slave->master;
  lcec_el3xxx_data_t *hal_data;
  lcec_el3xxx_chan_t *chan;
  int i;
  int err;
  int channels;
  const lcec_pindesc_t *slave_pins;

  channels = slave->pdo_entry_count / LCEC_EL3XXX_PDOS_PER_CHANNEL;

  rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "initing device as %s, bitdepth %d, channels %d, has_sync %d\n", slave->name, bitdepth,
                  channels, has_sync);

  // initialize settings that vary per bitdepth
  switch (bitdepth) {
    case 12:
      slave->proc_read = lcec_el3xxx_read_12;
      break;
    case 16:
      slave->proc_read = lcec_el3xxx_read_16;
      break;
    default:
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "Can't handle bitdepth %d for device %s\n", bitdepth, slave->name);
  }

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el3xxx_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el3xxx_data_t));
  slave->hal_data = hal_data;

  rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "setting up pins\n");

  // initialize pins
  for (i = 0; i < channels; i++) {
    chan = &hal_data->chans[i];

    // initialize POD entries
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x01, &chan->udr_pdo_os, &chan->udr_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x02, &chan->ovr_pdo_os, &chan->ovr_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x07, &chan->error_pdo_os, &chan->error_pdo_bp);
    if (has_sync) {
      // Only EL31xx devices have this PDO, if we try to initialize it
      // with 30xxs, then we get a PDO error and fail out.
      LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x0E, &chan->sync_err_pdo_os, &chan->sync_err_pdo_bp);
    }
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x11, &chan->val_pdo_os, NULL);

    // export pins
    if (has_sync)
      slave_pins = slave_pins_sync;
    else
      slave_pins = slave_pins_nosync;

    if ((err = lcec_pin_newf_list(chan, slave_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      return err;
    }

    // initialize pins
    *(chan->scale) = 1.0;
  }
  rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "done\n");

  return 0;
}

static int lcec_el3xxx_init_16_sync(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_el3xxx_init(comp_id, slave, pdo_entry_regs, 16, 1);  // 16 bits, with has_sync=1
}

static int lcec_el3xxx_init_12_nosync(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_el3xxx_init(comp_id, slave, pdo_entry_regs, 12, 0);  // 12 bits, with has_syhc=0
}

static void lcec_el3xxx_read(struct lcec_slave *slave, long period, uint32_t mask, int has_sync) {
  lcec_master_t *master = slave->master;
  lcec_el3xxx_data_t *hal_data = (lcec_el3xxx_data_t *)slave->hal_data;
  uint8_t *pd = master->process_data;
  int i;
  lcec_el3xxx_chan_t *chan;
  int16_t value;
  int channels;

  // really shouldn't need to do this on every read.
  channels = slave->pdo_entry_count / LCEC_EL3XXX_PDOS_PER_CHANNEL;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  // check inputs
  for (i = 0; i < channels; i++) {
    chan = &hal_data->chans[i];

    // update state
    // update state
    *(chan->overrange) = EC_READ_BIT(&pd[chan->ovr_pdo_os], chan->ovr_pdo_bp);
    *(chan->underrange) = EC_READ_BIT(&pd[chan->udr_pdo_os], chan->udr_pdo_bp);
    *(chan->error) = EC_READ_BIT(&pd[chan->error_pdo_os], chan->error_pdo_bp);
    if (has_sync)
      *(chan->sync_err) = EC_READ_BIT(&pd[chan->sync_err_pdo_os], chan->sync_err_pdo_bp);

    // update value
    value = EC_READ_S16(&pd[chan->val_pdo_os]);
    *(chan->raw_val) = value;
    *(chan->val) = *(chan->bias) + *(chan->scale) * (double)value * ((double)1 / (double)mask);
  }
}

static void lcec_el3xxx_read_16(struct lcec_slave *slave, long period) {
  lcec_el3xxx_read(slave, period, 0x7fff, true);
}

static void lcec_el3xxx_read_12(struct lcec_slave *slave, long period) {
  lcec_el3xxx_read(slave, period, 0x7ff, false);
}

