#include "sched.h"
#include "phyAlloc.h"

// GLOBAL
const unsigned int STACK_WORD_SIZE = sizeof(void *); // 4 on ARM
const unsigned int NUMBER_REGISTERS = 13;

// STATIC

// Initialized to NULL (when not allocated)
static struct pcb_s *current_ctx = NULL;
static struct pcb_s *init_ctx    = NULL;

static unsigned int process_count = 0;

static void
start_current_process()
{
	current_ctx->state = STATE_EXECUTING;

	current_ctx->entry_point(current_ctx->args);

	// Setting to STATE_ZOMBIE for later deallocation
	current_ctx->state = STATE_ZOMBIE;

	ctx_switch();
}

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
	pcb->stack -= NUMBER_REGISTERS * STACK_WORD_SIZE;

	pcb->instruction = start_current_process;
	pcb->entry_point = f;
	pcb->args = args;

	return pcb;
}

static void
free_process(struct pcb_s *zombie)
{
	// Deallocating
	phyAlloc_free(zombie->stack, zombie->stack_size);
	phyAlloc_free(zombie, sizeof(zombie));
}

static void
elect()
{
	struct pcb_s *first = current_ctx;

	if (STATE_EXECUTING == current_ctx->state) {
		current_ctx->state = STATE_PAUSED;
	}
	// Switching to the next element in the circular list
	current_ctx = current_ctx->next;

	if (STATE_ZOMBIE == current_ctx->state) {
		while (STATE_ZOMBIE == current_ctx->state && current_ctx != first)
		{
			// Deleting current_ctx from the list
			current_ctx->previous->next = current_ctx->next;
			current_ctx->next->previous = current_ctx->previous;

			struct pcb_s *next = current_ctx->next;

			// Deallocating the memory of the ZOMBIE
			free_process(current_ctx);

			// Switching to the next element
			current_ctx = next;
		}

		if (STATE_ZOMBIE == current_ctx->state) {
			// There are no process that wants to execute anything
			// TOO MANY ZOMBIES!!

			// We tell init_ctx to free the last zombie
			init_ctx->next = current_ctx;
			current_ctx = init_ctx;
		}
	}

	else {
		// There is at least one process that wants to execute something
		current_ctx->state = STATE_EXECUTING;
	}
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

	// Restoring context
	__asm("mov sp, %0" : : "r" (current_ctx->stack));
	__asm("mov lr, %0" : : "r" (current_ctx->instruction));
	__asm("pop {r0-r12}");
	__asm("bx      lr" );
}

__irq
void
ctx_switch_from_irq()
{
	
}

void
start_sched(unsigned int stack_size)
{
	set_tick_and_enable_timer();
	init_ctx = init_pcb(NULL, NULL, stack_size);

	init_ctx->next = current_ctx;
	current_ctx = init_ctx;

	ctx_switch();
	free_process(current_ctx->next);
}

void
end_sched()
{
	free_process(init_ctx);
}

