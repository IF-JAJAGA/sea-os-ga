#include "sched.h"
#include "phyAlloc.h"

void
init_ctx(struct ctx_s * ctx, func_t f, unsigned int stack_size)
{
	static unsigned int process_count = 0;

	ctx->pid = process_count++;
	ctx->stack = (unsigned int *) phyAlloc_alloc(sizeof(unsigned int) * stack_size);
	ctx->instruction = f;
}

void
__attribute__((naked)) switch_to(struct ctx_s * ctx)
{
	// Saving current context
	__asm("mov %0, lr" : "= r" (current_ctx->instruction));
	__asm("mov %0, sp" : "= r" (current_ctx->stack));

	// Switching to given context
	current_ctx = ctx;
	__asm("mov sp, %0" : : "r" (current_ctx->stack));
	__asm("bx      %0" : : "r" (current_ctx->instruction));
}

