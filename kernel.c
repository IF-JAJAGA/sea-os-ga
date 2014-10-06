#include <stdlib.h>

#include "hw.h"
#include "sched.h"

const unsigned int ROUT_NB = 2;
const unsigned int STACK_SIZE = 512;

struct ctx_s ctx_ping;
struct ctx_s ctx_pong;
struct ctx_s ctx_init;

void
funcA()
{
	int cptA = 0;

	for (;;) {
		cptA += 42;
		ctx_switch();
	}
}

void
funcB()
{
	int cptB = 1;

	for (;;) {
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

	start_sched();
	ctx_switch();

	return 0;
}

