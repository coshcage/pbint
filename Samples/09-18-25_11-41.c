#include <stdio.h>
#include "pbm.h"

int main()
{
	P_BINT pc;
	P_BNUM p;

	pc = pbkCreateBint(0);
	p = pbkCreateBnum(10);

	pbmUbFactorial(pc, 2000); // 2000!

	pbkBintToDecimalBnum(p, pc);

	pbkPrintBnum(p);
	
	printf("\n");
	
	pbkDeleteBint(pc);
	pbkDeleteBnum(p);

	return 0;
}

