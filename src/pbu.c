﻿/*
 * Name:        pbu.c
 * Description: Portable big integer library utilities.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0930252246B0930252300L00133
 * License:     GPLv3.
 */

#include <string.h> /* Using function memcpy. */
#include "pbu.h"

/* Function name: pbuDecimalSzToBint
 * Description:   Decimal zero terminated string to big integer.
 * Parameters:
 *        pbi Pointer to a big integer.
 *        str Pointer to zero terminated string..
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 */
_boolean pbuDecimalSzToBint(P_BINT pbi, const char * str)
{
	BNUM n = { 0 };
	
	if (!pbkInitBnum(&n, 10))
		return FALSE;
	
	if (!pbkDecimalSzToBnum(&n, str))
	{
		pbkFreeBnum(&n);
		return FALSE;
	}
		
	if (!pbkDecimalBnumToBint(pbi, &n))
	{
		pbkFreeBnum(&n);
		return FALSE;
	}
		
	pbkFreeBnum(&n);
	
	return TRUE;
}

/* Function name: pbuFPrintDecimalBint
 * Description:   Print a big integer to a file.
 * Parameters:
 *         fp Pointer to a file.
 *        pbi Pointer to a big integer.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 */
_boolean pbuFPrintDecimalBint(FILE * fp, P_BINT pbi)
{
	BNUM n = { 0 };
	
	if (!pbkInitBnum(&n, 10))
		return FALSE;
	
	if (!pbkBintToDecimalBnum(&n, pbi))
	{
		pbkFreeBnum(&n);
		return FALSE;
	}
	
	if (pbkIsNotANumber(pbi))
		fprintf(fp, "NaN");
	else
	{
		register size_t i;
		
		if (GETFLAG(pbi) < 0)
			fputc('-', fp);
		
		for (i = GETABS(GETFLAG(&n)); i > 0; --i)
			fputc('0' + n.data[i - 1], fp);
	}
	
	pbkFreeBnum(&n);
	return TRUE;
}

/* Function name: pbuPrintDecimalBint
 * Description:   Print a big integer to a stdout.
 * Parameter:
 *       pbi Pointer to a big integer.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 */
_boolean pbuPrintDecimalBint(P_BINT pbi)
{
	return pbuFPrintDecimalBint(stdout, pbi);
}

/* Function name: pbuSPrintDecimalBint
 * Description:   Print a big integer to a character string.
 * Parameters:
 *        str Pointer to a string.
 *        pbi Pointer to a big integer.
 * Return value:  TRUE:  Succeeded.
 *                FALSE: Failed.
 */
_boolean pbuSPrintDecimalBint(char * str, P_BINT pbi)
{
	BNUM n = { 0 };
	
	if (!pbkInitBnum(&n, 10))
		return FALSE;
	
	if (!pbkBintToDecimalBnum(&n, pbi))
	{
		pbkFreeBnum(&n);
		return FALSE;
	}
	
	if (pbkIsNotANumber(pbi))
		memcpy(str, "NaN", 4);
	else
	{
		register size_t i;
		
		if (GETFLAG(pbi) < 0)
			*str++ = '-';
		
		for (i = GETABS(GETFLAG(&n)); i > 0; --i)
			*str++ = (char)'0' + n.data[i - 1];
		
		*str = '\0';
	}
	
	pbkFreeBnum(&n);
	return TRUE;
}

