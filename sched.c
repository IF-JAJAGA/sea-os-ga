#include "sched.h"
#include "phyAlloc.h"

// GLOBAL
const unsigned int STACK_WORD_SIZE = sizeof(void *); // 4 on ARM
const unsigned int NUMBER_REGISTERS = 13;

void
init_ctx(struct ctx_s * ctx, func_t f, unsigned int stack_size)
{
	static unsigned int process_count = 0;

	ctx->pid = process_count++;
	ctx->stack = (uint8_t *) phyAlloc_alloc(stack_size);

	// Positioning the pointer to the bottom of the stack (highest address)
	ctx->stack += stack_size - STACK_WORD_SIZE;
	// Leaving space for the registers
	ctx->stack -= NUMBER_REGISTERS;

	ctx->instruction = f;
}

void
__attribute__((naked)) switch_to(struct ctx_s * ctx)
{
	// Saving current context
	__asm("push {r0-r12}");
	__asm("mov %0, lr" : "= r" (current_ctx->instruction));
	__asm("mov %0, sp" : "= r" (current_ctx->stack));

	// Restoring to given context
	current_ctx = ctx;

	__asm("mov sp, %0" : : "r" (current_ctx->stack));
	__asm("mov lr, %0" : : "r" (current_ctx->instruction));
	__asm("pop {r0-r12}");
	__asm("bx      lr" );
}

