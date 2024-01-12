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
//    A.Chimeno Edit 14/06/2019 Rev 2.0


#include "../lcec.h"
#include "lcec_el3403.h"

static int lcec_el3403_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs);

static lcec_typelist_t types[]={
  // analog in, 3ch, 16 bits
  { "EL3403", LCEC_BECKHOFF_VID, LCEC_EL3403_PID, LCEC_EL3403_PDOS, 0, NULL, lcec_el3403_init},
  { NULL },
};
ADD_TYPES(types);

//Scaling Factors EL3403-0010
#define EL3403_FACTOR_CURRENT (0.000005)
#define EL3403_FACTOR_VOLTAGE (0.0001)
#define EL3403_FACTOR_ACTIVE_POWER (0.01)
#define EL3403_FACTOR_APPARENT_POWER (0.01)
#define EL3403_FACTOR_REACTIVE_POWER (0.01)
#define EL3403_FACTOR_ENERGY (0.001)
#define EL3403_FACTOR_COSPHI (0.001)
#define EL3403_FACTOR_FREQUENCY (0.1)
#define EL3403_FACTOR_ENERGY_NEGATIVE (0.001)


typedef struct {

  hal_bit_t *sync_error;
  hal_bit_t *txpdo_toggle;
  hal_float_t *current;
  hal_float_t *voltage;
  hal_float_t *active_power;
  hal_float_t *apparent_power;
  hal_float_t *reactive_power;
  hal_float_t *energy;
  hal_float_t *cosphi;
  hal_float_t *frequency;
  hal_float_t *energy_negative;
  hal_bit_t *missing_zero_crossing;
      
  unsigned int sync_error_pdo_os;
  unsigned int sync_error_pdo_bp;
  unsigned int txpdo_toggle_pdo_os;
  unsigned int txpdo_toggle_pdo_bp;
  unsigned int current_pdo_os;
  unsigned int voltage_pdo_os;
  unsigned int active_power_pdo_os;
  unsigned int ovc_pdo_os;
  unsigned int variable_pdo_os;
  unsigned int missing_zero_crossing_pdo_os;
  unsigned int missing_zero_crossing_pdo_bp;
  unsigned int index_pdo_os;

} lcec_el3403_chan_t;

typedef struct{
  lcec_el3403_chan_t chans[LCEC_EL3403_CHANS];
      
  hal_bit_t *sync_error_status;
  hal_bit_t *phase_sequence_error;
  unsigned int phase_sequence_error_pdo_os;
  unsigned int phase_sequence_error_pdo_bp;
  unsigned int sync_error_status_pdo_os;
  unsigned int sync_error_status_pdo_bp;
  unsigned int index;
  unsigned int last_operational;
   
} lcec_el3403_data_t;
  
static const lcec_pindesc_t outputs_pins[] = {
  {HAL_BIT, HAL_OUT, offsetof(lcec_el3403_chan_t, sync_error), "%s.%s.%s.l%d.sync-error"},
  {HAL_BIT, HAL_OUT, offsetof(lcec_el3403_chan_t, txpdo_toggle), "%s.%s.%s.l%d.txpdo-toggle"},
  {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, current), "%s.%s.%s.l%d.current"},
  {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, voltage), "%s.%s.%s.l%d.voltage"},
  {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, active_power), "%s.%s.%s.l%d.active-power"},
  {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, apparent_power), "%s.%s.%s.l%d.apparent-power"},
  {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, reactive_power), "%s.%s.%s.l%d.reactive-power"},
  {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, energy), "%s.%s.%s.l%d.energy"},
  {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, cosphi), "%s.%s.%s.l%d.cosphi"},
  {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, frequency), "%s.%s.%s.l%d.frequency"},
  {HAL_FLOAT, HAL_OUT, offsetof(lcec_el3403_chan_t, energy_negative), "%s.%s.%s.l%d.energy-negative"},
  {HAL_BIT, HAL_OUT, offsetof(lcec_el3403_chan_t, missing_zero_crossing), "%s.%s.%s.l%d.missing-zero-crossing"},
  {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL}
};

static const lcec_pindesc_t single_outputs_pins[] = {
  {HAL_BIT, HAL_OUT, offsetof(lcec_el3403_data_t, sync_error_status), "%s.%s.%s.sync-error-status"},
  {HAL_BIT, HAL_OUT, offsetof(lcec_el3403_data_t, phase_sequence_error), "%s.%s.%s.phase-sequence-error"},
  {HAL_TYPE_UNSPECIFIED, HAL_DIR_UNSPECIFIED, -1, NULL}
};
  	  	  	  	  	  	  	  	   
ec_pdo_entry_info_t lcec_el3403_channel_1_in[] = {
    {0x0000, 0x00, 13}, //Gap
    {0x6000, 0x0e, 1},  //Sync Error
    {0x0000, 0x00, 1},  //Gap
    {0x1800, 0x09, 1},  //TxPDO Toggle
    {0x6000, 0x11, 32}, //Current
    {0x6000, 0x12, 32}, //Voltage
    {0x6000, 0x13, 32}, //Active Power
    {0x6000, 0x14, 8},  //Output Variable Channel
    {0x0000, 0x00, 8},  //Gap
    {0x6000, 0x1d, 32}, //Variable Value
};

ec_pdo_entry_info_t lcec_el3403_channel_2_in[] = {
    {0x0000, 0x00, 13}, //Gap
    {0x6010, 0x0e, 1},  //Sync Error
    {0x0000, 0x00, 1},  //Gap
    {0x1801, 0x09, 1},  //TxPDO Toggle
    {0x6010, 0x11, 32}, //Current
    {0x6010, 0x12, 32}, //Voltage
    {0x6010, 0x13, 32}, //Active Power
    {0x6010, 0x14, 8},  //Output Variable Channel
    {0x0000, 0x00, 8},  //Gap
    {0x6010, 0x1d, 32}, //Variable Value
};

ec_pdo_entry_info_t lcec_el3403_channel_3_in[] = {
    {0x0000, 0x00, 13}, //Gap
    {0x6020, 0x0e, 1},  //Sync Error
    {0x0000, 0x00, 1},  //Gap
    {0x1802, 0x09, 1},  //TxPDO Toggle
    {0x6020, 0x11, 32}, //Current
    {0x6020, 0x12, 32}, //Voltage
    {0x6020, 0x13, 32}, //Active Power
    {0x6020, 0x14, 8},  //Output Variable Channel
    {0x0000, 0x00, 8},  //Gap
    {0x6020, 0x1d, 32}, //Variable Value
};

ec_pdo_entry_info_t lcec_el3403_status_data[] = {
    {0x0000, 0x00, 3},  //Gap
    {0xf100, 0x04, 1},  //Missing zero crossing A
    {0xf100, 0x05, 1},  //Missing zero crossing B
    {0xf100, 0x06, 1},  //Missing Zero crossing C
    {0x0000, 0x00, 2},  //Gap
    {0xf100, 0x09, 1},  //Phase Sequence Error
    {0x0000, 0x00, 4},  //Gap
    {0xf100, 0x0e, 1},  //Sync Error
    {0x0000, 0x00, 2},  //Gap
};

ec_pdo_entry_info_t lcec_el3403_channel_1_out[] = {
    {0x7000, 0x01, 8}, // index 1
};

ec_pdo_entry_info_t lcec_el3403_channel_2_out[] = {
    {0x7010, 0x01, 8}, // index 2
};

ec_pdo_entry_info_t lcec_el3403_channel_3_out[] = {
    {0x7020, 0x01, 8}, // index 3
};

ec_pdo_info_t lcec_el3403_pdos_in[] = {
    {0x1a00, 10, lcec_el3403_channel_1_in}, 
    {0x1a01, 10, lcec_el3403_channel_2_in},
    {0x1a02, 10, lcec_el3403_channel_3_in},
    {0x1a03,  9, lcec_el3403_status_data},
};

ec_pdo_info_t lcec_el3403_pdos_out[] = {
    {0x1600, 1, lcec_el3403_channel_1_out}, 
    {0x1601, 1, lcec_el3403_channel_2_out},
    {0x1602, 1, lcec_el3403_channel_3_out},
};

ec_sync_info_t lcec_el3403_syncs[] = {
    {0, EC_DIR_INPUT  , 0, NULL},
    {1, EC_DIR_INPUT  , 0, NULL },
    {2, EC_DIR_OUTPUT , 3, lcec_el3403_pdos_out},
    {3, EC_DIR_INPUT  , 4, lcec_el3403_pdos_in},
    {0xff}
};

static void lcec_el3403_read(struct lcec_slave *slave, long period);

static int lcec_el3403_init(int comp_id, struct lcec_slave *slave, ec_pdo_entry_reg_t *pdo_entry_regs) {
  lcec_master_t *master = slave->master;
  lcec_el3403_data_t *hal_data;
  lcec_el3403_chan_t *chan;
  int err, i;

  // initialize callbacks
  slave->proc_read = lcec_el3403_read;

  // alloc hal memory
  if ((hal_data = hal_malloc(sizeof(lcec_el3403_data_t))) == NULL) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "hal_malloc() for slave %s.%s failed\n", master->name, slave->name);
    return -EIO;
  }
  memset(hal_data, 0, sizeof(lcec_el3403_data_t));
  slave->hal_data = hal_data;

  // initializer sync info
  slave->sync_info = lcec_el3403_syncs;

  // initialize POD entries
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xf100, 0x09, &hal_data->phase_sequence_error_pdo_os, &hal_data->phase_sequence_error_pdo_bp);
  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xf100, 0x0e, &hal_data->sync_error_status_pdo_os, &hal_data->sync_error_status_pdo_bp);

  // export pins
  if ((err=lcec_pin_newf_list(hal_data, single_outputs_pins, LCEC_MODULE_NAME, master->name, slave->name)) !=0) {
	 return err;
  } 

  // initialize variable
  hal_data->last_operational = 0;

  // initialize pins
  *(hal_data->sync_error_status) = 0;
  *(hal_data->phase_sequence_error) = 0;
  
  // initialize channel L1/L2/L3
  for (i=0 ; i<LCEC_EL3403_CHANS; i++) {
	  chan = &hal_data->chans[i];
	  
      // initialize POD entries
      LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i*0x10), 0x0e, &chan->sync_error_pdo_os, &chan->sync_error_pdo_bp);
	  LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x1800 + (i*0x01), 0x09, &chan->txpdo_toggle_pdo_os, &chan->txpdo_toggle_pdo_bp);
      LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i*0x10) , 0x11, &chan->current_pdo_os, NULL);
      LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i*0x10), 0x12, &chan->voltage_pdo_os, NULL);
      LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i*0x10), 0x13, &chan->active_power_pdo_os, NULL);
      LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i*0x10), 0x14, &chan->ovc_pdo_os, NULL);
      LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x6000 + (i*0x10), 0x1d, &chan->variable_pdo_os, NULL);
      LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0x7000 + (i*0x10), 0x01, &chan->index_pdo_os, NULL);
      LCEC_PDO_INIT(pdo_entry_regs, slave->index, slave->vid, slave->pid, 0xf100, 0x04 + (i*0x01), &chan->missing_zero_crossing_pdo_os, &chan->missing_zero_crossing_pdo_bp);

	  // export pins
      if ((err = lcec_pin_newf_list(chan, outputs_pins, LCEC_MODULE_NAME, master->name, slave->name, i)) != 0) {
	  return err;
      }
       
      // initialize pins
      *(chan->sync_error) = 0;
      *(chan->txpdo_toggle) = 0;
      *(chan->current) = 0.0;
      *(chan->voltage) = 0.0;
      *(chan->active_power) = 0.0;
      *(chan->apparent_power) = 0.0;
      *(chan->reactive_power) = 0.0;
      *(chan->energy) = 0.0;
      *(chan->cosphi) = 0.0;
      *(chan->frequency) = 0.0;
      *(chan->energy_negative) = 0.0;
      *(chan->missing_zero_crossing) = 0;
  }
  return 0;
}

static void lcec_el3403_read(struct lcec_slave *slave, long period) {
  lcec_master_t *master = slave->master;
  lcec_el3403_data_t *hal_data = (lcec_el3403_data_t *) slave->hal_data;
  lcec_el3403_chan_t * chan;
  
  int i;
  uint8_t *pd = master->process_data;
  int32_t current, voltage, active_power, apparent_power, reactive_power, energy, cosphi, frequency, energy_negative;
  uint8_t ovc;

  // wait for slave to be operational
  if (!slave->state.operational) {
	hal_data->last_operational = 0;
    return;
  }

  //Read channel L1/L2/L3
  for (i=0 ; i<LCEC_EL3403_CHANS; i++) {
	  chan = &hal_data->chans[i];
	  
	  // Update Status
      *(chan->sync_error) = EC_READ_BIT(&pd[chan->sync_error_pdo_os], chan->sync_error_pdo_bp);
      *(chan->txpdo_toggle) = EC_READ_BIT(&pd[chan->txpdo_toggle_pdo_os], chan->txpdo_toggle_pdo_bp);
      *(chan->missing_zero_crossing) = EC_READ_BIT(&pd[chan->missing_zero_crossing_pdo_os], chan->missing_zero_crossing_pdo_bp);
 
      // Update Current Channel     
      current = EC_READ_S32(&pd[chan->current_pdo_os]);
      *(chan->current) = (double)current * EL3403_FACTOR_CURRENT;

      // Update voltage Channel 
      voltage = EC_READ_S32(&pd[chan->voltage_pdo_os]);
      *(chan->voltage) = (double)voltage * EL3403_FACTOR_VOLTAGE;

      // Update Active Power Channel 
      active_power = EC_READ_S32(&pd[chan->active_power_pdo_os]);
      *(chan->active_power) = (double)active_power * EL3403_FACTOR_ACTIVE_POWER;     
 
     for (hal_data->index = 0; hal_data->index < 5; hal_data->index ++) {
		  EC_WRITE_U8(&pd[chan->index_pdo_os] , hal_data->index);
    
          ovc = EC_READ_U8(&pd[chan->ovc_pdo_os]);
		  switch (ovc)
			{
				case 0 :
					apparent_power = EC_READ_S32(&pd[chan->variable_pdo_os]);
					*(chan->apparent_power) = (double)apparent_power * EL3403_FACTOR_APPARENT_POWER;
				break;
				case 1 :
					reactive_power = EC_READ_S32(&pd[chan->variable_pdo_os]);
					*(chan->reactive_power) = (double)reactive_power * EL3403_FACTOR_REACTIVE_POWER; 		 
   				break;
				case 2 :
					energy = EC_READ_S32(&pd[chan->variable_pdo_os]);
					*(chan->energy) = (double)energy * EL3403_FACTOR_ENERGY; 
				break;
				case 3 :
					cosphi = EC_READ_S32(&pd[chan->variable_pdo_os]);
					*(chan->cosphi) = (double)cosphi * EL3403_FACTOR_COSPHI;
				break;
				case 4 :
					frequency = EC_READ_S32(&pd[chan->variable_pdo_os]);
					*(chan->frequency) = (double)frequency * EL3403_FACTOR_FREQUENCY;
				break;
				case 5 :
					energy_negative = EC_READ_S32(&pd[chan->variable_pdo_os]);
					*(chan->energy_negative) = (double)energy_negative * EL3403_FACTOR_ENERGY_NEGATIVE;				
				break;	       
			}
		}
	} 
           
  // Update Status
  *(hal_data->sync_error_status) = EC_READ_BIT(&pd[hal_data->sync_error_status_pdo_os], hal_data->sync_error_status_pdo_bp);
  *(hal_data->phase_sequence_error) = EC_READ_BIT(&pd[hal_data->phase_sequence_error_pdo_os], hal_data->phase_sequence_error_pdo_bp);
	
   hal_data->last_operational = 1;	
}
