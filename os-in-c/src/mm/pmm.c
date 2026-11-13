/**
 * CSE103-OS: Physical Memory Manager (PMM)
 * Multiboot memory map parser and bitmap page-frame allocator for 4KB frames.
 */

#include "pmm.h"
#include "string.h"
#include "kprintf.h"

extern uint32_t kernel_end; /* Exported by linker.ld */

static uint32_t *pmm_bitmap = NULL;
static uint32_t total_memory_bytes = 0;
static uint32_t total_frames = 0;
static uint32_t used_frames = 0;

#define BITMAP_INDEX(frame) ((frame) / 32)
#define BITMAP_OFFSET(frame) ((frame) % 32)

static inline void set_frame_bit(uint32_t frame) {
    pmm_bitmap[BITMAP_INDEX(frame)] |= (1U << BITMAP_OFFSET(frame));
}

static inline void clear_frame_bit(uint32_t frame) {
    pmm_bitmap[BITMAP_INDEX(frame)] &= ~(1U << BITMAP_OFFSET(frame));
}

static inline bool test_frame_bit(uint32_t frame) {
    return (pmm_bitmap[BITMAP_INDEX(frame)] & (1U << BITMAP_OFFSET(frame))) != 0;
}

static int32_t first_free_frame(void) {
    uint32_t num_words = (total_frames + 31) / 32;
    for (uint32_t i = 0; i < num_words; i++) {
        if (pmm_bitmap[i] != 0xFFFFFFFF) {
            for (int bit = 0; bit < 32; bit++) {
                uint32_t frame = i * 32 + (uint32_t)bit;
                if (frame >= total_frames) return -1;
                if (!(pmm_bitmap[i] & (1U << bit))) {
                    return (int32_t)frame;
                }
            }
        }
    }
    return -1;
}

void *pmm_alloc_frame(void) {
    int32_t frame = first_free_frame();
    if (frame == -1) {
        return NULL; /* Out of physical memory */
    }
    set_frame_bit((uint32_t)frame);
    used_frames++;
    return (void *)((uint32_t)frame * PAGE_SIZE);
}

void pmm_free_frame(void *paddr) {
    uint32_t addr = (uint32_t)paddr;
    uint32_t frame = addr / PAGE_SIZE;
    if (frame < total_frames && test_frame_bit(frame)) {
        clear_frame_bit(frame);
        if (used_frames > 0) used_frames--;
    }
}

uint32_t pmm_get_total_frames(void) {
    return total_frames;
}

uint32_t pmm_get_free_frames(void) {
    return total_frames - used_frames;
}

uint32_t pmm_get_used_frames(void) {
    return used_frames;
}

uint32_t pmm_get_total_memory_kb(void) {
    return total_memory_bytes / 1024;
}

void pmm_init(multiboot_info_t *mbi) {
    /* Place physical bitmap immediately after the kernel image in RAM */
    uint32_t kernel_end_addr = (uint32_t)&kernel_end;
    pmm_bitmap = (uint32_t *)((kernel_end_addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));

    uint64_t max_usable_addr = 0;

    if (mbi && (mbi->flags & MULTIBOOT_INFO_MEM_MAP)) {
        uint32_t mmap_addr = mbi->mmap_addr;
        uint32_t mmap_end  = mmap_addr + mbi->mmap_length;

        while (mmap_addr < mmap_end) {
            multiboot_mmap_entry_t *entry = (multiboot_mmap_entry_t *)mmap_addr;
            if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                uint64_t region_end = entry->addr + entry->len;
                if (region_end > max_usable_addr) {
                    max_usable_addr = region_end;
                }
            }
            mmap_addr += entry->size + sizeof(entry->size);
        }
    }

    if (max_usable_addr == 0) {
        /* Fallback if mmap not present: assume 32MB standard QEMU default */
        max_usable_addr = 32 * 1024 * 1024;
    }

    /* Cap at 32-bit physical boundary (4GB) */
    if (max_usable_addr > 0xFFFFFFFFULL) {
        max_usable_addr = 0xFFFFFFFFULL;
    }

    total_memory_bytes = (uint32_t)max_usable_addr;
    total_frames = total_memory_bytes / PAGE_SIZE;
    used_frames = total_frames;

    /* Initially mark all frames as used */
    uint32_t bitmap_size_bytes = (total_frames + 7) / 8;
    memset(pmm_bitmap, 0xFF, bitmap_size_bytes);

    /* Mark available memory ranges as free */
    if (mbi && (mbi->flags & MULTIBOOT_INFO_MEM_MAP)) {
        uint32_t mmap_addr = mbi->mmap_addr;
        uint32_t mmap_end  = mmap_addr + mbi->mmap_length;

        while (mmap_addr < mmap_end) {
            multiboot_mmap_entry_t *entry = (multiboot_mmap_entry_t *)mmap_addr;
            if (entry->type == MULTIBOOT_MEMORY_AVAILABLE && entry->addr < max_usable_addr) {
                uint32_t start_frame = (uint32_t)(entry->addr / PAGE_SIZE);
                uint32_t end_frame = (uint32_t)((entry->addr + entry->len) / PAGE_SIZE);
                if (end_frame > total_frames) end_frame = total_frames;

                for (uint32_t f = start_frame; f < end_frame; f++) {
                    clear_frame_bit(f);
                    used_frames--;
                }
            }
            mmap_addr += entry->size + sizeof(entry->size);
        }
    } else {
        /* Fallback: free from 1MB to max_usable_addr */
        for (uint32_t f = 256; f < total_frames; f++) {
            clear_frame_bit(f);
            used_frames--;
        }
    }

    /* Reserve first 1MB (BIOS, IVT, BDA, VGA Buffer at 0xB8000) */
    for (uint32_t f = 0; f < 256; f++) {
        if (!test_frame_bit(f)) {
            set_frame_bit(f);
            used_frames++;
        }
    }

    /* Reserve kernel image and the bitmap itself */
    uint32_t bitmap_end_addr = (uint32_t)pmm_bitmap + bitmap_size_bytes;
    uint32_t reserved_end_frame = (bitmap_end_addr + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint32_t f = 256; f < reserved_end_frame && f < total_frames; f++) {
        if (!test_frame_bit(f)) {
            set_frame_bit(f);
            used_frames++;
        }
    }
}
