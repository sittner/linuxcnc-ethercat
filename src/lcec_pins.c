//
//    Copyright (C) 2012 Sascha Ittner <sascha.ittner@modusoft.de>
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

static int lcec_pin_newfv(hal_type_t type, hal_pin_dir_t dir, void **data_ptr_addr, const char *fmt, va_list ap);
static int lcec_pin_newfv_list(void *base, const lcec_pindesc_t *list, va_list ap);
extern int lcec_comp_id;

static int lcec_pin_newfv(hal_type_t type, hal_pin_dir_t dir, void **data_ptr_addr, const char *fmt, va_list ap) {
  char name[HAL_NAME_LEN + 1];
  int sz;
  int err;

  sz = rtapi_vsnprintf(name, sizeof(name), fmt, ap);
  if(sz == -1 || sz > HAL_NAME_LEN) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "length %d too long for name starting '%s'\n", sz, name);
    return -ENOMEM;
  }

  err = hal_pin_new(name, type, dir, data_ptr_addr, lcec_comp_id);
  if (err) {
    rtapi_print_msg(RTAPI_MSG_ERR, LCEC_MSG_PFX "exporting pin %s failed\n", name);
    return err;
  }

  switch (type) {
    case HAL_BIT:
      **((hal_bit_t **) data_ptr_addr) = 0;
      break;
    case HAL_FLOAT:
      **((hal_float_t **) data_ptr_addr) = 0.0;
      break;
    case HAL_S32:
      **((hal_s32_t **) data_ptr_addr) = 0;
      break;
    case HAL_U32:
      **((hal_u32_t **) data_ptr_addr) = 0;
      break;
    default:
      break;
  }

  return 0;
}

int lcec_pin_newf(hal_type_t type, hal_pin_dir_t dir, void **data_ptr_addr, const char *fmt, ...) {
  va_list ap;
  int err;

  va_start(ap, fmt);
  err = lcec_pin_newfv(type, dir, data_ptr_addr, fmt, ap);
  va_end(ap);

  return err;
}

static int lcec_pin_newfv_list(void *base, const lcec_pindesc_t *list, va_list ap) {
  va_list ac;
  int err;
  const lcec_pindesc_t *p;

  for (p = list; p->type != HAL_TYPE_UNSPECIFIED; p++) {
    va_copy(ac, ap);
    err = lcec_pin_newfv(p->type, p->dir, (void **) (base + p->offset), p->fmt, ac);
    va_end(ac);
    if (err) {
      return err;
    }
  }

  return 0;
}

int lcec_pin_newf_list(void *base, const lcec_pindesc_t *list, ...) {
  va_list ap;
  int err;

  va_start(ap, list);
  err = lcec_pin_newfv_list(base, list, ap);
  va_end(ap);

  return err;
}
