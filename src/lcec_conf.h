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

#ifndef _LCEC_CONF_H_
#define _LCEC_CONF_H_

#include "ecrt.h"
#include "hal.h"

#define LCEC_MODULE_NAME "lcec"

#define LCEC_CONF_SHMEM_KEY   0xACB572C7
#define LCEC_CONF_SHMEM_MAGIC 0x036ED5A3

#define LCEC_CONF_STR_MAXLEN 48

#define LCEC_CONF_SDO_COMPLETE_SUBIDX -1
#define LCEC_CONF_GENERIC_MAX_SUBPINS 32
#define LCEC_CONF_GENERIC_MAX_BITLEN  255

typedef enum {
  lcecConfTypeNone = 0,
  lcecConfTypeMasters,
  lcecConfTypeMaster,
  lcecConfTypeSlave,
  lcecConfTypeDcConf,
  lcecConfTypeWatchdog,
  lcecConfTypeSyncManager,
  lcecConfTypePdo,
  lcecConfTypePdoEntry,
  lcecConfTypeSdoConfig,
  lcecConfTypeSdoDataRaw,
  lcecConfTypeIdnConfig,
  lcecConfTypeIdnDataRaw,
  lcecConfTypeInitCmds,
  lcecConfTypeComplexEntry,
  lcecConfTypeModParam
} LCEC_CONF_TYPE_T;

typedef enum {
  lcecPdoEntTypeSimple,
  lcecPdoEntTypeFloatSigned,
  lcecPdoEntTypeFloatUnsigned,
  lcecPdoEntTypeComplex,
  lcecPdoEntTypeFloatIeee,
  lcecPdoEntTypeFloatDoubleIeee,
} LCEC_PDOENT_TYPE_T;

typedef struct {
  uint32_t magic;
  size_t length;
} LCEC_CONF_HEADER_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  int index;
  uint32_t appTimePeriod;
  int refClockSyncCycles;
  char name[LCEC_CONF_STR_MAXLEN];
} LCEC_CONF_MASTER_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  int index;
  char typename[LCEC_CONF_STR_MAXLEN];
  uint32_t vid;
  uint32_t pid;
  int configPdos;
  unsigned int syncManagerCount;
  unsigned int pdoCount;
  unsigned int pdoEntryCount;
  unsigned int pdoMappingCount;
  size_t sdoConfigLength;
  size_t idnConfigLength;
  unsigned int modParamCount;
  char name[LCEC_CONF_STR_MAXLEN];
} LCEC_CONF_SLAVE_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  uint16_t assignActivate;
  uint32_t sync0Cycle;
  int32_t sync0Shift;
  uint32_t sync1Cycle;
  int32_t sync1Shift;
} LCEC_CONF_DC_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  uint16_t divider;
  uint16_t intervals;
} LCEC_CONF_WATCHDOG_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  uint8_t index;
  ec_direction_t dir;
  unsigned int pdoCount;
} LCEC_CONF_SYNCMANAGER_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  uint16_t index;
  unsigned int pdoEntryCount;
} LCEC_CONF_PDO_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  uint16_t index;
  uint8_t subindex;
  uint8_t bitLength;
  LCEC_PDOENT_TYPE_T subType;
  hal_type_t halType;
  hal_float_t floatScale;
  hal_float_t floatOffset;
  char halPin[LCEC_CONF_STR_MAXLEN];
} LCEC_CONF_PDOENTRY_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  uint8_t bitOffset;
  uint8_t bitLength;
  LCEC_PDOENT_TYPE_T subType;
  hal_type_t halType;
  hal_float_t floatScale;
  hal_float_t floatOffset;
  char halPin[LCEC_CONF_STR_MAXLEN];
} LCEC_CONF_COMPLEXENTRY_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
} LCEC_CONF_NULL_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  uint16_t index;
  int16_t subindex;
  size_t length;
  uint8_t data[];
} LCEC_CONF_SDOCONF_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  uint8_t drive;
  uint16_t idn;
  ec_al_state_t state;
  size_t length;
  uint8_t data[];
} LCEC_CONF_IDNCONF_T;

typedef union {
  hal_bit_t bit;
  hal_s32_t s32;
  hal_u32_t u32;
  hal_float_t flt;
  char str[LCEC_CONF_STR_MAXLEN];
} LCEC_CONF_MODPARAM_VAL_T;

typedef struct {
  LCEC_CONF_TYPE_T confType;
  int id;
  LCEC_CONF_MODPARAM_VAL_T value;
} LCEC_CONF_MODPARAM_T;

#endif
