/**
 * CSE103-OS: Global Descriptor Table (GDT) Management
 * Sets up 32-bit flat memory model: 4GB Kernel Code and 4GB Kernel Data segments,
 * reloading GDT and segment registers via a naked far jump.
 */

#include "gdt.h"

#define GDT_ENTRIES 3

static gdt_entry_t gdt_entries[GDT_ENTRIES];
static gdt_ptr_t   gdt_ptr;

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (uint16_t)(base & 0xFFFF);
    gdt_entries[num].base_middle = (uint8_t)((base >> 16) & 0xFF);
    gdt_entries[num].base_high   = (uint8_t)((base >> 24) & 0xFF);

    gdt_entries[num].limit_low   = (uint16_t)(limit & 0xFFFF);
    gdt_entries[num].granularity = (uint8_t)((limit >> 16) & 0x0F);

    gdt_entries[num].granularity |= (uint8_t)(gran & 0xF0);
    gdt_entries[num].access      = access;
}

void gdt_flush(uint32_t gdt_ptr_addr) __attribute__((naked));
void gdt_flush(uint32_t gdt_ptr_addr) {
    (void)gdt_ptr_addr;
    /*
     * Loading the Global Descriptor Table (GDT) and reloading the segment registers
     * requires the privileged lgdt instruction and a far jump to reload the CS register.
     * Plain C cannot directly manipulate CPU segment registers or execute far jumps.
     */
    __asm__ volatile (
        "movl 4(%%esp), %%eax\n\t"
        "lgdt (%%eax)\n\t"
        "movw $0x10, %%ax\n\t"
        "movw %%ax, %%ds\n\t"
        "movw %%ax, %%es\n\t"
        "movw %%ax, %%fs\n\t"
        "movw %%ax, %%gs\n\t"
        "movw %%ax, %%ss\n\t"
        "ljmp $0x08, $1f\n\t"
        "1:\n\t"
        "ret\n\t"
        :
        :
        : "memory"
    );
}

void gdt_init(void) {
    gdt_ptr.limit = (uint16_t)(sizeof(gdt_entry_t) * GDT_ENTRIES - 1);
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    /* 0x00: Null segment descriptor required by x86 architecture */
    gdt_set_gate(0, 0, 0, 0, 0);

    /* 0x08: Kernel Code segment: Base 0, Limit 4GB, Ring 0, Exec/Read, 4KB granularity */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* 0x10: Kernel Data segment: Base 0, Limit 4GB, Ring 0, Read/Write, 4KB granularity */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    gdt_flush((uint32_t)&gdt_ptr);
}
