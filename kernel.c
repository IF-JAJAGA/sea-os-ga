#include "sched.h"
#include "hw.h"

const unsigned int ROUT_NB = 2;
const unsigned int STACK_SIZE = 10;

struct ctx_s ctx_ping;
struct ctx_s ctx_pong;
struct ctx_s ctx_init;

void
ping()
{
	while ( 1 ) {
		switch_to(&ctx_pong);
	}
}

void
pong()
{
	while ( 1 ) {
		switch_to(&ctx_ping);
	}

}

//------------------------------------------------------------------------

int
kmain ( void )
{
	// Initialize material
	init_hw();

	// Initialize both ctx
	init_ctx(&ctx_ping, ping, STACK_SIZE);
	init_ctx(&ctx_pong, pong, STACK_SIZE);

	current_ctx = &ctx_init;

	switch_to(&ctx_ping);

	return 0;
}

