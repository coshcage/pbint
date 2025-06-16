/*
 * Name:        pbx.c
 * Description: Portable big integer library eXternal memory support.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0520240323F0520240555L00099
 * License:     GPLv3.
 */

#include "pbx.h"
#include <string.h>

#define pbxMagic "Pb" /* Magical number. */

static _ub _pbxGetPlatformSize(void);

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _pbxGetPlatformSize
 * Description:   Return sizeof(_ub).
 * Parameters:    N/A.
 * Return value:  sizeof(_ub).
 * Tip:           To inline this function is a better choice.
 */
static _ub _pbxGetPlatformSize(void)
{
	return sizeof(_ub);
}

/* Function name: pbxLoadBint
 * Description:   Load a big integer from a file.
 * Parameters:
 *        pbi Pointer to a big integer.
 *         fp Pointer to an opened file structure.
 * Return value:  Please reference to enumeration PBXERR.
 * Tip:           Big integer structure shall be initialized to 0.
 */
PBXERR pbxLoadBint(P_BINT pbi, FILE * fp)
{
	if (NULL != fp)
	{
		XBINT xb = { 0 };
		fread(&xb, sizeof(XBINT), 1, fp);
		if (memcmp(pbxMagic, xb.magic, sizeof(xb.magic)) == 0)
		{
			if (_pbxGetPlatformSize() == xb.pflg)
			{
				if (pbkReallocBint(pbi, (_ub)GETABS(xb.flag), TRUE))
				{
					register size_t i = 0;
					while (!feof(fp))
						fread(&pbi->data[i++], sizeof(_ub), 1, fp);
					if (i == GETABS(xb.flag))
					{
						SETFLAG(pbi, xb.flag);
						SETINCL(pbi, xb.incl);
						return PXE_NONE;
					}
					return PXE_BAD_FILE;
				}
				return PXE_ALLOCATION;
			}
			return PXE_WRONG_PLATF;
		}
		return PXE_BAD_FORMAT;
	}
	return PXE_BAD_FILE;
}

 /* Function name: pbxSaveBint
  * Description:   Save a big integer to a file.
  * Parameters:
  *         fp Pointer to an opened file structure.
  *        pbi Pointer to a big integer.
  * Return value:  Please reference to enumeration PBXERR.
  */
PBXERR pbxSaveBint(FILE * fp, P_BINT pbi)
{
	if (NULL != fp)
	{
		if (NULL != pbi)
		{
			register size_t i, j;
			XBINT xb;
			memcpy(xb.magic, pbxMagic, sizeof(xb.magic));
			xb.flag = GETFLAG(pbi);
			xb.incl = GETINCL(pbi);
			xb.pflg = _pbxGetPlatformSize();

			fwrite(&xb, sizeof(XBINT), 1, fp);
			j = GETABS(GETFLAG(pbi));
			for (i = 0; i < j; ++i)
				fwrite(pbi->data + i, sizeof(_ub), 1, fp);

			return PXE_NONE;
		}
		return PXE_ALLOCATION;
	}
	return PXE_BAD_FILE;
}

