#include "sched.h"
#include "phyAlloc.h"

// GLOBAL
const unsigned int STACK_WORD_SIZE = sizeof(void *); // 4 on ARM
const unsigned int NUMBER_REGISTERS = 13;

// STATIC

// Initialized to NULL (when not allocated)
static struct pcb_s *current_ctx = NULL;
static struct pcb_s *idle_ctx    = NULL;

static unsigned int process_count = 0;

static struct pcb_s *
init_pcb(func_t f, void *args, unsigned int stack_size)
{
	struct pcb_s *pcb = (struct pcb_s *) phyAlloc_alloc(sizeof(struct pcb_s));

	pcb->pid = process_count++;
	pcb->state = STATE_NEW;
	pcb->stack = (uint8_t *) phyAlloc_alloc(stack_size);
	pcb->stack_size = stack_size;

	// Positioning the pointer to the bottom of the stack (highest address)
	pcb->stack += stack_size - STACK_WORD_SIZE;
	// Leaving space for the registers
	pcb->stack -= NUMBER_REGISTERS;

	pcb->instruction = f;
	pcb->args = args;

	return pcb;
}

static void
start_current_process()
{
	current_ctx->state = STATE_EXECUTING;

	current_ctx->instruction(current_ctx->args);

	// Setting to STATE_ZOMBIE for later deallocation
	current_ctx->state = STATE_ZOMBIE;
}

static void
elect()
{
	// Switching to the next element in the circular list
	current_ctx->state = STATE_IDLE;
	current_ctx = current_ctx->next;

	if (STATE_ZOMBIE == current_ctx->state)
	{
		// Deleting current_ctx from the list
		current_ctx->previous->next = current_ctx->next;
		current_ctx->next->previous = current_ctx->previous;

		// Deallocating
		phyAlloc_free(current_ctx->stack, current_ctx->stack_size);
		phyAlloc_free(current_ctx, sizeof(current_ctx));

		// Switching to the next element
		current_ctx = current_ctx->next;
	}
	else if (STATE_NEW == current_ctx->state)
	{
		start_current_process();
	}

	current_ctx->state = STATE_EXECUTING;
}

//------------------------------------------------------------------------

void
create_process(func_t f, void *args, unsigned int stack_size)
{
	struct pcb_s *new_ctx = init_pcb(f, args, stack_size);

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
}

void
__attribute__((naked)) ctx_switch()
{
	// Saving current context
	__asm("push {r0-r12}");
	__asm("mov %0, lr" : "= r" (current_ctx->instruction));
	__asm("mov %0, sp" : "= r" (current_ctx->stack));

	// Electing the next current_ctx
	elect();

	__asm("mov sp, %0" : : "r" (current_ctx->stack));
	__asm("mov lr, %0" : : "r" (current_ctx->instruction));
	__asm("pop {r0-r12}");
	__asm("bx      lr" );
}

void
start_sched(unsigned int stack_size)
{
	idle_ctx = init_pcb(NULL, NULL, stack_size);
	idle_ctx->state = STATE_IDLE;

	idle_ctx->next = current_ctx;

	current_ctx = idle_ctx;
}

