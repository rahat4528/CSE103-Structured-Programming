/**
 * CSE103-OS: Virtual Memory and Paging Implementation
 * Sets up 4KB page directory and tables, identity maps the first 16MB of RAM,
 * handles page-fault exceptions (ISR 14), and manages CR0/CR2/CR3 control registers.
 */

#include "paging.h"
#include "pmm.h"
#include "isr.h"
#include "kprintf.h"
#include "panic.h"
#include "string.h"

/* 1024-entry Page Directory, 4KB aligned */
__attribute__((aligned(4096)))
static uint32_t page_directory[NUM_PAGE_DIRS];

/* 4 Page Tables covering 16MB (4 * 1024 * 4KB = 16MB), 4KB aligned */
#define NUM_IDENTITY_TABLES 4
__attribute__((aligned(4096)))
static uint32_t page_tables[NUM_IDENTITY_TABLES][NUM_PAGE_ENTRIES];

void page_fault_handler(registers_t *regs) {
    uint32_t faulting_address;

    /*
     * Reading the x86 CR2 control register to obtain the linear address that caused
     * the page fault exception. Plain C has no mechanism to access CPU control registers.
     */
    __asm__ volatile ("mov %%cr2, %0" : "=r"(faulting_address));

    bool present  = (regs->err_code & 0x1) != 0;
    bool write    = (regs->err_code & 0x2) != 0;
    bool user     = (regs->err_code & 0x4) != 0;
    bool reserved = (regs->err_code & 0x8) != 0;
    bool inst_fetch = (regs->err_code & 0x10) != 0;

    kprintf("\n================ [PAGE FAULT EXCEPTION] ================\n");
    kprintf("Faulting Address: 0x%x\n", faulting_address);
    kprintf("Cause: %s, %s access, in %s mode\n",
            present ? "Page-protection violation" : "Page not present",
            write ? "Write" : "Read",
            user ? "User" : "Kernel");
    if (reserved) kprintf("Reserved bit overwrite detected!\n");
    if (inst_fetch) kprintf("Instruction fetch fault!\n");
    kprintf("EIP: 0x%x  EFLAGS: 0x%x\n", regs->eip, regs->eflags);
    kprintf("========================================================\n");

    panic("Fatal Page Fault");
}

static inline void load_cr3(uint32_t cr3_val) {
    /* Loading page directory physical base address into CPU CR3 register */
    __asm__ volatile ("mov %0, %%cr3" : : "r"(cr3_val) : "memory");
}

static inline void enable_paging(void) {
    uint32_t cr0;
    /*
     * Enabling the x86 hardware paging unit by setting the PG bit (bit 31) in CR0.
     * Plain C has no direct representation of CPU control registers.
     */
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; /* Set PG bit */
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0) : "memory");
}

void paging_init(void) {
    /* Register Page Fault handler on vector 14 */
    isr_register_handler(14, page_fault_handler);

    /* Clear the entire Page Directory */
    for (int i = 0; i < NUM_PAGE_DIRS; i++) {
        /* Set as not present, supervisor read/write */
        page_directory[i] = 0x00000002;
    }

    /*
     * Identity map the first 16MB of physical RAM:
     * 4 page tables, each covering 4MB (1024 pages of 4KB).
     */
    for (int t = 0; t < NUM_IDENTITY_TABLES; t++) {
        for (int p = 0; p < NUM_PAGE_ENTRIES; p++) {
            uint32_t phys_addr = (uint32_t)((t * NUM_PAGE_ENTRIES + p) * PAGE_SIZE);
            page_tables[t][p] = phys_addr | PAGE_PRESENT | PAGE_WRITE;
        }

        /* Put page table into directory */
        page_directory[t] = ((uint32_t)&page_tables[t][0]) | PAGE_PRESENT | PAGE_WRITE;
    }

    /* Load page directory into CR3 */
    load_cr3((uint32_t)&page_directory[0]);

    /* Enable paging in CR0 */
    enable_paging();
}
