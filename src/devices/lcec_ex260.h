//
// Created by chad on 2021-05-28.
//

/// @file
/// @brief Driver for SMC EX260 Valve controllers

#ifndef _LCEC_EX260_H
#define _LCEC_EX260_H

#include "../lcec.h"
//#include "../lcec_conf.h"

/* Master 0, Slave 3, "EX260-SEC3"
 * Vendor ID:       0x00000114
 * Product code:    0x01000003
 * Revision number: 0x00010002
 */

#define LCEC_EX260_VID LCEC_SMC_VID

#define LCEC_EX260_SEC1_PDOS 4
#define LCEC_EX260_SEC2_PDOS 4
#define LCEC_EX260_SEC3_PDOS 2
#define LCEC_EX260_SEC4_PDOS 2
#endif  //_LCEC_EX260_H
