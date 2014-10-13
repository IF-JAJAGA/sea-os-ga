#ifndef SCHED_H
#define SCHED_H

#include <stdlib.h>
#include <stdint.h>

typedef void (*func_t) (void *);

enum state_e {STATE_NEW, STATE_EXECUTING, STATE_PAUSED, STATE_ZOMBIE};

struct pcb_s {
	// Stored in a circular doubly linked list
	struct pcb_s *previous;
	struct pcb_s *next;

	unsigned int  pid;
	enum state_e  state;

	uint8_t      *stack;
	unsigned int  stack_size;

	// First instruction to execute (when started)
	func_t        entry_point;
	// Pointer to the current instruction (lr register)
	func_t        instruction;
	void         *args;
};

// GLOBAL
const unsigned int STACK_WORD_SIZE;
const unsigned int NUMBER_REGISTERS;

void
create_process(func_t f, void *args, unsigned int stack_size);

void
ctx_switch();

void
ctx_switch_from_irq();

void
start_sched();

#endif

