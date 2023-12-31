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

#include "lcec.h"
#include "lcec_ax5100.h"
#include "lcec_ax5200.h"
#include "lcec_ax5805.h"
#include "lcec_deasda.h"
#include "lcec_dems300.h"
#include "lcec_ek1100.h"
#include "lcec_el1252.h"
#include "lcec_el1859.h"
#include "lcec_el1904.h"
#include "lcec_el1918_logic.h"
#include "lcec_el2202.h"
#include "lcec_el2521.h"
#include "lcec_el2904.h"
#include "lcec_el2xxx.h"
#include "lcec_el30x4.h"
#include "lcec_el31x2.h"
#include "lcec_el31x4.h"
#include "lcec_el3202.h"
#include "lcec_el3255.h"
#include "lcec_el3403.h"
#include "lcec_el40x1.h"
#include "lcec_el40x2.h"
#include "lcec_el40x8.h"
#include "lcec_el41x2.h"
#include "lcec_el41x4.h"
#include "lcec_el5002.h"
#include "lcec_el5032.h"
#include "lcec_el5101.h"
#include "lcec_el5151.h"
#include "lcec_el5152.h"
#include "lcec_el6090.h"
#include "lcec_el6900.h"
#include "lcec_el7041.h"
#include "lcec_el70x1.h"
#include "lcec_el7211.h"
#include "lcec_el7342.h"
#include "lcec_el7411.h"
#include "lcec_el95xx.h"
#include "lcec_em37xx.h"
#include "lcec_em7004.h"
#include "lcec_ep2316.h"
#include "lcec_ep23xx.h"
#include "lcec_generic.h"
#include "lcec_omrg5.h"
#include "lcec_ph3lm2rm.h"
#include "lcec_stmds5k.h"

static lcec_typelinkedlist_t *typeslist = NULL;

static const lcec_typelist_t types[] = {
  // bus coupler
  { "EK1100", LCEC_EK1100_VID, LCEC_EK1100_PID, LCEC_EK1100_PDOS, 0, NULL, NULL},
  { "EK1101", LCEC_EK1100_VID, LCEC_EK1101_PID, LCEC_EK1101_PDOS, 0, NULL, NULL},
  { "EK1110", LCEC_EK1100_VID, LCEC_EK1110_PID, LCEC_EK1110_PDOS, 0, NULL, NULL},
  { "EK1122", LCEC_EK1100_VID, LCEC_EK1122_PID, LCEC_EK1122_PDOS, 0, NULL, NULL},

  // AX5000 servo drives
  { "AX5101", LCEC_AX5100_VID, LCEC_AX5101_PID, 0, 0, lcec_ax5100_preinit, lcec_ax5100_init},
  { "AX5103", LCEC_AX5100_VID, LCEC_AX5103_PID, 0, 0, lcec_ax5100_preinit, lcec_ax5100_init},
  { "AX5106", LCEC_AX5100_VID, LCEC_AX5106_PID, 0, 0, lcec_ax5100_preinit, lcec_ax5100_init},
  { "AX5112", LCEC_AX5100_VID, LCEC_AX5112_PID, 0, 0, lcec_ax5100_preinit, lcec_ax5100_init},
  { "AX5118", LCEC_AX5100_VID, LCEC_AX5118_PID, 0, 0, lcec_ax5100_preinit, lcec_ax5100_init},
  { "AX5203", LCEC_AX5200_VID, LCEC_AX5203_PID, 0, 0, lcec_ax5200_preinit, lcec_ax5200_init},
  { "AX5206", LCEC_AX5200_VID, LCEC_AX5206_PID, 0, 0, lcec_ax5200_preinit, lcec_ax5200_init},

  // digital in; see also lcec_el1xxx.c
  { "EL1252", LCEC_EL1252_VID, LCEC_EL1252_PID, LCEC_EL1252_PDOS, 0, NULL, lcec_el1252_init},  // 2 fast channels with timestamp

  // digital out
  { "EL2002", LCEC_EL2xxx_VID, LCEC_EL2002_PID, LCEC_EL2002_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2004", LCEC_EL2xxx_VID, LCEC_EL2004_PID, LCEC_EL2004_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2008", LCEC_EL2xxx_VID, LCEC_EL2008_PID, LCEC_EL2008_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2022", LCEC_EL2xxx_VID, LCEC_EL2022_PID, LCEC_EL2022_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2024", LCEC_EL2xxx_VID, LCEC_EL2024_PID, LCEC_EL2024_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2032", LCEC_EL2xxx_VID, LCEC_EL2032_PID, LCEC_EL2032_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2034", LCEC_EL2xxx_VID, LCEC_EL2034_PID, LCEC_EL2034_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2042", LCEC_EL2xxx_VID, LCEC_EL2042_PID, LCEC_EL2042_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2084", LCEC_EL2xxx_VID, LCEC_EL2084_PID, LCEC_EL2084_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2088", LCEC_EL2xxx_VID, LCEC_EL2088_PID, LCEC_EL2088_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2124", LCEC_EL2xxx_VID, LCEC_EL2124_PID, LCEC_EL2124_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2202", LCEC_EL2202_VID, LCEC_EL2202_PID, LCEC_EL2202_PDOS, 0, NULL, lcec_el2202_init}, // 2 fast channels with tristate
  { "EL2612", LCEC_EL2xxx_VID, LCEC_EL2612_PID, LCEC_EL2612_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2622", LCEC_EL2xxx_VID, LCEC_EL2622_PID, LCEC_EL2622_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2634", LCEC_EL2xxx_VID, LCEC_EL2634_PID, LCEC_EL2634_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2652", LCEC_EL2xxx_VID, LCEC_EL2652_PID, LCEC_EL2652_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2808", LCEC_EL2xxx_VID, LCEC_EL2808_PID, LCEC_EL2808_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2798", LCEC_EL2xxx_VID, LCEC_EL2798_PID, LCEC_EL2798_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EL2809", LCEC_EL2xxx_VID, LCEC_EL2809_PID, LCEC_EL2809_PDOS, 0, NULL, lcec_el2xxx_init},

  { "EP2008", LCEC_EL2xxx_VID, LCEC_EP2008_PID, LCEC_EP2008_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EP2028", LCEC_EL2xxx_VID, LCEC_EP2028_PID, LCEC_EP2028_PDOS, 0, NULL, lcec_el2xxx_init},
  { "EP2809", LCEC_EL2xxx_VID, LCEC_EP2809_PID, LCEC_EP2809_PDOS, 0, NULL, lcec_el2xxx_init},

  // digital in/out
  { "EL1859", LCEC_EL1859_VID, LCEC_EL1859_PID, LCEC_EL1859_PDOS, 0, NULL, lcec_el1859_init},
  { "EP2308", LCEC_EP23xx_VID, LCEC_EP2308_PID, LCEC_EP2308_PDOS, 0, NULL, lcec_ep23xx_init},
  { "EP2318", LCEC_EP23xx_VID, LCEC_EP2318_PID, LCEC_EP2318_PDOS, 0, NULL, lcec_ep23xx_init},
  { "EP2328", LCEC_EP23xx_VID, LCEC_EP2328_PID, LCEC_EP2328_PDOS, 0, NULL, lcec_ep23xx_init},
  { "EP2338", LCEC_EP23xx_VID, LCEC_EP2338_PID, LCEC_EP2338_PDOS, 0, NULL, lcec_ep23xx_init},
  { "EP2349", LCEC_EP23xx_VID, LCEC_EP2349_PID, LCEC_EP2349_PDOS, 0, NULL, lcec_ep23xx_init},
  { "EP2316", LCEC_EP23xx_VID, LCEC_EP2316_PID, LCEC_EP2316_PDOS, 0, NULL, lcec_ep2316_init},

  // analog in, 4ch, 12 bits
  { "EL3004", LCEC_EL30x4_VID, LCEC_EL3004_PID, LCEC_EL30x4_PDOS, 0, NULL, lcec_el30x4_init},
  { "EL3044", LCEC_EL30x4_VID, LCEC_EL3044_PID, LCEC_EL30x4_PDOS, 0, NULL, lcec_el30x4_init},
  { "EL3054", LCEC_EL30x4_VID, LCEC_EL3054_PID, LCEC_EL30x4_PDOS, 0, NULL, lcec_el30x4_init},
  { "EL3064", LCEC_EL30x4_VID, LCEC_EL3064_PID, LCEC_EL30x4_PDOS, 0, NULL, lcec_el30x4_init},

  // analog in, 2ch, 16 bits
  { "EL3102", LCEC_EL31x2_VID, LCEC_EL3102_PID, LCEC_EL31x2_PDOS, 0, NULL, lcec_el31x2_init},
  { "EL3112", LCEC_EL31x2_VID, LCEC_EL3112_PID, LCEC_EL31x2_PDOS, 0, NULL, lcec_el31x2_init},
  { "EL3122", LCEC_EL31x2_VID, LCEC_EL3122_PID, LCEC_EL31x2_PDOS, 0, NULL, lcec_el31x2_init},
  { "EL3142", LCEC_EL31x2_VID, LCEC_EL3142_PID, LCEC_EL31x2_PDOS, 0, NULL, lcec_el31x2_init},
  { "EL3152", LCEC_EL31x2_VID, LCEC_EL3152_PID, LCEC_EL31x2_PDOS, 0, NULL, lcec_el31x2_init},
  { "EL3162", LCEC_EL31x2_VID, LCEC_EL3162_PID, LCEC_EL31x2_PDOS, 0, NULL, lcec_el31x2_init},
  { "EL3202", LCEC_EL3202_VID, LCEC_EL3202_PID, LCEC_EL3202_PDOS, 0, NULL, lcec_el3202_init},

  // analog in, 2ch, 16 bits
  { "EL3164", LCEC_EL31x4_VID, LCEC_EL3164_PID, LCEC_EL31x4_PDOS, 0, NULL, lcec_el31x4_init},

  // analog in, 5ch, 16 bits
  { "EL3255", LCEC_EL3255_VID, LCEC_EL3255_PID, LCEC_EL3255_PDOS, 0, NULL, lcec_el3255_init},

  // analog in, 3ch, 16 bits
  { "EL3403", LCEC_EL3403_VID, LCEC_EL3403_PID, LCEC_EL3403_PDOS, 0, NULL, lcec_el3403_init},

  // analog out, 1ch, 12 bits
  { "EL4001", LCEC_EL40x1_VID, LCEC_EL4001_PID, LCEC_EL40x1_PDOS, 0, NULL, lcec_el40x1_init},
  { "EL4011", LCEC_EL40x1_VID, LCEC_EL4011_PID, LCEC_EL40x1_PDOS, 0, NULL, lcec_el40x1_init},
  { "EL4021", LCEC_EL40x1_VID, LCEC_EL4021_PID, LCEC_EL40x1_PDOS, 0, NULL, lcec_el40x1_init},
  { "EL4031", LCEC_EL40x1_VID, LCEC_EL4031_PID, LCEC_EL40x1_PDOS, 0, NULL, lcec_el40x1_init},

  // analog out, 2ch, 12 bits
  { "EL4002", LCEC_EL40x2_VID, LCEC_EL4002_PID, LCEC_EL40x2_PDOS, 0, NULL, lcec_el40x2_init},
  { "EL4012", LCEC_EL40x2_VID, LCEC_EL4012_PID, LCEC_EL40x2_PDOS, 0, NULL, lcec_el40x2_init},
  { "EL4022", LCEC_EL40x2_VID, LCEC_EL4022_PID, LCEC_EL40x2_PDOS, 0, NULL, lcec_el40x2_init},
  { "EL4032", LCEC_EL40x2_VID, LCEC_EL4032_PID, LCEC_EL40x2_PDOS, 0, NULL, lcec_el40x2_init},

  // analog out, 2ch, 16 bits
  { "EL4102", LCEC_EL41x2_VID, LCEC_EL4102_PID, LCEC_EL41x2_PDOS, 0, NULL, lcec_el41x2_init},
  { "EL4112", LCEC_EL41x2_VID, LCEC_EL4112_PID, LCEC_EL41x2_PDOS, 0, NULL, lcec_el41x2_init},
  { "EL4122", LCEC_EL41x2_VID, LCEC_EL4122_PID, LCEC_EL41x2_PDOS, 0, NULL, lcec_el41x2_init},
  { "EL4132", LCEC_EL41x2_VID, LCEC_EL4132_PID, LCEC_EL41x2_PDOS, 0, NULL, lcec_el41x2_init},

  // analog out, 4ch, 16 bits
  { "EL4104", LCEC_EL41x4_VID, LCEC_EL4104_PID, LCEC_EL41x4_PDOS, 0, NULL, lcec_el41x4_init},
  { "EL4134", LCEC_EL41x4_VID, LCEC_EL4134_PID, LCEC_EL41x4_PDOS, 0, NULL, lcec_el41x4_init},

  // analog out, 8ch, 12 bits
  { "EL4008", LCEC_EL40x8_VID, LCEC_EL4008_PID, LCEC_EL40x8_PDOS, 0, NULL, lcec_el40x8_init},
  { "EL4018", LCEC_EL40x8_VID, LCEC_EL4018_PID, LCEC_EL40x8_PDOS, 0, NULL, lcec_el40x8_init},
  { "EL4028", LCEC_EL40x8_VID, LCEC_EL4028_PID, LCEC_EL40x8_PDOS, 0, NULL, lcec_el40x8_init},
  { "EL4038", LCEC_EL40x8_VID, LCEC_EL4038_PID, LCEC_EL40x8_PDOS, 0, NULL, lcec_el40x8_init},

  // encoder inputs
  { "EL5002", LCEC_EL5002_VID, LCEC_EL5002_PID, LCEC_EL5002_PDOS, 0, NULL, lcec_el5002_init},
  { "EL5032", LCEC_EL5032_VID, LCEC_EL5032_PID, LCEC_EL5032_PDOS, 0, NULL, lcec_el5032_init},
  { "EL5101", LCEC_EL5101_VID, LCEC_EL5101_PID, LCEC_EL5101_PDOS, 0, NULL, lcec_el5101_init},
  { "EL5151", LCEC_EL5151_VID, LCEC_EL5151_PID, LCEC_EL5151_PDOS, 0, NULL, lcec_el5151_init},
  { "EL5152", LCEC_EL5152_VID, LCEC_EL5152_PID, LCEC_EL5152_PDOS, 0, NULL, lcec_el5152_init},

  // pulse train (stepper) output
  { "EL2521", LCEC_EL2521_VID, LCEC_EL2521_PID, LCEC_EL2521_PDOS, 0, NULL, lcec_el2521_init},

  // stepper
  { "EL7031", LCEC_EL70x1_VID, LCEC_EL7031_PID, LCEC_EL70x1_PDOS, 0, NULL, lcec_el7031_init},
  { "EL7041", LCEC_EL7041_VID, LCEC_EL7041_PID, LCEC_EL7041_PDOS, 0, NULL, lcec_el7041_init},
  { "EL7041_1000", LCEC_EL7041_VID, LCEC_EL7041_1000_PID, LCEC_EL7041_1000_PDOS, 0, NULL, lcec_el7041_init},
  { "EP7041", LCEC_EL7041_VID, LCEC_EP7041_PID, LCEC_EP7041_PDOS, 0, NULL, lcec_el7041_init},

  // ac servo
  { "EL7201_9014", LCEC_EL7211_VID, LCEC_EL7201_9014_PID, LCEC_EL7201_9014_PDOS, 0, NULL, lcec_el7201_9014_init},
  { "EL7211", LCEC_EL7211_VID, LCEC_EL7211_PID, LCEC_EL7211_PDOS, 0, NULL, lcec_el7211_init},
  { "EL7221", LCEC_EL7211_VID, LCEC_EL7221_PID, LCEC_EL7211_PDOS, 0, NULL, lcec_el7211_init},

  // dc servo
  { "EL7342", LCEC_EL7342_VID, LCEC_EL7342_PID, LCEC_EL7342_PDOS, 0, NULL, lcec_el7342_init},

  // BLDC
  { "EL7411", LCEC_EL7411_VID, LCEC_EL7411_PID, LCEC_EL7411_PDOS, 0, NULL, lcec_el7411_init},

  // power supply
  { "EL9505", LCEC_EL95xx_VID, LCEC_EL9505_PID, LCEC_EL95xx_PDOS, 0, NULL, lcec_el95xx_init},
  { "EL9508", LCEC_EL95xx_VID, LCEC_EL9508_PID, LCEC_EL95xx_PDOS, 0, NULL, lcec_el95xx_init},
  { "EL9510", LCEC_EL95xx_VID, LCEC_EL9510_PID, LCEC_EL95xx_PDOS, 0, NULL, lcec_el95xx_init},
  { "EL9512", LCEC_EL95xx_VID, LCEC_EL9512_PID, LCEC_EL95xx_PDOS, 0, NULL, lcec_el95xx_init},
  { "EL9515", LCEC_EL95xx_VID, LCEC_EL9515_PID, LCEC_EL95xx_PDOS, 0, NULL, lcec_el95xx_init},
  { "EL9576", LCEC_EL95xx_VID, LCEC_EL9576_PID, LCEC_EL95xx_PDOS, 0, NULL, lcec_el95xx_init},

  // Display Terminal
  { "EL6090", LCEC_EL6090_VID, LCEC_EL6090_PID, LCEC_EL6090_PDOS, 0, NULL, lcec_el6090_init},

  // FSoE devices
  { "EL6900", LCEC_EL6900_VID, LCEC_EL6900_PID, 0, 1, lcec_el6900_preinit, lcec_el6900_init},
  { "EL1918_LOGIC", LCEC_EL1918_LOGIC_VID, LCEC_EL1918_LOGIC_PID, 0, 1, lcec_el1918_logic_preinit, lcec_el1918_logic_init},
  { "EL1904", LCEC_EL1904_VID, LCEC_EL1904_PID, LCEC_EL1904_PDOS, 0, lcec_el1904_preinit, lcec_el1904_init},
  { "EL2904", LCEC_EL2904_VID, LCEC_EL2904_PID, LCEC_EL2904_PDOS, 0, lcec_el2904_preinit, lcec_el2904_init},
  { "AX5805", LCEC_AX5805_VID, LCEC_AX5805_PID, 0, 0, lcec_ax5805_preinit, lcec_ax5805_init},

  // pressure sensor
  { "EM3701", LCEC_EM37XX_VID, LCEC_EM3701_PID, LCEC_EM37XX_PDOS, 0, NULL, lcec_em37xx_init},
  { "EM3702", LCEC_EM37XX_VID, LCEC_EM3702_PID, LCEC_EM37XX_PDOS, 0, NULL, lcec_em37xx_init},
  { "EM3712", LCEC_EM37XX_VID, LCEC_EM3712_PID, LCEC_EM37XX_PDOS, 0, NULL, lcec_em37xx_init},

  // multi axis interface
  { "EM7004", LCEC_EM7004_VID, LCEC_EM7004_PID, LCEC_EM7004_PDOS, 0, NULL, lcec_em7004_init},

  // stoeber MDS5000 series
  { "StMDS5k", LCEC_STMDS5K_VID, LCEC_STMDS5K_PID, 0, 0, lcec_stmds5k_preinit, lcec_stmds5k_init},

  // Delta ASDA series
  { "DeASDA", LCEC_DEASDA_VID, LCEC_DEASDA_PID, LCEC_DEASDA_PDOS, 0, NULL, lcec_deasda_init},

  // Delta MS/MH300 series
  { "DeMS300", LCEC_DEMS300_VID, LCEC_DEMS300_PID, LCEC_DEMS300_PDOS, 0, NULL, lcec_dems300_init},

  // Omron G5 series
  { "OmrG5_KNA5L",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KNA5L_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN01L",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN01L_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN02L",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN02L_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN04L",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN04L_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN01H",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN01H_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN02H",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN02H_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN04H",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN04H_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN08H",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN08H_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN10H",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN10H_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN15H",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN15H_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN20H",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN20H_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN30H",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN30H_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN50H",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN50H_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN75H",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN75H_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN150H", LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN150H_ECT_PID, LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN06F",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN06F_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN10F",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN10F_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN15F",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN15F_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN20F",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN20F_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN30F",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN30F_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN50F",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN50F_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN75F",  LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN75F_ECT_PID,  LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},
  { "OmrG5_KN150F", LCEC_OMRG5_VID, LCEC_OMRG5_R88D_KN150F_ECT_PID, LCEC_OMRG5_PDOS, 0, NULL, lcec_omrg5_init},

  // modusoft PH3LM2RM converter
  { "Ph3LM2RM", LCEC_PH3LM2RM_VID, LCEC_PH3LM2RM_PID, LCEC_PH3LM2RM_PDOS, 0, NULL, lcec_ph3lm2rm_init},

  { NULL }
};

// Add a single slave type to the `typeslist` linked-list, so it can
// be looked up by name.
void lcec_addtype(lcec_typelist_t *type) {
  lcec_typelinkedlist_t *t, *l;

  // using malloc instead of hal_malloc because this can be called
  // from either lcec.so (inside of LinuxCNC) or lcec_conf (a
  // standalone binary).
  t = malloc(sizeof(lcec_typelinkedlist_t));
  t->type = type;
  t->next = NULL;

  if (typeslist == NULL) {
    typeslist=t;
  } else {
    for (l=typeslist; l->next != NULL; l=l->next);
    l->next = t;
  }
}

// Add an array of slavetypes to the `typeslist` linked-lisk.
void lcec_addtypes(lcec_typelist_t types[]) {
  lcec_typelist_t *type;

  for (type = types; type->name != NULL; type++) {
    lcec_addtype(type);
  }
}

// Find a slave type by name.
lcec_typelist_t *lcec_findslavetype(char *name) {
  lcec_typelist_t *type;
  lcec_typelinkedlist_t *tl;
  
  // Look in the old-stype types[] array
  for (type = types; type->name != NULL && strcmp(type->name, name); type++);
  if (type->name != NULL) {
    return type;
  }
  
  // Look in the newer typeslist linked-list
  for (tl = typeslist; tl != NULL && tl->type != NULL && strcmp(tl->type->name, name) ; tl=tl->next);
  
  if (tl != NULL) {
    return tl->type;
  }
  
  // Not found
  return NULL;
}

