
#ifndef _PBK_H_
#define _PBK_H_

#include <stddef.h> /* Using type size_t and ptrdiff_t. */
#include <limits.h> /* Using macro CHAR_BIT. */

/* No need to explain these following two. */
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define BINT_INIT_SIZE 1024U /* Initial big integer size. */
#define BINT_INIT_INCL 1024U /* Initial big integer incremental. */

#define BNUM_INIT_SIZE 8192U /* Initial big number size. */
#define BNUM_INIT_INCL 4096U /* Initial big number incremental. */

/* Predefined data type. */
typedef int                _ib;      /* Integer block. */
typedef unsigned int       _ub;      /* Unsigned integer block. */
typedef long long          _idb;     /* Integer double block. */
typedef unsigned long long _udb;     /* Unsigned integer double block. */

/* Boolean type does not need to change usually. */
typedef unsigned int       _boolean; /* Boolean. */

#define IBLOCK_FULL ((_ib)((~0U) >> 1))
#define USIZE_FULL  (~0U)
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

#define pbkShrinkZeroFlag(pbi)                   \
do                                               \
{                                                \
	register size_t i, j = GETABS(GETFLAG(pbi)); \
	for (i = GETABS(GETFLAG(pbi)); i > 1; --i)   \
	{                                            \
		if (0 == (pbi)->data[i - 1])             \
			--j;                                 \
		else                                     \
			break;                               \
	}                                            \
	if (GETFLAG(pbi) >= 0)                       \
		SETFLAG(pbi,  (_ib)j);                   \
	else                                         \
		SETFLAG(pbi, -(_ib)j);                   \
}                                                \
while (0);

/* Functions for big integers. */
_boolean pbkInitBint(P_BINT pbi, _ib i);
_boolean pbkReallocBint(P_BINT pbi, _ub size, _boolean binc);
void pbkFreeBint(P_BINT pbi);
P_BINT pbkCreateBint(_ib i);
void pbkDeleteBint(P_BINT pbi);
_boolean pbkMoveBint(P_BINT a, P_BINT b);
P_BINT pbkCopyBint(P_BINT a);
_boolean pbkIb2Bint(P_BINT a, _ib i);
_ib pbkBint2Ib(P_BINT a);
int pbkCompareBint(P_BINT a, P_BINT b);
_boolean pbkAddBint(P_BINT c, P_BINT a, P_BINT b);
_boolean pbkSubtractBint(P_BINT c, P_BINT a, P_BINT b);
_boolean pbkLeftShiftBint(P_BINT a, _ub blocks, _ub bits);
_boolean pbkRightShiftBint(P_BINT a, _ub blocks, _ub bits);
_boolean pbkMultiplyBint(P_BINT c, P_BINT a, P_BINT b);
_boolean pbkDivideBint(P_BINT q, P_BINT r, P_BINT a, P_BINT b);
/* Functions for big numbers. */
_boolean pbkInitBnum(P_BNUM pbn, _ub base);
_boolean pbkReallocBnum(P_BNUM pbn, _ub size, _boolean binc);
void pbkFreeBnum(P_BNUM pbn);
P_BNUM pbkCreateBnum(_ub base);
void pbkDeleteBnum(P_BNUM pbn);
_boolean pbkMoveBnum(P_BNUM a, P_BNUM b);
_boolean pbkIbToBnum(P_BNUM pbn, _ib n);
_boolean pbkDecimalSzToBnum(P_BNUM pbn, char * str);
void pbkPrintBnum(P_BNUM pbn);
_boolean pbkBintToDecimalBnum(P_BNUM x, P_BINT a);
_boolean pbkDecimalBnumToBint(P_BINT x, P_BNUM a);

#endif
