/**
 * CSE103-OS: Interrupt Descriptor Table (IDT) Implementation
 * Configures the 256-entry x86 interrupt descriptor table and loads IDTR via lidt.
 */

#include "idt.h"
#include "string.h"

static idt_entry_t idt_entries[IDT_NUM_ENTRIES];
static idt_ptr_t   idt_ptr;

static void idt_load(uint32_t idt_ptr_addr) {
    /*
     * Loading the Interrupt Descriptor Table (IDT) requires the privileged x86 lidt instruction.
     * Plain C has no register concept for CPU IDTR.
     */
    __asm__ volatile ("lidt (%0)" : : "r"(idt_ptr_addr) : "memory");
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_low  = (uint16_t)(base & 0xFFFF);
    idt_entries[num].base_high = (uint16_t)((base >> 16) & 0xFFFF);
    idt_entries[num].sel       = sel;
    idt_entries[num].always0   = 0;
    idt_entries[num].flags     = flags;
}

void idt_init(void) {
    idt_ptr.limit = (uint16_t)(sizeof(idt_entry_t) * IDT_NUM_ENTRIES - 1);
    idt_ptr.base  = (uint32_t)&idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t) * IDT_NUM_ENTRIES);

    idt_load((uint32_t)&idt_ptr);
}
