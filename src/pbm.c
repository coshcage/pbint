/*
 * Name:        pbm.c
 * Description: Portable big integer library mathematics module.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0520240323D0917251700L00633
 * License:     GPLv3.
 */

#include "pbm.h"
#include <stdlib.h>

#define RAND_1_TIME(start, end) ((start) + rand() / (((_udb)RAND_MAX + 1) / (end)))

#define PTSIZ 169
unsigned short gPrimeTable1K[PTSIZ] =
{
	  0,   2,   3,   5,   7,  11,  13,  17,  19,  23,
	 29,  31,  37,  41,  43,  47,  53,  59,  61,  67,
	 71,  73,  79,  83,  89,  97, 101, 103, 107, 109,
	113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
	173, 179, 181, 191, 193, 197, 199, 211, 223, 227,
	229, 233, 239, 241, 251, 257, 263, 269, 271, 277,
	281, 283, 293, 307, 311, 313, 317, 331, 337, 347,
	349, 353, 359, 367, 373, 379, 383, 389, 397, 401,
	409, 419, 421, 431, 433, 439, 443, 449, 457, 461,
	463, 467, 479, 487, 491, 499, 503, 509, 521, 523,
	541, 547, 557, 563, 569, 571, 577, 587, 593, 599,
	601, 607, 613, 617, 619, 631, 641, 643, 647, 653,
	659, 661, 673, 677, 683, 691, 701, 709, 719, 727,
	733, 739, 743, 751, 757, 761, 769, 773, 787, 797,
	809, 811, 821, 823, 827, 829, 839, 853, 857, 859,
	863, 877, 881, 883, 887, 907, 911, 919, 929, 937,
	941, 947, 953, 967, 971, 977, 983, 991, 997
};

/* File level function declaration. */
_ub      _pbmRand1Block    (void);
_boolean _pbkIsPrimePrimary(P_BINT n);

/* Function name: pbmBintPower
 * Description:   Calculates the power of a big integer.
 * Parameters:
 *          r Pointer to a big integer that is the result.
 *          a Pointer to a big integer.
 *          n Power of n.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 * Tip:           r := a ^ n; (r := power(a, n);)
 *                This function uses quick power algorithm.
 */
_boolean pbmBintPower(P_BINT r, P_BINT a, _ub n)
{
	BINT A = { 0 }, T = { 0 }, R = { 0 };
	if (pbkIsNotANumber(a) || pbkIsNotANumber(r))
		return FALSE;
	else
	{
		SETFLAG(r, 1);
		r->data[0] = 1;

		pbkInitBint(&R, 0);
		pbkInitBint(&T, 0);
		pbkMoveBint(&A, a);

		while (n)
		{
			if (n & 1)
			{
				if (!pbkMoveBint(&R, r))
					goto Lbl_Failed;
				if (!pbkMultiplyBint(r, &R, &A))
					goto Lbl_Failed;
			}

			if (!pbkMultiplyBint(&T, &A, &A))
				goto Lbl_Failed;

			if (!pbkMoveBint(&A, &T))
				goto Lbl_Failed;

			n >>= 1;
		}

		pbkFreeBint(&A);
		pbkFreeBint(&T);
		pbkFreeBint(&R);

		return TRUE;
	}
Lbl_Failed:
	pbkFreeBint(&A);
	pbkFreeBint(&T);
	pbkFreeBint(&R);

	return FALSE;
}

/* Function name: pbmBintPowerBint
 * Description:   Calculates the power of a big integer.
 * Parameters:
 *          r Pointer to a big integer that is the result.
 *          a Pointer to a big integer.
 *          n Power of n.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 * Caution:       Big integer parameter n will change to value zero after invoking.
 * Tip:           r := a ^ n; (r := power(a, n);)
 *                This function uses quick power algorithm.
 */
_boolean pbmBintPowerBint(P_BINT r, P_BINT a, P_BINT n)
{
	BINT A = { 0 }, T = { 0 }, R = { 0 };
	if (pbkIsNotANumber(a) || pbkIsNotANumber(r))
		return FALSE;
	else
	{
		SETFLAG(r, 1);
		r->data[0] = 1;

		pbkInitBint(&R, 0);
		pbkInitBint(&T, 0);
		pbkMoveBint(&A, a);

		while (! pbkIsBintEqualToZero(n))
		{
			if (n->data[0] & 1)
			{
				if (!pbkMoveBint(&R, r))
					goto Lbl_Failed;
				if (!pbkMultiplyBint(r, &R, &A))
					goto Lbl_Failed;
			}

			if (!pbkMultiplyBint(&T, &A, &A))
				goto Lbl_Failed;

			if (!pbkMoveBint(&A, &T))
				goto Lbl_Failed;

			if (!pbkRightShiftBint(n, 0, 1))
				goto Lbl_Failed;
		}

		pbkFreeBint(&A);
		pbkFreeBint(&T);
		pbkFreeBint(&R);

		return TRUE;
	}
Lbl_Failed:
	pbkFreeBint(&A);
	pbkFreeBint(&T);
	pbkFreeBint(&R);

	return FALSE;
}

/* Function name: pbmBintExponentialModule
 * Description:   Calculates exp mod.
 * Parameters:
 *          r Pointer to a big integer that is the result.
 *          a Pointer to a big integer.
 *          n Power of n.
 *          m Pointer to a big integer.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 * Tip:           r := a ^ n mod m.
 *                This function uses quick power algorithm.
 */
_boolean pbmBintExponentialModule(P_BINT r, P_BINT a, _ub n, P_BINT m)
{
	BINT R = { 0 }, T = { 0 };
	if (pbkIsNotANumber(a) || pbkIsNotANumber(m))
		return FALSE;
	else
	{
		pbkInitBint(&R, 0);
		pbkInitBint(&T, 0);

		pbkDivideBint(NULL, &R, a, m);
		pbkMoveBint(a, &R);

		SETFLAG(r, 1);
		r->data[0] = 1;

		while (n)
		{
			if (n & 1)
			{
				if (!pbkMultiplyBint(&R, r, a))
					goto Lbl_Failed;
				if (!pbkDivideBint(NULL, &T, &R, m))
					goto Lbl_Failed;
				if (!pbkMoveBint(r, &T))
					goto Lbl_Failed;
			}

			if (!pbkMultiplyBint(&T, a, a))
				goto Lbl_Failed;
			if (!pbkDivideBint(NULL, a, &T, m))
				goto Lbl_Failed;

			n >>= 1;
		}

		pbkFreeBint(&R);
		pbkFreeBint(&T);

		return TRUE;
	}
Lbl_Failed:
	pbkFreeBint(&R);
	pbkFreeBint(&T);

	return FALSE;
}

/* Function name: pbmBintExponentialModuleBint
 * Description:   Calculates exp mod.
 * Parameters:
 *          r Pointer to a big integer that is the result.
 *          a Pointer to a big integer.
 *          n Power of n.
 *          m Pointer to a big integer.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 * Tip:           r := a ^ n mod m.
 *                This function uses quick power algorithm.
 * Caution:       n will change after invoking,
 */
_boolean pbmBintExponentialModuleBint(P_BINT r, P_BINT a, P_BINT n, P_BINT m)
{
	BINT R = { 0 }, T = { 0 };
	if (pbkIsNotANumber(a) || pbkIsNotANumber(m))
		return FALSE;
	else
	{
		pbkInitBint(&R, 0);
		pbkInitBint(&T, 0);

		pbkDivideBint(NULL, &R, a, m);
		pbkMoveBint(a, &R);

		SETFLAG(r, 1);
		r->data[0] = 1;

		while (! pbkIsBintEqualToZero(n))
		{
			if (n->data[0] & 1)
			{
				if (!pbkMultiplyBint(&R, r, a))
					goto Lbl_Failed;
				if (!pbkDivideBint(NULL, &T, &R, m))
					goto Lbl_Failed;
				if (!pbkMoveBint(r, &T))
					goto Lbl_Failed;
			}

			if (!pbkMultiplyBint(&T, a, a))
				goto Lbl_Failed;
			if (!pbkDivideBint(NULL, a, &T, m))
				goto Lbl_Failed;

			if (!pbkRightShiftBint(n, 0, 1))
				goto Lbl_Failed;
		}

		pbkFreeBint(&R);
		pbkFreeBint(&T);

		return TRUE;
	}
Lbl_Failed:
	pbkFreeBint(&R);
	pbkFreeBint(&T);

	return FALSE;
}

/* Function name: pbmUbFactorial
 * Description:   Factorial of an integer.
 * Parameters:
 *          r Pointer to a big integer to store the result.
 *          n fact(n).
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 * Tip:           Big value of n is slow to solve.
 */
_boolean pbmUbFactorial(P_BINT r, _ub n)
{
	BINT R = { 0 }, N = { 0 };

	pbkInitBint(&R, 0);
	pbkInitBint(&N, 0);

	SETFLAG(r, 1);
	r->data[0] = 1;

	SETFLAG(&N, 1);

	while (n - 1)
	{
		if (!pbkMoveBint(&R, r))
		{
			pbkFreeBint(&R);
			pbkFreeBint(&N);
			return FALSE;
		}
		N.data[0] = n;
		if (!pbkMultiplyBint(r, &N, &R))
		{
			pbkFreeBint(&R);
			pbkFreeBint(&N);
			return FALSE;
		}
		--n;
	}

	pbkFreeBint(&R);
	pbkFreeBint(&N);
	return TRUE;
}

/* Function name: pbmBintSquareRoot
 * Description:   Gets the square root of a big integer.
 * Parameters:
 *          r Pointer to a big integer to store result.
 *          n sqrt(n).
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 */
_boolean pbmBintSquareRoot(P_BINT r, P_BINT n)
{
	BINT * xk = r, XK1 = { 0 }, T = { 0 }, X = { 0 };
	if (pbkIsNotANumber(n))
		return FALSE;
	else
	{
		pbkInitBint(&T, 0);
		pbkInitBint(&X, 0);
		pbkMoveBint(&XK1, n);

		do
		{
			if (!pbkMoveBint(xk, &XK1))
				goto Lbl_Failure;
			if (!pbkDivideBint(&X, &T, n, xk))
				goto Lbl_Failure;
			if (!pbkAddBint(&XK1, &X, xk))
				goto Lbl_Failure;
			if (!pbkRightShiftBint(&XK1, 0, 1))
				goto Lbl_Failure;
		}
		while (pbkCompareBint(&XK1, xk) < 0);
		pbkFreeBint(&XK1);
		pbkFreeBint(&T);
		pbkFreeBint(&X);
		return TRUE;
	}
Lbl_Failure:
	pbkFreeBint(&XK1);
	pbkFreeBint(&T);
	pbkFreeBint(&X);
	return FALSE;
}

/* Function name: pbmBintGreatestCommonDivisor
 * Description:   Gets the greatest common divisor of two big integers.
 * Parameters:
 *          r Pointer to a big integer to store result.
 *          a Pointer to a big integer.
 *          b Pointer to a big integer.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 * Caution:       After invoking, the value of a and b are both CHANGED!
 * Tip:           This function uses Euclidian algorithm.
 */
_boolean pbmBintGreatestCommonDivisor(P_BINT r, P_BINT a, P_BINT b)
{
	if (pbkIsNotANumber(a) || pbkIsNotANumber(b) || pbkIsNotANumber(r))
		return FALSE;
	else
	{
		while (!pbkIsBintEqualToZero(b))
		{
			if (!pbkDivideBint(NULL, r, a, b))
				goto Lbl_Failed;

			if (!pbkMoveBint(a, b))
				goto Lbl_Failed;

			if (!pbkMoveBint(b, r))
				goto Lbl_Failed;
		}
		if (!pbkMoveBint(r, a))
			goto Lbl_Failed;

		return TRUE;
	}
Lbl_Failed:
	return FALSE;
}

/* Function name: pbmBintLeastCommonMultiple
 * Description:   Gets the least common multiple of two big integers.
 * Parameters:
 *          r Pointer to a big integer to store result.
 *          a Pointer to a big integer.
 *          b Pointer to a big integer.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 */
_boolean pbmBintLeastCommonMultiple(P_BINT r, P_BINT a, P_BINT b)
{
	BINT T = {0}, U = {0};
	
	if (pbkIsNotANumber(a) || pbkIsNotANumber(b) || pbkIsNotANumber(r))
		return FALSE;
	else
	{
		pbkInitBint(&T, 0);
		pbkInitBint(&U, 0);
		
		if (!pbkMultiplyBint(&U, a, b))
			goto Lbl_Failed;
		
		if (!pbmBintGreatestCommonDivisor(&T, a, b))
			goto Lbl_Failed;
		
		if (!pbkIsBintEqualToZero(&T))
		{
			if (!pbkDivideBint(r, NULL, &U, &T))
				goto Lbl_Failed;
		}
		else
			goto Lbl_Failed;
		
		pbkFreeBint(&T);
		pbkFreeBint(&U);
		return TRUE;
	}
Lbl_Failed:
	pbkFreeBint(&T);
	pbkFreeBint(&U);
	return FALSE;
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _pbmRand1Block
 * Description:   Generate 1 block random number.
 * Parameters:    N/A.
 * Return value:  Random number.
 */
_ub _pbmRand1Block(void)
{
	register _udb u = 1;
	register _ub v = 1;
	register size_t i, j = 0;
	while (!(u >> UB_BIT))
	{
		u *= RAND_MAX;
		++j;
	}
	for (i = 0; i < j; ++i)
		v *= rand();
	return v;
}

/* Function name: pbmRandomGenerator
 * Description:   An RNG.
 * Parameters:
 *          r Pointer to a big integer to store result.
 *     blocks Maximum number of blocks to be generate.
 *     blarge FALSE Generate a smaller integer than blocks.
 *            TRUE  Generate a larger integer than blocks - 1.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 */
_boolean pbmRandomGenerator(P_BINT r, _ub blocks, _boolean blarge)
{
	size_t i = RAND_1_TIME(1, blocks), j;
	if (!pbkReallocBint(r, (_ub)i, FALSE))
		return FALSE;
	for (j = 0; j < blarge ? i : i - 1; ++j)
		r->data[j] = _pbmRand1Block();
	pbkShrinkZeroFlag(r);
	return TRUE;
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _pbkIsPrimePrimary
 * Description:   Test prime number.
 * Parameter:
 *         n Pointer to a big integer to be tested.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 */
_boolean _pbkIsPrimePrimary(P_BINT n)
{
	size_t i;
	_boolean r = TRUE;

	BINT P = { 0 }, R = { 0 };

	pbkInitBint(&P, 0);

	for (i = 1; i < PTSIZ; ++i)
	{
		P.data[0] = gPrimeTable1K[i];
		pbkDivideBint(NULL, &R, n, &P);
		if (pbkIsBintEqualToZero(&R))
		{
			r = FALSE;
			goto Lbl_Finish;
		}
	}

Lbl_Finish:
	pbkFreeBint(&P);
	return r;
}

/* Function name: pbmMillerRabinTest
 * Description:   Miller Rabin primality test.
 * Parameters:
 *          r Pointer to a big integer to be tested.
 *          k K times of evaluation. K should be greater than 8.
 * Return value:  TRUE:  n is a pseudo prime.
 *                FALSE: n is NOT a prime.
 */
_boolean pbmMillerRabinTest(P_BINT n, _ub k)
{
	BINT A = { 0 }, X = { 0 }, N_1 = { 0 }, U = { 0 }, V = { 0 }, T = { 0 };
	_boolean r = TRUE;
	_udb t = 0, s;
	size_t i;
	if ((pbkBintToIb(n) < 3) || 0 == (pbkBintToIb(n) & 1))
	{
		if (2 == pbkBintToIb(n) && 1 == GETFLAG(n))
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if (FALSE == _pbkIsPrimePrimary(n))
			return FALSE;

		pbkInitBint(&A, 0);
		pbkInitBint(&X, 1);
		pbkInitBint(&N_1, 0);
		pbkInitBint(&V, 0);
		pbkInitBint(&T, 0);

		if (!pbkSubtractBint(&N_1, n, &X))
		{
			r = FALSE;
			goto Lbl_Finish;
		}

		if (!pbkMoveBint(&U, &N_1))
		{
			r = FALSE;
			goto Lbl_Finish;
		}

		while (0 == (U.data[0] & 1))
		{
			pbkRightShiftBint(&U, 0, 1);
			++t;
		}

		for (i = 0; i < k; ++i)
		{
			if (GETFLAG(n) > 1)
			{
				if (!pbmRandomGenerator(&A, GETABS(GETFLAG(n)) - 1, TRUE))
				{
					r = FALSE;
					goto Lbl_Finish;
				}
			}
			else
			{
				if (!pbmRandomGenerator(&A, 1, FALSE))
				{
					r = FALSE;
					goto Lbl_Finish;
				}
			}

			if (!pbkMoveBint(&T, &U))
			{
				r = FALSE;
				goto Lbl_Finish;
			}
			
			if (!pbmBintExponentialModuleBint(&V, &A, &T, n))
			{
				r = FALSE;
				goto Lbl_Finish;
			}

			if (1 == GETFLAG(&V) && 1 == V.data[0])
				continue;

			for (s = 0; s < t; ++s)
			{
				if (0 == pbkCompareBint(&V, &N_1))
					break;

				if (!pbmBintExponentialModule(&X, &V, 2, n))
				{
					r = FALSE;
					goto Lbl_Finish;
				}

				if (!pbkMoveBint(&V, &X))
				{
					r = FALSE;
					goto Lbl_Finish;
				}
			}
			if (s == t)
			{
				r = FALSE;
				goto Lbl_Finish;
			}
		}
		r = TRUE;
	}
Lbl_Finish:
	pbkFreeBint(&A);
	pbkFreeBint(&X);
	pbkFreeBint(&N_1);
	pbkFreeBint(&U);
	pbkFreeBint(&V);
	pbkFreeBint(&T);
	return r;
}

