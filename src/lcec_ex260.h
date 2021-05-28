//
// Created by chad on 2021-05-28.
//

#ifndef _LCEC_EX260_H
#define _LCEC_EX260_H

#include "lcec.h"

/* Master 0, Slave 3, "EX260-SEC3"
* Vendor ID:       0x00000114
* Product code:    0x01000003
* Revision number: 0x00010002
*/

#define LCEC_EX260_VID LCEC_SMC_VID
#define LCEC_EX260_PID 0x01000003

#define LCEC_EX260_PDOS 2

int lcec_ex260_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

#endif //_LCEC_EX260_H
