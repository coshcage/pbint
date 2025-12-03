/*
 * Name:        pbm.h
 * Description: Portable big integer library mathematics module.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0520240323C0917251700L00027
 * License:     GPLv3.
 */

#ifndef _PBM_H_
#define _PBM_H_

#include "pbk.h"

/* Function declarations. */
bool pbmBintPower                (P_BINT r, P_BINT a,      _ub      n);
bool pbmBintPowerBint            (P_BINT r, P_BINT a,      P_BINT   n);
bool pbmBintExponentialModule    (P_BINT r, P_BINT a,      _ub      n,     P_BINT m);
bool pbmBintExponentialModuleBint(P_BINT r, P_BINT a,      P_BINT   n,     P_BINT m);
bool pbmUbFactorial              (P_BINT r, _ub    n);
bool pbmBintSquareRoot           (P_BINT r, P_BINT n);
bool pbmBintGreatestCommonDivisor(P_BINT r, P_BINT a,      P_BINT   b);
bool pbmBintLeastCommonMultiple  (P_BINT r, P_BINT a,      P_BINT   b);
bool pbmRandomGenerator          (P_BINT r, _ub    blocks, bool     blarge);
bool pbmMillerRabinTest          (P_BINT n, _ub    k);

#endif

