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
#ifndef _LCEC_EL3XXX_H_
#define _LCEC_EL3XXX_H_

#include "../lcec.h"

#define LCEC_EL3XXX_MAXCHANS 8  // for sizing arrays

#define LCEC_EL30XX_PDOS_PER_CHANNEL 4
#define LCEC_EL30X1_PDOS (LCEC_EL30XX_PDOS_PER_CHANNEL * 1)
#define LCEC_EL30X2_PDOS (LCEC_EL30XX_PDOS_PER_CHANNEL * 2)
#define LCEC_EL30X4_PDOS (LCEC_EL30XX_PDOS_PER_CHANNEL * 4)
#define LCEC_EL30X8_PDOS (LCEC_EL30XX_PDOS_PER_CHANNEL * 8)

#define LCEC_EL31XX_PDOS_PER_CHANNEL 5
#define LCEC_EL31X1_PDOS (LCEC_EL31XX_PDOS_PER_CHANNEL * 1)
#define LCEC_EL31X2_PDOS (LCEC_EL31XX_PDOS_PER_CHANNEL * 2)
#define LCEC_EL31X4_PDOS (LCEC_EL31XX_PDOS_PER_CHANNEL * 4)
#define LCEC_EL31X8_PDOS (LCEC_EL31XX_PDOS_PER_CHANNEL * 8)

#define LCEC_EL32XX_PDOS_PER_CHANNEL 4
#define LCEC_EL32X1_PDOS (LCEC_EL32XX_PDOS_PER_CHANNEL * 1)
#define LCEC_EL32X2_PDOS (LCEC_EL32XX_PDOS_PER_CHANNEL * 2)
#define LCEC_EL32X4_PDOS (LCEC_EL32XX_PDOS_PER_CHANNEL * 4)
#define LCEC_EL32X8_PDOS (LCEC_EL32XX_PDOS_PER_CHANNEL * 8)

#define LCEC_EM37XX_PDOS_PER_CHANNEL 4
#define LCEC_EM37X1_PDOS (LCEC_EM37XX_PDOS_PER_CHANNEL * 1)
#define LCEC_EM37X2_PDOS (LCEC_EM37XX_PDOS_PER_CHANNEL * 2)
// The EP3744 is sort of similar, but there's some weirdness:
// - It has 5 pressure ports
// - It has 6 digital input ports
// - It has 2 digital output ports
// - The pressure ports are labeled 1-5, but start at 0x6020, which is where port 2 should live.
#endif
