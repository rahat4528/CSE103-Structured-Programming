/**
 * CSE103-OS: Kernel Panic and Crash Diagnostics
 * Freezes the CPU safely and outputs diagnostics to VGA and COM1 serial.
 */

#ifndef CSE103_PANIC_H
#define CSE103_PANIC_H

void panic(const char *message) __attribute__((noreturn));

#endif /* CSE103_PANIC_H */
