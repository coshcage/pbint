
#ifndef _PBX_H_
#define _PBX_H_

#include "pbk.h"
#include <stdio.h> /* Using structure FILE. */

typedef struct st_XBINT
{
	char magic[2];
	_ib flag;
	_ub pflg;
	_ub incl;
} XBINT, * P_XBINT;

typedef enum en_PBXERR /* File operation function result. */
{
	PE_NONE,        /* No error. */
	PE_BAD_FILE,    /* Can not open file. */
	PE_BAD_FORMAT,  /* Is not a big integer memory dump file. */
	PE_WRONG_PLATF, /* Conflicting platform. */
	PE_ALLOCATION
} PBXERR;

PBXERR pbxLoadBint(P_BINT pbi, FILE * fp);
PBXERR pbxSaveBint(FILE * fp, P_BINT pbi);

#endif
