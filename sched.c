#include "sched.h"
#include "phyAlloc.h"

// GLOBAL
const unsigned int STACK_WORD_SIZE = sizeof(void *); // 4 on ARM
const unsigned int NUMBER_REGISTERS = 13;

// Initialized to NULL (when not allocated)
static struct ctx_s *current_ctx = NULL;

void
create_process(func_t f, void *args, unsigned int stack_size)
{
	static unsigned int process_count = 0;

	struct ctx_s *new_ctx;
	new_ctx = (struct ctx_s *) phyAlloc_alloc(sizeof(struct ctx_s));

	if (!current_ctx)
	{
		// Single element list
		current_ctx           = new_ctx;
		current_ctx->previous = new_ctx;
		current_ctx->next     = new_ctx;
	}
	else
	{
		// Inserting new_ctx BEFORE current_ctx
		new_ctx->previous = current_ctx->previous;
		new_ctx->next = current_ctx;
		current_ctx->previous->next = new_ctx;
		current_ctx->previous = new_ctx;
	}

	new_ctx->pid = process_count++;
	new_ctx->stack = (uint8_t *) phyAlloc_alloc(stack_size);

	// Positioning the pointer to the bottom of the stack (highest address)
	new_ctx->stack += stack_size - STACK_WORD_SIZE;
	// Leaving space for the registers
	new_ctx->stack -= NUMBER_REGISTERS;

	new_ctx->instruction = f;
}

void
__attribute__((naked)) ctx_switch()
{
	// Saving current context
	__asm("push {r0-r12}");
	__asm("mov %0, lr" : "= r" (current_ctx->instruction));
	__asm("mov %0, sp" : "= r" (current_ctx->stack));

	// Switching to the next process in the circular list
	current_ctx = current_ctx->next;

	__asm("mov sp, %0" : : "r" (current_ctx->stack));
	__asm("mov lr, %0" : : "r" (current_ctx->instruction));
	__asm("pop {r0-r12}");
	__asm("bx      lr" );
}

void
start_sched()
{
	current_ctx->instruction();
}

