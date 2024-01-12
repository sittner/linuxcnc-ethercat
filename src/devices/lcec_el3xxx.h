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


#define LCEC_EL3001_PID 0x0bb93052  // https://beckhoff.com/EL3001, 1-port +/- 10V single-ended
#define LCEC_EL3002_PID 0x0bba3052  // https://beckhoff.com/EL3002, 2-port +/- 10V single-ended
//#define LCEC_EL3004_PID 0x0bbc3052  // https://beckhoff.com/EL3004, 1-port +/- 10V  single-ended // covered by lcec_el30x4 for now
#define LCEC_EL3008_PID 0x0bc03052  // https://beckhoff.com/EL3008, 8-port +/- 10V single-ended
#define LCEC_EL3011_PID 0x0bc33052  // https://beckhoff.com/EL3011, 1-port 0...20mA diff
#define LCEC_EL3012_PID 0x0bc43052  // https://beckhoff.com/EL3012, 2-port 0...20mA diff
#define LCEC_EL3014_PID 0x0bc63052  // https://beckhoff.com/EL3014, 4-port 0...20mA diff
#define LCEC_EL3021_PID 0x0bcd3052  // https://beckhoff.com/EL3011, 1-port 4...20mA diff
#define LCEC_EL3022_PID 0x0bce3052  // https://beckhoff.com/EL3011, 1-port 4...20mA diff
#define LCEC_EL3024_PID 0x0bd03052  // https://beckhoff.com/EL3011, 1-port 4...20mA diff
#define LCEC_EL3041_PID 0x0be13052  // https://beckhoff.com/EL3041, 1-port 0...20mA single-ended
#define LCEC_EL3042_PID 0x0be23052  // https://beckhoff.com/EL3042, 2-port 0...20mA single-ended
//#define LCEC_EL3044_PID 0x0be43052  // https://beckhoff.com/EL3044, 4-port 0...20mA single-ended  // covered by lcec_el30x4 for now
#define LCEC_EL3048_PID 0x0be83052  // https://beckhoff.com/EL3048, 8-port 0...20mA single-ended
#define LCEC_EL3051_PID 0x0beb3052  // https://beckhoff.com/EL3051, 1-port 4...20mA single-ended
#define LCEC_EL3052_PID 0x0bec3052  // https://beckhoff.com/EL3052, 2-port 4...20mA single-ended
//#define LCEC_EL3054_PID 0x0bee3052  // https://beckhoff.com/EL3054, 4-port 4...20mA single-ended  // covered by lcec_el30x4 for now
#define LCEC_EL3058_PID 0x0bf23052  // https://beckhoff.com/EL3058, 8-port 4...20mA single-ended
#define LCEC_EL3061_PID 0x0bf53052  // https://beckhoff.com/EL3061, 1-port 0...10V single-ended
#define LCEC_EL3062_PID 0x0bf63052  // https://beckhoff.com/EL3062, 2-port 0...10V single-ended
//#define LCEC_EL3064_PID 0x0bf83052  // https://beckhoff.com/EL3064, 4-port 0...10V single-ended // covered by lcec_el30x4 for now
#define LCEC_EL3068_PID 0x0bfc3052  // https://beckhoff.com/EL3068, 8-port 0...10V single-ended

#define LCEC_EL3101_PID 0x0c1d3052  // https://beckhoff.com/EL3101, 1-port +/- 10V diff
//#define LCEC_EL3102_PID 0x0c1e3052  // https://beckhoff.com/EL3102, 2-port +/- 10V diff  // covered by lcec_el31x2 for now
#define LCEC_EL3104_PID 0x0c203052  // https://beckhoff.com/EL3104, 4-port +/- 10V diff
#define LCEC_EL3111_PID 0x0c273052  // https://beckhoff.com/EL3111, 1-port 0...20mA diff
//#define LCEC_EL3112_PID 0x0c283052  // https://beckhoff.com/EL3112, 2-port 0...20mA diff  // covered by lcec_el31x2 for now
#define LCEC_EL3114_PID 0x0c2a3052  // https://beckhoff.com/EL3114, 4-port 0...20mA diff
#define LCEC_EL3121_PID 0x0c313052  // https://beckhoff.com/EL3121, 1-port 4...20mA diff
//#define LCEC_EL3122_PID 0x0c323052  // https://beckhoff.com/EL3122, 2-port 4...20mA diff  // covered by lcec_el31x2 for now
//#define LCEC_EL3124_PID 0x0c343052  // https://beckhoff.com/EL3124, 4-port 4...20mA diff  // covered by lcec_el31x4 for now
#define LCEC_EL3141_PID 0x0c453052  // https://beckhoff.com/EL3141, 1-port 0...20mA single-ended
//#define LCEC_EL3142_PID 0x0c463052  // https://beckhoff.com/EL3142, 2-port 0...20mA single-ended  // covered by lcec_el31x2 for now
#define LCEC_EL3144_PID 0x0c483052  // https://beckhoff.com/EL3144, 4-port 0...20mA single-ended
#define LCEC_EL3151_PID 0x0c4f3052  // https://beckhoff.com/EL3151, 1-port 4...20mA single-ended
//#define LCEC_EL3152_PID 0x0c503052  // https://beckhoff.com/EL3152, 2-port 4...20mA single-ended  // covered by lcec_el31x2 for now
#define LCEC_EL3154_PID 0x0c523052  // https://beckhoff.com/EL3154, 4-port 4...20mA single-ended
#define LCEC_EL3161_PID 0x0c593052  // https://beckhoff.com/EL3161, 1-port 0...10V single-ended
//#define LCEC_EL3162_PID 0x0c5a3052  // https://beckhoff.com/EL3162, 2-port 0...10V single-ended  // covered by lcec_el31x2 for now
//#define LCEC_EL3164_PID 0x0c5c3052  // https://beckhoff.com/EL3164, 4-port 0...10V single-ended // covered by lcec_el31x4 for now
#define LCEC_EL3182_PID 0x0c6e3052   // https://beckhoff.com/EL3182, 2-port 0/4...20mA single-ended HART
#define LCEC_EPX3158_PID 0x9809ab69  // https://beckhoff.com/EPX5185, 8-port 4..20mA single-ended, hazardous area

#define LCEC_EL3201_PID 0x0c813052  // https://beckhoff.com/EL3201, 1-port PT100 temperature sensor
#define LCEC_EL3202_PID 0x0c823052  // https://beckhoff.com/EL3202, 2-port PT100 temperature sensor
#define LCEC_EL3204_PID 0x0c843052  // https://beckhoff.com/EL3204, 4-port PT100 temperature sensor
#define LCEC_EL3208_PID 0x0c883052  // https://beckhoff.com/EL3208, 8-port PT100 temperature sensor
#define LCEC_EL3214_PID 0x0c8e3052  // https://beckhoff.com/EL3214, 4-port PT100 temperature sensor
#define LCEC_EL3218_PID 0x0c923052  // https://beckhoff.com/EL3218, 8-port PT100 temperature sensor
#define LCEC_EP3204_PID 0x0c844052  // https://beckhoff.com/EP3204, 4-port PT100 temperature sensor
#define LCEC_EJ3202_PID 0x0c822852  // https://beckhoff.com/EJ3202, 2-port PT100 temperature sensor
#define LCEC_EJ3214_PID 0x0c8e2852  // https://beckhoff.com/EJ3214, 4-port PT100 temperature sensor

#define LCEC_EM3701_PID 0x0e753452  // https://beckhoff.com/EM3701, 1-channel analog pressure sensor
#define LCEC_EM3702_PID 0x0e763452  // https://beckhoff.com/EM3702, 2-channel analog pressure sensor
#define LCEC_EM3712_PID 0x0e803452  // https://beckhoff.com/EM3712, 2-channel analog pressure sensor

// Related-but-not-quite-the-same devices, kept here for the moment.
//#define LCEC_EL3072_PID 0x0c003052  // https://beckhoff.com/EL3072, 2-port multifunction
//#define LCEC_EL3074_PID 0x0c023052  // https://beckhoff.com/EL3074, 4-port multifunction
//#define LCEC_EL3172_PID 0x0c643052  // https://beckhoff.com/EL3172, 2-port multifunction
//#define LCEC_EL3174_PID 0x0c663052  // https://beckhoff.com/EL3174, 4-port multifunction
//#define LCEC_EP3162_PID 0x0c5a4052  // https://beckhoff.com/EP3162, 2-port multifunction
//#define LCEC_EP3174_PID 0x0c664052  // https://beckhoff.com/EP3174, 4-port multifunction differential
//#define LCEC_EP3182_PID 0x0c6e4052  // https://beckhoff.com/EP3182, 2-port multifunction
//#define LCEC_EP3184_PID 0x0c704052  // https://beckhoff.com/EP3184, 4-port multifunction
//#define LCEC_EPP3174_PID 0x64768c69  // https://beckhoff.com/EP3174, 4-port multifunction differential
//#define LCEC_EPP3184_PID 0x64768d09  // https://beckhoff.com/EP3184, 4-port multifunction

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
