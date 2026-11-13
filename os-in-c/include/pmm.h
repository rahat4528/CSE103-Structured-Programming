/**
 * CSE103-OS: Physical Memory Manager (PMM)
 * Multiboot memory map parsing and bitmap page frame allocator (4KB pages).
 */

#ifndef CSE103_PMM_H
#define CSE103_PMM_H

#include "types.h"
#include "multiboot.h"

#define PAGE_SIZE 4096

void     pmm_init(multiboot_info_t *mbi);
void    *pmm_alloc_frame(void);
void     pmm_free_frame(void *paddr);
uint32_t pmm_get_total_frames(void);
uint32_t pmm_get_free_frames(void);
uint32_t pmm_get_used_frames(void);
uint32_t pmm_get_total_memory_kb(void);

#endif /* CSE103_PMM_H */
