#include <stdlib.h>

#include "hw.h"
#include "sched.h"

const unsigned int ROUT_NB = 2;
const unsigned int STACK_SIZE = 65536;

void
funcA(void *a)
{
	int cptA = 0;

	while (cptA < 84) {
		cptA += 42;
	}
}

void
funcB(void *a)
{
	int cptB = 1;

	while (cptB < 9) {
		cptB += 2;
	}
}

//------------------------------------------------------------------------

int
kmain ( void )
{
	// Initialize hardware
	init_hw();

	// Initialize all ctx
	create_process(funcA, NULL, STACK_SIZE);
	create_process(funcB, NULL, STACK_SIZE);

	start_sched(STACK_SIZE);
	end_sched();

	return 0;
}

