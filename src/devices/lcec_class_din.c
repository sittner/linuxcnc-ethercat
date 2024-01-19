//
//    Copyright (C) 2024 Scott Laird <scott@sigkill.org>
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

/// @file
/// @brief Library for digital input devices

#include "lcec_class_din.h"

#include "../lcec.h"

static const lcec_pindesc_t slave_pins[] = {
    {HAL_BIT, HAL_OUT, offsetof(lcec_class_din_pin_t, in), "%s.%s.%s.din-%d"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_class_din_pin_t, in_not), "%s.%s.%s.din-%d-not"},
    {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL},
};

/// \brief allocates a block of memory for holding the result of
/// `count` calls to `lcec_din_register_device()`.
///
/// It is the caller's responsibility to verify that the result is not
/// NULL.
///
/// @param count The number of input pins to allocate memory for.
lcec_class_din_pins_t *lcec_din_allocate_pins(int count) {
  lcec_class_din_pins_t *pins;

  pins = hal_malloc(sizeof(lcec_class_din_pins_t));
  if (pins == NULL) {
    return NULL;
  }
  pins->count = count;
  pins->pins = hal_malloc(sizeof(lcec_class_din_pin_t *) * count);
  if (pins->pins == NULL) {
    return NULL;
  }

  return pins;
}

/// \brief registers a single digital-input channel and publishes it as a LinuxCNC HAL pin.
///
/// @param pdo_entry_regs a pointer to the pdo_entry_regs passed into the device `_init` function.
/// @param slave the slave, from `_init`.
/// @param id  the pin ID.  Used for naming.  Should generally start at 0 and increment once per digital in pin.
/// @param idx the PDO index for the digital input.
/// @param sindx the PDO sub-index for the digital input.
///
/// See lcec_el1xxx.c for an example of use.
lcec_class_din_pin_t *lcec_din_register_pin(
    ec_pdo_entry_reg_t **pdo_entry_regs, struct lcec_slave *slave, int id, uint16_t idx, uint16_t sidx) {
  lcec_class_din_pin_t *data;
  int err;

  data = hal_malloc(sizeof(lcec_class_din_pin_t));
  if (data == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s pin %d failed\n", slave->master->name, slave->name, id);
    return NULL;
  }
  memset(data, 0, sizeof(lcec_class_din_pin_t));

  LCEC_PDO_INIT((*pdo_entry_regs), slave->index, slave->vid, slave->pid, idx, sidx, &data->pdo_os, &data->pdo_bp);
  err = lcec_pin_newf_list(data, slave_pins, LCEC_MODULE_NAME, slave->master->name, slave->name, id);
  if (err != 0) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "lcec_pin_newf_list for slave %s.%s pin %d failed\n", slave->master->name, slave->name, id);
    return NULL;
  }

  return data;
}

/// \brief reads data from a single digital in port.
///
/// @param slave the slave, passed from the per-device `_read`.
/// @param data  a lcec_class_din_pin_t *, as returned by lcec_din_register_pin.
///
/// Call this once per pin registered, from inside of your device's
/// read function.  See `lcec_din_read_all` for an alternative approach.
void lcec_din_read(struct lcec_slave *slave, lcec_class_din_pin_t *data) {
  lcec_master_t *master = slave->master;
  uint8_t *pd = master->process_data;
  int s;

  s = EC_READ_BIT(&pd[data->pdo_os], data->pdo_bp);
  *(data->in) = s;
  *(data->in_not) = !s;
}

/// \brief reads data from all digital in ports.
///
/// @param slave the slave, passed from the per-device `_read`.
/// @param pins a `lcec_class_din_pins_t *`, as returned by lcec_din_register_pin.
void lcec_din_read_all(struct lcec_slave *slave, lcec_class_din_pins_t *pins) {
  for (int i = 0; i < pins->count; i++) {
    lcec_class_din_pin_t *pin = pins->pins[i];

    lcec_din_read(slave, pin);
  }
}
