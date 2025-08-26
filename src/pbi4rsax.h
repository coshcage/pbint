/*
 * Name:        pbi4rsax.h
 * Description: Portable big integer library for RSA eXternal memory interface.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0825251835I0825251835L00036
 * License:     GPLv3.
 */

#ifndef _PBI4RSAX_H_
#define _PBI4RSAX_H_

#include "pbx.h"
#include "pbi4rsa.h"

/* Structure for external RSA key. */
typedef struct st_XRK
{
	char magic[2]; /* Magical number used to distinguish file types. */
	_ub  E;        /* Exponential number. 0 for private key. Non zero for public key. */
} XRK, * P_XRK;

/* Structure for external RSA cipher. */
typedef struct st_XRC
{
	char magic[2]; /* Magical number used to distinguish file types. */
	_ub  size;     /* Length of linked list. */
} XRC, * P_XRC;

/* Exported functions declared here. */
PBXERR pbrLoadRSAKey(P_RSA_KEY pk, FILE * fp);
PBXERR pbrSaveRSAKey(FILE * fp, P_RSA_KEY pk);
PBXERR pbrLoadRSACipher(P_RSA_CIPHER * pprchead, FILE * fp);
PBXERR pbrSaveRSACipher(FILE * fp, P_RSA_CIPHER prchead);

#endif
