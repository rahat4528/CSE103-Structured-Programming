/**
 * CSE103-OS: Kernel Heap Allocator
 * Dynamic memory allocation: kmalloc, kfree, krealloc with boundary-tag tracking.
 */

#ifndef CSE103_HEAP_H
#define CSE103_HEAP_H

#include "types.h"

#define KERNEL_HEAP_START 0x00800000 /* 8 MB */
#define KERNEL_HEAP_SIZE  0x00800000 /* 8 MB */

typedef struct heap_block {
    size_t size;
    bool is_free;
    struct heap_block *next;
} heap_block_t;

void  heap_init(void);
void *kmalloc(size_t size);
void  kfree(void *ptr);
void *krealloc(void *ptr, size_t size);

size_t heap_get_allocated_bytes(void);
size_t heap_get_free_bytes(void);

#endif /* CSE103_HEAP_H */
