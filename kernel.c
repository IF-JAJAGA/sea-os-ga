#include <stdlib.h>

#include "hw.h"
#include "sched.h"

const unsigned int ROUT_NB = 2;
const unsigned int STACK_SIZE = 1024;

void
funcA(void *a)
{
	int cptA = 0;

	while (cptA < 84) {
		cptA += 42;
		ctx_switch();
	}
}

void
funcB(void *a)
{
	int cptB = 1;

	while (cptB < 9) {
		cptB += 2;
		ctx_switch();
	}
}

//------------------------------------------------------------------------

int
kmain ( void )
{
	// Initialize material
	init_hw();

	// Initialize all ctx
	create_process(funcA, NULL, STACK_SIZE);
	create_process(funcB, NULL, STACK_SIZE);

	start_sched(STACK_SIZE);
	ctx_switch();

	return 0;
}

