
/* File generated by gen_cfile.py. Should not be modified. */

#ifndef LCNCMASTER_H
#define LCNCMASTER_H

#include "data.h"

/* Prototypes of function provided by object dictionnary */
UNS32 LCNCmaster_valueRangeTest (UNS8 typeValue, void * value);
const indextable * LCNCmaster_scanIndexOD (UNS16 wIndex, UNS32 * errorCode, ODCallback_t **callbacks);

/* Master node data struct */
extern CO_Data LCNCmaster_Data;

#endif // LCNCMASTER_H
