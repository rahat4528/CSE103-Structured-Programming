/**
 * CSE103-OS: Multiboot Header and Bare-Metal Entry Point
 * Implements the Multiboot 1 header in section .multiboot and naked C function _start.
 */

#include "types.h"
#include "multiboot.h"
#include "kmain.h"

/*
 * Multiboot 1 header positioned within the first 8192 bytes of the ELF binary
 * by the linker script (section .multiboot).
 */
__attribute__((section(".multiboot"), used))
const struct multiboot_header mb_header = {
    .magic = MULTIBOOT_HEADER_MAGIC,
    .flags = MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO,
    .checksum = (uint32_t)(-(MULTIBOOT_HEADER_MAGIC + (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO))),
    .header_addr = 0,
    .load_addr = 0,
    .load_end_addr = 0,
    .bss_end_addr = 0,
    .entry_addr = 0,
    .mode_type = 0,
    .width = 0,
    .height = 0,
    .depth = 0
};

/* 16 KB Kernel Stack allocated in .bss aligned to 16 bytes */
#define KERNEL_STACK_SIZE 16384
__attribute__((aligned(16)))
static uint8_t initial_kernel_stack[KERNEL_STACK_SIZE];

/*
 * Kernel entry point called directly by the Multiboot bootloader (GRUB).
 * Must be a naked function because standard compiler function prologues push EBP
 * and access the stack, which is undefined until ESP is initialized.
 */
void _start(void) __attribute__((naked, used));
void _start(void) {
    /*
     * CPU register initialization and stack setup:
     * 1. Disable maskable interrupts (cli).
     * 2. Set ESP to the top of initial_kernel_stack.
     * 3. Reset EFLAGS to a known state (0).
     * 4. Push Multiboot info pointer (EBX) and Multiboot magic (EAX) as parameters to kmain.
     * 5. Call kmain.
     * 6. If kmain returns, enter an infinite halt loop.
     * Plain C cannot initialize the stack pointer register (ESP) or read boot registers
     * without a stack frame interfering.
     */
    __asm__ volatile (
        "cli\n\t"
        "movl %0, %%esp\n\t"
        "pushl $0\n\t"
        "popf\n\t"
        "pushl %%ebx\n\t"
        "pushl %%eax\n\t"
        "call kmain\n\t"
        "1:\n\t"
        "hlt\n\t"
        "jmp 1b\n\t"
        :
        : "i"(&initial_kernel_stack[KERNEL_STACK_SIZE])
        : "memory"
    );
}
