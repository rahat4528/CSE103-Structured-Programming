/**
 * CSE103-OS: Interrupt Service Routines (ISRs) and CPU Exceptions
 * Defines the register frame pushed on interrupt and exception dispatch signatures.
 */

#ifndef CSE103_ISR_H
#define CSE103_ISR_H

#include "types.h"

typedef struct registers {
    uint32_t ds;                                     /* Data segment selector */
    uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax; /* Pushed by pusha */
    uint32_t int_no, err_code;                       /* Interrupt number and error code */
    uint32_t eip, cs, eflags, useresp, ss;           /* Pushed by CPU automatically */
} registers_t;

typedef void (*isr_handler_t)(registers_t *regs);

void isr_init(void);
void isr_register_handler(uint8_t int_no, isr_handler_t handler);
void isr_handler(registers_t *regs);

#endif /* CSE103_ISR_H */
