/*
 * Name:        main.c
 * Description: Portable big integer library for RSA test.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0525251834F0825251834L00095
 * License:     GPLv3.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pbi4rsax.h"

#define SZ_CIPHER "\xa1\x1c\xa1\x1c"
// #define SZ_CIPHER "\xa1\x1c"

int main()
{
	size_t i;
	FILE * fp;
	/* Two big prime number p and q. */
	P_BINT p, q;
	/* Public key and private key. */
	P_RSA_KEY pubk, pvtk;
	/* Cipher chain. */
	P_RSA_CIPHER prc;
	/* Text buffer. */
	unsigned char * psz;
	
	p = pbkCreateBint(61);
	q = pbkCreateBint(53);
	
	pubk = pbrCreateRSAKey();
	pvtk = pbrCreateRSAKey();
	
	pbrGetRSAKey(pubk, pvtk, p, q, 1);

	prc = pbrEncrypt(pubk, SZ_CIPHER, strlen(SZ_CIPHER));

	/* Save/load keys. */
	{
		fp = fopen("pvtk.bin", "wb");
		pbrSaveRSAKey(fp, pvtk);
		fclose(fp);

		fp = fopen("pubk.bin", "wb");
		pbrSaveRSAKey(fp, pubk);
		fclose(fp);

		pbrDeleteRSAKey(pubk);
		pbrDeleteRSAKey(pvtk);

		pubk = pbrCreateRSAKey();
		pvtk = pbrCreateRSAKey();

		fp = fopen("pvtk.bin", "rb");
		pbrLoadRSAKey(pvtk, fp);
		fclose(fp);

		fp = fopen("pubk.bin", "rb");
		pbrLoadRSAKey(pubk, fp);
		fclose(fp);
	}

	/* Save/load cipher. */
	{
		fp = fopen("cipher.bin", "wb");
		pbrSaveRSACipher(fp, prc);
		fclose(fp);

		pbrDestroyRSACipherChain(prc);

		fp = fopen("cipher.bin", "rb");
		pbrLoadRSACipher(&prc, fp);
		fclose(fp);
	}

	psz = pbrDecrypt(&i, prc, pvtk);

	pbrDestroyRSACipherChain(prc);
	
	pbkDeleteBint(p);
	pbkDeleteBint(q);
	
	pbrDeleteRSAKey(pubk);
	pbrDeleteRSAKey(pvtk);

	if (0 == memcmp(psz, SZ_CIPHER, i))
		printf("OK!\n");

	free(psz);
	
	return 0;
}
