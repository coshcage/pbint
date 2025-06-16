#include <stdio.h>
#include <Windows.h>
#include "pbm.h"

int main()
{
	P_BINT pc;
	P_BNUM p;
	LARGE_INTEGER la, lb, lc;

	pc = pbkCreateBint(0);
	p = pbkCreateBnum(10);

	QueryPerformanceFrequency(&lc);
	QueryPerformanceCounter(&la);

	pbmUbFactorial(pc, 2000); // 2000!

	QueryPerformanceCounter(&lb);
	printf("%lf\n", (double)(lb.QuadPart - la.QuadPart) / (double)lc.QuadPart);

	pbkBintToDecimalBnum(p, pc);

	pbkPrintBnum(p);

	return 0;
}

