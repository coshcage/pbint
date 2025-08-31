/*
 * Name:        pbi4rsa.h
 * Description: Portable big integer library for RSA interface.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0825251836G0825251836L00045
 * License:     GPLv3.
 */

#ifndef _PBI4RSA_H_
#define _PBI4RSA_H_

#include "pbm.h"

/* Structure for private and public key. */
typedef struct st_RSA_KEY
{
	_ub  E; /* Exponent. */
	BINT N; /* Multiply for p and q. */
	BINT D; /* Private key */
} RSA_KEY, * P_RSA_KEY;

/* Structure for cipher chain. */
typedef struct st_RSA_CIPHER
{
	struct st_RSA_CIPHER * next;
	size_t bilu; /* Byte in the last unsigned integer. */
	BINT M;      /* Cipher big integer. */
} RSA_CIPHER, * P_RSA_CIPHER;

/* Exported functions are listed here. */
_boolean        pbrInitRSAKey            (P_RSA_KEY pk);
void            pbrFreeRSAKey            (P_RSA_KEY pk);
P_RSA_KEY       pbrCreateRSAKey          (void);
void            pbrDeleteRSAKey          (P_RSA_KEY pk);
_boolean        pbrInitRSACipher         (P_RSA_CIPHER pc);
void            pbrFreeRSACipher         (P_RSA_CIPHER pc);
P_RSA_CIPHER    pbrCreateRSACipher       (void);
void            pbrDeleteRSACipher       (P_RSA_CIPHER pc);
_boolean        pbrGetRSAKey             (P_RSA_KEY pubk, P_RSA_KEY pvtk, P_BINT p, P_BINT q, int seed);
void            pbrDestroyRSACipherChain (P_RSA_CIPHER prc);
P_RSA_CIPHER    pbrEncrypt               (P_RSA_KEY pubk, unsigned char * s, size_t len);
unsigned char * pbrDecrypt               (size_t * plen, P_RSA_CIPHER  prc, P_RSA_KEY prvk);

#endif

