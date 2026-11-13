# CSE103-OS Architecture Specification

## Overview
CSE103-OS is a 32-bit x86 protected-mode operating system written strictly in freestanding C11. It demonstrates bare-metal systems programming, hardware abstraction, CPU memory segmentation and paging, interrupt routing, device drivers, multitasking, and an interactive command shell.

---

## System Subsystems

### 1. Boot Subsystem (`src/kernel/boot.c`)
- **Multiboot 1 Conformance**: Header placed in `.multiboot` section with 4-byte aligned magic `0x1BADB002` and flags `MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO`.
- **Naked Entry Point**: `_start` sets ESP to a dedicated 16KB stack in `.bss`, zeroes EFLAGS, pushes boot parameters (`EBX` and `EAX`), and calls `kmain`.

### 2. Low-Level CPU Tables (`src/arch/x86/`)
- **GDT (`gdt.c`)**: Defines Flat 4GB Kernel Code (0x08) and Kernel Data (0x10) segments. Segment registers and `CS` reloaded via a naked C far jump.
- **IDT (`idt.c`)**: 256 interrupt gates loaded via `lidt`.
- **Exceptions (`isr.c`)**: ISRs 0-31 generated via C preprocessor macros, capturing CPU register state into `registers_t` and dumping diagnostic crash information.

### 3. Interrupts and Hardware Drivers (`src/arch/x86/`, `src/drivers/`)
- **8259 PIC (`pic.c`)**: Remaps IRQs 0-15 to vectors 32-47.
- **PIT Timer (`timer.c`)**: 8253/8254 timer on IRQ 0 configured for 100 Hz.
- **PS/2 Keyboard (`keyboard.c`)**: Decodes Scancode Set 1 on IRQ 1, enqueuing printable characters into a thread-safe circular buffer.
- **Serial COM1 (`serial.c`)**: Configured at 115200 baud (8N1). Provides boot logging and feeds the shared shell input stream.
- **VGA Driver (`vga.c`)**: 80x25 text mode frame buffer at 0xB8000 with hardware cursor tracking and automatic scrolling.

### 4. Memory Management (`src/mm/`)
- **PMM (`pmm.c`)**: Parses Multiboot memory map, managing 4KB physical frames with a bitmap allocator.
- **Paging (`paging.c`)**: Hardware two-level paging identity-mapping the first 16MB of RAM. Page faults (vector 14) are handled by reading CR2.
- **Kernel Heap (`heap.c`)**: First-fit dynamic allocator providing `kmalloc`, `kfree`, and `krealloc` with 8-byte alignment and block coalescing.

### 5. Multitasking & File System (`src/arch/x86/context.c`, `src/kernel/fs.c`)
- **Multitasking**: Thread Control Blocks (TCBs) managed by a round-robin scheduler. Low-level context switching performed via a naked C function swapping stack pointers.
- **RAM-Disk (`ramfs`)**: In-memory file system storing embedded course guides and documentation.

### 6. Interactive Command Shell (`src/shell/shell.c`)
- Unified command line reading input from both keyboard and COM1 serial port.
- Supported commands: `help`, `clear`, `echo`, `uptime`, `meminfo`, `reboot`, `halt`, `tasks`, `ls`, `cat`, `test`.
