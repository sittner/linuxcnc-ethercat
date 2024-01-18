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

#include "lcec_el1859.h"

#include "../lcec.h"
#include "lcec_class_din.h"
#include "lcec_class_dout.h"

static int lcec_el1859_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[] = {
    {"EL1859", LCEC_BECKHOFF_VID, 0x07433052, LCEC_EL1859_PDOS, 0, NULL, lcec_el1859_init},
    {NULL},
};
ADD_TYPES(types);

typedef struct {
  lcec_class_din_pins_t *pins_in;
  lcec_class_dout_pins_t *pins_out;
} lcec_el1859_data_t;

static void lcec_el1859_read(struct lcec_slave *slave, long period);
static void lcec_el1859_write(struct lcec_slave *slave, long period);

static int lcec_el1859_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_el1859_data_t *hal_data;
  int i;

  // initialize callbacks
  slave->proc_read = lcec_el1859_read;
  slave->proc_write = lcec_el1859_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el1859_data_t) * LCEC_EL1859_PINS)) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", slave->master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el1859_data_t) * LCEC_EL1859_PINS);
  slave->hal_data = hal_data;

  hal_data->pins_in = lcec_din_allocate_pins(LCEC_EL1859_PINS);
  hal_data->pins_out = lcec_dout_allocate_pins(LCEC_EL1859_PINS);

  // initialize pins
  for (i = 0; i < LCEC_EL1859_PINS; i++) {
    hal_data->pins_in->pins[i] = lcec_din_register_pin(&pdo_entry_regs, slave, i, 0x6000 + (i << 4), 0x01);
    hal_data->pins_out->pins[i] = lcec_dout_register_pin(&pdo_entry_regs, slave, i, 0x7000 + (i << 4), 0x01);
  }
}

static void lcec_el1859_read(struct lcec_slave *slave, long period) {
  lcec_el1859_data_t *hal_data = (lcec_el1859_data_t *)slave->hal_data;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  lcec_din_read_all(slave, hal_data->pins_in);
}

static void lcec_el1859_write(struct lcec_slave *slave, long period) {
  lcec_el1859_data_t *hal_data = (lcec_el1859_data_t *)slave->hal_data;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  lcec_dout_write_all(slave, hal_data->pins_out);
}
