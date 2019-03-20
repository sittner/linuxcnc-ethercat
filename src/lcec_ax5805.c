//
//    Copyright (C) 2018 Sascha Ittner <sascha.ittner@modusoft.de>
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
#include "lcec_ax5805.h"

typedef struct {
  hal_u32_t *fsoe_master_cmd;
  hal_u32_t *fsoe_master_crc0;
  hal_u32_t *fsoe_master_crc1;
  hal_u32_t *fsoe_master_connid;

  hal_u32_t *fsoe_slave_cmd;
  hal_u32_t *fsoe_slave_crc0;
  hal_u32_t *fsoe_slave_crc1;
  hal_u32_t *fsoe_slave_connid;

  hal_bit_t *fsoe_in_sto1;
  hal_bit_t *fsoe_in_sto2;

  unsigned int fsoe_master_cmd_os;
  unsigned int fsoe_master_crc0_os;
  unsigned int fsoe_master_crc1_os;
  unsigned int fsoe_master_connid_os;

  unsigned int fsoe_slave_cmd_os;
  unsigned int fsoe_slave_crc0_os;
  unsigned int fsoe_slave_crc1_os;
  unsigned int fsoe_slave_connid_os;

  unsigned int fsoe_in_sto1_os;
  unsigned int fsoe_in_sto1_bp;
  unsigned int fsoe_in_sto2_os;
  unsigned int fsoe_in_sto2_bp;

} lcec_ax5805_data_t;

static const lcec_pindesc_t slave_pins[] = {
  { HAL_U32, HAL_OUT, offsetof(lcec_ax5805_data_t, fsoe_master_cmd), "%s.%s.%s.fsoe-master-cmd" },
  { HAL_U32, HAL_OUT, offsetof(lcec_ax5805_data_t, fsoe_master_crc0), "%s.%s.%s.fsoe-master-crc0" },
  { HAL_U32, HAL_OUT, offsetof(lcec_ax5805_data_t, fsoe_master_crc1), "%s.%s.%s.fsoe-master-crc1" },
  { HAL_U32, HAL_OUT, offsetof(lcec_ax5805_data_t, fsoe_master_connid), "%s.%s.%s.fsoe-master-connid" },
  { HAL_U32, HAL_OUT, offsetof(lcec_ax5805_data_t, fsoe_slave_cmd), "%s.%s.%s.fsoe-slave-cmd" },
  { HAL_U32, HAL_OUT, offsetof(lcec_ax5805_data_t, fsoe_slave_crc0), "%s.%s.%s.fsoe-slave-crc0" },
  { HAL_U32, HAL_OUT, offsetof(lcec_ax5805_data_t, fsoe_slave_crc1), "%s.%s.%s.fsoe-slave-crc1" },
  { HAL_U32, HAL_OUT, offsetof(lcec_ax5805_data_t, fsoe_slave_connid), "%s.%s.%s.fsoe-slave-connid" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_ax5805_data_t, fsoe_in_sto1), "%s.%s.%s.fsoe-in-sto-1" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_ax5805_data_t, fsoe_in_sto2), "%s.%s.%s.fsoe-in-sto-2" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static const LCEC_CONF_FSOE_T fsoe_conf = {
  .slave_data_len = 2,
  .master_data_len = 2,
  .data_channels = 2
};

void lcec_ax5805_read(struct lcec_slave *slave, long period);

int lcec_ax5805_preinit(struct lcec_slave *slave) {
  // set fsoe config
  slave->fsoeConf = &fsoe_conf;

  return 0;
}

int lcec_ax5805_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_ax5805_data_t *hal_data;
  int err;

  // initialize callbacks
  slave->proc_read = lcec_ax5805_read;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_ax5805_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_ax5805_data_t));
  slave->hal_data = hal_data;

  // initialize POD entries
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xE700, 0x01, &hal_data->fsoe_master_cmd_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xE700, 0x03, &hal_data->fsoe_master_crc0_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xE700, 0x04, &hal_data->fsoe_master_crc1_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xE700, 0x02, &hal_data->fsoe_master_connid_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xE600, 0x01, &hal_data->fsoe_slave_cmd_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6640, 0x00, &hal_data->fsoe_in_sto1_os, &hal_data->fsoe_in_sto1_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6E40, 0x00, &hal_data->fsoe_in_sto2_os, &hal_data->fsoe_in_sto2_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xE600, 0x03, &hal_data->fsoe_slave_crc0_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xE600, 0x04, &hal_data->fsoe_slave_crc1_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xE600, 0x02, &hal_data->fsoe_slave_connid_os, NULL);

  // export pins
  if ((err = lcec_pin_newf_list(hal_data, slave_pins, LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    return err;
  }

  return 0;
}

void lcec_ax5805_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_ax5805_data_t *hal_data = (lcec_ax5805_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;

  copy_fsoe_data(slave, hal_data->fsoe_slave_cmd_os, hal_data->fsoe_master_cmd_os);

  *(hal_data->fsoe_slave_cmd) = EC_READ_U8(&pd[hal_data->fsoe_slave_cmd_os]);
  *(hal_data->fsoe_slave_crc0) = EC_READ_U16(&pd[hal_data->fsoe_slave_crc0_os]);
  *(hal_data->fsoe_slave_crc1) = EC_READ_U16(&pd[hal_data->fsoe_slave_crc1_os]);
  *(hal_data->fsoe_slave_connid) = EC_READ_U16(&pd[hal_data->fsoe_slave_connid_os]);

  *(hal_data->fsoe_master_cmd) = EC_READ_U8(&pd[hal_data->fsoe_master_cmd_os]);
  *(hal_data->fsoe_master_crc0) = EC_READ_U16(&pd[hal_data->fsoe_master_crc0_os]);
  *(hal_data->fsoe_master_crc1) = EC_READ_U16(&pd[hal_data->fsoe_master_crc1_os]);
  *(hal_data->fsoe_master_connid) = EC_READ_U16(&pd[hal_data->fsoe_master_connid_os]);

  *(hal_data->fsoe_in_sto1) = EC_READ_BIT(&pd[hal_data->fsoe_in_sto1_os], hal_data->fsoe_in_sto1_bp);
  *(hal_data->fsoe_in_sto2) = EC_READ_BIT(&pd[hal_data->fsoe_in_sto2_os], hal_data->fsoe_in_sto2_bp);
}

