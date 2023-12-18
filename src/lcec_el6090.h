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
//    Edit A.Chimeno 12/3/19 Version 1.0

#ifndef _LCEC_EL6090_H_
#define _LCEC_EL6090_H_

#include "lcec.h"


#define LCEC_EL6090_VID LCEC_BECKHOFF_VID

#define LCEC_EL6090_PID 0x17ca3052

#define LCEC_EL6090_CHANS 4

#define LCEC_EL6090_PDOS 33


int lcec_el6090_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

#endif
