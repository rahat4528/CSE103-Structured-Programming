/**
 * CSE103-OS: Hardware Interrupt Request (IRQ) Stubs and Dispatcher
 * Dispatches hardware IRQs 0-15 to registered subsystem handlers and manages EOI.
 */

#include "irq.h"
#include "pic.h"
#include "idt.h"
#include "gdt.h"

static isr_handler_t irq_handlers[16];

void irq_register_handler(uint8_t irq, isr_handler_t handler) {
    if (irq < 16) {
        irq_handlers[irq] = handler;
        pic_clear_mask(irq);
    }
}

void irq_handler(registers_t *regs) {
    uint8_t irq_num = (uint8_t)(regs->int_no - 32);

    if (irq_num < 16 && irq_handlers[irq_num] != NULL) {
        irq_handlers[irq_num](regs);
    }

    pic_send_eoi(irq_num);
}

void irq_common_stub(void) __attribute__((naked));
void irq_common_stub(void) {
    /*
     * Common IRQ dispatch stub:
     * Saves registers and segment selectors, invokes C IRQ handler,
     * restores state, and executes iret.
     * Plain C cannot execute pusha/popa or the x86 iret instruction.
     */
    __asm__ volatile (
        "pusha\n\t"
        "pushl %ds\n\t"
        "movw $0x10, %ax\n\t"
        "movw %ax, %ds\n\t"
        "movw %ax, %es\n\t"
        "movw %ax, %fs\n\t"
        "movw %ax, %gs\n\t"
        "pushl %esp\n\t"
        "call irq_handler\n\t"
        "addl $4, %esp\n\t"
        "popl %eax\n\t"
        "movw %ax, %ds\n\t"
        "movw %ax, %es\n\t"
        "movw %ax, %fs\n\t"
        "movw %ax, %gs\n\t"
        "popa\n\t"
        "addl $8, %esp\n\t"
        "iret\n\t"
    );
}

#define DECLARE_IRQ(n, vector) \
    void irq##n(void) __attribute__((naked)); \
    void irq##n(void) { \
        /* Hardware IRQ stub: push dummy 0 error code, push vector, jump to common stub */ \
        __asm__ volatile ( \
            "pushl $0\n\t" \
            "pushl $" #vector "\n\t" \
            "jmp irq_common_stub\n\t" \
        ); \
    }

DECLARE_IRQ(0, 32)
DECLARE_IRQ(1, 33)
DECLARE_IRQ(2, 34)
DECLARE_IRQ(3, 35)
DECLARE_IRQ(4, 36)
DECLARE_IRQ(5, 37)
DECLARE_IRQ(6, 38)
DECLARE_IRQ(7, 39)
DECLARE_IRQ(8, 40)
DECLARE_IRQ(9, 41)
DECLARE_IRQ(10, 42)
DECLARE_IRQ(11, 43)
DECLARE_IRQ(12, 44)
DECLARE_IRQ(13, 45)
DECLARE_IRQ(14, 46)
DECLARE_IRQ(15, 47)

void irq_init(void) {
    for (int i = 0; i < 16; i++) {
        irq_handlers[i] = NULL;
    }

    idt_set_gate(32, (uint32_t)irq0,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(33, (uint32_t)irq1,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(34, (uint32_t)irq2,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(35, (uint32_t)irq3,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(36, (uint32_t)irq4,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(37, (uint32_t)irq5,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(38, (uint32_t)irq6,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(39, (uint32_t)irq7,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(40, (uint32_t)irq8,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(41, (uint32_t)irq9,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(42, (uint32_t)irq10, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(43, (uint32_t)irq11, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(44, (uint32_t)irq12, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(45, (uint32_t)irq13, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(46, (uint32_t)irq14, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(47, (uint32_t)irq15, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
}
