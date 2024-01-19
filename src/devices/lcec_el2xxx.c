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

#include "lcec_el2xxx.h"

#include "../lcec.h"
#include "lcec_class_dout.h"

static int lcec_el2xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[] = {
    {"EL2002", LCEC_BECKHOFF_VID, 0x07D23052, LCEC_EL2002_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2004", LCEC_BECKHOFF_VID, 0x07D43052, LCEC_EL2004_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2008", LCEC_BECKHOFF_VID, 0x07D83052, LCEC_EL2008_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2022", LCEC_BECKHOFF_VID, 0x07E63052, LCEC_EL2022_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2024", LCEC_BECKHOFF_VID, 0x07E83052, LCEC_EL2024_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2032", LCEC_BECKHOFF_VID, 0x07F03052, LCEC_EL2032_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2034", LCEC_BECKHOFF_VID, 0x07F23052, LCEC_EL2034_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2042", LCEC_BECKHOFF_VID, 0x07FA3052, LCEC_EL2042_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2084", LCEC_BECKHOFF_VID, 0x08243052, LCEC_EL2084_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2088", LCEC_BECKHOFF_VID, 0x08283052, LCEC_EL2088_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2124", LCEC_BECKHOFF_VID, 0x084C3052, LCEC_EL2124_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2612", LCEC_BECKHOFF_VID, 0x0A343052, LCEC_EL2612_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2622", LCEC_BECKHOFF_VID, 0x0A3E3052, LCEC_EL2622_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2634", LCEC_BECKHOFF_VID, 0x0A4A3052, LCEC_EL2634_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2652", LCEC_BECKHOFF_VID, 0x0A5C3052, LCEC_EL2652_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2808", LCEC_BECKHOFF_VID, 0x0AF83052, LCEC_EL2808_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2798", LCEC_BECKHOFF_VID, 0x0AEE3052, LCEC_EL2798_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EL2809", LCEC_BECKHOFF_VID, 0x0AF93052, LCEC_EL2809_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EP2008", LCEC_BECKHOFF_VID, 0x07D84052, LCEC_EP2008_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EP2028", LCEC_BECKHOFF_VID, 0x07EC4052, LCEC_EP2028_PDOS, 0, NULL, lcec_el2xxx_init},
    {"EP2809", LCEC_BECKHOFF_VID, 0x0AF94052, LCEC_EP2809_PDOS, 0, NULL, lcec_el2xxx_init},
    {NULL},
};
ADD_TYPES(types);

static void lcec_el2xxx_write(struct lcec_slave *slave, long period);

static int lcec_el2xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_class_dout_pins_t *hal_data;
  int i;

  // initialize callbacks
  slave->proc_write = lcec_el2xxx_write;

  hal_data = lcec_dout_allocate_pins(slave->pdo_entry_count);
  if (hal_data == NULL) {
    return -EIO;
  }
  slave->hal_data = hal_data;

  // initialize pins
  for (i = 0; i < slave->pdo_entry_count; i++) {
    hal_data->pins[i] = lcec_dout_register_pin(&pdo_entry_regs, slave, i, 0x7000 + (i << 4), 0x01);
  }

  return 0;
}

static void lcec_el2xxx_write(struct lcec_slave *slave, long period) {
  lcec_class_dout_pins_t *hal_data = (lcec_class_dout_pins_t *)slave->hal_data;

  if (!slave->state.operational) {
    return;
  }
  lcec_dout_write_all(slave, hal_data);
}
