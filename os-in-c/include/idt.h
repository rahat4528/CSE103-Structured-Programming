/**
 * CSE103-OS: Interrupt Descriptor Table (IDT) Structures and Management
 * Configures the 256-entry x86 interrupt gate table.
 */

#ifndef CSE103_IDT_H
#define CSE103_IDT_H

#include "types.h"

struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

typedef struct idt_entry idt_entry_t;
typedef struct idt_ptr   idt_ptr_t;

#define IDT_NUM_ENTRIES 256
#define IDT_GATE_INTERRUPT32 0x8E

void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif /* CSE103_IDT_H */
