//
//    Copyright (C) 2023 Scott Laird <scott@sigkill.org>
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
/// @brief Driver for "passive" devices with no PDOs.

#include "lcec.h"
#include "lcec_passive.h"

static lcec_typelist_t types[] = {
  // bus coupler, no actual driver.
  { "EK1100", LCEC_BECKHOFF_VID, 0x044C2C52, LCEC_EK1100_PDOS, 0, NULL, NULL},
  { "EK1101", LCEC_BECKHOFF_VID, 0x044D2C52, LCEC_EK1101_PDOS, 0, NULL, NULL},
  { "EK1110", LCEC_BECKHOFF_VID, 0x04562C52, LCEC_EK1110_PDOS, 0, NULL, NULL},
  { "EK1122", LCEC_BECKHOFF_VID, 0x04622C52, LCEC_EK1122_PDOS, 0, NULL, NULL},
  { "EP1122", LCEC_BECKHOFF_VID, 0x04624052, LCEC_EP1122_PDOS, 0, NULL, NULL},
  { NULL },
};

ADD_TYPES(types);
