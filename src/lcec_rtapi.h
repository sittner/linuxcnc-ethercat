/********************************************************************
 * fake unified build config_module.h
 *******************************************************************/

#ifndef _LCEC_RTAPI_H_
#define _LCEC_RTAPI_H_

//#include "config_module.h"

#define RTAPI_INC_CTYPE_H       "linux/ctype.h"
#define RTAPI_INC_DEVICE_H      "linux/device.h"
#define RTAPI_INC_FIRMWARE_H    "linux/firmware.h"
#define RTAPI_INC_GFP_H         "linux/gfp.h"
#define RTAPI_INC_LIST_H        "linux/list.h"
#define RTAPI_INC_SLAB_H        "linux/slab.h"
#define RTAPI_INC_STRING_H      "linux/string.h"
#define RTAPI_INC_SCHED_H       "linux/sched.h"
#define RTAPI_INC_JIFFIES_H     "linux/jiffies.h"
#define RTAPI_INC_TIME_H        "linux/time.h"

#define RTAPI_GETTIMEOFDAY(x) do_gettimeofday(x) 
//#define RTAPI_GETTIMEOFDAY(x) gettimeofday(x, NULL) 

#include RTAPI_INC_SCHED_H
#include RTAPI_INC_JIFFIES_H
#include RTAPI_INC_CTYPE_H
#include RTAPI_INC_SLAB_H
#include RTAPI_INC_STRING_H
#include RTAPI_INC_TIME_H

#endif /* CONFIG_MODULE_H */
