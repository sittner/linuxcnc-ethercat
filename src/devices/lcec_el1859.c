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

/// @file
/// @brief Driver for Beckhoff EL1859 and related digital input/output modules

#include "lcec_el1859.h"

#include "../lcec.h"
#include "lcec_class_din.h"
#include "lcec_class_dout.h"

// This driver should support any Beckhoff digital in/out board with
// equal numbers of in and out ports, with input PDOs on 0x60n0:01 and
// output PDOs on 0x70n0:01.  This covers most devices in Beckhoff's
// catalog, but not all.  Some (like the EP2316-0008) have PDOs on
// 0x6000:0n and 0x7000:0n instead; these will need a different
// driver.

static int lcec_el1859_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[] = {
    {"EL1852", LCEC_BECKHOFF_VID, 0x073c3052, LCEC_EL1859_PDOS, 0, NULL, lcec_el1859_init, NULL, 8},
    {"EL1859", LCEC_BECKHOFF_VID, 0x07433052, LCEC_EL1859_PDOS, 0, NULL, lcec_el1859_init, NULL, 8},
    {"EJ1859", LCEC_BECKHOFF_VID, 0x07432852, LCEC_EL1859_PDOS, 0, NULL, lcec_el1859_init, NULL, 8},
    {"EK1814", LCEC_BECKHOFF_VID, 0x07162c52, LCEC_EP2308_PDOS, 0, NULL, lcec_el1859_init, NULL, 4},
    {"EP2308", LCEC_BECKHOFF_VID, 0x09044052, LCEC_EP2308_PDOS, 0, NULL, lcec_el1859_init, NULL, 4},
    {"EP2318", LCEC_BECKHOFF_VID, 0x090E4052, LCEC_EP2318_PDOS, 0, NULL, lcec_el1859_init, NULL, 4},
    {"EP2328", LCEC_BECKHOFF_VID, 0x09184052, LCEC_EP2328_PDOS, 0, NULL, lcec_el1859_init, NULL, 4},
    {"EP2338", LCEC_BECKHOFF_VID, 0x09224052, LCEC_EP2338_PDOS, 0, NULL, lcec_el1859_init, NULL, 8},
    {"EP2339", LCEC_BECKHOFF_VID, 0x09234052, LCEC_EP2349_PDOS, 0, NULL, lcec_el1859_init, NULL, 16},
    {"EP2349", LCEC_BECKHOFF_VID, 0x092d4052, LCEC_EP2349_PDOS, 0, NULL, lcec_el1859_init, NULL, 16},
    {"EQ2339", LCEC_BECKHOFF_VID, 0x092d4052, LCEC_EP2349_PDOS, 0, NULL, lcec_el1859_init, NULL, 16},
    {"EPP2308", LCEC_BECKHOFF_VID, 0x64765649, LCEC_EP2308_PDOS, 0, NULL, lcec_el1859_init, NULL, 4},
    {"EPP2318", LCEC_BECKHOFF_VID, 0x647656e9, LCEC_EP2318_PDOS, 0, NULL, lcec_el1859_init, NULL, 4},
    {"EPP2328", LCEC_BECKHOFF_VID, 0x64765789, LCEC_EP2318_PDOS, 0, NULL, lcec_el1859_init, NULL, 4},
    {"EPP2334", LCEC_BECKHOFF_VID, 0x647657e9, LCEC_EP2318_PDOS, 0, NULL, lcec_el1859_init, NULL, 4},
    {"EPP2338", LCEC_BECKHOFF_VID, 0x09224052, LCEC_EP2338_PDOS, 0, NULL, lcec_el1859_init, NULL, 8},
    {"EPP2339", LCEC_BECKHOFF_VID, 0x64765839, LCEC_EP2338_PDOS, 0, NULL, lcec_el1859_init, NULL, 8},
    {"EPP2349", LCEC_BECKHOFF_VID, 0x647658d9, LCEC_EP2338_PDOS, 0, NULL, lcec_el1859_init, NULL, 8},
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
  int channels = slave->flags;

  // initialize callbacks
  slave->proc_read = lcec_el1859_read;
  slave->proc_write = lcec_el1859_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el1859_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", slave->master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el1859_data_t));
  slave->hal_data = hal_data;

  hal_data->pins_in = lcec_din_allocate_pins(channels);
  hal_data->pins_out = lcec_dout_allocate_pins(channels);

  // initialize pins
  for (i = 0; i < channels; i++) {
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
