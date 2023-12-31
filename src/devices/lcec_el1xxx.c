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

#include "../lcec.h"
#include "lcec_el1xxx.h"

static int lcec_el1xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

typedef struct {
  hal_bit_t *in;
  hal_bit_t *in_not;
  unsigned int pdo_os;
  unsigned int pdo_bp;
} lcec_el1xxx_pin_t;

static lcec_typelist_t types[]={
  { "EL1002", LCEC_EL1xxx_VID, LCEC_EL1002_PID, LCEC_EL1002_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1004", LCEC_EL1xxx_VID, LCEC_EL1004_PID, LCEC_EL1004_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1008", LCEC_EL1xxx_VID, LCEC_EL1008_PID, LCEC_EL1008_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1012", LCEC_EL1xxx_VID, LCEC_EL1012_PID, LCEC_EL1012_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1014", LCEC_EL1xxx_VID, LCEC_EL1014_PID, LCEC_EL1014_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1018", LCEC_EL1xxx_VID, LCEC_EL1018_PID, LCEC_EL1018_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1024", LCEC_EL1xxx_VID, LCEC_EL1024_PID, LCEC_EL1024_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1034", LCEC_EL1xxx_VID, LCEC_EL1034_PID, LCEC_EL1034_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1084", LCEC_EL1xxx_VID, LCEC_EL1084_PID, LCEC_EL1084_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1088", LCEC_EL1xxx_VID, LCEC_EL1088_PID, LCEC_EL1088_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1094", LCEC_EL1xxx_VID, LCEC_EL1094_PID, LCEC_EL1094_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1098", LCEC_EL1xxx_VID, LCEC_EL1098_PID, LCEC_EL1098_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1104", LCEC_EL1xxx_VID, LCEC_EL1104_PID, LCEC_EL1104_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1114", LCEC_EL1xxx_VID, LCEC_EL1114_PID, LCEC_EL1114_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1124", LCEC_EL1xxx_VID, LCEC_EL1124_PID, LCEC_EL1124_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1134", LCEC_EL1xxx_VID, LCEC_EL1134_PID, LCEC_EL1134_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1144", LCEC_EL1xxx_VID, LCEC_EL1144_PID, LCEC_EL1144_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1804", LCEC_EL1xxx_VID, LCEC_EL1804_PID, LCEC_EL1804_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1808", LCEC_EL1xxx_VID, LCEC_EL1808_PID, LCEC_EL1808_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EL1809", LCEC_EL1xxx_VID, LCEC_EL1809_PID, LCEC_EL1809_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EP1008", LCEC_EL1xxx_VID, LCEC_EP1008_PID, LCEC_EP1008_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EP1018", LCEC_EL1xxx_VID, LCEC_EP1018_PID, LCEC_EL1018_PDOS, 0, NULL, lcec_el1xxx_init},
  { "EP1819", LCEC_EL1xxx_VID, LCEC_EL1819_PID, LCEC_EL1819_PDOS, 0, NULL, lcec_el1xxx_init},
  { NULL },
};

ADD_TYPES(types);
     
static const lcec_pindesc_t slave_pins[] = {
  { HAL_BIT, HAL_OUT, offsetof(lcec_el1xxx_pin_t, in), "%s.%s.%s.din-%d" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el1xxx_pin_t, in_not), "%s.%s.%s.din-%d-not" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static void lcec_el1xxx_read(struct lcec_slave *slave, long period);

static int lcec_el1xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el1xxx_pin_t *hal_data;
  lcec_el1xxx_pin_t *pin;
  int i;
  int err;

  // initialize callbacks
  slave->proc_read = lcec_el1xxx_read;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el1xxx_pin_t) * slave->pdo_entry_count)) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el1xxx_pin_t) * slave->pdo_entry_count);
  slave->hal_data = hal_data;

  // initialize pins
  for (i=0, pin=hal_data; i<slave->pdo_entry_count; i++, pin++) {
    // initialize POD entry
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x01, &pin->pdo_os, &pin->pdo_bp);

    // export pins
    if ((err = lcec_pin_newf_list(pin, slave_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      return err;
    }
  }

  return 0;
}

static void lcec_el1xxx_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el1xxx_pin_t *hal_data = (lcec_el1xxx_pin_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  lcec_el1xxx_pin_t *pin;
  int i, s;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  // check inputs
  for (i=0, pin=hal_data; i<slave->pdo_entry_count; i++, pin++) {
    s = EC_READ_BIT(&pd[pin->pdo_os], pin->pdo_bp);
    *(pin->in) = s;
    *(pin->in_not) = !s;
  }
}

