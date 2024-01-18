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

#include "lcec_el1xxx.h"
#include "lcec_class_din.h"

#include "../lcec.h"

static int lcec_el1xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[] = {
    {"EL1002", LCEC_BECKHOFF_VID, 0x03EA3052, LCEC_EL1002_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1004", LCEC_BECKHOFF_VID, 0x03EC3052, LCEC_EL1004_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1008", LCEC_BECKHOFF_VID, 0x03F03052, LCEC_EL1008_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1012", LCEC_BECKHOFF_VID, 0x03F43052, LCEC_EL1012_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1014", LCEC_BECKHOFF_VID, 0x03F63052, LCEC_EL1014_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1018", LCEC_BECKHOFF_VID, 0x03FA3052, LCEC_EL1018_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1024", LCEC_BECKHOFF_VID, 0x04003052, LCEC_EL1024_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1034", LCEC_BECKHOFF_VID, 0x040A3052, LCEC_EL1034_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1084", LCEC_BECKHOFF_VID, 0x043C3052, LCEC_EL1084_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1088", LCEC_BECKHOFF_VID, 0x04403052, LCEC_EL1088_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1094", LCEC_BECKHOFF_VID, 0x04463052, LCEC_EL1094_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1098", LCEC_BECKHOFF_VID, 0x044A3052, LCEC_EL1098_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1104", LCEC_BECKHOFF_VID, 0x04503052, LCEC_EL1104_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1114", LCEC_BECKHOFF_VID, 0x045A3052, LCEC_EL1114_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1124", LCEC_BECKHOFF_VID, 0x04643052, LCEC_EL1124_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1134", LCEC_BECKHOFF_VID, 0x046E3052, LCEC_EL1134_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1144", LCEC_BECKHOFF_VID, 0x04783052, LCEC_EL1144_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1804", LCEC_BECKHOFF_VID, 0x070C3052, LCEC_EL1804_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1808", LCEC_BECKHOFF_VID, 0x07103052, LCEC_EL1808_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1809", LCEC_BECKHOFF_VID, 0x07113052, LCEC_EL1809_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EL1819", LCEC_BECKHOFF_VID, 0x071B3052, LCEC_EL1819_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EP1008", LCEC_BECKHOFF_VID, 0x03f04052, LCEC_EP1008_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EP1018", LCEC_BECKHOFF_VID, 0x03fa4052, LCEC_EL1018_PDOS, 0, NULL, lcec_el1xxx_init},
    {"EP1819", LCEC_BECKHOFF_VID, 0x071b4052, LCEC_EP1819_PDOS, 0, NULL, lcec_el1xxx_init},
    {NULL},
};

ADD_TYPES(types);

static void lcec_el1xxx_read(struct lcec_slave *slave, long period);

static int lcec_el1xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_class_din_pins_t *hal_data;
  int i;

  // initialize callbacks
  slave->proc_read = lcec_el1xxx_read;

  hal_data = lcec_din_allocate_pins(slave->pdo_entry_count);
  if (hal_data == NULL) { return -EIO; }
  slave->hal_data = hal_data;

  // initialize pins
  for (i = 0; i < slave->pdo_entry_count; i++) {
    hal_data->pins[i]=lcec_din_register_pin(&pdo_entry_regs, slave, i, 0x6000 + (i<<4), 0x01);

    if (hal_data->pins[i]==NULL) { return -EIO; }
  }

  return 0;
}

static void lcec_el1xxx_read(struct lcec_slave *slave, long period) {
  lcec_class_din_pins_t *hal_data = (lcec_class_din_pins_t *)slave->hal_data;
  int i;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  lcec_din_read_all(slave, hal_data);
}
