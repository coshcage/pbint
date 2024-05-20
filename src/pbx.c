/* X mem. */
#include "pbx.h"
#include <string.h>

#define pbxMagic "Pb"

size_t _pbxGetPlatformSize(void);

size_t _pbxGetPlatformSize(void)
{
	return sizeof(_ub);
}

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
						return PE_NONE;
					}
					return PE_BAD_FILE;
				}
				return PE_ALLOCATION;
			}
			return PE_WRONG_PLATF;
		}
		return PE_BAD_FORMAT;
	}
	return PE_BAD_FILE;
}

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

			return PE_NONE;
		}
		return PE_ALLOCATION;
	}
	return PE_BAD_FILE;
}
