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

#include "lcec_epocat.h"

#include "../lcec.h"

static int lcec_epocat_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[] = {
    {"EpoCAT", LCEC_ABET_VID, LCEC_EPOCAT_PID, LCEC_EPOCAT_PDOS, 0, NULL, lcec_epocat_init},
    {NULL},
};

ADD_TYPES(types);

typedef struct {
  hal_bit_t *DROK_0;
  hal_bit_t *DROK_1;
  hal_bit_t *DROK_2;
  hal_bit_t *DROK_3;
  hal_bit_t *DROK_4;
  hal_bit_t *DROK_INVERTER;
  hal_bit_t *ENABLE_24V_0;
  hal_bit_t *ENABLE_24V_1;
  hal_bit_t *ENABLE_24V_2;
  hal_bit_t *ENABLE_24V_3;
  hal_bit_t *ENABLE_24V_4;
  hal_bit_t *ENABLE_24V_INVERTER;
  hal_bit_t *ENABLE_5V_0;
  hal_bit_t *ENABLE_5V_1;
  hal_bit_t *ENABLE_5V_2;
  hal_bit_t *ENABLE_5V_3;
  hal_bit_t *ENABLE_5V_4;
  hal_bit_t *HOME_0;
  hal_bit_t *HOME_1;
  hal_bit_t *HOME_2;
  hal_bit_t *HOME_3;
  hal_bit_t *HOME_4;
  hal_bit_t *IN_0;
  hal_bit_t *IN_1;
  hal_bit_t *IN_2;
  hal_bit_t *IN_3;
  hal_bit_t *IN_4;
  hal_bit_t *OUT_0;
  hal_bit_t *OUT_1;

  hal_s32_t *HZ_0;
  hal_s32_t *HZ_1;
  hal_s32_t *HZ_2;
  hal_s32_t *HZ_3;
  hal_s32_t *HZ_4;

  hal_float_t *enc_00_position;
  hal_float_t *enc_01_position;
  hal_float_t *enc_02_position;
  hal_float_t *enc_03_position;
  hal_float_t *enc_04_position;

  hal_s32_t *enc_00_count;
  hal_s32_t *enc_01_count;
  hal_s32_t *enc_02_count;
  hal_s32_t *enc_03_count;
  hal_s32_t *enc_04_count;

  hal_float_t enc_00_scale;
  hal_float_t enc_01_scale;
  hal_float_t enc_02_scale;
  hal_float_t enc_03_scale;
  hal_float_t enc_04_scale;

  hal_bit_t *enc_00_index_enable;
  hal_bit_t *enc_01_index_enable;
  hal_bit_t *enc_02_index_enable;
  hal_bit_t *enc_03_index_enable;
  hal_bit_t *enc_04_index_enable;

  hal_float_t *dac_00_value;
  hal_float_t *dac_01_value;
  hal_float_t *dac_02_value;
  hal_float_t *dac_03_value;
  hal_float_t *dac_04_value;

  hal_float_t *dac_inverter_value;
  hal_bit_t *inverter_forward;
  hal_bit_t *inverter_reverse;

  hal_float_t dac_00_scale;
  hal_float_t dac_01_scale;
  hal_float_t dac_02_scale;
  hal_float_t dac_03_scale;
  hal_float_t dac_04_scale;

  hal_float_t *adc1;
  hal_float_t *adc2;

  unsigned int off_dig_out;
  unsigned int off_dig_inp;
  unsigned int off_PWM;
  unsigned int off_DIR;
  unsigned int off_ENC;
  unsigned int off_ENABLE_24V;
  unsigned int off_ENABLE_5V;
  unsigned int off_DRVOK;
  unsigned int off_HOME;
  unsigned int off_ADC1;
  unsigned int off_ADC2;
  unsigned int off_INVERTER;
  unsigned int off_LATCH;
  unsigned int off_FLAG;
  unsigned int off_RESET_FLAG;
  unsigned int off_POLARITY;
} lcec_epocat_data_t;

static const lcec_pindesc_t slave_pins[] = {
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, DROK_0), "%s.%s.%s.axis0-drvok"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, DROK_1), "%s.%s.%s.axis1-drvok"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, DROK_2), "%s.%s.%s.axis2-drvok"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, DROK_3), "%s.%s.%s.axis3-drvok"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, DROK_4), "%s.%s.%s.axis4-drvok"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, DROK_INVERTER), "%s.%s.%s.din-inverter-drvok"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_24V_0), "%s.%s.%s.axis0-enable24V"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_24V_1), "%s.%s.%s.axis1-enable24V"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_24V_2), "%s.%s.%s.axis2-enable24V"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_24V_3), "%s.%s.%s.axis3-enable24V"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_24V_4), "%s.%s.%s.axis4-enable24V"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_24V_INVERTER), "%s.%s.%s.dout-inverter-enable"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_5V_0), "%s.%s.%s.axis0-enable5V"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_5V_1), "%s.%s.%s.axis1-enable5V"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_5V_2), "%s.%s.%s.axis2-enable5V"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_5V_3), "%s.%s.%s.axis3-enable5V"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, ENABLE_5V_4), "%s.%s.%s.axis4-enable5V"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, HOME_0), "%s.%s.%s.axis0-home"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, HOME_1), "%s.%s.%s.axis1-home"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, HOME_2), "%s.%s.%s.axis2-home"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, HOME_3), "%s.%s.%s.axis3-home"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, HOME_4), "%s.%s.%s.axis4-home"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, IN_0), "%s.%s.%s.din-0"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, IN_1), "%s.%s.%s.din-1"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, IN_2), "%s.%s.%s.din-2"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, IN_3), "%s.%s.%s.din-3"},
    {HAL_BIT, HAL_OUT, offsetof(lcec_epocat_data_t, IN_4), "%s.%s.%s.probe"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, OUT_0), "%s.%s.%s.dout-0"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, OUT_1), "%s.%s.%s.dout-1"},

    {HAL_S32, HAL_OUT, offsetof(lcec_epocat_data_t, HZ_0), "%s.%s.%s.axis0-hz"},
    {HAL_S32, HAL_OUT, offsetof(lcec_epocat_data_t, HZ_1), "%s.%s.%s.axis1-hz"},
    {HAL_S32, HAL_OUT, offsetof(lcec_epocat_data_t, HZ_2), "%s.%s.%s.axis2-hz"},
    {HAL_S32, HAL_OUT, offsetof(lcec_epocat_data_t, HZ_3), "%s.%s.%s.axis3-hz"},
    {HAL_S32, HAL_OUT, offsetof(lcec_epocat_data_t, HZ_4), "%s.%s.%s.axis4-hz"},

    {HAL_FLOAT, HAL_OUT, offsetof(lcec_epocat_data_t, enc_00_position), "%s.%s.%s.axis0-enc-pos"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_epocat_data_t, enc_01_position), "%s.%s.%s.axis1-enc-pos"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_epocat_data_t, enc_02_position), "%s.%s.%s.axis2-enc-pos"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_epocat_data_t, enc_03_position), "%s.%s.%s.axis3-enc-pos"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_epocat_data_t, enc_04_position), "%s.%s.%s.axis4-enc-pos"},

    {HAL_S32, HAL_OUT, offsetof(lcec_epocat_data_t, enc_00_count), "%s.%s.%s.axis0-raw-count"},
    {HAL_S32, HAL_OUT, offsetof(lcec_epocat_data_t, enc_01_count), "%s.%s.%s.axis1-raw-count"},
    {HAL_S32, HAL_OUT, offsetof(lcec_epocat_data_t, enc_02_count), "%s.%s.%s.axis2-raw-count"},
    {HAL_S32, HAL_OUT, offsetof(lcec_epocat_data_t, enc_03_count), "%s.%s.%s.axis3-raw-count"},
    {HAL_S32, HAL_OUT, offsetof(lcec_epocat_data_t, enc_04_count), "%s.%s.%s.axis4-raw-count"},

    {HAL_BIT, HAL_IO, offsetof(lcec_epocat_data_t, enc_00_index_enable), "%s.%s.%s.axis0-index-enable"},
    {HAL_BIT, HAL_IO, offsetof(lcec_epocat_data_t, enc_01_index_enable), "%s.%s.%s.axis1-index-enable"},
    {HAL_BIT, HAL_IO, offsetof(lcec_epocat_data_t, enc_02_index_enable), "%s.%s.%s.axis2-index-enable"},
    {HAL_BIT, HAL_IO, offsetof(lcec_epocat_data_t, enc_03_index_enable), "%s.%s.%s.axis3-index-enable"},
    {HAL_BIT, HAL_IO, offsetof(lcec_epocat_data_t, enc_04_index_enable), "%s.%s.%s.axis4-index-enable"},

    {HAL_FLOAT, HAL_IN, offsetof(lcec_epocat_data_t, dac_00_value), "%s.%s.%s.axis0-dac-value"},
    {HAL_FLOAT, HAL_IN, offsetof(lcec_epocat_data_t, dac_01_value), "%s.%s.%s.axis1-dac-value"},
    {HAL_FLOAT, HAL_IN, offsetof(lcec_epocat_data_t, dac_02_value), "%s.%s.%s.axis2-dac-value"},
    {HAL_FLOAT, HAL_IN, offsetof(lcec_epocat_data_t, dac_03_value), "%s.%s.%s.axis3-dac-value"},
    {HAL_FLOAT, HAL_IN, offsetof(lcec_epocat_data_t, dac_04_value), "%s.%s.%s.axis4-dac-value"},

    {HAL_FLOAT, HAL_IN, offsetof(lcec_epocat_data_t, dac_inverter_value), "%s.%s.%s.inverter-dac-value"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, inverter_forward), "%s.%s.%s.inverter-forward"},
    {HAL_BIT, HAL_IN, offsetof(lcec_epocat_data_t, inverter_reverse), "%s.%s.%s.inverter-reverse"},

    {HAL_FLOAT, HAL_OUT, offsetof(lcec_epocat_data_t, adc1), "%s.%s.%s.adc0-value"},
    {HAL_FLOAT, HAL_OUT, offsetof(lcec_epocat_data_t, adc2), "%s.%s.%s.adc1-value"},

    {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL}};

static const lcec_pindesc_t slave_params[] = {
    {HAL_FLOAT, HAL_RW, offsetof(lcec_epocat_data_t, enc_00_scale), "%s.%s.%s.axis0-enc-scale"},
    {HAL_FLOAT, HAL_RW, offsetof(lcec_epocat_data_t, enc_01_scale), "%s.%s.%s.axis1-enc-scale"},
    {HAL_FLOAT, HAL_RW, offsetof(lcec_epocat_data_t, enc_02_scale), "%s.%s.%s.axis2-enc-scale"},
    {HAL_FLOAT, HAL_RW, offsetof(lcec_epocat_data_t, enc_03_scale), "%s.%s.%s.axis3-enc-scale"},
    {HAL_FLOAT, HAL_RW, offsetof(lcec_epocat_data_t, enc_04_scale), "%s.%s.%s.axis4-enc-scale"},

    {HAL_FLOAT, HAL_RW, offsetof(lcec_epocat_data_t, dac_00_scale), "%s.%s.%s.axis0-dac-scale"},
    {HAL_FLOAT, HAL_RW, offsetof(lcec_epocat_data_t, dac_01_scale), "%s.%s.%s.axis1-dac-scale"},
    {HAL_FLOAT, HAL_RW, offsetof(lcec_epocat_data_t, dac_02_scale), "%s.%s.%s.axis2-dac-scale"},
    {HAL_FLOAT, HAL_RW, offsetof(lcec_epocat_data_t, dac_03_scale), "%s.%s.%s.axis3-dac-scale"},
    {HAL_FLOAT, HAL_RW, offsetof(lcec_epocat_data_t, dac_04_scale), "%s.%s.%s.axis4-dac-scale"},

    {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL}};

static ec_pdo_entry_info_t lcec_epocat_pdo_entries[] = {
    {0x7000, 0x01, 16}, /* Spindle vel */
    {0x7001, 0x01, 16}, /* Drive 1 freq */
    {0x7001, 0x02, 16}, /* Drive 2 freq */
    {0x7001, 0x03, 16}, /* Drive 3 freq */
    {0x7001, 0x04, 16}, /* Drive 4 freq */
    {0x7001, 0x05, 16}, /* Drive 5 freq */
    {0x7002, 0x01, 1},  /* Drive 1 en */
    {0x7002, 0x02, 1},  /* Drive 2 en */
    {0x7002, 0x03, 1},  /* Drive 3 en */
    {0x7002, 0x04, 1},  /* Drive 4 en */
    {0x7002, 0x05, 1},  /* Drive 5 en */
    {0x0000, 0x06, 1},  /* Pad */
    {0x0000, 0x07, 1},  /* Pad */
    {0x0000, 0x08, 1},  /* Pad */
    {0x7003, 0x01, 1},  /* Drive 1 dir */
    {0x7003, 0x02, 1},  /* Drive 2 dir */
    {0x7003, 0x03, 1},  /* Drive 3 dir */
    {0x7003, 0x04, 1},  /* Drive 4 dir */
    {0x7003, 0x05, 1},  /* Drive 5 dir */
    {0x0000, 0x06, 1},  /* Pad */
    {0x0000, 0x07, 1},  /* Pad */
    {0x0000, 0x07, 1},  /* Pad */
    {0x7004, 0x01, 1},  /* Drive 1 res flag */
    {0x7004, 0x02, 1},  /* Drive 2 res flag */
    {0x7004, 0x03, 1},  /* Drive 3 res flag */
    {0x7004, 0x04, 1},  /* Drive 4 res flag */
    {0x7004, 0x05, 1},  /* Drive 5 res flag */
    {0x7004, 0x06, 1},  /* Selezione idx/probe */
    {0x7004, 0x07, 1},  /* Tristate single ended */
    {0x0000, 0x08, 1},  /* Pad */
    {0x7005, 0x01, 1},  /* Drive 1 polarity */
    {0x7005, 0x02, 1},  /* Drive 2 polarity */
    {0x7005, 0x03, 1},  /* Drive 3 polarity */
    {0x7005, 0x04, 1},  /* Drive 4 polarity */
    {0x7005, 0x05, 1},  /* Drive 5 polarity */
    {0x0000, 0x06, 1},  /* Pad */
    {0x0000, 0x07, 1},  /* Pad */
    {0x0000, 0x08, 1},  /* Pad */
    {0x7006, 0x01, 1},  /* Drive 1 en 24V */
    {0x7006, 0x02, 1},  /* Drive 2 en 24V */
    {0x7006, 0x03, 1},  /* Drive 3 en 24V */
    {0x7006, 0x04, 1},  /* Drive 4 en 24V */
    {0x7006, 0x05, 1},  /* Drive 5 en 24V */
    {0x7006, 0x06, 1},  /* Spindle en */
    {0x7006, 0x01, 1},  /* Out 1 */
    {0x7006, 0x01, 1},  /* Out 2 */
    {0x6000, 0x01, 16}, /* Ingresso 1 */
    {0x6000, 0x02, 16}, /* Ingresso 2 */
    {0x6001, 0x01, 16}, /* Drive 1 count */
    {0x6001, 0x02, 16}, /* Drive 2 count */
    {0x6001, 0x03, 16}, /* Drive 3 count */
    {0x6001, 0x04, 16}, /* Drive 4 count */
    {0x6001, 0x05, 16}, /* Drive 5 count */
    {0x6002, 0x01, 16}, /* Drive 1 latch */
    {0x6002, 0x02, 16}, /* Drive 2 latch */
    {0x6002, 0x03, 16}, /* Drive 3 latch */
    {0x6002, 0x04, 16}, /* Drive 4 latch */
    {0x6002, 0x05, 16}, /* Drive 5 latch */
    {0x6006, 0x01, 1},  /* Drive 1 flag */
    {0x6006, 0x02, 1},  /* Drive 2 flag */
    {0x6006, 0x03, 1},  /* Drive 3 flag */
    {0x6006, 0x04, 1},  /* Drive 4 flag */
    {0x6006, 0x05, 1},  /* Drive 5 flag */
    {0x0000, 0x06, 1},  /* Pad */
    {0x0000, 0x07, 1},  /* Pad */
    {0x0000, 0x08, 1},  /* Pad */
    {0x6004, 0x01, 1},  /* Drive 1 home */
    {0x6004, 0x02, 1},  /* Drive 2 home */
    {0x6004, 0x03, 1},  /* Drive 3 home */
    {0x6004, 0x04, 1},  /* Drive 4 home */
    {0x6004, 0x05, 1},  /* Drive 5 home */
    {0x0000, 0x06, 1},  /* Pad */
    {0x0000, 0x07, 1},  /* Pad */
    {0x0000, 0x08, 1},  /* Pad */
    {0x6003, 0x01, 1},  /* Drive 1 ok */
    {0x6003, 0x02, 1},  /* Drive 2 ok */
    {0x6003, 0x03, 1},  /* Drive 3 ok */
    {0x6003, 0x04, 1},  /* Drive 4 ok */
    {0x6003, 0x05, 1},  /* Drive 5 ok */
    {0x6003, 0x06, 1},  /* Spindle ok */
    {0x0000, 0x07, 1},  /* Pad */
    {0x0000, 0x08, 1},  /* Pad */
    {0x6005, 0x01, 1},  /* Drive 1 idx */
    {0x6005, 0x02, 1},  /* Drive 2 idx */
    {0x6005, 0x03, 1},  /* Drive 3 idx */
    {0x6005, 0x04, 1},  /* Drive 4 idx */
    {0x6005, 0x05, 1},  /* Drive 5 idx */
    {0x0000, 0x06, 1},  /* Pad */
    {0x0000, 0x07, 1},  /* Pad */
    {0x0000, 0x08, 1},  /* Pad */
    {0x6007, 0x01, 1},  /* In 1 */
    {0x6007, 0x02, 1},  /* In 2 */
    {0x6007, 0x03, 1},  /* In 3 */
    {0x6007, 0x04, 1},  /* In 4 */
    {0x6007, 0x05, 1},  /* Probe */
    {0x0000, 0x06, 1},  /* Pad */
    {0x0000, 0x07, 1},  /* Pad */
    {0x0000, 0x08, 1},  /* Pad */
};

static ec_pdo_info_t lcec_epocat_pdos[] = {
    {0x1600, 1, lcec_epocat_pdo_entries + 0},  /* Velocita' mandrino */
    {0x1601, 5, lcec_epocat_pdo_entries + 1},  /* Frequenza drive */
    {0x1602, 8, lcec_epocat_pdo_entries + 6},  /* Abilitazione drive */
    {0x1603, 8, lcec_epocat_pdo_entries + 14}, /* Direzione drive */
    {0x1604, 8, lcec_epocat_pdo_entries + 22}, /* Comandi drive */
    {0x1605, 8, lcec_epocat_pdo_entries + 30}, /* Impulso singolo */
    {0x1606, 8, lcec_epocat_pdo_entries + 38}, /* Uscite digitali */
    {0x1a00, 2, lcec_epocat_pdo_entries + 46}, /* Ingressi analogici */
    {0x1a01, 5, lcec_epocat_pdo_entries + 48}, /* Contatore impulsi */
    {0x1a02, 5, lcec_epocat_pdo_entries + 53}, /* Latch impulsi */
    {0x1a06, 8, lcec_epocat_pdo_entries + 58}, /* Flag latch idx/probe */
    {0x1a04, 8, lcec_epocat_pdo_entries + 66}, /* Home */
    {0x1a03, 8, lcec_epocat_pdo_entries + 74}, /* Stato drive */
    {0x1a05, 8, lcec_epocat_pdo_entries + 82}, /* Index */
    {0x1a07, 8, lcec_epocat_pdo_entries + 90}, /* Ingressi digitali */
};

static ec_sync_info_t lcec_epocat_syncs[] = {{0, EC_DIR_OUTPUT, 7, lcec_epocat_pdos + 0, EC_WD_DISABLE},
                                             {1, EC_DIR_INPUT, 8, lcec_epocat_pdos + 7, EC_WD_DISABLE},
                                             {0xff}};

static uint16_t raw_counts_old[5];
static int32_t counts[5];
static int32_t counts_latch[5];

static void lcec_epocat_read(struct lcec_slave *slave, long period);
static void lcec_epocat_write(struct lcec_slave *slave, long period);

static int lcec_epocat_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_epocat_data_t *hal_data;
  int err;

  // initialize callbacks
  slave->proc_read = lcec_epocat_read;
  slave->proc_write = lcec_epocat_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_epocat_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_epocat_data_t));
  slave->hal_data = hal_data;

  // initializer sync info
  slave->sync_info = lcec_epocat_syncs;

  // initialize POD entries
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7006, 1, &hal_data->off_dig_out, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6007, 1, &hal_data->off_dig_inp, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7001, 1, &hal_data->off_PWM, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7003, 1, &hal_data->off_DIR, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6001, 1, &hal_data->off_ENC, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7006, 1, &hal_data->off_ENABLE_24V, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7002, 1, &hal_data->off_ENABLE_5V, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6003, 1, &hal_data->off_DRVOK, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6004, 1, &hal_data->off_HOME, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000, 1, &hal_data->off_INVERTER, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 1, &hal_data->off_ADC1, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 2, &hal_data->off_ADC2, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6002, 1, &hal_data->off_LATCH, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6006, 1, &hal_data->off_FLAG, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7004, 1, &hal_data->off_RESET_FLAG, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7005, 1, &hal_data->off_POLARITY, NULL);

  // export pins
  if ((err = lcec_pin_newf_list(hal_data, slave_pins, LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    return err;
  }

  // export parameters
  if ((err = lcec_param_newf_list(hal_data, slave_params, LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    return err;
  }

  // initialize variables
  raw_counts_old[0] = 0;
  raw_counts_old[1] = 0;
  raw_counts_old[2] = 0;
  raw_counts_old[3] = 0;
  raw_counts_old[4] = 0;

  counts[0] = 0;
  counts[1] = 0;
  counts[2] = 0;
  counts[3] = 0;
  counts[4] = 0;

  counts_latch[0] = 0;
  counts_latch[1] = 0;
  counts_latch[2] = 0;
  counts_latch[3] = 0;
  counts_latch[4] = 0;

  hal_data->enc_00_scale = 1;
  hal_data->enc_01_scale = 1;
  hal_data->enc_02_scale = 1;
  hal_data->enc_03_scale = 1;
  hal_data->enc_04_scale = 1;

  hal_data->dac_00_scale = 1;
  hal_data->dac_01_scale = 1;
  hal_data->dac_02_scale = 1;
  hal_data->dac_03_scale = 1;
  hal_data->dac_04_scale = 1;

  return 0;
}

static void lcec_epocat_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_epocat_data_t *hal_data = (lcec_epocat_data_t *)slave->hal_data;
  uint8_t *pd = master->process_data;

  uint16_t raw_counts[5];
  uint16_t raw_latch_counts[5];

  *(hal_data->IN_0) = EC_READ_BIT(&pd[hal_data->off_dig_inp], 0);
  *(hal_data->IN_1) = EC_READ_BIT(&pd[hal_data->off_dig_inp], 1);
  *(hal_data->IN_2) = EC_READ_BIT(&pd[hal_data->off_dig_inp], 2);
  *(hal_data->IN_3) = EC_READ_BIT(&pd[hal_data->off_dig_inp], 3);
  *(hal_data->IN_4) = EC_READ_BIT(&pd[hal_data->off_dig_inp], 4);

  *(hal_data->DROK_0) = EC_READ_BIT(&pd[hal_data->off_DRVOK], 0);
  *(hal_data->DROK_1) = EC_READ_BIT(&pd[hal_data->off_DRVOK], 1);
  *(hal_data->DROK_2) = EC_READ_BIT(&pd[hal_data->off_DRVOK], 2);
  *(hal_data->DROK_3) = EC_READ_BIT(&pd[hal_data->off_DRVOK], 3);
  *(hal_data->DROK_4) = EC_READ_BIT(&pd[hal_data->off_DRVOK], 4);
  *(hal_data->DROK_INVERTER) = EC_READ_BIT(&pd[hal_data->off_DRVOK], 5);

  *(hal_data->HOME_0) = EC_READ_BIT(&pd[hal_data->off_HOME], 0);
  *(hal_data->HOME_1) = EC_READ_BIT(&pd[hal_data->off_HOME], 1);
  *(hal_data->HOME_2) = EC_READ_BIT(&pd[hal_data->off_HOME], 2);
  *(hal_data->HOME_3) = EC_READ_BIT(&pd[hal_data->off_HOME], 3);
  *(hal_data->HOME_4) = EC_READ_BIT(&pd[hal_data->off_HOME], 4);

  *(hal_data->adc1) = EC_READ_U16(&pd[hal_data->off_ADC1]);
  *(hal_data->adc2) = EC_READ_U16(&pd[hal_data->off_ADC2]);

  raw_counts[0] = EC_READ_U16(&pd[hal_data->off_ENC]);
  raw_counts[1] = EC_READ_U16(&pd[hal_data->off_ENC + 2]);
  raw_counts[2] = EC_READ_U16(&pd[hal_data->off_ENC + 4]);
  raw_counts[3] = EC_READ_U16(&pd[hal_data->off_ENC + 6]);
  raw_counts[4] = EC_READ_U16(&pd[hal_data->off_ENC + 8]);

  // check for index pulse detected
  if (EC_READ_BIT(&pd[hal_data->off_FLAG], 0) == 1) {
    // cancel index-enable
    *(hal_data->enc_00_index_enable) = 0;

    raw_latch_counts[0] = EC_READ_U16(&pd[hal_data->off_LATCH]);
    counts_latch[0] = counts[0];
    counts_latch[0] += (int16_t)(raw_latch_counts[0] - raw_counts_old[0]);
  }
  if (EC_READ_BIT(&pd[hal_data->off_FLAG], 1) == 1) {
    // cancel index-enable
    *(hal_data->enc_01_index_enable) = 0;

    raw_latch_counts[1] = EC_READ_U16(&pd[hal_data->off_LATCH + 2]);
    counts_latch[1] = counts[1];
    counts_latch[1] += (int16_t)(raw_latch_counts[1] - raw_counts_old[1]);
  }
  if (EC_READ_BIT(&pd[hal_data->off_FLAG], 2) == 1) {
    // cancel index-enable
    *(hal_data->enc_02_index_enable) = 0;

    raw_latch_counts[2] = EC_READ_U16(&pd[hal_data->off_LATCH + 4]);
    counts_latch[2] = counts[2];
    counts_latch[2] += (int16_t)(raw_latch_counts[2] - raw_counts_old[2]);
  }
  if (EC_READ_BIT(&pd[hal_data->off_FLAG], 3) == 1) {
    // cancel index-enable
    *(hal_data->enc_03_index_enable) = 0;

    raw_latch_counts[3] = EC_READ_U16(&pd[hal_data->off_LATCH + 6]);
    counts_latch[3] = counts[3];
    counts_latch[3] += (int16_t)(raw_latch_counts[3] - raw_counts_old[3]);
  }
  if (EC_READ_BIT(&pd[hal_data->off_FLAG], 4) == 1) {
    // cancel index-enable
    *(hal_data->enc_04_index_enable) = 0;

    raw_latch_counts[4] = EC_READ_U16(&pd[hal_data->off_LATCH + 8]);
    counts_latch[4] = counts[4];
    counts_latch[4] += (int16_t)(raw_latch_counts[4] - raw_counts_old[4]);
  }

  counts[0] += (int16_t)(raw_counts[0] - raw_counts_old[0]);
  raw_counts_old[0] = raw_counts[0];
  counts[0] -= counts_latch[0];

  counts[1] += (int16_t)(raw_counts[1] - raw_counts_old[1]);
  raw_counts_old[1] = raw_counts[1];
  counts[1] -= counts_latch[1];

  counts[2] += (int16_t)(raw_counts[2] - raw_counts_old[2]);
  raw_counts_old[2] = raw_counts[2];
  counts[2] -= counts_latch[2];

  counts[3] += (int16_t)(raw_counts[3] - raw_counts_old[3]);
  raw_counts_old[3] = raw_counts[3];
  counts[3] -= counts_latch[3];

  counts[4] += (int16_t)(raw_counts[4] - raw_counts_old[4]);
  raw_counts_old[4] = raw_counts[4];
  counts[4] -= counts_latch[4];

  *(hal_data->enc_00_position) = counts[0] * hal_data->enc_00_scale;
  *(hal_data->enc_01_position) = counts[1] * hal_data->enc_01_scale;
  *(hal_data->enc_02_position) = counts[2] * hal_data->enc_02_scale;
  *(hal_data->enc_03_position) = counts[3] * hal_data->enc_03_scale;
  *(hal_data->enc_04_position) = counts[4] * hal_data->enc_04_scale;

  *(hal_data->enc_00_count) = counts[0];
  *(hal_data->enc_01_count) = counts[1];
  *(hal_data->enc_02_count) = counts[2];
  *(hal_data->enc_03_count) = counts[3];
  *(hal_data->enc_04_count) = counts[4];
}

static void lcec_epocat_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_epocat_data_t *hal_data = (lcec_epocat_data_t *)slave->hal_data;
  uint8_t *pd = master->process_data;

  float fValue;

  float fTmp;
  int iDir1;
  int iDir2;
  int iDir3;
  int iDir4;
  int iDir5;

  // Selezione idx/probe (0=Probe, 1 = index)
  EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 5, 1);

  // Tristate single ended
  EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 6, 0);

  // Polarity
  EC_WRITE_BIT(&pd[hal_data->off_POLARITY], 0, 0);
  EC_WRITE_BIT(&pd[hal_data->off_POLARITY], 1, 0);
  EC_WRITE_BIT(&pd[hal_data->off_POLARITY], 2, 0);
  EC_WRITE_BIT(&pd[hal_data->off_POLARITY], 3, 0);
  EC_WRITE_BIT(&pd[hal_data->off_POLARITY], 4, 0);

  // Check for index enable request from HAL
  if (*(hal_data->enc_00_index_enable)) {
    // tell hardware to latch on index
    EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 0, 0);
  } else {
    // cancel hardware latch on index
    EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 0, 1);
  }
  if (*(hal_data->enc_01_index_enable)) {
    // tell hardware to latch on index
    EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 1, 0);
  } else {
    // cancel hardware latch on index
    EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 1, 1);
  }
  if (*(hal_data->enc_02_index_enable)) {
    // tell hardware to latch on index
    EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 2, 0);
  } else {
    // cancel hardware latch on index
    EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 2, 1);
  }
  if (*(hal_data->enc_03_index_enable)) {
    // tell hardware to latch on index
    EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 3, 0);
  } else {
    // cancel hardware latch on index
    EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 3, 1);
  }
  if (*(hal_data->enc_04_index_enable)) {
    // tell hardware to latch on index
    EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 4, 0);
  } else {
    // cancel hardware latch on index
    EC_WRITE_BIT(&pd[hal_data->off_RESET_FLAG], 4, 1);
  }

  EC_WRITE_BIT(&pd[hal_data->off_dig_out], 6, *(hal_data->OUT_0));
  EC_WRITE_BIT(&pd[hal_data->off_dig_out], 7, *(hal_data->OUT_1));

  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_24V], 0, *(hal_data->ENABLE_24V_0));
  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_24V], 1, *(hal_data->ENABLE_24V_1));
  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_24V], 2, *(hal_data->ENABLE_24V_2));
  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_24V], 3, *(hal_data->ENABLE_24V_3));
  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_24V], 4, *(hal_data->ENABLE_24V_4));
  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_24V], 5, *(hal_data->ENABLE_24V_INVERTER));

  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_5V], 0, *(hal_data->ENABLE_5V_0));
  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_5V], 1, *(hal_data->ENABLE_5V_1));
  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_5V], 2, *(hal_data->ENABLE_5V_2));
  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_5V], 3, *(hal_data->ENABLE_5V_3));
  EC_WRITE_BIT(&pd[hal_data->off_ENABLE_5V], 4, *(hal_data->ENABLE_5V_4));

  fValue = (((*(hal_data->dac_00_value) * 1000) / hal_data->enc_00_scale) / 16) * hal_data->dac_00_scale;
  if (fValue > 65534) {
    fValue = 65534;
  }
  if (fValue < -65534) {
    fValue = -65534;
  }

  if (fValue >= 0) {
    iDir1 = 0;
    fTmp = fValue;
  } else {
    iDir1 = 1;
    fTmp = -fValue;
  }

  *(hal_data->HZ_0) = fTmp;

  fValue = (((*(hal_data->dac_01_value) * 1000) / hal_data->enc_01_scale) / 16) * hal_data->dac_01_scale;
  if (fValue > 65534) {
    fValue = 65534;
  }
  if (fValue < -65534) {
    fValue = -65534;
  }

  if (fValue >= 0) {
    iDir2 = 0;
    fTmp = fValue;
  } else {
    iDir2 = 1;
    fTmp = -fValue;
  }

  *(hal_data->HZ_1) = fTmp;

  fValue = (((*(hal_data->dac_02_value) * 1000) / hal_data->enc_02_scale) / 16) * hal_data->dac_02_scale;
  if (fValue > 65534) {
    fValue = 65534;
  }
  if (fValue < -65534) {
    fValue = -65534;
  }

  if (fValue >= 0) {
    iDir3 = 0;
    fTmp = fValue;
  } else {
    iDir3 = 1;
    fTmp = -fValue;
  }

  *(hal_data->HZ_2) = fTmp;

  fValue = (((*(hal_data->dac_03_value) * 1000) / hal_data->enc_03_scale) / 16) * hal_data->dac_03_scale;
  if (fValue > 65534) {
    fValue = 65534;
  }
  if (fValue < -65534) {
    fValue = -65534;
  }

  if (fValue >= 0) {
    iDir4 = 0;
    fTmp = fValue;
  } else {
    iDir4 = 1;
    fTmp = -fValue;
  }

  *(hal_data->HZ_3) = fTmp;

  fValue = (((*(hal_data->dac_04_value) * 1000) / hal_data->enc_04_scale) / 16) * hal_data->dac_04_scale;
  if (fValue > 65534) {
    fValue = 65534;
  }
  if (fValue < -65534) {
    fValue = -65534;
  }

  if (fValue >= 0) {
    iDir5 = 0;
    fTmp = fValue;
  } else {
    iDir5 = 1;
    fTmp = -fValue;
  }

  *(hal_data->HZ_4) = fTmp;

  EC_WRITE_BIT(&pd[hal_data->off_DIR], 0, iDir1);
  EC_WRITE_U16(&pd[hal_data->off_PWM], *(hal_data->HZ_0));

  EC_WRITE_BIT(&pd[hal_data->off_DIR], 1, iDir2);
  EC_WRITE_U16(&pd[hal_data->off_PWM + 2], *(hal_data->HZ_1));

  EC_WRITE_BIT(&pd[hal_data->off_DIR], 2, iDir3);
  EC_WRITE_U16(&pd[hal_data->off_PWM + 4], *(hal_data->HZ_2));

  EC_WRITE_BIT(&pd[hal_data->off_DIR], 3, iDir4);
  EC_WRITE_U16(&pd[hal_data->off_PWM + 6], *(hal_data->HZ_3));

  EC_WRITE_BIT(&pd[hal_data->off_DIR], 4, iDir5);
  EC_WRITE_U16(&pd[hal_data->off_PWM + 8], *(hal_data->HZ_4));

  fValue = *(hal_data->dac_inverter_value) / 10 * 0x7fff;

  EC_WRITE_U16(&pd[hal_data->off_INVERTER], (uint16_t)fValue);
}
