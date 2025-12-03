/*
 * Name:        pbk.h
 * Description: Portable big integer library kernel.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0520240323A0907251909L00141
 * License:     GPLv3.
 */

#ifndef _PBK_H_
#define _PBK_H_

#include <stddef.h>  /* Using type size_t and ptrdiff_t. */
#include <limits.h>  /* Using macro CHAR_BIT. */
#include <stdbool.h> /* Boolean type and constants. */

#define BINT_INIT_SIZE 1024U /* Initial big integer size. */
#define BINT_INIT_INCL 1024U /* Initial big integer incremental. */

#define BNUM_INIT_SIZE 8192U /* Initial big number size. */
#define BNUM_INIT_INCL 4096U /* Initial big number incremental. */

/* Predefined data type. */
typedef int                _ib;  /* Integer block. */
typedef unsigned int       _ub;  /* Unsigned integer block. */
typedef long long          _idb; /* Integer double block. */
typedef unsigned long long _udb; /* Unsigned integer double block. */

#define UBLOCK_FULL (~(_ub)0)
#define IBLOCK_FULL ((_ib)((UBLOCK_FULL) >> 1))
#define UB_BIT      (sizeof(_ub) * CHAR_BIT)
#define CARRY(udb)  (((_udb)udb) >> UB_BIT)

/* Big integer type. */
typedef struct st_BINT
{
	_ib    flag; /* Real capacity and sign. */
	_ub    size; /* Allocated capacity. */
	_ub    incl; /* Incremental. */
	_ub *  data; /* Data pointer. */
} BINT, * P_BINT;

typedef struct st_BNUM
{
	_ib    flag; /* Real capacity and sign. */
	_ub    size; /* Allocated capacity. */
	_ub    incl; /* Incremental. */
	_ub    base; /* Base. */
	char * data; /* Data pointer. */
} BNUM, * P_BNUM;

/* Unsigned block integer division. */
typedef struct _st_ubdiv_t
{
	_ub quot; /* Quotient.  */
	_ub rem;  /* Remainder. */
} _ubdiv_t;

/* Useful macros. */
#define GETABS(ib)   ((_ub)((ib) >= 0 ? (ib) : -(ib)))
#define GETMAX(a, b) ((a) >= (b) ? (a) : (b))
#define GETMIN(a, b) ((a) <= (b) ? (a) : (b))
#define GETSGN(ib)   ((ib) > 0 ? 1 : ((ib) < 0 ? -1 : 0))

#define GETFLAG(pbi) ((pbi)->flag)
#define GETSIZE(pbi) ((pbi)->size)
#define GETINCL(pbi) ((pbi)->incl)
#define GETBASE(pbn) ((pbn)->base)

#define SETFLAG(pbi, ib) (GETFLAG(pbi) = (ib))
#define SETSIZE(pbi, ub) (GETSIZE(pbi) = (ub))
#define SETINCL(pbi, ub) (GETINCL(pbi) = (ub))
#define SETBASE(pbn, ub) (GETBASE(pbn) = (ub))

#define pbkIsNotANumber(pbi) ((pbi)->flag == 0)

#define pbkShrinkZeroFlag(pbi)                     \
do                                                 \
{                                                  \
	register size_t _i, _j = GETABS(GETFLAG(pbi)); \
	for (_i = GETABS(GETFLAG(pbi)); _i > 1; --_i)  \
	{                                              \
		if (0 == (pbi)->data[_i - 1])              \
			--_j;                                  \
		else                                       \
			break;                                 \
	}                                              \
	if (GETFLAG(pbi) >= 0)                         \
		SETFLAG(pbi,  (_ib)_j);                    \
	else                                           \
		SETFLAG(pbi, -(_ib)_j);                    \
}                                                  \
while (0);

#define pbkIsBintEqualToZero(pbi) (1 == GETABS(GETFLAG(pbi)) && !(*(pbi)->data))

/* Functions for big integers. */
bool   pbkInitBint       (P_BINT pbi, _ib  i);
bool   pbkReallocBint    (P_BINT pbi, _ub  size, bool binc);
void   pbkFreeBint       (P_BINT pbi);
P_BINT pbkCreateBint     (_ib    i);
void   pbkDeleteBint     (P_BINT pbi);
bool   pbkMoveBint       (P_BINT a, P_BINT b);
P_BINT pbkCopyBint       (P_BINT a);
bool   pbkIbToBint       (P_BINT a, _ib    i);
_ib    pbkBintToIb       (P_BINT a);
int    pbkCompareBint    (P_BINT a, P_BINT b);
bool   pbkAddBint        (P_BINT c, P_BINT a,    P_BINT   b);
bool   pbkSubtractBint   (P_BINT c, P_BINT a,    P_BINT   b);
bool   pbkLeftShiftBint  (P_BINT a, _ub    blocks, _ub    bits);
bool   pbkRightShiftBint (P_BINT a, _ub    blocks, _ub    bits);
bool   pbkMultiplyBintOld(P_BINT c, P_BINT a,    P_BINT   b);
bool   pbkMultiplyBintNew(P_BINT c, P_BINT a,    P_BINT   b);
bool   pbkMultiplyBintFFT(P_BINT c, P_BINT a,    P_BINT   b);
bool   pbkDivideBint     (P_BINT q, P_BINT r,    P_BINT   a, P_BINT b);
/* Functions for big numbers. */
bool   pbkInitBnum         (P_BNUM pbn, _ub    base);
bool   pbkReallocBnum      (P_BNUM pbn, _ub    size, bool binc);
void   pbkFreeBnum         (P_BNUM pbn);
P_BNUM pbkCreateBnum       (_ub    base);
void   pbkDeleteBnum       (P_BNUM pbn);
bool   pbkMoveBnum         (P_BNUM a,   P_BNUM b);
bool   pbkIbToBnum         (P_BNUM pbn, _ib    n);
bool   pbkDecimalSzToBnum  (P_BNUM pbn, const char * str);
void   pbkPrintBnum        (P_BNUM pbn);
bool   pbkBintToDecimalBnum(P_BNUM x,   P_BINT a);
bool   pbkDecimalBnumToBint(P_BINT x,   P_BNUM a);

#define pbkMultiplyBint pbkMultiplyBintNew

#endif

