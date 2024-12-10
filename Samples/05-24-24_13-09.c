/*
 * Name:        05-24-24_13-09.c
 * Description: A big integer calculator.
 * Author:      cosh.cage#hotmail.com
 * License:     GPLv3.
 */
#include <stdio.h>
#include <string.h>
#include "pbm.h"

#define MAXLEN 100

P_BINT CatchFirstNumber();
P_BINT CalcBrackets();
P_BINT CalcAdditional();
P_BINT CalcAdvanced();
P_BINT CalcPrimary();

char * expr;

P_BINT CalcPrimary()
{
	P_BINT x, y = NULL, r;
	int c;
	r = pbkCreateBint(0);
	x = CalcAdvanced(expr);
	c = expr[0];
	while ('+' == c || '-' == c)
	{
		++expr;
		y = CalcAdvanced(expr);
		if ('+' == c)
		{
			pbkAddBint(r, x, y);
			pbkMoveBint(x, r);
		}
		else if ('-' == c)
		{
			pbkSubtractBint(r, x, y);
			pbkMoveBint(x, r);
		}
		c = expr[0];
	}
	if (expr[0] == ')')
		++expr;
	pbkDeleteBint(r);
	if (NULL != y)
		pbkDeleteBint(y);
	return x;
}

P_BINT CalcAdvanced()
{
	P_BINT x, y = NULL, r, k;
	int c;
	r = pbkCreateBint(0);
	k = pbkCreateBint(0);
	x = CalcAdditional(expr);
	c = expr[0];
	while ('*' == c || '/' == c)
	{
		++expr;
		y = CalcAdditional(expr);
		if ('*' == c)
		{
			pbkMultiplyBint(r, x, y);
			pbkMoveBint(x, r);
		}
		else if (c == '/')
		{
			pbkDivideBint(r, k, x, y);
			pbkMoveBint(x, k);
		}
		c = expr[0];
	}
	pbkDeleteBint(r);
	pbkDeleteBint(k);
	if (NULL != y)
		pbkDeleteBint(y);
	return x;
}

P_BINT CalcAdditional()
{
	P_BINT x, y, r;
	r = pbkCreateBint(0);
	x = CalcBrackets(expr);
	if ('^' == expr[0])
	{
		_ub n;
		++expr;
		y = CalcAdditional(expr);
		n = GETABS(pbkBintToIb(y));
		pbmBintPower(r, x, n);
		pbkDeleteBint(y);
		pbkDeleteBint(x);
		return r;
	}
	pbkDeleteBint(r);
	return x;
}

P_BINT CalcBrackets()
{
	if ('(' == expr[0])
	{
		++expr;
		return CalcPrimary(expr);
	}
	else
	{
		return CatchFirstNumber(expr);
	}
}

P_BINT CatchFirstNumber()
{
	size_t i;
	P_BINT r;
	P_BNUM t;
	char szT[2] = { 0 };
	char szTar[MAXLEN] = { 0 };
	r = pbkCreateBint(0);
	t = pbkCreateBnum(10);
	for (i = 0; expr[0] != 0 && i < MAXLEN; ++expr, ++i)
	{
		if (expr[0] >= '0' && expr[0] <= '9')
		{
			szT[0] = expr[0];
			strcat(szTar, szT);
		}
		else
		{
			pbkDecimalSzToBnum(t, szTar);
			pbkDecimalBnumToBint(r, t);
			pbkDeleteBnum(t);
			return r;
		}
	}
	pbkDecimalSzToBnum(t, szTar);
	pbkDecimalBnumToBint(r, t);
	pbkDeleteBnum(t);
	return r;
}

int main(int argc, char ** argv)
{
	char szExpr[MAXLEN] = { 0 };
	while (scanf("%s", szExpr))
	{
		P_BINT r;
		P_BNUM t;
		expr = szExpr;
		t = pbkCreateBnum(10);
		r = CalcPrimary();
		pbkBintToDecimalBnum(t, r);
		printf("= ");
		pbkPrintBnum(t);
		pbkDeleteBnum(t);
		pbkDeleteBint(r);
		printf("\n");
	}
	return 0;

}
