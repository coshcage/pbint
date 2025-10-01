/*
 * Name:        pbu.h
 * Description: Portable big integer library utilities.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0930252207A0930252244L00022
 * License:     GPLv3.
 */

#ifndef _PBU_H_
#define _PBU_H_

#include <stdio.h> /* Using structure type FILE, and function fprintf and fputc. */
#include "pbk.h"

/* Function declarations. */
_boolean pbuDecimalSzToBint   (P_BINT pbi, const char * str);
_boolean pbuFPrintDecimalBint (FILE * fp,  P_BINT       pbi);
_boolean pbuPrintDecimalBint  (P_BINT pbi);
_boolean pbuSPrintDecimalBint (char * str, P_BINT       pbi);

#endif

