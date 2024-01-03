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
#include "devices/lcec_ek1100.h"

static lcec_typelinkedlist_t *typeslist = NULL;

static const lcec_typelist_t types[] = {
  // bus coupler, no actual driver.
  { "EK1100", LCEC_EK1100_VID, LCEC_EK1100_PID, LCEC_EK1100_PDOS, 0, NULL, NULL},
  { "EK1101", LCEC_EK1100_VID, LCEC_EK1101_PID, LCEC_EK1101_PDOS, 0, NULL, NULL},
  { "EK1110", LCEC_EK1100_VID, LCEC_EK1110_PID, LCEC_EK1110_PDOS, 0, NULL, NULL},
  { "EK1122", LCEC_EK1100_VID, LCEC_EK1122_PID, LCEC_EK1122_PDOS, 0, NULL, NULL},
  { "EP1122", LCEC_EK1100_VID, LCEC_EP1122_PID, LCEC_EP1122_PDOS, 0, NULL, NULL},

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
    // This should really validate that we don't have duplicate names,
    // but there's no good way to print error messages here.  If we're
    // running in lcec_main, then we would need to use different code
    // from lcec_conf.
    //
    // Note that if there are duplicate names, then the first match
    // found will win in lcec_findslavetype, below.
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
const lcec_typelist_t *lcec_findslavetype(const char *name) {
  const lcec_typelist_t *type;
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

