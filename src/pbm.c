/* Math. */
#include "pbm.h"

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
				pbkMoveBint(&R, r);
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

_boolean pbmBintGreatestCommonDivisor(P_BINT r, P_BINT a, P_BINT b)
{
	BINT Q = { 0 };
	if (pbkIsNotANumber(a) || pbkIsNotANumber(b) || pbkIsNotANumber(r))
		return FALSE;
	else
	{
		pbkInitBint(&Q, 0);

		while (!(1 == GETFLAG(b) && !*b->data))
		{
			if (!pbkDivideBint(&Q, r, a, b))
				goto Lbl_Failed;

			if (!pbkMoveBint(a, b))
				goto Lbl_Failed;

			if (!pbkMoveBint(b, r))
				goto Lbl_Failed;
		}
		if (!pbkMoveBint(r, a))
			goto Lbl_Failed;

		pbkFreeBint(&Q);

		return TRUE;
	}
Lbl_Failed:
	pbkFreeBint(&Q);
	return FALSE;
}
