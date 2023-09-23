//
//    Copyright (C) 2023 Sascha Ittner <sascha.ittner@modusoft.de>
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
#include "lcec_el1918.h"

typedef struct {
  hal_bit_t *fsoe_in;
  hal_bit_t *fsoe_in_not;
  hal_bit_t *fsoe_in_fault;
  hal_bit_t *fsoe_in_errack;

  unsigned int fsoe_in_os;
  unsigned int fsoe_in_bp;

  unsigned int fsoe_fault_os;
  unsigned int fsoe_fault_bp;

  unsigned int fsoe_errack_os;
  unsigned int fsoe_errack_bp;
} lcec_el1918_data_in_t;

typedef struct {
  hal_u32_t *fsoe_master_cmd;
  hal_u32_t *fsoe_master_crc;
  hal_u32_t *fsoe_master_connid;

  hal_u32_t *fsoe_slave_cmd;
  hal_u32_t *fsoe_slave_crc;
  hal_u32_t *fsoe_slave_connid;

  lcec_el1918_data_in_t inputs[LCEC_EL1918_INPUT_COUNT];

  unsigned int fsoe_master_cmd_os;
  unsigned int fsoe_master_crc_os;
  unsigned int fsoe_master_connid_os;

  unsigned int fsoe_slave_cmd_os;
  unsigned int fsoe_slave_crc_os;
  unsigned int fsoe_slave_connid_os;

} lcec_el1918_data_t;

static const lcec_pindesc_t slave_pins[] = {
  { HAL_U32, HAL_OUT, offsetof(lcec_el1918_data_t, fsoe_master_cmd), "%s.%s.%s.fsoe-master-cmd" },
  { HAL_U32, HAL_OUT, offsetof(lcec_el1918_data_t, fsoe_master_crc), "%s.%s.%s.fsoe-master-crc" },
  { HAL_U32, HAL_OUT, offsetof(lcec_el1918_data_t, fsoe_master_connid), "%s.%s.%s.fsoe-master-connid" },
  { HAL_U32, HAL_OUT, offsetof(lcec_el1918_data_t, fsoe_slave_cmd), "%s.%s.%s.fsoe-slave-cmd" },
  { HAL_U32, HAL_OUT, offsetof(lcec_el1918_data_t, fsoe_slave_crc), "%s.%s.%s.fsoe-slave-crc" },
  { HAL_U32, HAL_OUT, offsetof(lcec_el1918_data_t, fsoe_slave_connid), "%s.%s.%s.fsoe-slave-connid" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static const lcec_pindesc_t slave_in_pins[] = {
  { HAL_BIT, HAL_OUT, offsetof(lcec_el1918_data_in_t, fsoe_in), "%s.%s.%s.fsoe-in-%d" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el1918_data_in_t, fsoe_in_not), "%s.%s.%s.fsoe-in-%d-not" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el1918_data_in_t, fsoe_in_fault), "%s.%s.%s.fsoe-in-fault-%d" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el1918_data_in_t, fsoe_in_errack), "%s.%s.%s.fsoe-in-errack-%d" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static const LCEC_CONF_FSOE_T fsoe_conf = {
  .slave_data_len = 2,
  .master_data_len = 1,
  .data_channels = 1
};

void lcec_el1918_read(struct lcec_slave *slave, long period);
void lcec_el1918_write(struct lcec_slave *slave, long period);

int lcec_el1918_preinit(struct lcec_slave *slave) {
  // set fsoe config
  slave->fsoeConf = &fsoe_conf;

  return 0;
}

int lcec_el1918_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el1918_data_t *hal_data;
  int i, err;
  lcec_el1918_data_in_t *in;

  // initialize callbacks
  slave->proc_read = lcec_el1918_read;
  slave->proc_write = lcec_el1918_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el1918_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el1918_data_t));
  slave->hal_data = hal_data;

  // initialize POD entries
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7080, 0x01, &hal_data->fsoe_master_cmd_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7080, 0x03, &hal_data->fsoe_master_crc_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7080, 0x02, &hal_data->fsoe_master_connid_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6080, 0x01, &hal_data->fsoe_slave_cmd_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6080, 0x03, &hal_data->fsoe_slave_crc_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6080, 0x02, &hal_data->fsoe_slave_connid_os, NULL);
  for (i = 0, in = hal_data->inputs; i < LCEC_EL1918_INPUT_COUNT; i++, in++) {
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6001 + (i << 4), 0x01, &in->fsoe_in_os, &in->fsoe_in_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6002 + (i << 4), 0x01, &in->fsoe_fault_os, &in->fsoe_fault_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7002 + (i << 4), 0x01, &in->fsoe_errack_os, &in->fsoe_errack_bp);
  }

  // export pins
  if ((err = lcec_pin_newf_list(hal_data, slave_pins, LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    return err;
  }
  for (i = 0, in = hal_data->inputs; i < LCEC_EL1918_INPUT_COUNT; i++, in++) {
    if ((err = lcec_pin_newf_list(in, slave_in_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      return err;
    }
  }

  return 0;
}

void lcec_el1918_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el1918_data_t *hal_data = (lcec_el1918_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  int i;
  lcec_el1918_data_in_t *in;

  copy_fsoe_data(slave, hal_data->fsoe_slave_cmd_os, hal_data->fsoe_master_cmd_os);

  *(hal_data->fsoe_slave_cmd) = EC_READ_U8(&pd[hal_data->fsoe_slave_cmd_os]);
  *(hal_data->fsoe_slave_crc) = EC_READ_U16(&pd[hal_data->fsoe_slave_crc_os]);
  *(hal_data->fsoe_slave_connid) = EC_READ_U16(&pd[hal_data->fsoe_slave_connid_os]);

  *(hal_data->fsoe_master_cmd) = EC_READ_U8(&pd[hal_data->fsoe_master_cmd_os]);
  *(hal_data->fsoe_master_crc) = EC_READ_U16(&pd[hal_data->fsoe_master_crc_os]);
  *(hal_data->fsoe_master_connid) = EC_READ_U16(&pd[hal_data->fsoe_master_connid_os]);

//TODO: printf("in: %d %d %d %d\n", hal_data->fsoe_master_cmd_os, hal_data->fsoe_master_connid_os, hal_data->fsoe_slave_cmd_os, hal_data->fsoe_slave_connid_os);

  for (i = 0, in = hal_data->inputs; i < LCEC_EL1918_INPUT_COUNT; i++, in++) {
    *(in->fsoe_in) = EC_READ_BIT(&pd[in->fsoe_in_os], in->fsoe_in_bp);
    *(in->fsoe_in_not) = ! *(in->fsoe_in);
    *(in->fsoe_in_fault) = EC_READ_BIT(&pd[in->fsoe_fault_os], in->fsoe_fault_bp);
  }
}

void lcec_el1918_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el1918_data_t *hal_data = (lcec_el1918_data_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  int i;
  lcec_el1918_data_in_t *in;

  for (i = 0, in = hal_data->inputs; i < LCEC_EL1918_INPUT_COUNT; i++, in++) {
    EC_WRITE_BIT(&pd[in->fsoe_errack_os], in->fsoe_errack_bp, *(in->fsoe_in_errack));
  }
}

