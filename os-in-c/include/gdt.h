/**
 * CSE103-OS: Global Descriptor Table (GDT) Management
 * Defines GDT segment descriptors and reloading routines.
 */

#ifndef CSE103_GDT_H
#define CSE103_GDT_H

#include "types.h"

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

typedef struct gdt_entry gdt_entry_t;
typedef struct gdt_ptr   gdt_ptr_t;

#define GDT_KERNEL_CODE_SEG 0x08
#define GDT_KERNEL_DATA_SEG 0x10

void gdt_init(void);

#endif /* CSE103_GDT_H */
