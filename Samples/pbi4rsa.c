/*
 * Name:        pbi4rsa.c
 * Description: Portable big integer library for RSA.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0525251833H0831251014L00717
 * License:     GPLv3.
 */

#include <limits.h> /* Using macro CHAR_BIT and INT_MAX. */
#include <stdlib.h> /* Using function malloc, free, srand and rand. */
#include <string.h> /* Using function memset. */
#include "pbi4rsa.h"

/* Unsigned size_t integer division. */
typedef struct _st_stdiv_t
{
	size_t quot; /* Quotient.  */
	size_t rem;  /* Remainder. */
} _stdiv_t;

/* File level function declarations. */
static _stdiv_t _stdiv            (size_t numerator, size_t denominator);
static _boolean _pbrFi            (P_BINT f, P_BINT p, P_BINT q);
static _ub      _pbrE             (P_BINT f, int seed);
static _boolean _pbrXGCD          (P_BINT or, P_BINT os, P_BINT ot, P_BINT a, P_BINT m);
static _boolean _pbrD             (P_BINT d, P_BINT e, P_BINT f);
static void     _pbkSecureFreeBint(P_BINT pbi);

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _stdiv
 * Description:   Compute a quotient that truncates toward 0 and the corresponding remainder.
 * Parameters:
 *  numerator Numerator.
 *denominator Denominator.
 * Return value:  A _stdiv_t structure which contains the result.
 */
static _stdiv_t _stdiv(size_t numerator, size_t denominator)
{
	_stdiv_t result;
	/* Just give me some optimization. */
	result.quot = numerator / denominator;
	result.rem  = numerator % denominator;
	return result;
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _pbkSecureFreeBint
 * Description:   Securely free a big integer.
 * Parameter:
 *       pbi Pointer to a big integer.
 * Return value:  N/A.
 */
static void _pbkSecureFreeBint(P_BINT pbi)
{
	register size_t i;
	for (i = GETABS(GETFLAG(pbi)); i > 0; --i)
	{
		((volatile _ub *)pbi->data)[i - 1] = 0; 
	}
	if (pbi->data)
		free(pbi->data);
	pbi->data = NULL;
	SETFLAG(pbi, 0);
	SETSIZE(pbi, 0U);	
}

/* Function name: pbrInitRSAKey
 * Description:   Initialize a RSA key.
 * Parameter:
 *        pk Pointer to the RSA key you want to initialize.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 */
_boolean pbrInitRSAKey(P_RSA_KEY pk)
{
	pk->E = 0;
	if (! pbkInitBint(&pk->N, 0))
		return FALSE;
	if (! pbkInitBint(&pk->D, 0))
	{
		pbkFreeBint(&pk->N);
		return FALSE;
	}
	return TRUE;
}

/* Function name: pbrFreeRSAKey
 * Description:   Free a RSA key.
 * Parameter:
 *        pk Pointer to the RSA key you want to free.
 * Return value:  N/A.
 */
void pbrFreeRSAKey(P_RSA_KEY pk)
{
	_pbkSecureFreeBint(&pk->N);
	_pbkSecureFreeBint(&pk->D);
}

/* Function name: pbrCreateRSAKey
 * Description:   Create a RSA key.
 * Parameter:     N/A.
 * Return value:  New generated RSA key structure.
 */
P_RSA_KEY pbrCreateRSAKey(void)
{
	P_RSA_KEY pk = (P_RSA_KEY) malloc(sizeof(RSA_KEY));
	if (NULL != pk)
	{
		memset(pk, 0, sizeof(RSA_KEY));
		if (pbrInitRSAKey(pk))
			return pk;
		else
			free(pk);
	}
	return NULL;
}

/* Function name: pbrDeleteRSAKey
 * Description:   Purge a RSA key.
 * Parameter:
 *        pk Pointer to the key you want to delete.
 * Return value:  N/A.
 */
void pbrDeleteRSAKey(P_RSA_KEY pk)
{
	pbrFreeRSAKey(pk);
	free(pk);
}

/* Function name: pbrInitRSACipher
 * Description:   Initialize a RSA cipher structure.
 * Parameter:
 *        pc Pointer to the RSA cipher structure you want to initialize.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 */
_boolean pbrInitRSACipher(P_RSA_CIPHER pc)
{
	pc->next = NULL;
	pc->bilu = 0;
	if (! pbkInitBint(&pc->M, 0))
		return FALSE;
	return TRUE;
}

/* Function name: pbrFreeRSAKey
 * Description:   Free a RSA cipher structure.
 * Parameter:
 *        pc Pointer to the RSA cipher structure you want to free.
 * Return value:  N/A.
 */
void pbrFreeRSACipher(P_RSA_CIPHER pc)
{
	_pbkSecureFreeBint(&pc->M);
}

/* Function name: pbrCreateRSACipher
 * Description:   Create a RSA cipher structure.
 * Parameter:     N/A.
 * Return value:  New generated RSA cipher structure.
 */
P_RSA_CIPHER pbrCreateRSACipher(void)
{
	P_RSA_CIPHER pc = (P_RSA_CIPHER) malloc(sizeof(RSA_CIPHER));
	if (NULL != pc)
	{
		memset(pc, 0, sizeof(RSA_CIPHER));
		if (pbrInitRSACipher(pc))
			return pc;
		else
			free(pc);
	}
	return NULL;
}

/* Function name: pbrDeleteRSACipher
 * Description:   Deallocate a RSA cipher structure.
 * Parameter:
 *        pc Pointer to the RSA cipher structure you want to delete.
 * Return value:  N/A.
 */
void pbrDeleteRSACipher(P_RSA_CIPHER pc)
{
	pbrFreeRSACipher(pc);
	free(pc);
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _pbrFi
 * Description:   Compute the Eula function Phi.
 * Parameters:
 *          f Stores the result.
 *          p Big prime number p.
 *          q Big prime number q.
 * Return value:  TRUE  Compute succeeded.
 *                FALSE Compute failed.
 * Tip:           Phi/Fi(N) = (p - 1) * (q - 1)
 */
static _boolean _pbrFi(P_BINT f, P_BINT p, P_BINT q)
{
	_ub ss = 1;
	BINT S = { 1, 1, 0, &ss };
	
	P_BINT pr, qr;
	
	pr = pbkCreateBint(0);
	qr = pbkCreateBint(0);
	
	if (! pbkSubtractBint(pr, p, &S))
		goto Lbl_Error;

	if (! pbkSubtractBint(qr, q, &S))
		goto Lbl_Error;
	
	if (! pbkMultiplyBint(f, pr, qr))
		goto Lbl_Error;
	
	pbkDeleteBint(pr);
	pbkDeleteBint(qr);
	
	return TRUE;

Lbl_Error:
	pbkDeleteBint(pr);
	pbkDeleteBint(qr);
	return FALSE;
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _pbrE
 * Description:   Compute the exponent value of public key.
 * Parameters:
 *          f Pointer to Fi(N).
 *       seed RNG seed.
 * Return value:  Exponent value.
 */
static _ub _pbrE(P_BINT f, int seed)
{
	register _ub re;
	int max;
	P_BINT r;
	BINT E = { 0 }, F = { 0 };
	
	srand(seed);
	
	r = pbkCreateBint(0);
	
	if (pbkIsNotANumber(f))
		return 0;
	
	max = *f->data;
	
	if (GETABS(GETFLAG(f)) > 1)
		max = INT_MAX;

	if (max > RAND_MAX)
		max = RAND_MAX;
	
	pbkInitBint(&E, re = 1 + rand() / ((RAND_MAX + 1u) / max));
	pbkReallocBint(&E, GETABS(GETFLAG(&E)), FALSE);
	pbkInitBint(&F, 0);
	pbkMoveBint(&F, f);
	
	do
	{
		if (++re >= (_ub)max)
			re = 2;
		
		if (! pbkMoveBint(&F, f))
			goto Lbl_Error;
		
		*E.data = re;
		SETFLAG(&E, 1);
		
		if (! pbmBintGreatestCommonDivisor(r, &E, &F))
			goto Lbl_Error;
	}
	while (! (1 == GETFLAG(r) && 1 == *r->data));
	
	pbkFreeBint(&E);
	pbkFreeBint(&F);
	pbkDeleteBint(r);
	
	return re;
	
Lbl_Error:
	pbkFreeBint(&E);
	pbkFreeBint(&F);
	pbkDeleteBint(r);
	return 0;
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _pbrXGCD
 * Description:   Compute D candidate by extended Euclidean algorithm.
 * Parameters:
 *         or Stores the result of GCD.
 *         os Stores D candidate.
 *         ot Big number t is usually useless.
 *          a GCD(a, b);
 *          b GCD(a, b);
 * Return value:  TRUE  Compute succeeded.
 *                FALSE Compute failed.
 */
static _boolean _pbrXGCD(P_BINT or, P_BINT os, P_BINT ot, P_BINT a, P_BINT b)
{
	BINT R  = { 0 };
	BINT S  = { 0 };
	BINT T  = { 0 };
	BINT Q  = { 0 };
	BINT U  = { 0 };
	BINT V  = { 0 };
	BINT W  = { 0 };

	pbkInitBint(&R, 0);
	pbkReallocBint(&R, GETABS(GETFLAG(b)), FALSE);
	pbkInitBint(&S, 0);
	pbkReallocBint(&S, GETABS(GETFLAG(b)), FALSE);
	pbkInitBint(&T, 1);
	pbkReallocBint(&T, GETABS(GETFLAG(b)), FALSE);
	pbkInitBint(&Q, 0);
	pbkReallocBint(&Q, GETABS(GETFLAG(b)), FALSE);
	pbkInitBint(&U, 0);
	pbkReallocBint(&U, GETABS(GETFLAG(b)), FALSE);
	pbkInitBint(&V, 0);
	pbkReallocBint(&V, GETABS(GETFLAG(b)), FALSE);
	pbkInitBint(&W, 0);
	pbkReallocBint(&W, GETABS(GETFLAG(b)), FALSE);
	
	pbkMoveBint(&R, b);
	pbkMoveBint(or, a);
	
	if (! pbkIsNotANumber(os))
	{
		*os->data = 1;
		SETFLAG(os, 1);
	}
	else
		goto Lbl_Error;
	
	if (! pbkIsNotANumber(ot))
	{
		*ot->data = 0;
		SETFLAG(ot, 1);
	}
	else
		goto Lbl_Error;
		
	while (! pbkIsBintEqualToZero(&R))
	{
		if (! pbkDivideBint(&Q, NULL, or, &R))
			goto Lbl_Error;
		
		if (! pbkMoveBint(&W, &R))
			goto Lbl_Error;
		if (! pbkMultiplyBint(&U, &Q, &R))
			goto Lbl_Error;
		if (! pbkSubtractBint(&V, or, &U))
			goto Lbl_Error;
		if (! pbkMoveBint(&R, &V))
			goto Lbl_Error;
		if (! pbkMoveBint(or, &W))
			goto Lbl_Error;
		
		if (! pbkMoveBint(&W, &S))
			goto Lbl_Error;
		if (! pbkMultiplyBint(&U, &Q, &S))
			goto Lbl_Error;
		if (! pbkSubtractBint(&V, os, &U))
			goto Lbl_Error;
		if (! pbkMoveBint(&S, &V))
			goto Lbl_Error;
		if (! pbkMoveBint(os, &W))
			goto Lbl_Error;
		
		if (! pbkMoveBint(&W, &T))
			goto Lbl_Error;
		if (! pbkMultiplyBint(&U, &Q, &T))
			goto Lbl_Error;
		if (! pbkSubtractBint(&V, ot, &U))
			goto Lbl_Error;
		if (! pbkMoveBint(&T, &V))
			goto Lbl_Error;
		if (! pbkMoveBint(ot, &W))
			goto Lbl_Error;
	}
	
	pbkFreeBint(&R);
	pbkFreeBint(&S);
	pbkFreeBint(&T);
	pbkFreeBint(&Q);
	pbkFreeBint(&U);
	pbkFreeBint(&V);
	pbkFreeBint(&W);
	return TRUE;
	
Lbl_Error:
	pbkFreeBint(&R);
	pbkFreeBint(&S);
	pbkFreeBint(&T);
	pbkFreeBint(&Q);
	pbkFreeBint(&U);
	pbkFreeBint(&V);
	pbkFreeBint(&W);
	return FALSE;
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _pbrD
 * Description:   Compute D.
 * Parameters:
 *          d Stores the result.
 *          e Stores exponent value.
 *          b Stores Phi(N).
 * Return value:  TRUE  Compute succeeded.
 *                FALSE Compute failed.
 * Tip:           (e * d) mod Phi(N) == 1;
 */
static _boolean _pbrD(P_BINT d, P_BINT e, P_BINT f)
{
	BINT G = { 0 };
	BINT T = { 0 };
	
	pbkInitBint(&G, 0);
	pbkReallocBint(&G, GETABS(GETFLAG(f)), FALSE);
	pbkInitBint(&T, 0);
	pbkReallocBint(&T, GETABS(GETFLAG(f)), FALSE);
	
	if (! _pbrXGCD(&G, d, &T, e, f))
		goto Lbl_Error;
	
	if (! (1 == GETABS(GETFLAG(&G)) && 1 == *G.data))
		goto Lbl_Error;
		
	if (! pbkDivideBint(NULL, &T, d, f))
		goto Lbl_Error;
	if (! pbkAddBint(&G, &T, f))
		goto Lbl_Error;
	if (! pbkDivideBint(NULL, &T, &G, f))
		goto Lbl_Error;
	if (! pbkMoveBint(d, &T))
		goto Lbl_Error;
	
	pbkFreeBint(&G);
	pbkFreeBint(&T);
	return TRUE;
	
Lbl_Error:
	pbkFreeBint(&G);
	pbkFreeBint(&T);
	return FALSE;
}

/* Function name: pbrGetRSAKey
 * Description:   Get the RSA public and private keys.
 * Parameters:
 *       pubk Pointer to the outputted public key.
 *       pubk Pointer to the outputted private key.
 *          p Pointer to the big prime integer p.
 *          q Pointer to the big prime integer q.
 *       seed RNG seed for calculating exponent value.
 * Return value:  TRUE  Fetch succeeded.
 *                FALSE Fetch failed.
 * Tip:           Users shall use function pbrCreateRSAKey to allocate keys.
 *                Public key:  (e, N);
 *                Private key: (d, N);
 */
_boolean pbrGetRSAKey(P_RSA_KEY pubk, P_RSA_KEY pvtk, P_BINT p, P_BINT q, int seed)
{
	BINT N = { 0 };
	BINT F = { 0 };
	BINT D = { 0 };
	BINT E = { 0 };
	
	pbkInitBint(&N, 0);
	pbkInitBint(&F, 0);
	pbkInitBint(&D, 0);
	pbkInitBint(&E, 0);
	
	if (! pbkMultiplyBint(&N, p, q))
		goto Lbl_Error;

	pbkReallocBint(&E, GETABS(GETFLAG(&N)), FALSE);
	pbkReallocBint(&F, GETABS(GETFLAG(&N)), FALSE);
	pbkReallocBint(&D, GETABS(GETFLAG(&N)), FALSE);
	
	if (! _pbrFi(&F, p, q))
		goto Lbl_Error;
		
	pubk->E = _pbrE(&F, seed);
	pvtk->E = 0;
	
	E.data[0] = pubk->E;
	SETFLAG(&E, 1);
	
	if (! _pbrD(&D, &E, &F))
		goto Lbl_Error;
	
	pbkMoveBint(&pubk->N, &N);
	pbkMoveBint(&pvtk->N, &N);
	
	pbkMoveBint(&pvtk->D, &D);
	
	pbkFreeBint(&N);
	pbkFreeBint(&F);
	pbkFreeBint(&D);
	pbkFreeBint(&E);

	pbkReallocBint(&pubk->D, GETABS(GETFLAG(&pubk->D)), FALSE);
	pbkReallocBint(&pubk->N, GETABS(GETFLAG(&pubk->N)), FALSE);
	
	return TRUE;

Lbl_Error:
	pbkFreeBint(&N);
	pbkFreeBint(&F);
	pbkFreeBint(&D);
	pbkFreeBint(&E);
	
	return FALSE;
}

/* Function name: pbrEncrypt
 * Description:   RSA encryption algorithm.
 * Parameter:
 *       prc Pointer to the new allocated cipher chain.
 * Return value:  N/A.
 */
void pbrDestroyRSACipherChain(P_RSA_CIPHER prc)
{
	register P_RSA_CIPHER ppp = prc;
	while (NULL != ppp)
	{
		register P_RSA_CIPHER p = ppp->next;
		pbrDeleteRSACipher(ppp);
		ppp = p;
	}
}

/* Function name: pbrEncrypt
 * Description:   RSA encryption algorithm.
 * Parameters:
 *       pubk Pointer to the public key.
 *          s Pointer to the buffer you want to encrypt.
 *        len The length of the input buffer.
 * Return value:  Pointer to the header of a RSA cipher chain which is newly allocated.
 * Tip:           Users shall use pbrDestroyRSACipherChain(pbrEncrypt(pubk, s, len)); to deallocate the returned chain.
 *                C = (M ^ e) mod N;
 */
P_RSA_CIPHER pbrEncrypt(P_RSA_KEY pubk, unsigned char * s, size_t len)
{
	register size_t i, j, k, l;
	register unsigned char c = 0;
	register _boolean bcl = FALSE; /* Boolean c last. */
	register unsigned char * ps = s;
	_stdiv_t z = _stdiv(len, sizeof(_ub));
	
	P_RSA_CIPHER prc = pbrCreateRSACipher();
	P_RSA_CIPHER prr = pbrCreateRSACipher();
	P_RSA_CIPHER phd = prc, ppp = prc, rrp = prr;
	
	/* Shrink capacity. */
	pbkReallocBint(&prc->M, GETABS(GETFLAG(&pubk->N)), FALSE);
	pbkReallocBint(&prr->M, GETABS(GETFLAG(&pubk->N)), FALSE);
	
	for (k = i = 0; i < z.quot; ++i, ++k)
	{
		for (l = j = 0; j < sizeof(_ub) || bcl; ++j, ++l)
		{
			if (bcl)
			{
				l = 1;
				prc->bilu = 1;
				prc->M.data[k] = 0;
				prc->M.data[k] |= c;
				SETFLAG(&prc->M, (_ib)(k + 1));
				bcl = FALSE;
			}

			if (ps - s < (ptrdiff_t)len)
			{
				prc->bilu = l + 1;
				c = *ps++;
				prc->M.data[k] |= (c << (CHAR_BIT * l));
				if (pbkCompareBint(&pubk->N, &prc->M) <= 0) /* M > N. */
				{
					--prc->bilu;
					prc->M.data[k] ^= (c << (CHAR_BIT * l));
					prc->next = pbrCreateRSACipher();
					prc = prc->next;
					pbkReallocBint(&prc->M, GETABS(GETFLAG(&pubk->N)), FALSE);
					bcl = TRUE;
					k = 0;
					continue;
				}
			}
		}
	}
	
	for (l = j = 0; j < z.rem || bcl; ++j, ++l)
	{
		if (bcl)
		{
			l = 1;
			prc->bilu = 1;
			prc->M.data[k] = 0;
			prc->M.data[k] |= c;
			SETFLAG(&prc->M, (_ib)(k + 1));
			bcl = FALSE;
		}

		if (ps - s < (ptrdiff_t)len)
		{
			prc->bilu = l + 1;
			c = *ps++;
			prc->M.data[k] |= (c << (CHAR_BIT * l));
			if (pbkCompareBint(&pubk->N, &prc->M) <= 0) /* M > N. */
			{
				--prc->bilu;
				prc->M.data[k] ^= (c << (CHAR_BIT * l));
				prc->next = pbrCreateRSACipher();
				prc = prc->next;
				pbkReallocBint(&prc->M, GETABS(GETFLAG(&pubk->N)), FALSE);
				bcl = TRUE;
				k = 0;
				continue;
			}
		}
	}
	
	/* Verbose: prc->bilu = z.rem; */
	
	while (NULL != ppp)
	{
		prr->bilu = ppp->bilu;
		pbmBintExponentialModule(&prr->M, &ppp->M, pubk->E, &pubk->N);
		pbkReallocBint(&prr->M, GETABS(GETFLAG(&pubk->N)), FALSE);

		if (NULL != ppp->next)
		{
			prr->next = pbrCreateRSACipher();
			prr = prr->next;
			pbkReallocBint(&prr->M, GETABS(GETFLAG(&pubk->N)), FALSE);
		}
		ppp = ppp->next;
	}
	
	pbrDestroyRSACipherChain(phd);
	
	return rrp;
}

/* Function name: pbrDecrypt
 * Description:   RSA decryption algorithm.
 * Parameters:
 *       plen Pointer to a size_t integer that is used to store the length of returned buffer.
 *        prc Pointer to the header of a RSA cipher chain.
 *       pvtk Pointer to the private key.
 * Return value:  Pointer to a buffer that stores the text.
 * Tip:           Users shall use free(pbrDecrypt(&l, prc, pvtk)); to deallocate the returned buffer.
 *                M = (C ^ d) mod N;
 */
unsigned char * pbrDecrypt(size_t * plen, P_RSA_CIPHER  prc, P_RSA_KEY pvtk)
{
	BINT M = { 0 };	
	
	register size_t i, j;
	
	size_t len = 0;
	
	unsigned char * str, * psz;
	
	P_RSA_CIPHER ppp = prc;
	
	pbkInitBint(&M, 0);
	pbkReallocBint(&M, GETABS(GETFLAG(&pvtk->N)), FALSE);

	while (NULL != ppp)
	{
		if (GETABS(GETFLAG(&ppp->M)) < 1)
			len += (ppp->bilu);
		else
			len += (GETABS(GETFLAG(&ppp->M)) - 1) * sizeof(_ub) + ppp->bilu;
		ppp = ppp->next;
	}
	
	psz = str = (unsigned char *) malloc(len);
	
	if (NULL != str)
	{
		ppp = prc;
		
		while (NULL != ppp)
		{
			pbmBintExponentialModule(&M, &ppp->M, pvtk->D.data[0], &pvtk->N);
			pbkReallocBint(&M, GETABS(GETFLAG(&pvtk->N)), FALSE);
			
			for (i = 0; i < GETABS(GETFLAG(&M)); ++i)
			{
				for (j = 0; j < ppp->bilu; ++j)
				{
					*(psz++) = (unsigned char)(M.data[i] >> (j * CHAR_BIT));
				}
			}
			
			ppp = ppp->next;
		}
	}
	
	pbkFreeBint(&M);
	
	if (NULL != plen)
		*plen = len;
	
	return str;
}

