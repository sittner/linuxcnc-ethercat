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
#include "lcec_el2xxx.h"

static int lcec_el2xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[]={
  { "EL2002", LCEC_BECKHOFF_VID, 0x07D23052, LCEC_EL2002_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2004", LCEC_BECKHOFF_VID, 0x07D43052, LCEC_EL2004_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2008", LCEC_BECKHOFF_VID, 0x07D83052, LCEC_EL2008_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2022", LCEC_BECKHOFF_VID, 0x07E63052, LCEC_EL2022_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2024", LCEC_BECKHOFF_VID, 0x07E83052, LCEC_EL2024_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2032", LCEC_BECKHOFF_VID, 0x07F03052, LCEC_EL2032_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2034", LCEC_BECKHOFF_VID, 0x07F23052, LCEC_EL2034_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2042", LCEC_BECKHOFF_VID, 0x07FA3052, LCEC_EL2042_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2084", LCEC_BECKHOFF_VID, 0x08243052, LCEC_EL2084_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2088", LCEC_BECKHOFF_VID, 0x08283052, LCEC_EL2088_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2124", LCEC_BECKHOFF_VID, 0x084C3052, LCEC_EL2124_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2612", LCEC_BECKHOFF_VID, 0x0A343052, LCEC_EL2612_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2622", LCEC_BECKHOFF_VID, 0x0A3E3052, LCEC_EL2622_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2634", LCEC_BECKHOFF_VID, 0x0A4A3052, LCEC_EL2634_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2652", LCEC_BECKHOFF_VID, 0x0A5C3052, LCEC_EL2652_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2808", LCEC_BECKHOFF_VID, 0x0AF83052, LCEC_EL2808_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2798", LCEC_BECKHOFF_VID, 0x0AEE3052, LCEC_EL2798_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2809", LCEC_BECKHOFF_VID, 0x0AF93052, LCEC_EL2809_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EP2008", LCEC_BECKHOFF_VID, 0x07D84052, LCEC_EP2008_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EP2028", LCEC_BECKHOFF_VID, 0x07EC4052, LCEC_EP2028_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EP2809", LCEC_BECKHOFF_VID, 0x0AF94052, LCEC_EP2809_PDOS, 0, NULL, lcec_el2xxx_init},
  { NULL },
};
ADD_TYPES(types);

typedef struct {
  hal_bit_t *out;
  hal_bit_t invert;
  unsigned int pdo_os;
  unsigned int pdo_bp;
} lcec_el2xxx_pin_t;

static void lcec_el2xxx_write(struct lcec_slave *slave, long period);

static int lcec_el2xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el2xxx_pin_t *hal_data;
  lcec_el2xxx_pin_t *pin;
  int i;
  int err;

  // initialize callbacks
  slave->proc_write = lcec_el2xxx_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el2xxx_pin_t) * slave->pdo_entry_count)) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el2xxx_pin_t) * slave->pdo_entry_count);
  slave->hal_data = hal_data;

  // initialize pins
  for (i=0, pin=hal_data; i<slave->pdo_entry_count; i++, pin++) {
    // initialize POD entry
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000 + (i << 4), 0x01, &pin->pdo_os, &pin->pdo_bp);

    // export pins
    if ((err = lcec_pin_newf(HAL_BIT, HAL_IN, (void **) &(pin->out), "%s.%s.%s.dout-%d", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      return err;
    }
    if ((err = lcec_param_newf(HAL_BIT, HAL_RW, (void *) &(pin->invert), "%s.%s.%s.dout-%d-invert", LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      return err;
    }

    // initialize pins
    *(pin->out) = 0;
    pin->invert = 0;
  }

  return 0;
}

static void lcec_el2xxx_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el2xxx_pin_t *hal_data = (lcec_el2xxx_pin_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  lcec_el2xxx_pin_t *pin;
  int i, s;

  // set outputs
  for (i=0, pin=hal_data; i<slave->pdo_entry_count; i++, pin++) {
    s = *(pin->out);
    if (pin->invert) {
      s = !s;
    }
    EC_WRITE_BIT(&pd[pin->pdo_os], pin->pdo_bp, s);
  }
}

