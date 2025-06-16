/*
 * Name:        pbx.h
 * Description: Portable big integer library external memory support module.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0520240323E0520240324L00037
 * License:     GPLv3.
 */

#ifndef _PBX_H_
#define _PBX_H_

#include "pbk.h"
#include <stdio.h> /* Using structure FILE, function feof, fread, fwrite. */

typedef struct st_XBINT
{
	char magic[2]; /* Magical number used to distinguish file types. */
	_ib flag;      /* Big integer flag. */
	_ub pflg;      /* Platform integer size. */
	_ub incl;      /* Big integer incremental. */
} XBINT, * P_XBINT;

typedef enum en_PBXERR /* File operation function result. */
{
	PXE_NONE,        /* No error. */
	PXE_BAD_FILE,    /* Can not open file or file corrupted. */
	PXE_BAD_FORMAT,  /* Is not a big integer memory dump file. */
	PXE_WRONG_PLATF, /* Conflicting platform. */
	PXE_ALLOCATION   /* Memory error. */
} PBXERR;

/* Function declarations. */
PBXERR pbxLoadBint(P_BINT pbi, FILE * fp);
PBXERR pbxSaveBint(FILE * fp, P_BINT pbi);

#endif

