/**
 * CSE103-OS: Interrupt Service Routines (ISRs) and CPU Exceptions
 * Implements macro-generated naked ISR stubs 0-31, register state dumping,
 * and dispatching to custom C exception handlers.
 */

#include "isr.h"
#include "idt.h"
#include "kprintf.h"
#include "panic.h"
#include "gdt.h"

static isr_handler_t interrupt_handlers[256];

static const char *exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

void isr_register_handler(uint8_t int_no, isr_handler_t handler) {
    interrupt_handlers[int_no] = handler;
}

void isr_handler(registers_t *regs) {
    if (interrupt_handlers[regs->int_no] != NULL) {
        interrupt_handlers[regs->int_no](regs);
        return;
    }

    /* Unhandled CPU Exception: Display readable diagnostic crash dump and panic */
    kprintf("\n================ [CPU EXCEPTION PANIC] ================\n");
    kprintf("Exception %d: %s (Error Code: %u / 0x%x)\n",
            regs->int_no,
            (regs->int_no < 32) ? exception_messages[regs->int_no] : "Unknown",
            regs->err_code, regs->err_code);
    kprintf("CS:EIP  = 0x%x:0x%x\n", regs->cs, regs->eip);
    kprintf("EFLAGS  = 0x%x\n", regs->eflags);
    kprintf("EAX: 0x%x  EBX: 0x%x  ECX: 0x%x  EDX: 0x%x\n",
            regs->eax, regs->ebx, regs->ecx, regs->edx);
    kprintf("ESI: 0x%x  EDI: 0x%x  EBP: 0x%x  ESP: 0x%x\n",
            regs->esi, regs->edi, regs->ebp, regs->esp_dummy);
    kprintf("DS:  0x%x\n", regs->ds);
    kprintf("=========================================================\n");

    panic("Fatal unhandled CPU exception");
}

void isr_common_stub(void) __attribute__((naked));
void isr_common_stub(void) {
    /*
     * Common ISR dispatch stub:
     * Saves general registers and segment selectors, invokes C exception handler,
     * restores state, and executes iret to resume or fault.
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
        "call isr_handler\n\t"
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

/* Macro to generate ISR naked stub without CPU error code */
#define DECLARE_ISR_NOERR(n) \
    void isr##n(void) __attribute__((naked)); \
    void isr##n(void) { \
        /* Plain C cannot create interrupt entry stubs or push error codes */ \
        __asm__ volatile ( \
            "pushl $0\n\t" \
            "pushl $" #n "\n\t" \
            "jmp isr_common_stub\n\t" \
        ); \
    }

/* Macro to generate ISR naked stub with CPU error code */
#define DECLARE_ISR_ERR(n) \
    void isr##n(void) __attribute__((naked)); \
    void isr##n(void) { \
        /* Plain C cannot create interrupt entry stubs */ \
        __asm__ volatile ( \
            "pushl $" #n "\n\t" \
            "jmp isr_common_stub\n\t" \
        ); \
    }

DECLARE_ISR_NOERR(0)
DECLARE_ISR_NOERR(1)
DECLARE_ISR_NOERR(2)
DECLARE_ISR_NOERR(3)
DECLARE_ISR_NOERR(4)
DECLARE_ISR_NOERR(5)
DECLARE_ISR_NOERR(6)
DECLARE_ISR_NOERR(7)
DECLARE_ISR_ERR(8)
DECLARE_ISR_NOERR(9)
DECLARE_ISR_ERR(10)
DECLARE_ISR_ERR(11)
DECLARE_ISR_ERR(12)
DECLARE_ISR_ERR(13)
DECLARE_ISR_ERR(14)
DECLARE_ISR_NOERR(15)
DECLARE_ISR_NOERR(16)
DECLARE_ISR_ERR(17)
DECLARE_ISR_NOERR(18)
DECLARE_ISR_NOERR(19)
DECLARE_ISR_NOERR(20)
DECLARE_ISR_NOERR(21)
DECLARE_ISR_NOERR(22)
DECLARE_ISR_NOERR(23)
DECLARE_ISR_NOERR(24)
DECLARE_ISR_NOERR(25)
DECLARE_ISR_NOERR(26)
DECLARE_ISR_NOERR(27)
DECLARE_ISR_NOERR(28)
DECLARE_ISR_NOERR(29)
DECLARE_ISR_ERR(30)
DECLARE_ISR_NOERR(31)

void isr_init(void) {
    for (int i = 0; i < 256; i++) {
        interrupt_handlers[i] = NULL;
    }

    idt_set_gate(0,  (uint32_t)isr0,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(1,  (uint32_t)isr1,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(2,  (uint32_t)isr2,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(3,  (uint32_t)isr3,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(4,  (uint32_t)isr4,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(5,  (uint32_t)isr5,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(6,  (uint32_t)isr6,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(7,  (uint32_t)isr7,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(8,  (uint32_t)isr8,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(9,  (uint32_t)isr9,  GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(10, (uint32_t)isr10, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(11, (uint32_t)isr11, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(12, (uint32_t)isr12, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(13, (uint32_t)isr13, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(14, (uint32_t)isr14, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(15, (uint32_t)isr15, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(16, (uint32_t)isr16, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(17, (uint32_t)isr17, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(18, (uint32_t)isr18, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(19, (uint32_t)isr19, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(20, (uint32_t)isr20, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(21, (uint32_t)isr21, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(22, (uint32_t)isr22, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(23, (uint32_t)isr23, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(24, (uint32_t)isr24, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(25, (uint32_t)isr25, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(26, (uint32_t)isr26, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(27, (uint32_t)isr27, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(28, (uint32_t)isr28, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(29, (uint32_t)isr29, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(30, (uint32_t)isr30, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
    idt_set_gate(31, (uint32_t)isr31, GDT_KERNEL_CODE_SEG, IDT_GATE_INTERRUPT32);
}
