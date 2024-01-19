//
//    Copyright (C) 2018 Sascha Ittner <sascha.ittner@modusoft.de>
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
//    Edit A.Chimeno 12/3/19 Version 1.0

/// @file
/// @brief Driver for Beckhoff EL6090 Display terminals

#include "../lcec.h"
#include "lcec_el6090.h"

static void lcec_el6090_read(struct lcec_slave *slave, long period);
static void lcec_el6090_write(struct lcec_slave *slave, long period);
static int lcec_el6090_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[]={
  { "EL6090", LCEC_BECKHOFF_VID, 0x17ca3052, LCEC_EL6090_PDOS, 0, NULL, lcec_el6090_init},
  { NULL },
};
ADD_TYPES(types);

#define EL6090_HOUR_SCALE (3600)


typedef struct {

  hal_u32_t *timer;
  hal_u32_t *counter;

  hal_bit_t *timer_start;
  hal_bit_t *timer_reset;
  hal_bit_t *counter_clock;
  hal_bit_t *counter_reset;

unsigned int timer_channel_pdo_os;

unsigned int counter_channel_pdo_os;

unsigned int timer_start_channel_pdo_os;
unsigned int timer_start_channel_pdo_bp;

unsigned int timer_reset_channel_pdo_os;
unsigned int timer_reset_channel_pdo_bp;

unsigned int counter_clock_channel_pdo_os;
unsigned int counter_clock_channel_pdo_bp;

unsigned int counter_reset_channel_pdo_os;
unsigned int counter_reset_channel_pdo_bp;
 
} lcec_el6090_chan_t;


typedef struct {
  lcec_el6090_chan_t chans[LCEC_EL6090_CHANS];

  hal_bit_t *button_up;
  hal_bit_t *button_down;
  hal_bit_t *button_left;
  hal_bit_t *button_right;
  hal_bit_t *button_enter;
  hal_bit_t *button_toggle;
 
  hal_u32_t *operating_time; 
  hal_u32_t *operating_time_hour;

  hal_u32_t *value_1;
  hal_u32_t *value_2;

  unsigned int button_up_pdo_os;
  unsigned int button_down_pdo_os;
  unsigned int button_left_pdo_os;
  unsigned int button_right_pdo_os;
  unsigned int button_enter_pdo_os;
  unsigned int button_toggle_pdo_os;

  unsigned int button_up_pdo_bp;
  unsigned int button_down_pdo_bp;
  unsigned int button_left_pdo_bp;
  unsigned int button_right_pdo_bp;
  unsigned int button_enter_pdo_bp;
  unsigned int button_toggle_pdo_bp;

  unsigned int operating_time_pdo_os;

  unsigned int value_row1_pdo_os;
  unsigned int value_row2_pdo_os;

  int last_operational;

} lcec_el6090_data_t;

static const lcec_pindesc_t button_pins[] = {
  { HAL_BIT, HAL_OUT, offsetof(lcec_el6090_data_t, button_up), "%s.%s.%s.keyboard.button-up" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el6090_data_t, button_down), "%s.%s.%s.keyboard.button-down" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el6090_data_t, button_left), "%s.%s.%s.keyboard.button-left" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el6090_data_t, button_right), "%s.%s.%s.keyboard.button-right" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el6090_data_t, button_enter), "%s.%s.%s.keyboard.button-enter" },
  { HAL_BIT, HAL_OUT, offsetof(lcec_el6090_data_t, button_toggle), "%s.%s.%s.keyboard.button-toggle" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static const lcec_pindesc_t operating_time_pins[] = {
  { HAL_U32, HAL_OUT, offsetof(lcec_el6090_data_t, operating_time), "%s.%s.%s.operating-time" },
  { HAL_U32, HAL_OUT, offsetof(lcec_el6090_data_t, operating_time_hour), "%s.%s.%s.operating-time-hour" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static const lcec_pindesc_t value_lcd_pins[] = {
  { HAL_U32, HAL_IN, offsetof(lcec_el6090_data_t, value_1), "%s.%s.%s.value-1" },
  { HAL_U32, HAL_IN, offsetof(lcec_el6090_data_t, value_2), "%s.%s.%s.value-2" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static const lcec_pindesc_t outputs_pins[] = {
  { HAL_U32, HAL_OUT, offsetof(lcec_el6090_chan_t, timer), "%s.%s.%s.ch%d.timer" },
  { HAL_U32, HAL_OUT, offsetof(lcec_el6090_chan_t, counter), "%s.%s.%s.ch%d.counter" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static const lcec_pindesc_t inputs_pins[] = {
  { HAL_BIT, HAL_IN, offsetof(lcec_el6090_chan_t, timer_start), "%s.%s.%s.ch%d.timer-start" },
  { HAL_BIT, HAL_IN, offsetof(lcec_el6090_chan_t, timer_reset), "%s.%s.%s.ch%d.timer-reset" },
  { HAL_BIT, HAL_IN, offsetof(lcec_el6090_chan_t, counter_clock), "%s.%s.%s.ch%d.counter-clock" },
  { HAL_BIT, HAL_IN, offsetof(lcec_el6090_chan_t, counter_reset), "%s.%s.%s.ch%d.counter-reset" },
  { HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL }
};

static ec_pdo_entry_info_t lcec_el6090_dis_outputs[] = {
   {0x7000, 0x11, 16}, // Value Row 1
   {0x7000, 0x12, 16}  // Value Row 2
};

static ec_pdo_entry_info_t lcec_el6090_ucp_outputs_channel_1[] = {
   {0x7010, 0x01, 1}, // Ctrl Timer Start
   {0x7010, 0x02, 1}, // Ctrl Timer Reset
   {0x0000, 0x00, 5}, // Gap
   {0x7010, 0x08, 1}, // Ctrl Counter Clk
   {0x7010, 0x09, 1}, // Ctrl Counter Reset
   {0x0000, 0x00, 7}  // Gap
};

static ec_pdo_entry_info_t lcec_el6090_ucp_outputs_channel_2[] = {
   {0x7020, 0x01, 1}, // Ctrl Timer Start
   {0x7020, 0x02, 1}, // Ctrl Timer Reset
   {0x0000, 0x00, 5}, // Gap
   {0x7020, 0x08, 1}, // Ctrl Counter Clk
   {0x7020, 0x09, 1}, // Ctrl Counter Reset
   {0x0000, 0x00, 7}  // Gap
};

static ec_pdo_entry_info_t lcec_el6090_ucp_outputs_channel_3[] = {
   {0x7030, 0x01, 1}, // Ctrl Timer Start
   {0x7030, 0x02, 1}, // Ctrl Timer Reset
   {0x0000, 0x00, 5}, // Gap
   {0x7030, 0x08, 1}, // Ctrl Counter Clk
   {0x7030, 0x09, 1}, // Ctrl Counter Reset
   {0x0000, 0x00, 7}  // Gap
};

static ec_pdo_entry_info_t lcec_el6090_ucp_outputs_channel_4[] = {
   {0x7040, 0x01, 1}, // Ctrl Timer Start
   {0x7040, 0x02, 1}, // Ctrl Timer Reset
   {0x0000, 0x00, 5}, // Gap
   {0x7040, 0x08, 1}, // Ctrl Counter Clk
   {0x7040, 0x09, 1}, // Ctrl Counter Reset
   {0x0000, 0x00, 7}  // Gap
};

static ec_pdo_entry_info_t lcec_el6090_dis_inputs[] = {
   {0x0000, 0x00, 2}, // Gap
   {0x6000, 0x03, 1}, // Status Up
   {0x6000, 0x04, 1}, // Status Down
   {0x6000, 0x05, 1}, // Status Left
   {0x6000, 0x06, 1}, // Status Right
   {0x6000, 0x07, 1}, // Status Enter
   {0x0000, 0x00, 8}, // Gap
   {0x6000, 0x10, 1}  // Status TxPDO Toggle
};

static ec_pdo_entry_info_t lcec_el6090_ucp_input_channel_1[] = {
   {0x0000, 0x00, 14}, // Gap
   {0x6010, 0x0f,  2}, // Input Cycle Counter
   {0x6010, 0x11, 32}, // Timer
   {0x6010, 0x12, 32}  // Counter
};

static ec_pdo_entry_info_t lcec_el6090_ucp_input_channel_2[] = {
   {0x0000, 0x00, 14}, // Gap
   {0x6020, 0x0f,  2}, // Input Cycle Counter
   {0x6020, 0x11, 32}, // Timer
   {0x6020, 0x12, 32}  // Counter
};

static ec_pdo_entry_info_t lcec_el6090_ucp_input_channel_3[] = {
   {0x0000, 0x00, 14}, // Gap
   {0x6030, 0x0f,  2}, // Input Cycle Counter
   {0x6030, 0x11, 32}, // Timer
   {0x6030, 0x12, 32}  // Counter
};

static ec_pdo_entry_info_t lcec_el6090_ucp_input_channel_4[] = {
   {0x0000, 0x00, 14}, // Gap
   {0x6040, 0x0f,  2}, // Input Cycle Counter
   {0x6040, 0x11, 32}, // Timer
   {0x6040, 0x12, 32}  // Counter
};

static ec_pdo_entry_info_t lcec_el6090_ucp_inputs_operating_time[] = {
   {0x0000, 0x00, 14}, // Gap
   {0xf600, 0x0f,  2}, // Input Cycle Counter
   {0xf600, 0x11, 32}  // Operating Time
};

static ec_pdo_info_t lcec_el6090_pdos_out[] = {
    {0x1600, 2, lcec_el6090_dis_outputs},
    {0x1601, 6, lcec_el6090_ucp_outputs_channel_1},
    {0x1602, 6, lcec_el6090_ucp_outputs_channel_2},
    {0x1603, 6, lcec_el6090_ucp_outputs_channel_3},
    {0x1604, 6, lcec_el6090_ucp_outputs_channel_4}
};

static ec_pdo_info_t lcec_el6090_pdos_in[] = {
    {0x1a00, 8, lcec_el6090_dis_inputs},
    {0x1a01, 4, lcec_el6090_ucp_input_channel_1},
    {0x1a02, 4, lcec_el6090_ucp_input_channel_2},
    {0x1a03, 4, lcec_el6090_ucp_input_channel_3},
    {0x1a04, 4, lcec_el6090_ucp_input_channel_4},
    {0x1a05, 3, lcec_el6090_ucp_inputs_operating_time}
};

static ec_sync_info_t lcec_el6090_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL},
    {1, EC_DIR_INPUT,  0, NULL},
    {2, EC_DIR_OUTPUT, 5, lcec_el6090_pdos_out},
    {3, EC_DIR_INPUT,  6, lcec_el6090_pdos_in},
    {0xff}
};

static int lcec_el6090_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el6090_data_t *hal_data;
  lcec_el6090_chan_t *chan;

  int err, i;

  // initialize callbacks
  slave->proc_read = lcec_el6090_read;
  slave->proc_write = lcec_el6090_write;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el6090_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el6090_data_t));
  slave->hal_data = hal_data;

  // initialize sync info
  slave->sync_info = lcec_el6090_syncs;

  // initialize POD entries
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x03, &hal_data->button_up_pdo_os, &hal_data->button_up_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x04, &hal_data->button_down_pdo_os, &hal_data->button_down_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x05, &hal_data->button_left_pdo_os, &hal_data->button_left_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x06, &hal_data->button_right_pdo_os, &hal_data->button_right_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x07, &hal_data->button_enter_pdo_os, &hal_data->button_enter_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000, 0x10, &hal_data->button_toggle_pdo_os, &hal_data->button_toggle_pdo_bp);

  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xf600, 0x11, &hal_data->operating_time_pdo_os, NULL);

  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000, 0x11, &hal_data->value_row1_pdo_os, NULL);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000, 0x12, &hal_data->value_row2_pdo_os, NULL);

  // export pins
  if ((err = lcec_pin_newf_list(hal_data, button_pins, LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    return err; 
  }

  if ((err = lcec_pin_newf_list(hal_data, operating_time_pins, LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    return err; 
  }

  if ((err = lcec_pin_newf_list(hal_data, value_lcd_pins, LCEC_MODULE_NAME, master->name, slave->name)) != 0) {
    return err; 
  }

  // Initialize Variables
   hal_data->last_operational = 0;

  // initialize Pins
  *(hal_data->button_up)     = 0;
  *(hal_data->button_down)   = 0;
  *(hal_data->button_left)   = 0;
  *(hal_data->button_right)  = 0;
  *(hal_data->button_enter)  = 0;
  *(hal_data->button_toggle) = 0;

  *(hal_data->value_1)       = 0;
  *(hal_data->value_2)       = 0;

  // initialize Channel 1/2/3/4
  for (i=0; i<LCEC_EL6090_CHANS; i++) {
	chan = &hal_data->chans[i];

	//initalize POD entries
	  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6010 + (i*0x10), 0x11, &chan->timer_channel_pdo_os, NULL);
	  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6010 + (i*0x10), 0x12, &chan->counter_channel_pdo_os, NULL); 

 	  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7010 + (i*0x10), 0x01, &chan->timer_start_channel_pdo_os, &chan->timer_start_channel_pdo_bp);    
	  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7010 + (i*0x10), 0x02, &chan->timer_reset_channel_pdo_os, &chan->timer_reset_channel_pdo_bp);    
	  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7010 + (i*0x10), 0x08, &chan->counter_clock_channel_pdo_os, &chan->counter_clock_channel_pdo_bp);    
	  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7010 + (i*0x10), 0x09, &chan->counter_reset_channel_pdo_os, &chan->counter_reset_channel_pdo_bp);

	   // export pins
          if ((err = lcec_pin_newf_list(chan, inputs_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
          return err; 
          }

          if ((err = lcec_pin_newf_list(chan, outputs_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
          return err; 
        }

	  // initialize Pins	
          *(chan->timer_start)   = 0;
          *(chan->timer_reset)   = 0;
	  *(chan->counter_clock) = 0;
	  *(chan->counter_reset) = 0;
	
   }  

  return 0;

}

static void lcec_el6090_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el6090_data_t *hal_data = (lcec_el6090_data_t *) slave->hal_data;
  lcec_el6090_chan_t *chan;
  
  uint8_t *pd = master->process_data;
  int i;
  uint32_t operating_time;

  // wait for slave to be operational
  if (!slave->state.operational) {
      hal_data->last_operational = 0;
    return;  
    }

  // Read Keyboard 
  *(hal_data->button_up) = EC_READ_BIT(&pd[hal_data->button_up_pdo_os], hal_data->button_up_pdo_bp);
  *(hal_data->button_down) = EC_READ_BIT(&pd[hal_data->button_down_pdo_os], hal_data->button_down_pdo_bp);
  *(hal_data->button_left) = EC_READ_BIT(&pd[hal_data->button_left_pdo_os], hal_data->button_left_pdo_bp);
  *(hal_data->button_right) = EC_READ_BIT(&pd[hal_data->button_right_pdo_os], hal_data->button_right_pdo_bp);
  *(hal_data->button_enter) = EC_READ_BIT(&pd[hal_data->button_enter_pdo_os], hal_data->button_enter_pdo_bp);
  *(hal_data->button_toggle) = EC_READ_BIT(&pd[hal_data->button_toggle_pdo_os], hal_data->button_toggle_pdo_bp);

  // Read Operating Hours Counter (second)
  operating_time = EC_READ_U32(&pd[hal_data->operating_time_pdo_os]);
  *(hal_data->operating_time) = operating_time;
  *(hal_data->operating_time_hour)= operating_time / EL6090_HOUR_SCALE;

  // Read Channel 1/2/3/4
  for (i=0; i<LCEC_EL6090_CHANS; i++) {
  	chan = &hal_data->chans[i];
	
	//Read Channel
 	*(chan->timer)   = EC_READ_U32(&pd[chan->timer_channel_pdo_os]);
	*(chan->counter) = EC_READ_U32(&pd[chan->counter_channel_pdo_os]);
  }

  hal_data->last_operational = 1;
}

static void lcec_el6090_write(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el6090_data_t *hal_data = (lcec_el6090_data_t *) slave->hal_data;
  lcec_el6090_chan_t *chan;

  uint8_t *pd = master->process_data;
  int i;

  // Write Value LCD
  EC_WRITE_U16(&pd[hal_data->value_row1_pdo_os], *(hal_data->value_1));
  EC_WRITE_U16(&pd[hal_data->value_row2_pdo_os], *(hal_data->value_2));

  // Write Channel 1/2/3/4
  for (i=0; i<LCEC_EL6090_CHANS; i++) {
	chan = &hal_data->chans[i];
	
	EC_WRITE_BIT(&pd[chan->timer_start_channel_pdo_os],chan->timer_start_channel_pdo_bp, *(chan->timer_start));
  	EC_WRITE_BIT(&pd[chan->timer_reset_channel_pdo_os],chan->timer_reset_channel_pdo_bp, *(chan->timer_reset));      
  	EC_WRITE_BIT(&pd[chan->counter_clock_channel_pdo_os],chan->counter_clock_channel_pdo_bp, *(chan->counter_clock));  
  	EC_WRITE_BIT(&pd[chan->counter_reset_channel_pdo_os],chan->counter_reset_channel_pdo_bp, *(chan->counter_reset));  	
  }
  
}
