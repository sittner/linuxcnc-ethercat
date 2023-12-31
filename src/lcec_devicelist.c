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
#include "lcec_ek1100.h"
#include "lcec_generic.h"
#include "lcec_deasda.h"
#include "lcec_dems300.h"
#include "lcec_omrg5.h"
#include "lcec_ph3lm2rm.h"
#include "lcec_stmds5k.h"

static lcec_typelinkedlist_t *typeslist = NULL;

static const lcec_typelist_t types[] = {
  // bus coupler, no actual driver.
  { "EK1100", LCEC_EK1100_VID, LCEC_EK1100_PID, LCEC_EK1100_PDOS, 0, NULL, NULL},
  { "EK1101", LCEC_EK1100_VID, LCEC_EK1101_PID, LCEC_EK1101_PDOS, 0, NULL, NULL},
  { "EK1110", LCEC_EK1100_VID, LCEC_EK1110_PID, LCEC_EK1110_PDOS, 0, NULL, NULL},
  { "EK1122", LCEC_EK1100_VID, LCEC_EK1122_PID, LCEC_EK1122_PDOS, 0, NULL, NULL},

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

