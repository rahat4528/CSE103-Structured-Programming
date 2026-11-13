/*
 * CSE103-OS: Linker Script
 * Positions the kernel at 1MB physical memory, ensuring the .multiboot section
 * is placed within the first 8192 bytes as mandated by the Multiboot specification.
 */

ENTRY(_start)

SECTIONS
{
    /* The kernel starts at 1MB physical address */
    . = 1M;

    /* Multiboot header and executable code */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
        *(.text.*)
    }

    /* Read-only data sections */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)
    }

    /* Initialized read-write data sections */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
        *(.data.*)
    }

    /* Uninitialized read-write data (BSS) and initial kernel stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }

    /* Symbol indicating the end of the loaded kernel binary */
    . = ALIGN(4K);
    kernel_end = .;
}
