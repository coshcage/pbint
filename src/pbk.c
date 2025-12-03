/*
 * Name:        pbk.c
 * Description: Portable big integer library kernel.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0520240323B1101251046L01605
 * License:     GPLv3.
 */

#include "pbk.h"
#include <stdio.h>  /* Using function putchar, printf. */
#include <stdlib.h> /* Using function memcpy, memmove, malloc, realloc. */
#include <string.h> /* Using function memset, strlen. */

/* File level function declarations. */
static _ubdiv_t _ubdiv(_ub numerator, _ub denominator);

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _ubdiv
 * Description:   Compute a quotient that truncates toward 0 and the corresponding remainder.
 * Parameters:
 *  numerator Numerator.
 *denominator Denominator.
 * Return value:  A _ubdiv_t structure which contains the result.
 */
static _ubdiv_t _ubdiv(_ub numerator, _ub denominator)
{
	_ubdiv_t result;
	/* Just give me some optimization. */
	result.quot = numerator / denominator;
	result.rem  = numerator % denominator;
	return result;
}

/* Function name: pbkInitBint
 * Description:   Initialize a big integer.
 * Parameters:
 *        pbi Pointer to a big integer.
 *          i Initial value you want to set to big integer.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       Big integer structure shall be initialized to 0.
 * Tip:           This function is used only to initialize big integers,
 *                but not for change the value or capacity for any big integer.
 */
bool pbkInitBint(P_BINT pbi, _ib i)
{
	if (NULL == pbi->data)
	{
		SETFLAG(pbi, (i == 0 ? 1 : GETSGN(i)));
		SETSIZE(pbi, BINT_INIT_SIZE);
		SETINCL(pbi, BINT_INIT_INCL);
		pbi->data = (_ub *)malloc(sizeof(_ub) * (GETSIZE(pbi) + GETINCL(pbi)));
		if (NULL != pbi->data)
		{
			pbi->data[0] = GETABS(i);
			return true;
		}
		else
		{
			SETFLAG(pbi, 0);
			SETSIZE(pbi, 0U);
		}
	}
	return false;
}

/* Function name: pbkReallocBint
 * Description:   Reallocate a big integer.
 * Parameters:
 *        pbi Pointer to a big integer.
 *       size New size of capacity to big integer.
 *       binc Increase the capacity or decrease it.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Tip:           This function can be used to initialize big integers.
 */
bool pbkReallocBint(P_BINT pbi, _ub size, bool binc)
{
	if (!binc && (GETSIZE(pbi) == size))
	{
		return true;
	}
	else if (binc && (GETSIZE(pbi) >= size))
	{
		return true;
	}
	else
	{
		_ub * pnew = (_ub *)realloc(pbi->data, SETSIZE(pbi, (binc ? size + GETINCL(pbi) : size)) * sizeof(_ub));
		if (NULL != pnew)
		{
			pbi->data = pnew;
			return true;
		}
		else
		{
			SETFLAG(pbi, 0);
			SETSIZE(pbi, 0U);
			pbi->data = NULL;
			return false;
		}
	}
}

/* Function name: pbkFreeBint
 * Description:   Initialize a big integer.
 * Parameter:
 *       pbi Pointer to a big integer.
 * Return value:  N/A.
 */
void pbkFreeBint(P_BINT pbi)
{
	if (pbi->data)
		free(pbi->data);
	pbi->data = NULL;
	SETFLAG(pbi, 0);
	SETSIZE(pbi, 0U);
}

/* Function name: pbkCreateBint
 * Description:   Create a big integer in main memory.
 * Parameter:
 *         i Initial value you want to set to big integer.
 * Return value:  Pointer to new created big integer structure.
 */
P_BINT pbkCreateBint(_ib i)
{
	register P_BINT pbi = malloc(sizeof(BINT));
	if (NULL != pbi)
	{
		memset(pbi, 0, sizeof(BINT));
		if (!pbkInitBint(pbi, i))
		{
			free(pbi);
			pbi = NULL;
		}
	}
	return pbi;
}

/* Function name: pbkDeleteBint
 * Description:   Deallocate a big integer from main memory.
 * Parameter:
 *       pbi Pointer to a big integer.
 * Return value:  N/A.
 */
void pbkDeleteBint(P_BINT pbi)
{
	pbkFreeBint(pbi);
	free(pbi);
}

/* Function name: pbkMoveBint
 * Description:   Move the memory of a big integer to another big integer.
 * Parameters:
 *          a Pointer to a big integer.
 *          b Pointer to another big integer.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       Parameter a and b shall not be NULL.
 * Tip:           After invoking, the capacity of a will equal to b's. 
 */
bool pbkMoveBint(P_BINT a, P_BINT b)
{
	if (pbkReallocBint(a, GETSIZE(b), true))
	{
		memmove(a->data, b->data, GETABS(GETFLAG(b)) * sizeof(_ub));
		SETFLAG(a, GETFLAG(b));
		return true;
	}
	return false;
}

/* Function name: pbkCopyBint
 * Description:   Copy a big integer.
 * Parameter:
 *         a Pointer to a big integer.
 * Return value:  Pointer to the copy of big integer a.
 */
P_BINT pbkCopyBint(P_BINT a)
{
	register P_BINT pbi = pbkCreateBint(0);
	if (NULL != pbi)
	{
		if (pbkMoveBint(pbi, a))
			return pbi;
		pbkFreeBint(pbi);
		pbi = NULL;
	}
	return pbi;
}

/* Function name: pbkIbToBint
 * Description:   Convert an integer to big integer.
 * Parameters:
 *          a Pointer to a big integer.
 *          i Value you want to set to the big integer.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Tip:           This function will not change a's capacity.
 */
bool pbkIbToBint(P_BINT a, _ib i)
{
	if (pbkIsNotANumber(a))
		return false;
	a->data[0] = GETABS(i);
	SETFLAG(a, 0 == GETSGN(i) ? 1 : GETSGN(i));
	return true;
}

/* Function name: pbkBintToIb
 * Description:   Convert a big integer to signed block integer.
 * Parameter:
 *         a Pointer to a big integer.
 * Return value:  Converted signed block integer.
 *                If this function fails to convert, it would return 0.
 * Caution:       Only the latest block of big integer can be converted.
 */
_ib pbkBintToIb(P_BINT a)
{
	if (pbkIsNotANumber(a))
		return 0;
	return GETFLAG(a) >= 0 ? (_ib)*a->data : -(_ib)*a->data;
}

/* Function name: pbkCompareBint
 * Description:   Compare two big integers.
 * Parameters:
 *          a Pointer to a big integer.
 *          b Pointer to another big integer.
 * Return value:  0:  a == b. Or can not compare.
 *                1:  a > b.
 *                -1: a < b.
 * Caution:       If a or b is NaN, function will return 0.
 */
int pbkCompareBint(P_BINT a, P_BINT b)
{
	if (pbkIsNotANumber(a) || pbkIsNotANumber(b))
		return 0; /* Can not compare. */
	else
	{
		if (GETFLAG(a) == GETFLAG(b) && a->data == b->data)
			return 0;
		if (GETFLAG(a) > 0 && GETFLAG(b) > 0) /* +1 . +1. */
		{
			if (GETFLAG(a) > GETFLAG(b))
			{
				return 1;
			}
			else if (GETFLAG(a) < GETFLAG(b))
			{
				return -1;
			}
			else
			{
				_ub f = 1;
				if (*(char *)&f)
				{	/* Little endian. */
					register size_t i, j;
					register _udb x, y;
					if (0 == (GETFLAG(a) & 0x1)) /* 0 == GETFLAG(a) % 2; */
					{
						for (i = GETFLAG(a); i > 0; i -= 2)
						{
							j = i - 2;
							x = *(_udb *)&j[a->data];
							y = *(_udb *)&j[b->data];
							if (x > y)
								return 1;
							else if (x < y)
								return -1;
						}
					}
					else
					{
						if (GETFLAG(a) > 2)
						{
							for (i = GETFLAG(a); i > 1; i -= 2)
							{
								j = i - 2;
								x = *(_udb *)&j[a->data];
								y = *(_udb *)&j[b->data];
								if (x > y)
									return 1;
								else if (x < y)
									return -1;
							}
						}
						x = 0[a->data];
						y = 0[b->data];
						if (x > y)
							return 1;
						else if (x < y)
							return -1;
					}
					return 0;
				}
				else /* Big endian. */
				{
					register size_t i, j;
					register _ub x, y;
					for (i = GETFLAG(a); i > 0; --i)
					{
						j = i - 1;
						x = j[a->data];
						y = j[b->data];
						if (x > y)
							return 1;
						else if (x < y)
							return -1;
					}
					return 0;
				}
			}
		}
		else if (GETFLAG(a) < 0 && GETFLAG(b) > 0) /* -1 . +1. */
		{
			return -1;
		}
		else if (GETFLAG(a) > 0 && GETFLAG(b) < 0) /* +1 . -1. */
		{
			return 1;
		}
		else /* -1 . -1. */
		{
			register int r;
			register _ib fa, fb;

			fa = GETFLAG(a);
			fb = GETFLAG(b);

			SETFLAG(a, GETABS(GETFLAG(a)));
			SETFLAG(b, GETABS(GETFLAG(b)));

			r = -pbkCompareBint(a, b);

			SETFLAG(a, fa);
			SETFLAG(b, fb);

			return r;
		}
	}
}

/* Function name: pbkAddBint
 * Description:   Adds two big integers and stores result to c.
 * Parameters:
 *          c Pointer to a big integer.
 *          a Pointer to a big integer.
 *          b Pointer to a big integer.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       The address of c shall not equal to a or b.
 * Tip:           c := a + b;
 */
bool pbkAddBint(P_BINT c, P_BINT a, P_BINT b)
{
	if (pbkReallocBint(c, GETSIZE(a) > GETSIZE(b) ? GETSIZE(a) : GETSIZE(b), true))
	{
		if (GETFLAG(a) > 0 && GETFLAG(b) > 0) /* +1 + +1. */
		{
			register size_t i, j = GETMIN(GETFLAG(a), GETFLAG(b)), k = GETMAX(GETFLAG(a), GETFLAG(b));
			register _udb r = 0;
			P_BINT pir = GETFLAG(a) > GETFLAG(b) ? a : b;

			if (pbkIsNotANumber(a) || pbkIsNotANumber(b))
				return false;

			if (a == b && a->data == b->data) /* a + a = 2 * a. */
			{
				if (c->data != a->data)
					pbkMoveBint(c, a);

				if (!pbkLeftShiftBint(c, 0, 1))
					return false;

				return true;
			}

			for (i = 0; i < j; ++i)
			{
				r = CARRY(r) + (_udb)a->data[i] + (_udb)b->data[i];
				c->data[i] = (_ub)r;
			}
			for (; i < k; ++i)
			{
				r = CARRY(r) + (_udb)pir->data[i];
				c->data[i] = (_ub)r;
			}

			if (GETFLAG(c) + 1 >= (_ib)GETSIZE(c))
			{
				if (!pbkReallocBint(c, GETSIZE(c), true))
					return false;
			}
			c->data[i] = (_ub)CARRY(r);
			SETFLAG(c, (_ib)i + 1);
			pbkShrinkZeroFlag(c);
			return true;
		}
		else if (GETFLAG(a) < 0 && GETFLAG(b) > 0) /* -1 + +1. */
		{
			register int r;
			register _ib fa;

			fa = GETFLAG(a);

			SETFLAG(a, -GETFLAG(a));

			r = pbkSubtractBint(c, b, a);

			SETFLAG(a, fa);

			return r;
		}
		else if (GETFLAG(a) > 0 && GETFLAG(b) < 0) /* +1 + -1. */
		{
			return pbkSubtractBint(c, a, b);;
		}
		else /* -1 + -1. */
		{
			register int r;
			register _ib fa, fb;

			fa = GETFLAG(a);
			fb = GETFLAG(b);

			SETFLAG(a, GETABS(GETFLAG(a)));
			SETFLAG(b, GETABS(GETFLAG(b)));

			r = pbkAddBint(c, a, b);

			SETFLAG(a, fa);
			SETFLAG(b, fb);

			if (r)
				SETFLAG(c, -GETFLAG(c));

			return r;
		}
	}
	return false;
}

/* Function name: pbkSubtractBint
 * Description:   Subtracts two big integers and stores result to c.
 * Parameters:
 *          c Pointer to a big integer.
 *          a Pointer to a big integer.
 *          b Pointer to a big integer.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       The address of c shall not equal to a or b.
 * Tip:           c := a - b;
 */
bool pbkSubtractBint(P_BINT c, P_BINT a, P_BINT b)
{
	if (pbkReallocBint(c, GETSIZE(a) > GETSIZE(b) ? GETSIZE(a) : GETSIZE(b), false))
	{
		if (pbkIsNotANumber(a) || pbkIsNotANumber(b))
			return false;
		else if (GETFLAG(a) > 0 && GETFLAG(b) > 0) /* +1 - +1. */
		{
			if (pbkCompareBint(a, b) >= 0)
			{
				register size_t i, j = GETMIN(GETFLAG(a), GETFLAG(b)), k = GETMAX(GETFLAG(a), GETFLAG(b));
				register _ub x, y;
				register bool borrow = false;

				if (a == b && a->data == b->data) /* a - a = 0. */
				{
					pbkFreeBint(c);

					if (!pbkInitBint(c, 0))
						return false;

					return true;
				}
				
				for (i = 0; i < j; ++i)
				{
					y = a->data[i];

					if (borrow)
					{
						x = y - 1;
					}
					else
					{
						x = y;
					}

					y = b->data[i];
					c->data[i] = x - y;

					if (x < y)
					{
						borrow = true;
					}
					else
					{
						borrow = false;
					}
				}
				for (; i < k; ++i)
				{
					y = a->data[i];

					if (borrow)
					{
						x = y - 1;
					}
					else
					{
						x = y;
					}

					c->data[i] = x;

					if (0 == y)
						borrow = true;
					else
						borrow = false;
				}
				SETFLAG(c, (_ib)i);
				pbkShrinkZeroFlag(c);
				if (GETFLAG(c) < (_ib)(GETSIZE(c) - GETINCL(c)))
				{
					if (!pbkReallocBint(c, GETSIZE(c) - GETINCL(c), false))
						return false;
				}
				return true;
			}
			else /* +1 - +10. */
			{
				register bool r;

				r = pbkSubtractBint(c, b, a);

				if (r)
					SETFLAG(c, -GETFLAG(c));

				return r;
			}

		}
		else
		{
			register int r;
			register _ib fb;

			fb = GETFLAG(b);

			SETFLAG(b, -GETFLAG(b));

			r = pbkAddBint(c, a, b);

			SETFLAG(b, fb);

			return r;
		}
	}
	return false;
}

/* Function name: pbkLeftShiftBint
 * Description:   Left shift a big integer.
 * Parameters:
 *          c Pointer to a big integer.
 *     blocks Blocks of integer you want to shift.
 *       bits Bits of integer you want to shift.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 */
bool pbkLeftShiftBint(P_BINT a, _ub blocks, _ub bits)
{
	if (pbkIsNotANumber(a))
		return false;
	else
	{
		register _ubdiv_t u = _ubdiv(bits, UB_BIT);
		register size_t i, j;
		register _udb n = 0;

		blocks += u.quot;

		/* Handle blocks first. */
		if (GETFLAG(a) + blocks + (u.rem ? 1 : 0) >= GETSIZE(a))
		{
			if (!pbkReallocBint(a, GETSIZE(a) + blocks + (u.rem ? 1 : 0), true))
				return false;
		}
		if (blocks)
		{
			memmove(a->data + blocks, a->data, GETABS(GETFLAG(a)) * sizeof(_ub));
			memset(a->data, 0, sizeof(_ub) * blocks);
			SETFLAG(a, (GETFLAG(a) >= 0 ? GETFLAG(a) + blocks : -(_ib)(GETABS(GETFLAG(a)) + blocks)));
		}

		/* Handle bits next. */
		if (u.rem)
		{
			j = GETABS(GETFLAG(a));
			a->data[j] = 0;
			for (i = j; i > 0; --i)
			{
				n = (_udb)a->data[i - 1] << u.rem;
				a->data[i - 1] = (_ub)n;
				a->data[i] |= CARRY(n);
			}
			SETFLAG(a, (GETFLAG(a) >= 0 ? GETFLAG(a) + 1 : -(_ib)(GETABS(GETFLAG(a)) + 1)));
			pbkShrinkZeroFlag(a);
		}

		return true;
	}
}

/* Function name: pbkRightShiftBint
 * Description:   Right shift a big integer.
 * Parameters:
 *          a Pointer to a big integer.
 *     blocks Blocks of integer you want to shift.
 *       bits Bits of integer you want to shift.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 */
bool pbkRightShiftBint(P_BINT a, _ub blocks, _ub bits)
{
	if (pbkIsNotANumber(a))
		return false;
	else
	{
		register _ubdiv_t u = _ubdiv(bits, UB_BIT);
		register _ub n, n2;
		register size_t i;

		/* Handle blocks first. */
		if (blocks)
		{
			register _ib b = GETABS(GETFLAG(a)) - blocks;
			memmove(a->data, a->data + blocks, ((_ib)b >= 0 ? (_ub)b : GETABS(GETFLAG(a))) * sizeof(_ub));
			b = b > 0 ? b : 1;
			SETFLAG(a, (GETFLAG(a) >= 0 ? b : -b));
		}

		if (GETFLAG(a) - (u.rem ? 1 : 0) - blocks < (_ib)(GETSIZE(a) - GETINCL(a)))
		{
			if (!pbkReallocBint(a, GETSIZE(a) - GETINCL(a), false))
				return false;
		}

		/* Handle bits next. */
		if (u.rem)
		{
			for (n2 = n = 0, i = GETABS(GETFLAG(a)); i > 0; --i)
			{
				n = a->data[i - 1] << (UB_BIT - u.rem);
				a->data[i - 1] >>= u.rem;
				a->data[i - 1] |= n2;
				n2 = n;
				
			}
			pbkShrinkZeroFlag(a);
		}

		return true;
	}
}

/* Function name: pbkMultiplyBintOld
 * Description:   Multiplies two big integers and stores result to c.
 * Parameters:
 *          c Pointer to a big integer.
 *          a Pointer to a big integer.
 *          b Pointer to a big integer.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       The address of c shall not equal to a or b.
 * Tip:           c := a * b;
 */
bool pbkMultiplyBintOld(P_BINT c, P_BINT a, P_BINT b)
{
	if (GETFLAG(a) > 0 && GETFLAG(b) > 0) /* +1 * +1. */
	{
		if (GETFLAG(a) > GETFLAG(b))
		{
			return pbkMultiplyBint(c, b, a);
		}
		else /* GETFLAG(a) <= GETFLAG(b). */
		{
			register size_t i, j, k = GETFLAG(a), l = 0, m = 0;
			register _ub n;
			BINT B = { 0 }, C = { 0 };
			bool r = true;

			if (pbkIsNotANumber(a) || pbkIsNotANumber(b))
				return false;

			SETFLAG(c, 1);
			c->data[0] = 0;

			if (pbkIsBintEqualToZero(a) || pbkIsBintEqualToZero(b)) /* 0 * a = 0. */
				return true;

			if (!pbkMoveBint(&B, b))
			{
				r = false;
				goto Lbl_Clear;
			}
			
			for (i = 0; i < k; ++i)
			{
				n = a->data[i];

				if (n)
				{
					for (j = 0; j < UB_BIT; ++j)
					{
						if (((_ub)1 << j) & n)
						{
							if (!pbkLeftShiftBint(&B, (_ub)m, (_ub)l))
							{
								r = false;
								goto Lbl_Clear;
							}
							if (!pbkMoveBint(&C, c))
							{
								r = false;
								goto Lbl_Clear;
							}
							if (!pbkAddBint(c, &B, &C))
							{
								r = false;
								goto Lbl_Clear;
							}
							l = 1;
							m = 0;
						}
						else
						{
							++l;
						}
					}
				}
				else
				{
					++m;
				}
			}
		Lbl_Clear:
			pbkFreeBint(&B);
			pbkFreeBint(&C);
			return r;
		}
	}
	else
	{
		register int r;
		register _ib fa, fb;

		fa = GETFLAG(a);
		fb = GETFLAG(b);

		SETFLAG(a, GETABS(GETFLAG(a)));
		SETFLAG(b, GETABS(GETFLAG(b)));

		r = pbkMultiplyBint(c, a, b);

		SETFLAG(a, fa);
		SETFLAG(b, fb);

		if (r)
		{
			if (GETFLAG(a) > 0 || GETFLAG(b) > 0)
				SETFLAG(c, -GETFLAG(c));
		}

		return r;
	}
}

/* Function name: pbkMultiplyBintNew
 * Description:   Multiplies two big integers and stores result to c.
 * Parameters:
 *          c Pointer to a big integer.
 *          a Pointer to a big integer.
 *          b Pointer to a big integer.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       The address of c shall not equal to a or b.
 * Tip:           c := a * b;
 */
bool pbkMultiplyBintNew(P_BINT c, P_BINT a, P_BINT b)
{
	if (GETFLAG(a) > 0 && GETFLAG(b) > 0) /* +1 * +1. */
	{
		if (GETABS(GETFLAG(a)) <= GETABS(GETFLAG(b)))
		{
			register size_t i, j, k = GETFLAG(a), l = GETFLAG(b), m;
			register _udb carry;
			register _ub  t;
			BINT A = { 0 };
			bool r = true;

			if (pbkIsNotANumber(a) || pbkIsNotANumber(b))
				return false;

			SETFLAG(c, 1);
			c->data[0] = 0;

			if (pbkIsBintEqualToZero(a) || pbkIsBintEqualToZero(b)) /* 0 * a = 0. */
				return true;

			if (!pbkReallocBint(&A, k + l + 1, true))
			{
				r = false;
				goto Lbl_Clear;
			}

			for (i = 0; i < k; ++i)
			{
				SETFLAG(&A, 1);
				A.data[0] = 0;
				carry = 0;
				for (j = 0, m = l; j < l; ++j)
				{
					A.data[j] = CARRY(carry);
					carry = (_udb)a->data[i] * (_udb)b->data[j] + (_udb)A.data[j];
					A.data[j] = (_ub)carry;
				}
				t = CARRY(carry);
				if (t)
				{
					A.data[j] = t;
					++m;
				}

				SETFLAG(&A, (_ib)m);

				if (!pbkLeftShiftBint(&A, i, 0))
				{
					r = false;
					goto Lbl_Clear;
				}
				if (!pbkAddBint(c, c, &A))
				{
					r = false;
					goto Lbl_Clear;
				}
			}
		Lbl_Clear:
			pbkFreeBint(&A);
			return r;
		}
		else
			return pbkMultiplyBintNew(c, b, a);
	}
	else
	{
		register int r;
		register _ib fa, fb;

		fa = GETFLAG(a);
		fb = GETFLAG(b);

		SETFLAG(a, GETABS(GETFLAG(a)));
		SETFLAG(b, GETABS(GETFLAG(b)));

		r = pbkMultiplyBint(c, a, b);

		SETFLAG(a, fa);
		SETFLAG(b, fb);

		if (r)
		{
			if (GETFLAG(a) > 0 || GETFLAG(b) > 0)
				SETFLAG(c, -GETFLAG(c));
		}

		return r;
	}
}

/* Function name: pbkDivideBint
 * Description:   Divides two big integers and stores quotient to q and stores reminder to r.
 * Parameters:
 *          q Pointer to a big integer that is the quotient. This value can be NULL.
 *          r Pointer to a big integer that is the reminder. This value can be NULL.
 *          a Pointer to a big integer that is the numerator.
 *          b Pointer to a big integer that is the denominator.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       The address of c shall not equal to a or b.
 * Tip:           q := a / b; r := a mod b;
 *                Parameter q or r can be NULL.
 */
bool pbkDivideBint(P_BINT q, P_BINT r, P_BINT a, P_BINT b)
{
	if (pbkIsNotANumber(a) || pbkIsNotANumber(b))
		return false;
	else
	{
		if (GETFLAG(a) > 0 && GETFLAG(b) > 0) /* +10 / +1. */
		{
			if (pbkIsBintEqualToZero(b)) /* 1 / 0 = NaN. */
				return false;

			if (pbkIsBintEqualToZero(a)) /* 0 / 1 = 0...1 */
			{
				if (NULL != q)
				{
					SETFLAG(q, 1);
					q->data[0] = 0;
				}
				if (NULL != r)
					if (!pbkMoveBint(r, b))
						return false;
				return true;
			}

			if (pbkCompareBint(a, b) < 0)
			{
				if (NULL != r)
					if (!pbkMoveBint(r, a))
						return false;
				if (NULL != q)
					if (!pbkIbToBint(q, 0))
						return false;
				return true;
			}
			else
			{
				if ((NULL != r && pbkReallocBint(r, GETSIZE(a), false)) || (NULL != q && pbkReallocBint(q, GETSIZE(a), false)))
				{
					register _ub max, mid, min;
					register ptrdiff_t i;
					register _ub u;
					register size_t j, k;
					bool rtn = false;
					/* Divisor and quotient. */
					BINT D = { 0 }, Q = { 0 }, C = { 0 }, T = { 0 }, X = { 0 };

					pbkInitBint(&D, 0);
					pbkInitBint(&Q, 0);
					pbkInitBint(&C, 0);
					pbkInitBint(&T, 0);
					pbkInitBint(&X, 0);

					pbkMoveBint(&Q, a);
					
					if (NULL != q)
						SETFLAG(q, 0);
					
					/* Put GETFLAG(b) blocks from a to Q. */
					memcpy(Q.data, a->data + GETFLAG(a) - GETFLAG(b), GETFLAG(b) * sizeof(_ub));
					SETFLAG(&Q, GETFLAG(b));

					if (pbkCompareBint(b, &Q) > 0) /* b is bigger, enlarge Q. */
					{
						pbkLeftShiftBint(&Q, 1, 0);
						memcpy(Q.data, a->data + GETFLAG(a) - GETFLAG(b) - 1, sizeof(_ub));
						SETFLAG(&Q, GETFLAG(b) + 1);
					}

					if (!pbkMoveBint(&D, b))
					{
						rtn = false;
						goto Lbl_Failure;
					}

					if (!pbkMoveBint(&C, &Q))
					{
						rtn = false;
						goto Lbl_Failure;
					}

					/* Start dichotomization. */
					max = UBLOCK_FULL;
					min = 0;
					mid = (_ub)((((_udb)max + min) >> 1) + 1);
					i = GETFLAG(a) - GETFLAG(&Q);

					for ( ;; )
					{
						T.data[0] = mid;
						SETFLAG(&T, 1);

						if (!pbkMultiplyBint(&X, &T, &D))
						{
							rtn = false;
							goto Lbl_Failure;
						}
						
						if (!pbkSubtractBint(&T, &Q, &X))
						{
							rtn = false;
							goto Lbl_Failure;
						}

						if (GETFLAG(&T) <= 0)
						{
							max = mid;
							mid = (_ub)(((_udb)max + min) >> 1);
						}
						else if (pbkCompareBint(&T, b) < 0)
						{
							/* Record the result as a part of answer. */
							if (NULL != q)
								q->data[GETFLAG(q)++] = mid;
							for ( ;; )
							{
								if (--i < 0) /* Congratulations! */
								{
									if (NULL != r)
									{
										if (!pbkMoveBint(r, &T))
										{
											rtn = false;
											goto Lbl_Failure;
										}
									}
									rtn = true;
									goto Lbl_Solved;
								}

								/* If reminder is not equal to zero, then left shit r for 1 section. */
								if (!pbkLeftShiftBint(&T, 1, 0))
								{
									rtn = false;
									goto Lbl_Failure;
								}

								T.data[0] = a->data[i];

								if (pbkCompareBint(&T, b) < 0)
								{
									if (NULL != q)
										q->data[GETFLAG(q)++] = 0;
								}
								else
									break;
							}
							if (!pbkMoveBint(&C, &T))
							{
								rtn = false;
								goto Lbl_Failure;
							}
							/* Reset dichotomization relative variables. */
							max = UBLOCK_FULL;
							min = 0x0;
							mid = (_ub)((((_udb)max + min) >> 1) + 1);
							/* One section is done! */
						}
						else
						{
							min = mid;
							mid = (_ub)((((_udb)max + min + 1) >> 1));
						}
						if (!pbkMoveBint(&D, b))
						{
							rtn = false;
							goto Lbl_Failure;
						}
						if (!pbkMoveBint(&Q, &C))
						{
							rtn = false;
							goto Lbl_Failure;
						}
					}
				Lbl_Solved:
					if (NULL != q)
					{
						/* Reverse quotient. */
						k = GETFLAG(q) >> 1;
						j = GETFLAG(q) & 1;
						for (i = 0; i < (_ib)k; ++i)
						{
							u = q->data[i];
							q->data[i] = q->data[k + i + j];
							q->data[k + i + j] = u;
						}
					}
				Lbl_Failure:
					pbkFreeBint(&D);
					pbkFreeBint(&Q);
					pbkFreeBint(&C);
					pbkFreeBint(&T);
					pbkFreeBint(&X);
					return rtn;
				}
				return false;
			}
		}
		else
		{
			register int rtn = false;
			register _ib fa, fb;

			fa = GETFLAG(a);
			fb = GETFLAG(b);

			SETFLAG(a, GETABS(GETFLAG(a)));
			SETFLAG(b, GETABS(GETFLAG(b)));

			rtn = pbkDivideBint(q, r, a, b);

			SETFLAG(a, fa);
			SETFLAG(b, fb);

			if (rtn)
			{
				if (GETFLAG(a) > 0 || GETFLAG(b) > 0)
				{
					if (NULL != q)
						SETFLAG(q, -GETFLAG(q));
					if (NULL != r)
						SETFLAG(r, -GETFLAG(r));
				}
			}

			return rtn;
		}
	}
}

/* This section of current C file is for big numbers. */
static bool _pbkAddAbstractBnum     (P_BNUM c, P_BNUM a, P_BNUM b);
static bool _pbkMultiplyAbstractBnum(P_BNUM c, P_BNUM a, P_BNUM b);

/* Function name: pbkInitBnum
 * Description:   Initialize a big number.
 * Parameters:
 *        pbn Pointer to a big number.
 *       base Base value of the big number.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       Big number structure shall be initialized to 0.
 * Tip:           This function is used only to initialize big numbers,
 *                but not for change the value or capacity for any big number.
 */
bool pbkInitBnum(P_BNUM pbn, _ub base)
{
	if (NULL == pbn->data)
	{
		SETFLAG(pbn, 1);
		SETSIZE(pbn, BNUM_INIT_SIZE);
		SETINCL(pbn, BNUM_INIT_INCL);
		SETBASE(pbn, base);
		pbn->data = (char *)malloc(GETSIZE(pbn) + GETINCL(pbn));
		if (NULL == pbn->data)
		{
			SETFLAG(pbn, 0);
			SETSIZE(pbn, 0U);
		}
		else
		{
			pbn->data[0] = 0;
			return true;
		}
	}
	return false;
}

/* Function name: pbkReallocBnum
 * Description:   Reallocate a big number.
 * Parameters:
 *        pbi Pointer to a big number.
 *       size New size of capacity to big number.
 *       binc Increase the capacity or decrease it.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Tip:           This function can be used to initialize big numbers.
 */
bool pbkReallocBnum(P_BNUM pbn, _ub size, bool binc)
{
	if (!binc && (GETSIZE(pbn) == size))
	{
		return true;
	}
	else if (binc && (GETSIZE(pbn) >= size))
	{
		return true;
	}
	else
	{
		char * pnew = (char *)realloc(pbn->data, SETSIZE(pbn, (binc ? size + GETINCL(pbn) : size)));
		if (NULL != pnew)
		{
			pbn->data = pnew;
			return true;
		}
		else
		{
			SETFLAG(pbn, 0);
			SETSIZE(pbn, 0U);
			pbn->data = NULL;
			return false;
		}
	}
}

/* Function name: pbkFreeBnum
 * Description:   Initialize a big number.
 * Parameter:
 *       pbi Pointer to a big number.
 * Return value:  N/A.
 */
void pbkFreeBnum(P_BNUM pbn)
{
	if (pbn->data)
		free(pbn->data);
	pbn->data = NULL;
	SETFLAG(pbn, 0);
	SETSIZE(pbn, 0U);
}

/* Function name: pbkCreateBnum
 * Description:   Create a big number in main memory.
 * Parameter:
 *      base Base value you want to set to big number.
 * Return value:  Pointer to new created big number structure.
 */
P_BNUM pbkCreateBnum(_ub base)
{
	register P_BNUM pbn = malloc(sizeof(BINT));
	if (NULL != pbn)
	{
		memset(pbn, 0, sizeof(BNUM));
		if (!pbkInitBnum(pbn, base))
		{
			free(pbn);
			pbn = NULL;
		}
	}
	return pbn;
}

/* Function name: pbkDeleteBnum
 * Description:   Deallocate a big number from main memory.
 * Parameter:
 *       pbi Pointer to a big number.
 * Return value:  N/A.
 */
void pbkDeleteBnum(P_BNUM pbn)
{
	pbkFreeBnum(pbn);
	free(pbn);
}

/* Function name: pbkMoveBnum
 * Description:   Move the memory of a big number to another big number.
 * Parameters:
 *         a Pointer to a big number.
 *         b Pointer to another big number.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       Parameter a and b shall not be NULL.
 * Tip:           After invoking, the capacity of a will equal to b's.
 */
bool pbkMoveBnum(P_BNUM a, P_BNUM b)
{
	if (pbkReallocBnum(a, GETSIZE(b), true))
	{
		memmove(a->data, b->data, GETABS(GETFLAG(b)));
		SETFLAG(a, GETFLAG(b));
		SETBASE(a, GETBASE(b));
		return true;
	}
	return false;
}

/* Function name: pbkIbToBnum
 * Description:   Convert an integer to big number.
 * Parameters:
 *        pbn Pointer to a big number.
 *          n Value you want to set to the big number.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Tip:           This function will not change pbn's capacity.
 */
bool pbkIbToBnum(P_BNUM pbn, _ib n)
{
	register _ib sign = GETSGN(n);
	if (pbkIsNotANumber(pbn))
		return false;
	if (n == 0)
	{
		SETFLAG(pbn, 1);
		pbn->data[0] = 0;
		return true;
	}

	n = GETABS(n);

	for (SETFLAG(pbn, 0); n > 0;)
	{
		register _ub base = pbn->base;

		if (GETFLAG(pbn) + 1 >= (_ib)GETSIZE(pbn))
		{
			if (!pbkReallocBnum(pbn, GETSIZE(pbn), true))
				return false;
		}

		pbn->data[GETFLAG(pbn)++] = n % base;
		n /= base;
	}

	SETFLAG(pbn, GETFLAG(pbn) * sign);

	return true;
}

/* Function name: pbkDecimalSzToBnum
 * Description:   Convert a base 10 presented string to big number.
 * Parameters:
 *        pbn Pointer to a big number.
 *        str Pointer to a string.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       This string shall contain ASCII number 0-9. And Shall not contain minus sign(-).
 */
bool pbkDecimalSzToBnum(P_BNUM pbn, const char * str)
{
	register size_t i, j = strlen(str);
	if (pbkReallocBnum(pbn, (_ub)j, true))
	{
		register char c;
		SETBASE(pbn, 10);
		for (i = 0, --j; j > 0; --j, ++i)
		{
			c = str[j];
			if (!(c >= '0' && c <= '9'))
				return false;
			pbn->data[i] = c - '0';
		}
		pbn->data[i] = str[j] - '0';
		SETFLAG(pbn, (_ib)i + 1);
		pbkShrinkZeroFlag(pbn);
		return true;
	}
	return false;
}

/* Function name: pbkPrintBnum
 * Description:   Print a big number.
 * Parameter:
 *       pbn Pointer to a big number.
 * Return value:  N/A.
 */
void pbkPrintBnum(P_BNUM pbn)
{
	if (pbkIsNotANumber(pbn))
	{
		printf("NaN");
		return;
	}
	else
	{
		register size_t i;
		register _ub base = pbn->base;
		if (GETFLAG(pbn) < 0)
			putchar('-');
		for (i = GETABS(GETFLAG(pbn)); i > 0; --i)
		{
			switch (base)
			{
			case 8:
			case 10:
				putchar('0' + pbn->data[i - 1]);
				break;
			case 16:
				printf("%x", pbn->data[i - 1]);
			}
		}
	}
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _pbkAddAbstractBnum
 * Description:   Adds two positive big numbers and stores the result to c.
 * Parameters:
 *          c Pointer to a big number.
 *          a Pointer to a big number.
 *          b Pointer to a big number.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       Value of parameter a and b shall be positive.
 * Tip:           c := a + b;
 */
static bool _pbkAddAbstractBnum(P_BNUM c, P_BNUM a, P_BNUM b)
{
	if (pbkReallocBnum(c, GETSIZE(a) > GETSIZE(b) ? GETSIZE(a) : GETSIZE(b), true))
	{
		register _ub base = c->base;
		register size_t i;
		register char carry;
		register _ib fa, fb;

		fa = GETFLAG(a);
		fb = GETFLAG(b);

		SETFLAG(a, GETABS(GETFLAG(a)));
		SETFLAG(b, GETABS(GETFLAG(b)));

		for (i = 0, carry = 0; i < (_ub)GETFLAG(a) || i < (_ub)GETFLAG(b) || carry; ++i)
		{
			carry += (i < (_ub)GETFLAG(a) ? a->data[i] : 0) + (i < (_ub)GETFLAG(b) ? b->data[i] : 0);
			c->data[i] = carry % base;
			carry /= base;
		}

		SETFLAG(a, fa);
		SETFLAG(b, fb);

		SETFLAG(c, (_ib)i);

		return true;
	}
	return false;
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _pbkMultiplyAbstractBnum
 * Description:   Subtracts two positive big numbers and stores the result to c.
 * Parameters:
 *          c Pointer to a big number.
 *          a Pointer to a big number.
 *          b Pointer to a big number.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Caution:       Value of parameter a and b shall be positive.
 * Tip:           c := a - b;
 */
static bool _pbkMultiplyAbstractBnum(P_BNUM c, P_BNUM a, P_BNUM b)
{
	BNUM B = { 0 };

	register _ib fa, fb;

	fa = GETFLAG(a);
	fb = GETFLAG(b);

	SETFLAG(a, GETABS(GETFLAG(a)));
	SETFLAG(b, GETABS(GETFLAG(b)));

	if (pbkReallocBnum(c, GETFLAG(a) + GETFLAG(b), true))
	{
		register size_t i;
		register char k;

		if (!pbkInitBnum(&B, GETBASE(b)))
			return false;

		if (!pbkMoveBnum(&B, b))
			return false;

		c->data[0] = 0;
		SETFLAG(c, 1);

		for (i = 0, k = a->data[i]; i < (_ub)GETFLAG(a); ++i, k = a->data[i])
		{
			while (k--)
				if (!_pbkAddAbstractBnum(c, c, &B)) /* ith digit is k, so we add k times. */
					goto Lbl_Failed;

			/* Multiply by 10. */
			if (GETFLAG(&B) + 1 >= (_ib)GETSIZE(&B))
			{
				if (!pbkReallocBnum(&B, GETSIZE(&B) + 1, true))
					goto Lbl_Failed;
			}
			memmove(B.data + 1, B.data, GETABS(GETFLAG(&B)));
			B.data[0] = 0;
			++GETFLAG(&B);
		}

		pbkShrinkZeroFlag(c);

		pbkFreeBnum(&B);

		SETFLAG(a, fa);
		SETFLAG(b, fb);

		return true;
	}

Lbl_Failed:
	pbkFreeBnum(&B);

	SETFLAG(a, fa);
	SETFLAG(b, fb);

	return false;
}

/* Function name: pbkBintToDecimalBnum
 * Description:   Convert a big integer to base 10 big number.
 * Parameters:
 *          x Pointer to a big number.
 *          a Pointer to a big integer.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Tip:           This function is used to output a big integer.
 */
bool pbkBintToDecimalBnum(P_BNUM x, P_BINT a)
{
	BNUM B = { 0 }, ak = { 0 }, X = { 0 };
	if (pbkIsNotANumber(a))
		return false;
	else
	{
		register size_t i, j;
		register _ub as;
		
		pbkInitBnum(&ak, 10);
		pbkInitBnum(&B, 10);
		pbkInitBnum(&X, 10);

		if (!pbkIbToBnum(&X, 0))
			goto Lbl_Failed;
		if (!pbkIbToBnum(&B, 2))
			goto Lbl_Failed;
		if (!pbkIbToBnum(&ak, 1))
			goto Lbl_Failed;

		for (i = GETABS(GETFLAG(a)); i > 0; --i)
		{
			as = a->data[i - 1];
			for (j = sizeof(_ub) * CHAR_BIT; j > 0; --j)
			{
				if (!pbkMoveBnum(&X, x))
					goto Lbl_Failed;
				if (!_pbkMultiplyAbstractBnum(x, &B, &X))
					goto Lbl_Failed;
				if (as & (1 << (j - 1)))
					if (!_pbkAddAbstractBnum(x, x, &ak))
						goto Lbl_Failed;
			}
		}
		pbkFreeBnum(&B);
		pbkFreeBnum(&ak);
		pbkFreeBnum(&X);
		GETFLAG(x) *= GETSGN(GETFLAG(a));
		return true;
	}
Lbl_Failed:
	pbkFreeBnum(&B);
	pbkFreeBnum(&ak);
	pbkFreeBnum(&X);
	return false;
}

/* Function name: pbkBintToDecimalBnum
 * Description:   Convert a big number to base 2 big integer.
 * Parameters:
 *          x Pointer to a big integer.
 *          a Pointer to a big number.
 * Return value:  true:  Succeeded.
 *                false: Failed.
 * Tip:           This function is used to input a big integer from a big number.
 */
bool pbkDecimalBnumToBint(P_BINT x, P_BNUM a)
{
	BINT B = { 0 }, ak = { 0 }, X = { 0 };
	if (pbkIsNotANumber(a))
		return false;
	else
	{
		register _ub d;
		register size_t i;
		
		pbkInitBint(&B, 10);
		pbkInitBint(&ak, 0);
		pbkInitBint(&X, 0);

		SETFLAG(x, 1);
		x->data[0] = 0;

		for (i = GETABS(GETFLAG(a)); i > 0; --i)
		{
			if (!pbkMoveBint(&X, x))
				goto Lbl_Failed;
			if (!pbkMultiplyBint(x, &B, &X))
				goto Lbl_Failed;
			d = ak.data[0] = a->data[i - 1];
			if (d)
				if (!pbkAddBint(x, x, &ak))
					goto Lbl_Failed;
		}
		pbkFreeBint(&B);
		pbkFreeBint(&ak);
		pbkFreeBint(&X);
		return true;
	}
Lbl_Failed:
	pbkFreeBint(&B);
	pbkFreeBint(&ak);
	pbkFreeBint(&X);
	GETFLAG(x) *= GETSGN(GETFLAG(a));
	return false;
}

