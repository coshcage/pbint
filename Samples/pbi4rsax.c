/*
 * Name:        pbi4rsax.c
 * Description: Portable big integer library for RSA eXternal memory support.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0525251832J0825251846L00216
 * License:     GPLv3.
 */

#include "pbi4rsax.h"
#include <string.h> /* Using function memcpy, memcmp. */

#define pbrKeyMagic    "Rk" /* Magical number for RSA key. */
#define pbrCipherMagic "Rc" /* Magical number for RSA cipher. */

/* Function name: pbrLoadRSAKey
 * Description:   Load RSA key from a file.
 * Parameters:
 *         pk Pointer to a RSA key.
 *         fp Pointer to an opened file structure.
 * Return value:  Please reference to enumeration PBXERR.
 */
PBXERR pbrLoadRSAKey(P_RSA_KEY pk, FILE * fp)
{
	if (NULL != fp)
	{
		XRK xrk = { 0 };

		if (1 != fread(&xrk, sizeof(XRK), 1, fp))
			return PXE_BAD_FILE;

		if (memcmp(pbrKeyMagic, xrk.magic, sizeof(xrk.magic)) == 0)
		{
			if (0 == xrk.E) /* Private key. */
			{
				PBXERR r = pbxLoadBint(&pk->N, fp);
				if (PXE_NONE != r)
					return r;

				return pbxLoadBint(&pk->D, fp);
			}
			else /* Public key. */
				return pbxLoadBint(&pk->N, fp);

			return PXE_NONE;
		}
		return PXE_BAD_FORMAT;
	}
	return PXE_BAD_FILE;
}

/* Function name: pbrSaveRSAKey
 * Description:   Save a RSA key to a file.
 * Parameters:
 *         fp Pointer to an opened file structure.
 *         pk Pointer to a RSA key.
 * Return value:  Please reference to enumeration PBXERR.
 */
PBXERR pbrSaveRSAKey(FILE * fp, P_RSA_KEY pk)
{
	if (NULL != fp)
	{
		XRK xrk = { 0 };

		memcpy(xrk.magic, pbrKeyMagic, sizeof(xrk.magic));

		xrk.E = pk->E;
		fwrite(&xrk, sizeof(XRK), 1, fp);

		if (0 == pk->E) /* Private key. */
		{
			PBXERR r = pbxSaveBint(fp, &pk->N);
			if (PXE_NONE != r)
				return r;

			return pbxSaveBint(fp, &pk->D);
		}
		else /* Public key. */
			return pbxSaveBint(fp, &pk->N);
	}
	return PXE_BAD_FILE;
}

/* Function name: pbrLoadRSACipher
 * Description:   Load a RSA cipher chain from a file.
 * Parameters:
 *   pprchead Pointer to a P_RSA_CIPHER pointer which used to store chain header.
 *         fp Pointer to an opened file structure.
 * Return value:  Please reference to enumeration PBXERR.
 */
PBXERR pbrLoadRSACipher(P_RSA_CIPHER * pprchead, FILE * fp)
{
	if (NULL != fp)
	{
		_ub i = 0;
		XRC xrc = { 0 };

		if (1 != fread(&xrc, sizeof(XRC), 1, fp))
			return PXE_BAD_FILE;

		if (NULL != pprchead)
			*pprchead = NULL;

		if (memcmp(pbrCipherMagic, xrc.magic, sizeof(xrc.magic)) == 0)
		{
			_ub i;
			P_RSA_CIPHER   p;
			P_RSA_CIPHER * pp;

			for (pp = pprchead, i = 0; i < xrc.size; ++i)
			{
				p = pbrCreateRSACipher();

				if (NULL != pprchead && NULL == *pprchead)
					*pprchead = p;

				if (NULL != p)
					p->bilu = fgetc(fp);
				else
				{
					if (NULL != pprchead)
					{
						p = *pprchead;
						while (NULL != p)
						{
							register P_RSA_CIPHER pn = p->next;
							pbrDeleteRSACipher(p);
							p = pn;
						}
						*pprchead = NULL;
					}
					return PXE_ALLOCATION;
				}

				*pp = p;
				pp = &p->next;
			}

			for (p = *pprchead, i = 0; i < xrc.size; ++i)
			{
				PBXERR r = pbxLoadBint(&p->M, fp);
				pbkReallocBint(&p->M, GETABS(GETFLAG(&p->M)), false);
				if (PXE_NONE != r)
				{
					if (NULL != pprchead)
					{
						p = *pprchead;
						while (NULL != p)
						{
							register P_RSA_CIPHER pn = p->next;
							pbrDeleteRSACipher(p);
							p = pn;
						}
						*pprchead = NULL;
					}
					return r;
				}

				p = p->next;
			}
			return PXE_NONE;
		}
		return PXE_BAD_FORMAT;
	}
	return PXE_BAD_FILE;
}

/* Function name: pbrSaveRSACipher
 * Description:   Save a RSA cipher chain to a file.
 * Parameters:
 *         fp Pointer to an opened file structure.
 *    prchead Pointer to the chain header you want to save.
 * Return value:  Please reference to enumeration PBXERR.
 */
PBXERR pbrSaveRSACipher(FILE * fp, P_RSA_CIPHER prchead)
{
	if (NULL != fp)
	{
		_ub i = 0;
		XRC xrc = { 0 };
		P_RSA_CIPHER p = prchead;

		memcpy(xrc.magic, pbrCipherMagic, sizeof(xrc.magic));

		while (NULL != p)
		{
			++i;
			p = p->next;
		}
		
		xrc.size = i;
		fwrite(&xrc, sizeof(XRC), 1, fp);

		p = prchead;

		while (NULL != p)
		{
			fputc((int)p->bilu, fp);
			p = p->next;
		}

		p = prchead;

		while (NULL != p)
		{
			PBXERR r = pbxSaveBint(fp, &p->M);
			if (PXE_NONE != r)
				return r;

			p = p->next;
		}

		return PXE_NONE;
	}
	return PXE_BAD_FILE;
}
