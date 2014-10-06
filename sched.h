#ifndef SCHED_H
#define SCHED_H

#include <stdlib.h>
#include <stdint.h>

typedef void (*func_t) (void);

struct ctx_s {
	// Stored in a circular doubly linked list
	struct ctx_s *previous;
	struct ctx_s *next;

	unsigned int  pid;

	uint8_t      *stack;

	// Pointer to the current instruction (lr register)
	func_t        instruction;
};

// GLOBAL
const unsigned int STACK_WORD_SIZE;
const unsigned int NUMBER_REGISTERS;

void
create_process(func_t f, void *args, unsigned int stack_size);

void
ctx_switch();

void
start_sched();

#endif

