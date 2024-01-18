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
#ifndef _LCEC_EL1859_H_
#define _LCEC_EL1859_H_

#include "../lcec.h"

#define LCEC_EL1859_PINS 8
#define LCEC_EL1859_PDOS (2 * LCEC_EL1859_PINS)
#define LCEC_EP2308_PDOS 8   // Can be in or out on each port, so 2 PDOs per port.
#define LCEC_EP2318_PDOS 8   // Can be in or out on each port, so 2 PDOs per port.
#define LCEC_EP2328_PDOS 8   // Can be in or out on each port, so 2 PDOs per port.
#define LCEC_EP2338_PDOS 16  // Can be in or out on each port, so 2 PDOs per port.
#define LCEC_EP2349_PDOS 32  // Can be in or out on each port, so 2 PDOs per port.
#endif
