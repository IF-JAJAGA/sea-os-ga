#ifndef SCHED_H
#define SCHED_H

typedef void (*func_t) (void);

struct ctx_s {
	unsigned int  pid;
	unsigned int *stack;

	// Pointer to the current instruction (lr register)
	func_t        instruction;
};

// GLOBAL
struct ctx_s *current_ctx;

void
init_ctx(struct ctx_s* ctx, func_t f, unsigned int stack_size);

void
switch_to(struct ctx_s * ctx);

#endif

