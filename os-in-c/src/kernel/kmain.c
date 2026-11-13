/**
 * CSE103-OS: Kernel Main Entry Point
 * Coordinates the full boot sequence: VGA, COM1 serial, GDT, IDT, PIC, PIT,
 * keyboard, PMM, paging, heap, ramfs, multitasking, and the interactive shell.
 */

#include "kmain.h"
#include "vga.h"
#include "serial.h"
#include "kprintf.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "pic.h"
#include "irq.h"
#include "timer.h"
#include "keyboard.h"
#include "pmm.h"
#include "paging.h"
#include "heap.h"
#include "fs.h"
#include "context.h"
#include "shell.h"
#include "io.h"

void kmain(uint32_t magic, multiboot_info_t *mbi) {
    /* Step 1: Initialize early display and serial I/O drivers */
    vga_init();
    serial_init();

    /*
     * Step 2: Milestone 1 Boot Verification
     * Required fixed boot marker for automated test harness detection.
     */
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    kprintf("================================================================\n");
    kprintf("            Hello from CSE103-OS (32-bit x86 Kernel)            \n");
    kprintf("================================================================\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    serial_puts("[BOOT] CSE103-OS kernel started\n");
    kprintf("[INIT] Early VGA text console and COM1 serial initialized.\n");

    /* Verify Multiboot loader handoff */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        kprintf("[WARN] Invalid Multiboot magic: 0x%x (expected 0x%x)\n",
                magic, MULTIBOOT_BOOTLOADER_MAGIC);
    } else {
        kprintf("[INIT] Multiboot handshake verified successfully.\n");
    }

    /* Step 3: Milestone 2 CPU Tables (GDT, IDT, Exception ISRs) */
    kprintf("[INIT] Setting up Global Descriptor Table (GDT)... ");
    gdt_init();
    kprintf("OK\n");

    kprintf("[INIT] Setting up Interrupt Descriptor Table (IDT)... ");
    idt_init();
    kprintf("OK\n");

    kprintf("[INIT] Registering CPU Exception Handlers (ISRs 0-31)... ");
    isr_init();
    kprintf("OK\n");

    /* Step 4: Milestone 3 PIC Remapping, Hardware IRQs & Drivers */
    kprintf("[INIT] Remapping 8259 PIC vectors to 0x20-0x2F... ");
    pic_init();
    irq_init();
    kprintf("OK\n");

    kprintf("[INIT] Initializing PIT Timer (100 Hz)... ");
    timer_init(PIT_DEFAULT_HZ);
    kprintf("OK\n");

    kprintf("[INIT] Initializing PS/2 Keyboard Driver... ");
    keyboard_init();
    kprintf("OK\n");

    /* Step 5: Milestone 4 Physical Memory Manager (PMM) */
    kprintf("[INIT] Initializing Physical Memory Manager (Bitmap)... ");
    pmm_init(mbi);
    kprintf("OK (%u KB RAM available)\n", pmm_get_total_memory_kb());

    /* Step 6: Milestone 5 Virtual Memory Paging and Kernel Heap */
    kprintf("[INIT] Initializing Hardware Paging (CR0/CR3 16MB Identity)... ");
    paging_init();
    kprintf("OK\n");

    kprintf("[INIT] Initializing Kernel Dynamic Heap (8MB)... ");
    heap_init();
    kprintf("OK\n");

    /* Step 7: Milestone 7 RAM-Disk and Multitasking */
    kprintf("[INIT] Initializing In-Memory RAM-Disk File System... ");
    fs_init();
    kprintf("OK\n");

    kprintf("[INIT] Initializing Multitasking Thread Subsystem... ");
    tasking_init();
    kprintf("OK\n");

    /* Enable CPU interrupts */
    kprintf("[INIT] Enabling hardware interrupts (sti)... ");
    sti();
    kprintf("OK\n");

    /* Step 8: Milestone 6 Interactive Command Shell */
    kprintf("\n[BOOT] All CSE103-OS subsystems online. Launching shell...\n");
    shell_init();

    /* Step 9: Kernel Main Execution Loop */
    while (1) {
        shell_update();
        task_yield();
        hlt();
    }
}
