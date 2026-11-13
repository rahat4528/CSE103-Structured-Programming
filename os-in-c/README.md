# CSE103-OS: 32-Bit x86 Freestanding C Operating System

A clean, modular, bare-metal 32-bit x86 protected-mode operating system written strictly in **C11 (freestanding)** without external C runtimes or standalone assembly files (`.s`/`.asm`).

---

## 1. Features by Milestone

| Milestone | Subsystem | Description | Status |
|---|---|---|---|
| **Milestone 1** | **Boot & Output** | Multiboot 1 header, naked C entry point `_start`, 16KB stack, VGA 80x25 text mode driver, COM1 115200 baud serial UART, fixed boot marker `[BOOT] CSE103-OS kernel started`. | **DONE** |
| **Milestone 2** | **CPU Tables & Exceptions** | GDT with flat 4GB code/data segments and naked far-jump reload, 256-entry IDT via `lidt`, macro-generated ISR stubs (0–31), register state dump, kernel panic routine. | **DONE** |
| **Milestone 3** | **Interrupts & Drivers** | 8259 PIC remapped to vectors 32–47, IRQ stubs 0–15, PIT timer at 100 Hz (`sleep_ticks`, `uptime`), PS/2 keyboard driver (Scancode Set 1 with Shift/Backspace), unified input queue. | **DONE** |
| **Milestone 4** | **Physical Memory** | Multiboot memory map parser, bitmap page-frame allocator (4KB pages), frame allocation/freeing, memory statistics reporting. | **DONE** |
| **Milestone 5** | **Virtual Memory & Heap** | Hardware two-level paging (16MB identity mapped), CR0/CR2/CR3 management, Page Fault handler (ISR 14), 8MB kernel heap (`kmalloc`, `kfree`, `krealloc`). | **DONE** |
| **Milestone 6** | **Interactive Shell** | Dual-input command interpreter (Keyboard + Serial COM1), line buffer editing, extensible command table: `help`, `clear`, `echo`, `uptime`, `meminfo`, `reboot`, `halt`, `test`. | **DONE** |
| **Milestone 7** | **Tasking & RAM-Disk** | Round-robin multitasking with naked C `context_switch`, in-memory RAM disk (`ramfs`) with embedded course files, commands `tasks`, `ls`, `cat`. | **DONE** |

---

## 2. Prerequisites & Installation

### Windows Subsystem for Linux (WSL Ubuntu - Recommended)
```bash
sudo apt update
sudo apt install -y build-essential gcc-multilib qemu-system-x86 grub-pc-bin grub-common xorriso make git python3
```
*(Optional standalone cross-compiler: `sudo apt install -y gcc-i686-linux-gnu binutils-i686-linux-gnu`)*

### Native Windows (MSYS2 / Windows CLI)
1. **QEMU**: `winget install --id SoftwareFreedomConservancy.QEMU -e`
2. **Make**: `winget install GnuWin32.Make -e`
3. **i686-elf Cross Compiler**: Download prebuilt binaries from [lordmilko/i686-elf-tools](https://github.com/lordmilko/i686-elf-tools/releases) and add `bin/` to `PATH`.

---

## 3. Build, Run, Test & Debug

From within the `os-in-c/` directory:

- **Build Kernel ELF**:
  ```bash
  make
  ```
- **Build Bootable GRUB ISO**:
  ```bash
  make iso
  ```
- **Run Interactively in QEMU (VGA Window + Serial stdio)**:
  ```bash
  make run
  ```
- **Run Direct Kernel in QEMU**:
  ```bash
  make run-kernel
  ```
- **Run Headless Automated Verification Tests**:
  ```bash
  make test
  ```
- **Debug with GDB**:
  ```bash
  make debug
  # In another terminal: gdb build/kernel.elf -ex "target remote localhost:1234"
  ```
- **Clean Build Artifacts**:
  ```bash
  make clean
  ```

---

## 4. Project Structure

```
os-in-c/
├── README.md               # Operating system documentation and verification guide
├── Makefile                # Unified build, test, and emulation targets
├── linker.ld               # Multiboot ELF link script (kernel mapped at 1MB)
├── .gitignore              # Ignores build/, *.o, *.elf, *.iso
├── boot/
│   └── grub/
│       └── grub.cfg        # GRUB Multiboot bootloader configuration
├── include/
│   ├── types.h             # Freestanding primitive integer and boolean types
│   ├── io.h                # Hardware port I/O (inb, outb, inw, outw, cli, sti, hlt)
│   ├── multiboot.h         # Multiboot 1 specification structures and flags
│   ├── vga.h               # VGA text driver definitions and color attributes
│   ├── serial.h            # COM1 UART 16550 serial definitions
│   ├── string.h            # Freestanding string and memory library declarations
│   ├── kprintf.h           # Formatted dual-output printf routine
│   ├── gdt.h               # Global Descriptor Table definitions
│   ├── idt.h               # Interrupt Descriptor Table definitions
│   ├── isr.h               # Interrupt service routine frame & dispatch signatures
│   ├── pic.h               # 8259 PIC initialization and EOI management
│   ├── irq.h               # Hardware IRQ vector mappings (32-47)
│   ├── timer.h             # 8253/8254 PIT timer driver
│   ├── keyboard.h          # PS/2 keyboard driver & circular input buffer
│   ├── pmm.h               # Physical memory manager declarations
│   ├── paging.h            # Hardware paging structures and CR0/CR3 controls
│   ├── heap.h              # Dynamic kernel heap allocator (kmalloc/kfree)
│   ├── context.h           # Multitasking TCBs and naked context switch
│   ├── fs.h                # In-memory RAM disk declarations
│   ├── shell.h             # Interactive shell command interpreter
│   ├── panic.h             # Kernel panic diagnostic routine
│   └── kmain.h             # Kernel entry point signature
├── src/
│   ├── kernel/
│   │   ├── boot.c          # Multiboot header struct + naked _start entry point
│   │   ├── kmain.c         # Boot orchestrator and system loop
│   │   ├── panic.c         # Panic alert formatter and CPU freezer
│   │   └── fs.c            # RAM-disk file system implementation
│   ├── arch/x86/
│   │   ├── gdt.c           # GDT setup with naked far-jump segment reload
│   │   ├── idt.c           # IDT table setup and lidt instruction wrapper
│   │   ├── isr.c           # Macro-generated naked ISR stubs 0-31
│   │   ├── pic.c           # 8259 PIC remapping and masking
│   │   ├── irq.c           # Hardware IRQ stubs 0-15 and dispatching
│   │   └── context.c       # Round-robin scheduler and naked context switch
│   ├── drivers/
│   │   ├── vga.c           # 80x25 text mode buffer and cursor controller
│   │   ├── serial.c        # COM1 serial transmitter and receiver
│   │   ├── timer.c         # PIT 100 Hz tick handler and sleep routines
│   │   └── keyboard.c      # PS/2 keyboard interrupt handler & decoder
│   ├── mm/
│   │   ├── pmm.c           # Physical memory bitmap frame allocator
│   │   ├── paging.c        # Paging identity mapper and Page Fault ISR
│   │   └── heap.c          # First-fit aligned heap allocator
│   ├── lib/
│   │   ├── string.c        # memcpy, memset, memcmp, memmove, strlen, strcmp, etc.
│   │   └── kprintf.c       # kprintf and ksnprintf formatted output
│   └── shell/
│       └── shell.c         # Dual keyboard/serial shell with built-in commands
└── docs/
    ├── architecture.md     # Detailed architecture documentation
    └── memory-map.md       # Complete physical and virtual memory map
```

---

## 5. Strict C-Only Policy & Assembly Exception Registry

Per project requirements, zero standalone assembly files (`.s`/`.S`/`.asm`) are used. All low-level x86 CPU interactions are written as `__attribute__((naked))` C functions or static inline asm wrappers, each carrying an explicit rationale comment:

1. **`src/kernel/boot.c` (`_start`)**:
   - *Reason*: Bare-metal entry point called directly by GRUB. ESP must be initialized to the kernel stack before any standard C function prologue executes.
2. **`include/io.h` (`inb`, `outb`, `inw`, `outw`, `cli`, `sti`, `hlt`)**:
   - *Reason*: Standard C has no language operators for x86 I/O port address space or interrupt flag manipulation.
3. **`src/arch/x86/gdt.c` (`gdt_flush`)**:
   - *Reason*: Loading GDT requires the privileged `lgdt` instruction; reloading the code segment selector (`CS = 0x08`) requires a far jump (`ljmp`).
4. **`src/arch/x86/idt.c` (`idt_load`)**:
   - *Reason*: Loading the IDT requires the privileged `lidt` instruction.
5. **`src/arch/x86/isr.c` & `src/arch/x86/irq.c` (`isr_common_stub`, `irq_common_stub`, macro stubs)**:
   - *Reason*: Interrupt service routines must save all CPU registers (`pusha`), load kernel data segments, invoke C handlers, restore registers (`popa`), and return via the privileged `iret` instruction.
6. **`src/mm/paging.c` (`load_cr3`, `enable_paging`, `page_fault_handler`)**:
   - *Reason*: Manipulating CPU control registers (CR0 for paging enable, CR2 for faulting address, CR3 for page directory base) cannot be expressed in standard C.
7. **`src/arch/x86/context.c` (`context_switch`)**:
   - *Reason*: CPU thread context switching requires direct stack pointer manipulation and register swapping across execution contexts.

---

## 6. Design Decisions

1. **Freestanding C11**: Compiled with `-ffreestanding -nostdlib -fno-builtin -fno-stack-protector -Wall -Wextra -Werror`.
2. **Unified Input Architecture**: Both PS/2 keyboard keystrokes and COM1 serial UART bytes feed into a single circular input buffer (`input_queue`), allowing headless test harnesses (`make test`) and interactive GUI sessions (`make run`) to share identical shell behavior.
3. **16MB Paging Identity Map**: The first 16MB of physical RAM is identity-mapped during boot. This guarantees that VGA display memory (`0xB8000`), BIOS data structures, kernel code/data, the PMM bitmap, and the initial 8MB kernel heap operate without virtual address translation overhead during early kernel phases.
4. **First-Fit Dynamic Heap with Coalescing**: `kmalloc` uses an 8-byte aligned linked-list header model that automatically splits larger blocks on allocation and coalesces adjacent free blocks on `kfree`.

---

## 7. Verification Record

- **Headless Verification (`make test`)**:
  - Automatically launches QEMU with `-kernel build/kernel.elf -display none -serial stdio`.
  - Captures and verifies the required fixed boot marker: `[BOOT] CSE103-OS kernel started`.
  - Sends commands over serial input: `help`, `meminfo`, `uptime`, `test`, `halt`.
  - Verifies command table execution and kernel diagnostics (`[PASS] PMM frame allocator`, `[PASS] Kernel heap`, `[PASS] RAM disk`).
- **Interactive Verification (`make run`)**:
  - Bootable ISO tested via GNU GRUB with 80x25 color text output on VGA.
  - Interactive keyboard typing with Shift and Backspace support.
