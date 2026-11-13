/**
 * CSE103-OS: Multitasking & Context Switching
 * Implements Thread Control Blocks (TCB), round-robin task scheduler,
 * and naked C register context switch.
 */

#include "context.h"
#include "heap.h"
#include "string.h"
#include "kprintf.h"
#include "timer.h"
#include "io.h"

#define TASK_STACK_SIZE 4096
#define MAX_TASKS 16

static task_t tasks[MAX_TASKS];
static task_t *current_task = NULL;
static uint32_t next_task_id = 1;
static bool multitasking_enabled = false;

void context_switch(uint32_t *old_esp, uint32_t new_esp) __attribute__((naked));
void context_switch(uint32_t *old_esp, uint32_t new_esp) {
    (void)old_esp;
    (void)new_esp;
    /*
     * Low-level CPU register context switch:
     * 1. Push callee-saved registers (EBP, EBX, ESI, EDI).
     * 2. Save current ESP into *old_esp.
     * 3. Load new_esp into ESP.
     * 4. Pop callee-saved registers (EDI, ESI, EBX, EBP) of target task.
     * 5. Execute ret to jump to target task's saved instruction pointer.
     * Plain C cannot manipulate stack pointers or perform register-level task swaps.
     */
    __asm__ volatile (
        "pushl %ebp\n\t"
        "pushl %ebx\n\t"
        "pushl %esi\n\t"
        "pushl %edi\n\t"
        "movl 20(%esp), %eax\n\t"
        "movl %esp, (%eax)\n\t"
        "movl 24(%esp), %esp\n\t"
        "popl %edi\n\t"
        "popl %esi\n\t"
        "popl %ebx\n\t"
        "popl %ebp\n\t"
        "ret\n\t"
    );
}

void task_exit(void) {
    if (current_task) {
        current_task->state = TASK_STATE_TERMINATED;
        task_yield();
    }
    while (1) {
        hlt();
    }
}

task_t *task_create(const char *name, void (*entry)(void)) {
    int slot = -1;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_STATE_TERMINATED) {
            slot = i;
            break;
        }
    }
    if (slot == -1) return NULL;

    task_t *t = &tasks[slot];
    t->id = next_task_id++;
    strncpy(t->name, name, sizeof(t->name) - 1);
    t->name[sizeof(t->name) - 1] = '\0';
    t->state = TASK_STATE_READY;
    t->sleep_until_tick = 0;

    if (!t->stack_base) {
        t->stack_base = (uint32_t *)kmalloc(TASK_STACK_SIZE);
        if (!t->stack_base) {
            t->state = TASK_STATE_TERMINATED;
            return NULL;
        }
    }

    /*
     * Build initial stack frame for context_switch:
     * Top of stack holds:
     * [task_exit wrapper]
     * [entry function pointer]
     * [ebp = 0]
     * [ebx = 0]
     * [esi = 0]
     * [edi = 0]
     */
    uint32_t *stk = (uint32_t *)((uint8_t *)t->stack_base + TASK_STACK_SIZE);
    *(--stk) = (uint32_t)task_exit;  /* Return address if entry returns */
    *(--stk) = (uint32_t)entry;      /* Return address for context_switch ret */
    *(--stk) = 0;                    /* EBP */
    *(--stk) = 0;                    /* EBX */
    *(--stk) = 0;                    /* ESI */
    *(--stk) = 0;                    /* EDI */

    t->esp = (uint32_t)stk;
    return t;
}

void task_yield(void) {
    if (!multitasking_enabled) return;
    task_schedule();
}

void task_sleep(uint32_t ticks) {
    if (current_task && multitasking_enabled) {
        current_task->sleep_until_tick = timer_get_ticks() + ticks;
        current_task->state = TASK_STATE_SLEEPING;
        task_yield();
    } else {
        timer_sleep_ticks(ticks);
    }
}

task_t *task_get_current(void) {
    return current_task;
}

void task_schedule(void) {
    if (!multitasking_enabled || !current_task) return;

    /* Wake up any sleeping tasks */
    uint32_t now = timer_get_ticks();
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_STATE_SLEEPING && now >= tasks[i].sleep_until_tick) {
            tasks[i].state = TASK_STATE_READY;
        }
    }

    /* Find next READY task */
    int current_idx = (int)(current_task - &tasks[0]);
    int next_idx = -1;

    for (int i = 1; i <= MAX_TASKS; i++) {
        int idx = (current_idx + i) % MAX_TASKS;
        if (tasks[idx].state == TASK_STATE_READY) {
            next_idx = idx;
            break;
        }
    }

    if (next_idx == -1 || next_idx == current_idx) {
        return; /* No other ready task */
    }

    task_t *prev = current_task;
    task_t *next = &tasks[next_idx];

    if (prev->state == TASK_STATE_RUNNING) {
        prev->state = TASK_STATE_READY;
    }
    next->state = TASK_STATE_RUNNING;
    current_task = next;

    context_switch(&prev->esp, next->esp);
}

void task_list(void) {
    kprintf("ID   Name             State      Stack Base\n");
    kprintf("--   ----             -----      ----------\n");
    const char *state_names[] = {"READY", "RUNNING", "SLEEPING", "TERMINATED"};

    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state != TASK_STATE_TERMINATED) {
            kprintf("%-4d %-16s %-10s 0x%x\n",
                    tasks[i].id,
                    tasks[i].name,
                    state_names[tasks[i].state],
                    (uint32_t)tasks[i].stack_base);
        }
    }
}

void tasking_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].id = 0;
        tasks[i].state = TASK_STATE_TERMINATED;
        tasks[i].stack_base = NULL;
    }

    /* Initialize Task 0 as the main kernel thread */
    tasks[0].id = 0;
    strncpy(tasks[0].name, "kernel_main", sizeof(tasks[0].name) - 1);
    tasks[0].state = TASK_STATE_RUNNING;
    tasks[0].stack_base = NULL; /* Uses boot stack */
    tasks[0].esp = 0;

    current_task = &tasks[0];
    multitasking_enabled = true;
}
