/**
 * CSE103-OS: Hardware Port I/O and Low-Level CPU Control
 * Provides port communication routines (inb, outb, inw, outw), delay loops,
 * and interrupt flag management (cli, sti, hlt).
 */

#ifndef CSE103_IO_H
#define CSE103_IO_H

#include "types.h"

/*
 * x86 I/O port architecture uses a separate address space accessed via
 * dedicated CPU instructions (in/out). Standard C has no language operators
 * or memory semantics for this address space, requiring inline assembly.
 */

static inline void outb(uint16_t port, uint8_t val) {
    /* Write single byte to x86 I/O port via outb instruction */
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    /* Read single byte from x86 I/O port via inb instruction */
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    /* Write 16-bit word to x86 I/O port via outw instruction */
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    /* Read 16-bit word from x86 I/O port via inw instruction */
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_wait(void) {
    /*
     * Writing to unused diagnostic port 0x80 inserts a 1-4 microsecond hardware delay
     * needed for legacy PIC bus settle times. Plain C has no cycle-accurate delay primitive.
     */
    outb(0x80, 0x00);
}

static inline void cli(void) {
    /* Disables CPU maskable interrupts by clearing the IF flag in EFLAGS */
    __asm__ volatile ("cli" : : : "memory");
}

static inline void sti(void) {
    /* Enables CPU maskable interrupts by setting the IF flag in EFLAGS */
    __asm__ volatile ("sti" : : : "memory");
}

static inline void hlt(void) {
    /* Halts the CPU until the next external interrupt occurs, reducing power and heat */
    __asm__ volatile ("hlt" : : : "memory");
}

#endif /* CSE103_IO_H */
