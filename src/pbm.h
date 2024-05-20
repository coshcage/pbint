/*
 * Name:        pbk.h
 * Description: Portable big integer library mathematics module.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0520240323C0520240324L00021
 * License:     GPLv3.
 */

#ifndef _PBM_H_
#define _PBM_H_

#include "pbk.h"

/* Function declarations. */
_boolean pbmBintPower                (P_BINT r, P_BINT a, _ub    n);
_boolean pbmUbFactorial              (P_BINT r, _ub    n);
_boolean pbmBintSquareRoot           (P_BINT r, P_BINT n);
_boolean pbmBintGreatestCommonDivisor(P_BINT r, P_BINT a, P_BINT b);

#endif
