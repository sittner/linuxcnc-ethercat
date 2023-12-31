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

#include "lcec.h"
#include "lcec_ep23xx.h"

// This has been tested with the EP2338.  With the addition of a
// couple config entries, it should work fine with the EP2339 and
// EP2349 devices.  It *may* work with the EP23[012][89] and EP1859
// devices, depending on how their PDOs are configured.

static int lcec_ep23xx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[]={
  { "EP2308", LCEC_EP23xx_VID, LCEC_EP2308_PID, LCEC_EP2308_PDOS, 0, NULL, lcec_ep23xx_init},
  { "EP2318", LCEC_EP23xx_VID, LCEC_EP2318_PID, LCEC_EP2318_PDOS, 0, NULL, lcec_ep23xx_init},
  { "EP2328", LCEC_EP23xx_VID, LCEC_EP2328_PID, LCEC_EP2328_PDOS, 0, NULL, lcec_ep23xx_init},
  { "EP2338", LCEC_EP23xx_VID, LCEC_EP2338_PID, LCEC_EP2338_PDOS, 0, NULL, lcec_ep23xx_init},
  { "EP2349", LCEC_EP23xx_VID, LCEC_EP2349_PID, LCEC_EP2349_PDOS, 0, NULL, lcec_ep23xx_init},
  { NULL },
};
ADD_TYPES(types);

typedef struct {
  hal_bit_t *in;
  hal_bit_t *in_not;
  hal_bit_t *out;
  hal_bit_t invert;
  unsigned int in_pdo_os;
  unsigned int in_pdo_bp;
  unsigned int out_pdo_os;
  unsigned int out_pdo_bp;
} lcec_ep23xx_pin_t;

static const lcec_pindesc_t slave_pins[] = {
  { HAL_BIT, HAL_OUT, offsetof(lcec_ep23xx_pin_t, in), "%s.%s.%s.din-%d" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_ep23xx_pin_t, in_not), "%s.%s.%s.din-%d-not" },
  { HAL_BIT, HAL_IN, offsetof(lcec_ep23xx_pin_t, out), "%s.%s.%s.dout-%d"},
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static void lcec_ep23xx_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_ep23xx_pin_t *hal_data = (lcec_ep23xx_pin_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  lcec_ep23xx_pin_t *pin;
  int i, s;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  // check inputs
  for (i=0; i<slave->pdo_entry_count/2; i++) {
    pin = hal_data+i;
    s = EC_READ_BIT(&pd[pin->in_pdo_os], pin->in_pdo_bp);
    *(pin->in) = s;
    *(pin->in_not) = !s;
  }
}


static void lcec_ep23xx_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_ep23xx_pin_t *hal_data = (lcec_ep23xx_pin_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  lcec_ep23xx_pin_t *pin;
  int i, s;

  // set outputs
  for (i=0; i<slave->pdo_entry_count/2; i++) {
    pin = hal_data+i;
    s = *(pin->out);
    if (pin->invert) {
      s = !s;

    }
    EC_WRITE_BIT(&pd[pin->out_pdo_os], pin->out_pdo_bp, s);
  }
}


static int lcec_ep23xx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_ep23xx_pin_t *hal_data;
  lcec_ep23xx_pin_t *pin;
  int i;
  int err;
  int pin_count = slave->pdo_entry_count / 2;

  ec_pdo_entry_reg_t *initial_pdo = pdo_entry_regs;


  // initialize callbacks
  slave->proc_read = lcec_ep23xx_read;
  slave->proc_write = lcec_ep23xx_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_ep23xx_pin_t) * pin_count)) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_ep23xx_pin_t) * pin_count);
  slave->hal_data = hal_data;

  // initialize pins
  for (i=0, pin=hal_data; i<pin_count; i++, pin++) {  // div by 2 because of in+out
    // initialize POD entry
    int idx_offset = (i<<4);
    
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + idx_offset, 0x01, &pin->in_pdo_os, &pin->in_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000 + idx_offset, 0x01, &pin->out_pdo_os, &pin->out_pdo_bp);
 
    // export pins
    if ((err = lcec_pin_newf_list(pin, slave_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      return err;
    }

    // initialize pins
    *(pin->out) = 0;
    pin->invert = 0;
  }

  return 0;
}
