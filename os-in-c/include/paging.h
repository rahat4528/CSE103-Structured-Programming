/**
 * CSE103-OS: Virtual Memory and Paging
 * Two-level x86 paging: page directory, page tables, CR0/CR3 manipulation,
 * identity mapping, and page-fault exception handling.
 */

#ifndef CSE103_PAGING_H
#define CSE103_PAGING_H

#include "types.h"
#include "isr.h"

#define PAGE_PRESENT  0x01
#define PAGE_WRITE    0x02
#define PAGE_USER     0x04

#define NUM_PAGE_DIRS 1024
#define NUM_PAGE_ENTRIES 1024

void paging_init(void);
void page_fault_handler(registers_t *regs);

#endif /* CSE103_PAGING_H */
