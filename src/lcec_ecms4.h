//
//    Copyright (C) 2015 Claudio Lorini <claudio.lorini@iit.it>
//    Copyright (C) 2014 Sascha Ittner <sascha.ittner@modusoft.de>
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

#ifndef _LCEC_ECMS4_H_
#define _LCEC_ECMS4_H_

#include "lcec.h"

/** \brief Vendor ID */
#define LCEC_ECMS4_VID LCEC_IIT_VID

/** \brief Product Code */
#define LCEC_ECMS4_PID 0x00001421

/** \brief Number of channels */
// #define LCEC_ECMS4_CHANS 2 \todo: rimettere due canali
#define LCEC_ECMS4_CHANS 1

/** \brief Number of PDO */
#define LCEC_ECMS4_PDOS (7 * LCEC_ECMS4_CHANS)

int lcec_ecms4_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

#endif

