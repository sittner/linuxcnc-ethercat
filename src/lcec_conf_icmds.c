//
//  Copyright (C) 2018 Sascha Ittner <sascha.ittner@modusoft.de>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <expat.h>
#include <signal.h>
#include <sys/eventfd.h>

#include "lcec_conf.h"

#define BUFFSIZE 8192

extern char *modname;
extern void *addOutputBuffer(size_t len);

typedef enum {
  icmdTypeNone,
  icmdTypeMailbox,
  icmdTypeCoe,
  icmdTypeCoeIcmds,
  icmdTypeCoeIcmd,
  icmdTypeCoeIcmdTrans,
  icmdTypeCoeIcmdComment,
  icmdTypeCoeIcmdTimeout,
  icmdTypeCoeIcmdCcs,
  icmdTypeCoeIcmdIndex,
  icmdTypeCoeIcmdSubindex,
  icmdTypeCoeIcmdData,
  icmdTypeSoe,
  icmdTypeSoeIcmds,
  icmdTypeSoeIcmd,
  icmdTypeSoeIcmdTrans,
  icmdTypeSoeIcmdComment,
  icmdTypeSoeIcmdTimeout,
  icmdTypeSoeIcmdOpcode,
  icmdTypeSoeIcmdDriveno,
  icmdTypeSoeIcmdIdn,
  icmdTypeSoeIcmdElements,
  icmdTypeSoeIcmdAttribute,
  icmdTypeSoeIcmdData
} LCEC_ICMD_TYPE_T;

static LCEC_CONF_SLAVE_T *currSlave;

static XML_Parser parser;

static int currIcmdType;
static LCEC_CONF_SDOCONF_T *currSdoConf;
static LCEC_CONF_IDNCONF_T *currIdnConf;

static long int parse_int(const char *s, int len, long int min, long int max);
static int parse_data(const char *s, int len);

static void xml_start_handler(void *data, const char *el, const char **attr);
static void xml_end_handler(void *data, const char *el);
static void xml_data_handler(void *data, const XML_Char *s, int len);

int parseIcmds(LCEC_CONF_SLAVE_T *slave, const char *filename) {
  int ret = 1;
  int done;
  char buffer[BUFFSIZE];
  FILE *file;

  // open file
  file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "%s: ERROR: unable to open config file %s\n", modname, filename);
    goto fail1;
  }

  // create xml parser
  parser = XML_ParserCreate(NULL);
  if (parser == NULL) {
    fprintf(stderr, "%s: ERROR: Couldn't allocate memory for parser\n", modname);
    goto fail2;
  }

  // setup handlers
  XML_SetElementHandler(parser, xml_start_handler, xml_end_handler);
  XML_SetCharacterDataHandler(parser, xml_data_handler);

  currSlave = slave;
  currIcmdType = icmdTypeNone;
  currSdoConf = NULL;
  currIdnConf = NULL;
  for (done=0; !done;) {
    // read block
    int len = fread(buffer, 1, BUFFSIZE, file);
    if (ferror(file)) {
      fprintf(stderr, "%s: ERROR: Couldn't read from file %s\n", modname, filename);
      goto fail3;
    }

    // check for EOF
    done = feof(file);

    // parse current block
    if (!XML_Parse(parser, buffer, len, done)) {
      fprintf(stderr, "%s: ERROR: Parse error at line %u: %s\n", modname,
        (unsigned int)XML_GetCurrentLineNumber(parser),
        XML_ErrorString(XML_GetErrorCode(parser)));
      goto fail3;
    }
  }

  // everything is fine
  ret = 0;

fail3:
  XML_ParserFree(parser);
fail2:
  fclose(file);
fail1:
  return ret;
}

static long int parse_int(const char *s, int len, long int min, long int max) {
  char buf[32];
  char *end;
  long int ret;

  if (s == NULL || len == 0) {
    fprintf(stderr, "%s: ERROR: Missing number value\n", modname);
    XML_StopParser(parser, 0);
    return 0;
  }

  if (len >= sizeof(buf)) {
    fprintf(stderr, "%s: ERROR: Number value size exceeded\n", modname);
    XML_StopParser(parser, 0);
    return 0;
  }

  strncpy(buf, s, len);
  buf[len] = 0;

  ret = strtol(buf, &end, 0);
  if (*end != 0 || ret < min || ret > max) {
    fprintf(stderr, "%s: ERROR: Invalid number value '%s'\n", modname, s);
    XML_StopParser(parser, 0);
    return 0;
  }

  return ret;
}

static int parse_data(const char *s, int len) {
  uint8_t *p;
  char c;
  int size;

  // length must be modulo of 2
  if (len & 1) {
    fprintf(stderr, "%s: ERROR: Invalid data size\n", modname);
    XML_StopParser(parser, 0);
    return 0;
  }
  size = len >> 1;

  // allocate memory
  p = (uint8_t *) addOutputBuffer(size);
  if (p == NULL) {
    XML_StopParser(parser, 0);
    return 0;
  }

  while (len > 0) {
    c = *(s++);

    // get nibble value
    if (c >= '0' && c <= '9') {
      c = c - '0';
    } else if (c >= 'a' && c <= 'f') {
      c = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
      c = c - 'A' + 10;
    } else {
      fprintf(stderr, "%s: ERROR: Non-hex value data\n", modname);
      XML_StopParser(parser, 0);
      return 0;
    }

    // alternate nibbles
    if (!(len & 1)) {
      *p = c << 4;
    } else {
      *p |= c & 0x0f;
      p++;
    }
    len--;
  }

  return size;
}

static void xml_start_handler(void *data, const char *el, const char **attr) {
  switch (currIcmdType) {
    case icmdTypeNone:
      if (strcmp(el, "EtherCATMailbox") == 0) {
        currIcmdType = icmdTypeMailbox;
        return;
      }
      break;
    case icmdTypeMailbox:
      if (strcmp(el, "CoE") == 0) {
        currIcmdType = icmdTypeCoe;
        return;
      }
      if (strcmp(el, "SoE") == 0) {
        currIcmdType = icmdTypeSoe;
        return;
      }
      break;

    case icmdTypeCoe:
      if (strcmp(el, "InitCmds") == 0) {
        currIcmdType = icmdTypeCoeIcmds;
        return;
      }
      break;
    case icmdTypeCoeIcmds:
      if (strcmp(el, "InitCmd") == 0) {
        currIcmdType = icmdTypeCoeIcmd;
        currSdoConf = addOutputBuffer(sizeof(LCEC_CONF_SDOCONF_T));
        if (currSdoConf == NULL) {
          XML_StopParser(parser, 0);
          return;
        }
        currSdoConf->confType = lcecConfTypeSdoConfig;
        currSdoConf->index = 0xffff;
        currSdoConf->subindex = 0xff;
        return;
      }
      break;
    case icmdTypeCoeIcmd:
      if (strcmp(el, "Transition") == 0) {
        currIcmdType = icmdTypeCoeIcmdTrans;
        return;
      }
      if (strcmp(el, "Comment") == 0) {
        currIcmdType = icmdTypeCoeIcmdComment;
        return;
      }
      if (strcmp(el, "Timeout") == 0) {
        currIcmdType = icmdTypeCoeIcmdTimeout;
        return;
      }
      if (strcmp(el, "Ccs") == 0) {
        currIcmdType = icmdTypeCoeIcmdCcs;
        return;
      }
      if (strcmp(el, "Index") == 0) {
        currIcmdType = icmdTypeCoeIcmdIndex;
        return;
      }
      if (strcmp(el, "SubIndex") == 0) {
        currIcmdType = icmdTypeCoeIcmdSubindex;
        return;
      }
      if (strcmp(el, "Data") == 0) {
        currIcmdType = icmdTypeCoeIcmdData;
        return;
      }
      break;

    case icmdTypeSoe:
      if (strcmp(el, "InitCmds") == 0) {
        currIcmdType = icmdTypeSoeIcmds;
        return;
      }
      break;
    case icmdTypeSoeIcmds:
      if (strcmp(el, "InitCmd") == 0) {
        currIcmdType = icmdTypeSoeIcmd;
        currIdnConf = addOutputBuffer(sizeof(LCEC_CONF_IDNCONF_T));
        if (currIdnConf == NULL) {
          XML_StopParser(parser, 0);
          return;
        }
        currIdnConf->confType = lcecConfTypeIdnConfig;
        currIdnConf->drive = 0;
        currIdnConf->idn = 0xffff;
        currIdnConf->state = 0;
        return;
      }
      break;
    case icmdTypeSoeIcmd:
      if (strcmp(el, "Transition") == 0) {
        currIcmdType = icmdTypeSoeIcmdTrans;
        return;
      }
      if (strcmp(el, "Comment") == 0) {
        currIcmdType = icmdTypeSoeIcmdComment;
        return;
      }
      if (strcmp(el, "Timeout") == 0) {
        currIcmdType = icmdTypeSoeIcmdTimeout;
        return;
      }
      if (strcmp(el, "OpCode") == 0) {
        currIcmdType = icmdTypeSoeIcmdOpcode;
        return;
      }
      if (strcmp(el, "DriveNo") == 0) {
        currIcmdType = icmdTypeSoeIcmdDriveno;
        return;
      }
      if (strcmp(el, "IDN") == 0) {
        currIcmdType = icmdTypeSoeIcmdIdn;
        return;
      }
      if (strcmp(el, "Elements") == 0) {
        currIcmdType = icmdTypeSoeIcmdElements;
        return;
      }
      if (strcmp(el, "Attribute") == 0) {
        currIcmdType = icmdTypeSoeIcmdAttribute;
        return;
      }
      if (strcmp(el, "Data") == 0) {
        currIcmdType = icmdTypeSoeIcmdData;
        return;
      }
      break;
  }

  fprintf(stderr, "%s: ERROR: unexpected node %s found\n", modname, el);
  XML_StopParser(parser, 0);
}

static void xml_end_handler(void *data, const char *el) {
  switch (currIcmdType) {
    case icmdTypeMailbox:
      if (strcmp(el, "EtherCATMailbox") == 0) {
        currIcmdType = icmdTypeNone;
        return;
      }
      break;

    case icmdTypeCoe:
      if (strcmp(el, "CoE") == 0) {
        currIcmdType = icmdTypeMailbox;
        return;
      }
      break;
    case icmdTypeCoeIcmds:
      if (strcmp(el, "InitCmds") == 0) {
        currIcmdType = icmdTypeCoe;
        return;
      }
      break;
    case icmdTypeCoeIcmd:
      if (strcmp(el, "InitCmd") == 0) {
        if (currSdoConf->index == 0xffff) {
          fprintf(stderr, "%s: ERROR: sdoConfig has no idx attribute\n", modname);
          XML_StopParser(parser, 0);
          return;
        }
        if (currSdoConf->subindex == 0xff) {
          fprintf(stderr, "%s: ERROR: sdoConfig has no subIdx attribute\n", modname);
          XML_StopParser(parser, 0);
          return;
        }
        currIcmdType = icmdTypeCoeIcmds;
        currSlave->sdoConfigLength += sizeof(LCEC_CONF_SDOCONF_T) + currSdoConf->length;
        return;
      }
      break;
    case icmdTypeCoeIcmdTrans:
      if (strcmp(el, "Transition") == 0) {
        currIcmdType = icmdTypeCoeIcmd;
        return;
      }
      break;
    case icmdTypeCoeIcmdComment:
      if (strcmp(el, "Comment") == 0) {
        currIcmdType = icmdTypeCoeIcmd;
        return;
      }
      break;
    case icmdTypeCoeIcmdTimeout:
      if (strcmp(el, "Timeout") == 0) {
        currIcmdType = icmdTypeCoeIcmd;
        return;
      }
      break;
    case icmdTypeCoeIcmdCcs:
      if (strcmp(el, "Ccs") == 0) {
        currIcmdType = icmdTypeCoeIcmd;
        return;
      }
      break;
    case icmdTypeCoeIcmdIndex:
      if (strcmp(el, "Index") == 0) {
        currIcmdType = icmdTypeCoeIcmd;
        return;
      }
      break;
    case icmdTypeCoeIcmdSubindex:
      if (strcmp(el, "SubIndex") == 0) {
        currIcmdType = icmdTypeCoeIcmd;
        return;
      }
      break;
    case icmdTypeCoeIcmdData:
      if (strcmp(el, "Data") == 0) {
        currIcmdType = icmdTypeCoeIcmd;
        return;
      }
      break;

    case icmdTypeSoe:
      if (strcmp(el, "SoE") == 0) {
        currIcmdType = icmdTypeMailbox;
        return;
      }
      break;
    case icmdTypeSoeIcmds:
      if (strcmp(el, "InitCmds") == 0) {
        currIcmdType = icmdTypeSoe;
        return;
      }
      break;
    case icmdTypeSoeIcmd:
      if (strcmp(el, "InitCmd") == 0) {
        if (currIdnConf->idn == 0xffff) {
          fprintf(stderr, "%s: ERROR: idnConfig has no idn attribute\n", modname);
          XML_StopParser(parser, 0);
          return;
        }
        if (currIdnConf->state == 0) {
          fprintf(stderr, "%s: ERROR: idnConfig has no state attribute\n", modname);
          XML_StopParser(parser, 0);
          return;
        }
        currIcmdType = icmdTypeSoeIcmds;
        currSlave->idnConfigLength += sizeof(LCEC_CONF_IDNCONF_T) + currIdnConf->length;
        return;
      }
      break;
    case icmdTypeSoeIcmdTrans:
      if (strcmp(el, "Transition") == 0) {
        currIcmdType = icmdTypeSoeIcmd;
        return;
      }
      break;
    case icmdTypeSoeIcmdComment:
      if (strcmp(el, "Comment") == 0) {
        currIcmdType = icmdTypeSoeIcmd;
        return;
      }
      break;
    case icmdTypeSoeIcmdTimeout:
      if (strcmp(el, "Timeout") == 0) {
        currIcmdType = icmdTypeSoeIcmd;
        return;
      }
      break;
    case icmdTypeSoeIcmdOpcode:
      if (strcmp(el, "OpCode") == 0) {
        currIcmdType = icmdTypeSoeIcmd;
        return;
      }
      break;
    case icmdTypeSoeIcmdDriveno:
      if (strcmp(el, "DriveNo") == 0) {
        currIcmdType = icmdTypeSoeIcmd;
        return;
      }
      break;
    case icmdTypeSoeIcmdIdn:
      if (strcmp(el, "IDN") == 0) {
        currIcmdType = icmdTypeSoeIcmd;
        return;
      }
      break;
    case icmdTypeSoeIcmdElements:
      if (strcmp(el, "Elements") == 0) {
        currIcmdType = icmdTypeSoeIcmd;
        return;
      }
      break;
    case icmdTypeSoeIcmdAttribute:
      if (strcmp(el, "Attribute") == 0) {
        currIcmdType = icmdTypeSoeIcmd;
        return;
      }
      break;
    case icmdTypeSoeIcmdData:
      if (strcmp(el, "Data") == 0) {
        currIcmdType = icmdTypeSoeIcmd;
        return;
      }
      break;
  }

  fprintf(stderr, "%s: ERROR: unexpected close tag %s found\n", modname, el);
  XML_StopParser(parser, 0);
}

static void xml_data_handler(void *data, const XML_Char *s, int len) {
  switch (currIcmdType) {
    case icmdTypeCoeIcmdTrans:
      if (len == 2) {
        if (strncmp("PS", s, len) == 0) {
          return;
        }
      }
      fprintf(stderr, "%s: ERROR: Invalid Transition state\n", modname);
      XML_StopParser(parser, 0);
      return;
    case icmdTypeCoeIcmdIndex:
      currSdoConf->index = parse_int(s, len, 0, 0xffff);
      return;
    case icmdTypeCoeIcmdSubindex:
      currSdoConf->subindex = parse_int(s, len, 0, 0xff);
      return;
    case icmdTypeCoeIcmdData:
      currSdoConf->length += parse_data(s, len);
      return;

    case icmdTypeSoeIcmdTrans:
      if (len == 2) {
        if (strncmp("IP", s, len) == 0) {
          currIdnConf->state = EC_AL_STATE_PREOP;
          return;
        }
        if (strncmp("PS", s, len) == 0) {
          currIdnConf->state = EC_AL_STATE_PREOP;
          return;
        }
        if (strncmp("SO", s, len) == 0) {
          currIdnConf->state = EC_AL_STATE_SAFEOP;
          return;
        }
      }
      fprintf(stderr, "%s: ERROR: Invalid Transition state\n", modname);
      XML_StopParser(parser, 0);
      return;
    case icmdTypeSoeIcmdDriveno:
      currIdnConf->drive = parse_int(s, len, 0, 7);
      return;
    case icmdTypeSoeIcmdIdn:
      currIdnConf->idn = parse_int(s, len, 0, 0xffff);
      return;
    case icmdTypeSoeIcmdData:
      currIdnConf->length += parse_data(s, len);
      return;
  }
}

