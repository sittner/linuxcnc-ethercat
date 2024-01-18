//
//    Copyright (C) 2015 Claudio lorini <claudio.lorini@iit.it>
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

/**   \brief Linuxcnc and Machinekit HAL driver for Beckhoff EL1252
      2-channel fast digital input terminal with timestamp.

      \details LatchPosX is time of the last positive edge event (0->1) on input X.
      LatchNegX is time of the last negative edge event (1_>0) on input X.
      StatusX.0 (bit 0) is set when a positive edge event (0->1) is detected on the inputX.
      StatusX.1 (bit 1) is set when a negative edge event (1->0) is detected on the inputX.
      StatusX capture bits are cleared by reading the corrisponding latch register.

      \note This driver at the moment provides pins ONLY for input data,
      timestamping infos is not exposed to HAL.

      http://www.beckhoff.com/english.asp?EtherCAT/el1252.htm */

#include "lcec_el1252.h"

#include "../lcec.h"
#include "lcec_class_din.h"

static int lcec_el1252_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[] = {
    {"EL1252", LCEC_BECKHOFF_VID, 0x04E43052, LCEC_EL1252_PDOS, 0, NULL, lcec_el1252_init},  // 2 fast channels with timestamp
    {NULL},
};
ADD_TYPES(types);

static void lcec_el1252_read(struct lcec_slave *slave, long period);

static int lcec_el1252_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_class_din_pins_t *hal_data;
  int i;

  // initialize callbacks
  slave->proc_read = lcec_el1252_read;

  hal_data = lcec_din_allocate_pins(2);
  if (hal_data == NULL) {
    return -EIO;
  }
  slave->hal_data = hal_data;

  for (i = 0; i < 2; i++) {
    hal_data->pins[i] = lcec_din_register_pin(&pdo_entry_regs, slave, i, 0x6000, 0x01 + i);
  }

  return 0;
}

static void lcec_el1252_read(struct lcec_slave *slave, long period) {
  lcec_class_din_pins_t *hal_data = (lcec_class_din_pins_t *)slave->hal_data;

  if (!slave->state.operational) {
    return;
  }

  lcec_din_read_all(slave, hal_data);
}
