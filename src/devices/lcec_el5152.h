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


/// @file
/// @brief Driver for Beckhoff EL5152 Encoder modules

#ifndef _LCEC_EL5152_H_
#define _LCEC_EL5152_H_

#include "../lcec.h"

#define LCEC_EL5152_CHANS 2
#define LCEC_EL5152_PDOS (9 * LCEC_EL5152_CHANS)

#define LCEC_EL5152_PERIOD_SCALE 1e-7
#endif
