//
// Created by chad on 2021-05-28.
//

#include "lcec_ex260.h"

typedef struct {
    hal_bit_t *sol_1a;
    hal_bit_t *sol_1b;
    hal_bit_t *sol_2a;
    hal_bit_t *sol_2b;
    hal_bit_t *sol_3a;
    hal_bit_t *sol_3b;
    hal_bit_t *sol_4a;
    hal_bit_t *sol_4b;
    hal_bit_t *sol_5a;
    hal_bit_t *sol_5b;
    hal_bit_t *sol_6a;
    hal_bit_t *sol_6b;
    hal_bit_t *sol_7a;
    hal_bit_t *sol_7b;
    unsigned int pdo_os;
    unsigned int pdo_bp;
} lcec_ex260_pin_t;

static const lcec_pindesc_t slave_pins[] = {
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_1a), "%s.%s.%s.sol-%d-1a" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_1b), "%s.%s.%s.sol-%d-1b" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_2a), "%s.%s.%s.sol-%d-2a" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_2b), "%s.%s.%s.sol-%d-2b" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_3a), "%s.%s.%s.sol-%d-3a" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_3b), "%s.%s.%s.sol-%d-3b" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_4a), "%s.%s.%s.sol-%d-4a" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_4b), "%s.%s.%s.sol-%d-4b" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_5a), "%s.%s.%s.sol-%d-5a" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_5b), "%s.%s.%s.sol-%d-5b" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_6a), "%s.%s.%s.sol-%d-6a" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_6b), "%s.%s.%s.sol-%d-6b" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_7a), "%s.%s.%s.sol-%d-7a" },
        { HAL_BIT, HAL_IN, offsetof(lcec_ex260_pin_t, sol_7b), "%s.%s.%s.sol-%d-7b" },
        { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

void lcec_ex260_write(struct lcec_slave *slave, long period);

int lcec_ex260_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_ex260_pin_t *hal_data;
  lcec_ex260_pin_t *pin;
  int i;
  int err;

  // initialize callbacks
  slave->proc_write = lcec_ex260_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_ex260_pin_t) * slave->pdo_entry_count)) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_ex260_pin_t) * slave->pdo_entry_count);
  slave->hal_data = hal_data;

  // initialize pins
  for (i=0, pin=hal_data; i<slave->pdo_entry_count; i++, pin++) {
    // initialize PDO entry
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x3101, 0x01 + i, &pin->pdo_os, &pin->pdo_bp);

    if ((err = lcec_pin_newf_list(pin, slave_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      return err;
    }

  }

  return 0;
}

void lcec_ex260_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_ex260_pin_t *hal_data = (lcec_ex260_pin_t *) slave->hal_data;
  uint8_t *pd = master->process_data;
  lcec_ex260_pin_t *pin;
  int i, s;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  // set outputs
  for (i=0, pin=hal_data; i<slave->pdo_entry_count; i++, pin++) {
    s = *(pin->sol_1a);

    EC_WRITE_BIT(&pd[pin->pdo_os], pin->pdo_bp, s);
  }
}

