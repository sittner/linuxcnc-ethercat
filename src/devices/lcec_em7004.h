//
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

/// @file
/// @brief Driver for Beckhoff EM7004 4-axis interface units

#ifndef _LCEC_EM7004_H_
#define _LCEC_EM7004_H_

#include "../lcec.h"

#define LCEC_EM7004_DIN_COUNT 16
#define LCEC_EM7004_DOUT_COUNT 16
#define LCEC_EM7004_AOUT_COUNT 4
#define LCEC_EM7004_ENC_COUNT 4

#define LCEC_EM7004_PDOS (LCEC_EM7004_DIN_COUNT + LCEC_EM7004_DOUT_COUNT + LCEC_EM7004_AOUT_COUNT + (LCEC_EM7004_ENC_COUNT * 12))

#endif
