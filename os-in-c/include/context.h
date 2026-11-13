/**
 * CSE103-OS: Multitasking & Context Switching
 * Thread Control Blocks (TCB), round-robin scheduler, and naked C context switch.
 */

#ifndef CSE103_CONTEXT_H
#define CSE103_CONTEXT_H

#include "types.h"

typedef enum {
    TASK_STATE_READY,
    TASK_STATE_RUNNING,
    TASK_STATE_SLEEPING,
    TASK_STATE_TERMINATED
} task_state_t;

typedef struct task {
    uint32_t id;
    char name[32];
    uint32_t esp;
    uint32_t *stack_base;
    task_state_t state;
    uint32_t sleep_until_tick;
    struct task *next;
} task_t;

void tasking_init(void);
task_t *task_create(const char *name, void (*entry)(void));
void task_yield(void);
void task_sleep(uint32_t ticks);
void task_exit(void);
task_t *task_get_current(void);
void task_schedule(void);
void task_list(void);

/* Low-level CPU context switch written as a naked C function */
void context_switch(uint32_t *old_esp, uint32_t new_esp) __attribute__((naked));

#endif /* CSE103_CONTEXT_H */
