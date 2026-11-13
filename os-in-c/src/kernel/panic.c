/**
 * CSE103-OS: Kernel Panic and Diagnostic Dump Routine
 * Freezes CPU safely after outputting diagnostic alerts to VGA and COM1.
 */

#include "panic.h"
#include "kprintf.h"
#include "vga.h"
#include "serial.h"
#include "io.h"

void panic(const char *message) {
    /* Disable interrupts to stop further execution and timer interrupts */
    cli();

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    kprintf("\n\n");
    kprintf("****************************************************************\n");
    kprintf("                    KERNEL PANIC: SYSTEM HALTED                 \n");
    kprintf("****************************************************************\n");
    kprintf("Reason: %s\n\n", message ? message : "Unspecified kernel condition");
    kprintf("System has been safely halted to prevent corruption.\n");
    kprintf("Please reboot your computer.\n");
    kprintf("****************************************************************\n");

    serial_puts("\n[PANIC] Kernel panic: ");
    if (message) serial_puts(message);
    serial_puts("\n[PANIC] CPU halted.\n");

    while (1) {
        hlt();
    }
}
