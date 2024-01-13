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

#include "lcec_el3xxx.h"

#include "../lcec.h"

// TODO(scottlaird): add support for additional EL31xx operating modes, as per
//   https://download.beckhoff.com/download/Document/io/ethercat-terminals/el31xxen.pdf#page=251
//   Specifically, support disabling the filter (0x8000:06) and DC mode.
//
// TODO(scottlaird): using generic support, add EL3255 pot input.
//
// TODO(scottlaird): using generic support, add EM37xx pressure sensor input.
//
// TODO(scottlaird): Figure out what to do with older EL31xx devices
//   without 0x6000:e (the sync error PDO).  It looks like it was
//   added in r18.  Is there a point in keeping the sync error pin at all?

#define FLAG_BITS16 1 << 0       // Device is 16 bits
#define FLAG_BITS12 1 << 1       // Device is 12 bits
#define FLAG_SYNC 1 << 4         // on for SYNC, off for no sync
#define FLAG_TEMPERATURE 1 << 5  // Device is a temperature sensor
#define FLAG_PRESSURE 1 << 6     // Device is a pressure sensor

#define LCEC_EL3XXX_MODPARAM_SENSOR 0
#define LCEC_EL3XXX_MODPARAM_RESOLUTION 8
#define LCEC_EL3XXX_MODPARAM_WIRES 16

static int lcec_el3xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static const lcec_modparam_desc_t modparams_temperature[] = {
    {"ch0Sensor", LCEC_EL3XXX_MODPARAM_SENSOR + 0, MODPARAM_TYPE_STRING},
    {"ch1Sensor", LCEC_EL3XXX_MODPARAM_SENSOR + 1, MODPARAM_TYPE_STRING},
    {"ch2Sensor", LCEC_EL3XXX_MODPARAM_SENSOR + 2, MODPARAM_TYPE_STRING},
    {"ch3Sensor", LCEC_EL3XXX_MODPARAM_SENSOR + 3, MODPARAM_TYPE_STRING},
    {"ch4Sensor", LCEC_EL3XXX_MODPARAM_SENSOR + 4, MODPARAM_TYPE_STRING},
    {"ch5Sensor", LCEC_EL3XXX_MODPARAM_SENSOR + 5, MODPARAM_TYPE_STRING},
    {"ch6Sensor", LCEC_EL3XXX_MODPARAM_SENSOR + 6, MODPARAM_TYPE_STRING},
    {"ch7Sensor", LCEC_EL3XXX_MODPARAM_SENSOR + 7, MODPARAM_TYPE_STRING},
    {"ch0Resolution", LCEC_EL3XXX_MODPARAM_RESOLUTION + 0, MODPARAM_TYPE_STRING},
    {"ch1Resolution", LCEC_EL3XXX_MODPARAM_RESOLUTION + 1, MODPARAM_TYPE_STRING},
    {"ch2Resolution", LCEC_EL3XXX_MODPARAM_RESOLUTION + 2, MODPARAM_TYPE_STRING},
    {"ch3Resolution", LCEC_EL3XXX_MODPARAM_RESOLUTION + 3, MODPARAM_TYPE_STRING},
    {"ch4Resolution", LCEC_EL3XXX_MODPARAM_RESOLUTION + 4, MODPARAM_TYPE_STRING},
    {"ch5Resolution", LCEC_EL3XXX_MODPARAM_RESOLUTION + 5, MODPARAM_TYPE_STRING},
    {"ch6Resolution", LCEC_EL3XXX_MODPARAM_RESOLUTION + 6, MODPARAM_TYPE_STRING},
    {"ch7Resolution", LCEC_EL3XXX_MODPARAM_RESOLUTION + 7, MODPARAM_TYPE_STRING},
    {"ch0Wires", LCEC_EL3XXX_MODPARAM_WIRES + 0, MODPARAM_TYPE_STRING},
    {"ch1Wires", LCEC_EL3XXX_MODPARAM_WIRES + 1, MODPARAM_TYPE_STRING},
    {"ch2Wires", LCEC_EL3XXX_MODPARAM_WIRES + 2, MODPARAM_TYPE_STRING},
    {"ch3Wires", LCEC_EL3XXX_MODPARAM_WIRES + 3, MODPARAM_TYPE_STRING},
    {"ch4Wires", LCEC_EL3XXX_MODPARAM_WIRES + 4, MODPARAM_TYPE_STRING},
    {"ch5Wires", LCEC_EL3XXX_MODPARAM_WIRES + 5, MODPARAM_TYPE_STRING},
    {"ch6Wires", LCEC_EL3XXX_MODPARAM_WIRES + 6, MODPARAM_TYPE_STRING},
    {"ch7Wires", LCEC_EL3XXX_MODPARAM_WIRES + 7, MODPARAM_TYPE_STRING},
    {NULL},
};

typedef struct {
  char *name;       // Sensor type name
  uint16_t value;   // Which value needs to be set in 0x80x0:19 to enable this sensor
  int is_unsigned;  // Ohm measurements are unsigned, and need the extra bit to cover the documented range.
  double scale;     // Default conversion factor for this sensor type
} temp_sensor_t;

// From https://download.beckhoff.com/download/Document/io/ethercat-terminals/el32xxen.pdf#page=223
static const temp_sensor_t temp_sensors[] = {
    {"Pt100", 0, 0, 0.1},          // Pt100 sensor,
    {"Ni100", 1, 0, 0.1},          // Ni100 sensor, -60 to 250C
    {"Pt1000", 2, 0, 0.1},         // Pt1000 sensor, -200 to 850C
    {"Pt500", 3, 0, 0.1},          // Pt500 sensor, -200 to 850C
    {"Pt200", 4, 0, 0.1},          // Pt200 sensor, -200 to 850C
    {"Ni1000", 5, 0, 0.1},         // Ni1000 sensor, -60 to 250C
    {"Ni1000-TK5000", 6, 0, 0.1},  // Ni1000-TK5000, -30 to 160C
    {"Ni120", 7, 0, 0.1},          // Ni120 sensor, -60 to 320C
    {"Ohm/16", 8, 1, 1.0 / 16},    // no sensor, report Ohms directly.  0-4095 Ohms
    {"Ohm/64", 9, 1, 1.0 / 64},    // no sensor, report Ohms directly.  0-1023 Ohms
    {NULL},
};

typedef struct {
  char *name;
  uint16_t value;
  double scale_multiplier;
} temp_resolution_t;

// From https://download.beckhoff.com/download/Document/io/ethercat-terminals/el32xxen.pdf#page=222
static const temp_resolution_t temp_resolutions[] = {
    {"Signed", 0, 1.0},    // 0.1C per bit, default on most devices
    {"Standard", 0, 1.0},  // Same as "signed", but easier to remember WRT "High".
    // { "Absolute", 1, 1.0 }, // ones-compliment presentation, why?
    {"High", 2, 0.1},  // 0.01C per bit, default on "high precision" devices.
    {NULL},
};

typedef struct {
  char *name;
  uint16_t value;
} temp_wires_t;

static const temp_wires_t temp_wires[] = {{"2", 0}, {"3", 1}, {"4", 2}, {NULL}};

static const lcec_typelist_t types[] = {
    // 12-bit devices
    {"EL3001", LCEC_BECKHOFF_VID, 0x0bb93052, LCEC_EL30X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3002", LCEC_BECKHOFF_VID, 0x0bba3052, LCEC_EL30X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3004", LCEC_BECKHOFF_VID, 0x0bbc3052, LCEC_EL30X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3008", LCEC_BECKHOFF_VID, 0x0bc03052, LCEC_EL30X8_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3011", LCEC_BECKHOFF_VID, 0x0bc33052, LCEC_EL30X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3012", LCEC_BECKHOFF_VID, 0x0bc43052, LCEC_EL30X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3014", LCEC_BECKHOFF_VID, 0x0bc63052, LCEC_EL30X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3021", LCEC_BECKHOFF_VID, 0x0bcd3052, LCEC_EL30X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3022", LCEC_BECKHOFF_VID, 0x0bce3052, LCEC_EL30X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3024", LCEC_BECKHOFF_VID, 0x0bd03052, LCEC_EL30X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3041", LCEC_BECKHOFF_VID, 0x0be13052, LCEC_EL30X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3042", LCEC_BECKHOFF_VID, 0x0be23052, LCEC_EL30X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3044", LCEC_BECKHOFF_VID, 0x0be43052, LCEC_EL30X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3048", LCEC_BECKHOFF_VID, 0x0be83052, LCEC_EL30X8_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3051", LCEC_BECKHOFF_VID, 0x0beb3052, LCEC_EL30X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3052", LCEC_BECKHOFF_VID, 0x0bec3052, LCEC_EL30X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3054", LCEC_BECKHOFF_VID, 0x0bee3052, LCEC_EL30X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3058", LCEC_BECKHOFF_VID, 0x0bf23052, LCEC_EL30X8_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3061", LCEC_BECKHOFF_VID, 0x0bf53052, LCEC_EL30X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3062", LCEC_BECKHOFF_VID, 0x0bf63052, LCEC_EL30X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3064", LCEC_BECKHOFF_VID, 0x0bf83052, LCEC_EL30X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EL3068", LCEC_BECKHOFF_VID, 0x0bfc3052, LCEC_EL30X8_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},
    {"EJ3004", LCEC_BECKHOFF_VID, 0x0bbc2852, LCEC_EL30X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS12},

    // 16-bit devices
    {"EL3101", LCEC_BECKHOFF_VID, 0x0c1d3052, LCEC_EL31X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    //  { "EL3102", LCEC_BECKHOFF_VID, 0x0c1e3052, LCEC_EL31X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16|FLAG_SYNC},
    {"EL3104", LCEC_BECKHOFF_VID, 0x0c203052, LCEC_EL31X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    {"EL3111", LCEC_BECKHOFF_VID, 0x0c273052, LCEC_EL31X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    //  { "EL3112", LCEC_BECKHOFF_VID, 0x0c283052, LCEC_EL31X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16|FLAG_SYNC},
    {"EL3114", LCEC_BECKHOFF_VID, 0x0c2a3052, LCEC_EL31X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    {"EL3121", LCEC_BECKHOFF_VID, 0x0c313052, LCEC_EL31X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    //  { "EL3122", LCEC_BECKHOFF_VID, 0x0c323052, LCEC_EL31X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16|FLAG_SYNC},
    //  { "EL3124", LCEC_BECKHOFF_VID, 0x0c343052, LCEC_EL31X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16|FLAG_SYNC},
    {"EL3141", LCEC_BECKHOFF_VID, 0x0c453052, LCEC_EL31X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    //  { "EL3142", LCEC_BECKHOFF_VID, 0x0c463052, LCEC_EL31X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16|FLAG_SYNC},
    {"EL3144", LCEC_BECKHOFF_VID, 0x0c483052, LCEC_EL31X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    {"EL3151", LCEC_BECKHOFF_VID, 0x0c4f3052, LCEC_EL31X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    //  { "EL3152", LCEC_BECKHOFF_VID, 0x0c503052, LCEC_EL31X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16|FLAG_SYNC},
    {"EL3154", LCEC_BECKHOFF_VID, 0x0c523052, LCEC_EL31X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    {"EL3161", LCEC_BECKHOFF_VID, 0x0c593052, LCEC_EL31X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    //  { "EL3162", LCEC_BECKHOFF_VID, 0x0c5a3052, LCEC_EL31X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16|FLAG_SYNC},
    //  { "EL3164", LCEC_BECKHOFF_VID, 0x0c5c3052, LCEC_EL31X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16|FLAG_SYNC},
    {"EL3182", LCEC_BECKHOFF_VID, 0x0c6e3052, LCEC_EL31X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    {"EP3174", LCEC_BECKHOFF_VID, 0x0c664052, LCEC_EL31X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    {"EP3184", LCEC_BECKHOFF_VID, 0x0c704052, LCEC_EL31X4_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},
    {"EPX3158", LCEC_BECKHOFF_VID, 0x9809ab69, LCEC_EL31X8_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_SYNC},

    {"EJ3202", LCEC_BECKHOFF_VID, 0x0c822852, LCEC_EL32X2_PDOS, 0, NULL, lcec_el3xxx_init, modparams_temperature, FLAG_BITS16 | FLAG_TEMPERATURE},
    {"EJ3214", LCEC_BECKHOFF_VID, 0x0c8e2852, LCEC_EL32X4_PDOS, 0, NULL, lcec_el3xxx_init, modparams_temperature, FLAG_BITS16 | FLAG_TEMPERATURE},
    {"EL3201", LCEC_BECKHOFF_VID, 0x0c813052, LCEC_EL32X1_PDOS, 0, NULL, lcec_el3xxx_init, modparams_temperature, FLAG_BITS16 | FLAG_TEMPERATURE},
    {"EL3202", LCEC_BECKHOFF_VID, 0x0c823052, LCEC_EL32X2_PDOS, 0, NULL, lcec_el3xxx_init, modparams_temperature, FLAG_BITS16 | FLAG_TEMPERATURE},
    {"EL3204", LCEC_BECKHOFF_VID, 0x0c843052, LCEC_EL32X4_PDOS, 0, NULL, lcec_el3xxx_init, modparams_temperature, FLAG_BITS16 | FLAG_TEMPERATURE},
    {"EL3208", LCEC_BECKHOFF_VID, 0x0c883052, LCEC_EL32X8_PDOS, 0, NULL, lcec_el3xxx_init, modparams_temperature, FLAG_BITS16 | FLAG_TEMPERATURE},
    {"EL3214", LCEC_BECKHOFF_VID, 0x0c8e3052, LCEC_EL32X4_PDOS, 0, NULL, lcec_el3xxx_init, modparams_temperature, FLAG_BITS16 | FLAG_TEMPERATURE},
    {"EL3218", LCEC_BECKHOFF_VID, 0x0c923052, LCEC_EL32X8_PDOS, 0, NULL, lcec_el3xxx_init, modparams_temperature, FLAG_BITS16 | FLAG_TEMPERATURE},
    {"EP3204", LCEC_BECKHOFF_VID, 0x0c844052, LCEC_EL32X4_PDOS, 0, NULL, lcec_el3xxx_init, modparams_temperature, FLAG_BITS16 | FLAG_TEMPERATURE},

    {"EM3701", LCEC_BECKHOFF_VID, 0x0e753452, LCEC_EM37X1_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_PRESSURE},
    {"EM3702", LCEC_BECKHOFF_VID, 0x0e763452, LCEC_EM37X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_PRESSURE},
    {"EM3712", LCEC_BECKHOFF_VID, 0x0e803452, LCEC_EM37X2_PDOS, 0, NULL, lcec_el3xxx_init, NULL, FLAG_BITS16 | FLAG_PRESSURE},
    {NULL},
};
ADD_TYPES(types);

typedef struct {
  hal_bit_t *overrange;
  hal_bit_t *underrange;
  hal_bit_t *error;
  hal_bit_t *sync_err;
  hal_s32_t *raw_val;
  hal_float_t *scale;
  hal_float_t *bias;
  hal_float_t *val;
  unsigned int ovr_pdo_os;
  unsigned int ovr_pdo_bp;
  unsigned int udr_pdo_os;
  unsigned int udr_pdo_bp;
  unsigned int error_pdo_os;
  unsigned int error_pdo_bp;
  unsigned int sync_err_pdo_os;
  unsigned int sync_err_pdo_bp;
  unsigned int val_pdo_os;
  unsigned int is_unsigned;
} lcec_el3xxx_chan_t;

static const lcec_pindesc_t slave_pins_sync[] = {
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, error), "%s.%s.%s.ain-%d-error"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, sync_err), "%s.%s.%s.ain-%d-sync-err"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, overrange), "%s.%s.%s.ain-%d-overrange"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, underrange), "%s.%s.%s.ain-%d-underrange"},
    {HAL_S32, HAL_OUT, offsetof(lcec_el3xxx_chan_t, raw_val), "%s.%s.%s.ain-%d-raw"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, val), "%s.%s.%s.ain-%d-val"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, scale), "%s.%s.%s.ain-%d-scale"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, bias), "%s.%s.%s.ain-%d-bias"},
    {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL}};

static const lcec_pindesc_t slave_pins_nosync[] = {
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, error), "%s.%s.%s.ain-%d-error"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, overrange), "%s.%s.%s.ain-%d-overrange"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, underrange), "%s.%s.%s.ain-%d-underrange"},
    {HAL_S32, HAL_OUT, offsetof(lcec_el3xxx_chan_t, raw_val), "%s.%s.%s.ain-%d-raw"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, val), "%s.%s.%s.ain-%d-val"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, scale), "%s.%s.%s.ain-%d-scale"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, bias), "%s.%s.%s.ain-%d-bias"},
    {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL}};

static const lcec_pindesc_t slave_pins_temperature[] = {
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, error), "%s.%s.%s.temp-%d-error"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, overrange), "%s.%s.%s.temp-%d-overrange"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, underrange), "%s.%s.%s.temp-%d-underrange"},
    {HAL_S32, HAL_OUT, offsetof(lcec_el3xxx_chan_t, raw_val), "%s.%s.%s.temp-%d-raw"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, val), "%s.%s.%s.temp-%d-temperature"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, scale), "%s.%s.%s.temp-%d-scale"},  // deleteme
    {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL}};

static const lcec_pindesc_t slave_pins_pressure[] = {
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, error), "%s.%s.%s.press-%d-error"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, overrange), "%s.%s.%s.press-%d-overrange"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, underrange), "%s.%s.%s.press-%d-underrange"},
    {HAL_S32, HAL_OUT, offsetof(lcec_el3xxx_chan_t, raw_val), "%s.%s.%s.press-%d-raw"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3xxx_chan_t, val), "%s.%s.%s.press-%d-pressure"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, scale), "%s.%s.%s.press-%d-scale"},
    {HAL_FLOAT, HAL_IO, offsetof(lcec_el3xxx_chan_t, bias), "%s.%s.%s.press-%d-bias"},
    {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL}};

typedef struct {
  uint32_t channels;
  lcec_el3xxx_chan_t chans[LCEC_EL3XXX_MAXCHANS];
} lcec_el3xxx_data_t;

static void lcec_el3xxx_read_temp16(struct lcec_slave *slave, long period);
static void lcec_el3xxx_read_16(struct lcec_slave *slave, long period);
static void lcec_el3xxx_read_12(struct lcec_slave *slave, long period);
static const temp_sensor_t *sensor_type(char *sensortype);
static const temp_resolution_t *sensor_resolution(char *sensorresolution);
static const temp_wires_t *sensor_wires(char *sensorwires);

static int lcec_el3xxx_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el3xxx_data_t *hal_data;
  lcec_el3xxx_chan_t *chan;
  int i;
  int err;
  const lcec_pindesc_t *slave_pins;
  uint64_t flags;
  int pdos_per_channel;

  flags = slave->flags;

  if (flags & FLAG_SYNC) {
    pdos_per_channel = 5;
  } else {
    pdos_per_channel = 4;
  }

  rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "initing device as %s, flags %lx\n", slave->name, flags);
  rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "- slave is %p\n", slave);
  rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "- pdo_entry_regs is %p\n", pdo_entry_regs);

  // initialize settings that vary per bitdepth
  if (flags & FLAG_BITS12) {
    slave->proc_read = lcec_el3xxx_read_12;
  } else if (flags & FLAG_BITS16) {
    if (flags & FLAG_TEMPERATURE) {
      slave->proc_read = lcec_el3xxx_read_temp16;
    } else {
      slave->proc_read = lcec_el3xxx_read_16;
    }
  }

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el3xxx_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el3xxx_data_t));
  slave->hal_data = hal_data;
  hal_data->channels = slave->pdo_entry_count / pdos_per_channel;

  rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "- setting up pins\n");

  // initialize pins
  for (i = 0; i < hal_data->channels; i++) {
    chan = &hal_data->chans[i];
    rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "- setting up channel %d (%p)\n", i, chan);

    // initialize POD entries
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x01, &chan->udr_pdo_os,
                  &chan->udr_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x02, &chan->ovr_pdo_os,
                  &chan->ovr_pdo_bp);
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x07, &chan->error_pdo_os,
                  &chan->error_pdo_bp);
    if (flags & FLAG_SYNC) {
      // Only EL31xx devices have this PDO, if we try to initialize it
      // with 30xxs, then we get a PDO error and fail out.
      LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x0E, &chan->sync_err_pdo_os,
                    &chan->sync_err_pdo_bp);
    }
    LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i << 4), 0x11, &chan->val_pdo_os, NULL);

    // export pins
    if (flags & FLAG_TEMPERATURE) {
      slave_pins = slave_pins_temperature;
    } else if (flags & FLAG_PRESSURE) {
      slave_pins = slave_pins_pressure;
    } else if (flags & FLAG_SYNC)
      slave_pins = slave_pins_sync;
    else
      slave_pins = slave_pins_nosync;

    if ((err = lcec_pin_newf_list(chan, slave_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "lcec_pin_newf_list() for slave %s.%s failed: %d\n", master->name, slave->name,
                      err);
      return err;
    }

    // initialize scale
    if (flags & FLAG_TEMPERATURE) {
      *(chan->scale) = 0.1;
    } else {
      *(chan->scale) = 1.0;
    }

    chan->is_unsigned = 0;

    // Handle modparams
    if (flags & FLAG_TEMPERATURE) {
      LCEC_CONF_MODPARAM_VAL_T *pval;

      // Handle <modParam> entries from the XML.

      // <modParam name="chXSensor" value="???"/>
      rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "  - checking modparam sensor for %s %d\n", slave->name, i);
      pval = lcec_modparam_get(slave, LCEC_EL3XXX_MODPARAM_SENSOR + i);
      if (pval != NULL) {
        rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "    - found sensor param\n");
        temp_sensor_t const *sensor;

        sensor = sensor_type(pval->str);
        if (sensor != NULL) {
          rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "    - setting sensor for %s %d to %d\n", slave->name, i, sensor->value);
          *(chan->scale) = sensor->scale;
          chan->is_unsigned = sensor->is_unsigned;

          if (ecrt_slave_config_sdo16(slave->config, 0x8000 + (i << 4), 0x19, sensor->value) != 0) {
            rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "failed to configure slave %s.%s sdo sensor!\n", master->name,
                            slave->name);
            return -1;
          }
        } else {
          rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "unknown sensor type \"%s\" for slave %s.%s channel %d!\n", pval->str,
                          master->name, slave->name, i);
          return -1;
        }
      }

      // <modParam name="chXResolution" value="???"/>
      rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "  - checking modparam resolution for %s %d\n", slave->name, i);
      pval = lcec_modparam_get(slave, LCEC_EL3XXX_MODPARAM_RESOLUTION + i);
      if (pval != NULL) {
        rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "    - found resolution param\n");
        temp_resolution_t const *resolution;

        resolution = sensor_resolution(pval->str);
        if (resolution != NULL) {
          rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "    - setting resolution for %s %d to %d\n", slave->name, i,
                          resolution->value);
          *(chan->scale) = *(chan->scale) * resolution->scale_multiplier;

          if (ecrt_slave_config_sdo8(slave->config, 0x8000 + (i << 4), 0x2, resolution->value) != 0) {
            rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "failed to configure slave %s.%s sdo resolution!\n", master->name,
                            slave->name);
            return -1;
          }
        } else {
          rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "unknown resolution \"%s\" for slave %s.%s channel %d!\n", pval->str,
                          master->name, slave->name, i);
          return -1;
        }
      }

      // <modParam name="chXWires", value="???"/>
      rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "   - checking modparam wires for %s %d\n", slave->name, i);
      pval = lcec_modparam_get(slave, LCEC_EL3XXX_MODPARAM_WIRES + i);
      if (pval != NULL) {
        rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "     - found wires param\n");
        temp_wires_t const *wires;

        wires = sensor_wires(pval->str);
        if (wires != NULL) {
          rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "      - setting wires for %s %d to %d\n", slave->name, i, wires->value);

          if (ecrt_slave_config_sdo16(slave->config, 0x8000 + (i << 4), 0x1a, wires->value) != 0) {
            rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "failed to configure slave %s.%s sdo wires!\n", master->name,
                            slave->name);
            return -1;
          }
        } else {
          rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "unknown wire setting \"%s\" for slave %s.%s channel %d!\n", pval->str,
                          master->name, slave->name, i);
          return -1;
        }
      }
    }
  }
  rtapi_print_msg(RTAPI_MSG_DBG, LCEC_MSG_PFX "done\n");

  return 0;
}

static void lcec_el3xxx_read(struct lcec_slave *slave, long period, uint32_t mask, int has_sync) {
  lcec_master_t *master = slave->master;
  lcec_el3xxx_data_t *hal_data = (lcec_el3xxx_data_t *)slave->hal_data;
  uint8_t *pd = master->process_data;
  int i;
  lcec_el3xxx_chan_t *chan;
  int16_t value;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  // check inputs
  for (i = 0; i < hal_data->channels; i++) {
    chan = &hal_data->chans[i];

    // update state
    // update state
    *(chan->overrange) = EC_READ_BIT(&pd[chan->ovr_pdo_os], chan->ovr_pdo_bp);
    *(chan->underrange) = EC_READ_BIT(&pd[chan->udr_pdo_os], chan->udr_pdo_bp);
    *(chan->error) = EC_READ_BIT(&pd[chan->error_pdo_os], chan->error_pdo_bp);
    if (has_sync) *(chan->sync_err) = EC_READ_BIT(&pd[chan->sync_err_pdo_os], chan->sync_err_pdo_bp);

    // update value
    value = EC_READ_S16(&pd[chan->val_pdo_os]) & mask;
    *(chan->raw_val) = value;
    *(chan->val) = *(chan->bias) + *(chan->scale) * (double)value * ((double)1 / (double)mask);
  }
}

static void lcec_el3xxx_read_16(struct lcec_slave *slave, long period) { lcec_el3xxx_read(slave, period, 0x7fff, true); }

static void lcec_el3xxx_read_12(struct lcec_slave *slave, long period) {
  // According to
  // https://download.beckhoff.com/download/Document/io/ethercat-terminals/el30xxen.pdf,
  // the EL30xx devices still return results between 0 and 0x7FFF --
  // that is, a 15-bit result, even though it's a "12-bit" device.
  //
  // Note that this makes ..._read_12 identical to ..._read_16.
  lcec_el3xxx_read(slave, period, 0x7fff, false);
}

static void lcec_el3xxx_read_temp16(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el3xxx_data_t *hal_data = (lcec_el3xxx_data_t *)slave->hal_data;
  uint8_t *pd = master->process_data;
  int i;
  lcec_el3xxx_chan_t *chan;
  int32_t value;

  // wait for slave to be operational
  if (!slave->state.operational) {
    return;
  }

  // check inputs
  for (i = 0; i < hal_data->channels; i++) {
    chan = &hal_data->chans[i];

    // update state
    // update state
    *(chan->overrange) = EC_READ_BIT(&pd[chan->ovr_pdo_os], chan->ovr_pdo_bp);
    *(chan->underrange) = EC_READ_BIT(&pd[chan->udr_pdo_os], chan->udr_pdo_bp);
    *(chan->error) = EC_READ_BIT(&pd[chan->error_pdo_os], chan->error_pdo_bp);

    // update value
    if (chan->is_unsigned) {
      value = EC_READ_U16(&pd[chan->val_pdo_os]);
    } else {
      value = EC_READ_S16(&pd[chan->val_pdo_os]);
    }
    *(chan->raw_val) = value;
    *(chan->val) = (double)value * *(chan->scale);
  }
}

// Match the sensor_type in modparams and return the definition
// associated with that sensor, from
// https://download.beckhoff.com/download/Document/io/ethercat-terminals/el32xxen.pdf#page=223
static const temp_sensor_t *sensor_type(char *sensortype) {
  temp_sensor_t const *type;

  for (type = temp_sensors; type != NULL; type++) {
    if (!strcasecmp(sensortype, type->name)) {
      return type;
    }
  }

  return NULL;
}

// Match the sensor resolutiuon in modparams and return the settings for that resolution.
static const temp_resolution_t *sensor_resolution(char *sensorresolution) {
  temp_resolution_t const *res;

  for (res = temp_resolutions; res != NULL; res++) {
    if (!strcasecmp(sensorresolution, res->name)) {
      return res;
    }
  }

  return NULL;
}

// Match the sensor wire configuration in modparams and return the settings for that number of wires.
static const temp_wires_t *sensor_wires(char *sensorwires) {
  temp_wires_t const *wires;

  for (wires = temp_wires; wires != NULL; wires++) {
    if (!strcasecmp(sensorwires, wires->name)) {
      return wires;
    }
  }

  return NULL;
}
