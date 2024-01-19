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
/// @brief Library for digital output devices

#include "../lcec.h"

typedef struct {
  hal_bit_t *out;
  hal_bit_t invert;
  unsigned int pdo_os, pdo_bp;
} lcec_class_dout_pin_t;

typedef struct {
  int count;
  lcec_class_dout_pin_t **pins;
} lcec_class_dout_pins_t;

lcec_class_dout_pins_t *lcec_dout_allocate_pins(int count);
lcec_class_dout_pin_t *lcec_dout_register_pin(
    ec_pdo_entry_reg_t **pdo_entry_regs, struct lcec_slave *slave, int id, uint16_t idx, uint16_t sidx);
void lcec_dout_write(struct lcec_slave *slave, lcec_class_dout_pin_t *data);
void lcec_dout_write_all(struct lcec_slave *slave, lcec_class_dout_pins_t *pins);
