/**
 * CSE103-OS: Kernel Heap Allocator
 * Dynamic memory allocation: kmalloc, kfree, krealloc with 8-byte alignment,
 * block splitting, and adjacent block coalescing.
 */

#include "heap.h"
#include "string.h"

#define ALIGN8(s) (((s) + 7) & ~7)

static heap_block_t *heap_head = NULL;
static size_t heap_allocated_bytes = 0;

void heap_init(void) {
    heap_head = (heap_block_t *)KERNEL_HEAP_START;
    heap_head->size = KERNEL_HEAP_SIZE - sizeof(heap_block_t);
    heap_head->is_free = true;
    heap_head->next = NULL;
    heap_allocated_bytes = 0;
}

void *kmalloc(size_t size) {
    if (size == 0) return NULL;
    size = ALIGN8(size);

    heap_block_t *curr = heap_head;
    while (curr != NULL) {
        if (curr->is_free && curr->size >= size) {
            /* Split block if remainder is large enough for a new block header + data */
            if (curr->size >= size + sizeof(heap_block_t) + 8) {
                heap_block_t *split = (heap_block_t *)((uint8_t *)curr + sizeof(heap_block_t) + size);
                split->size = curr->size - size - sizeof(heap_block_t);
                split->is_free = true;
                split->next = curr->next;

                curr->size = size;
                curr->next = split;
            }
            curr->is_free = false;
            heap_allocated_bytes += curr->size;
            return (void *)((uint8_t *)curr + sizeof(heap_block_t));
        }
        curr = curr->next;
    }

    return NULL; /* Out of kernel heap memory */
}

static void heap_coalesce(void) {
    heap_block_t *curr = heap_head;
    while (curr != NULL && curr->next != NULL) {
        if (curr->is_free && curr->next->is_free) {
            curr->size += sizeof(heap_block_t) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void kfree(void *ptr) {
    if (!ptr) return;

    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    if (!block->is_free) {
        block->is_free = true;
        if (heap_allocated_bytes >= block->size) {
            heap_allocated_bytes -= block->size;
        }
        heap_coalesce();
    }
}

void *krealloc(void *ptr, size_t new_size) {
    if (!ptr) return kmalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }

    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    if (block->size >= new_size) {
        return ptr;
    }

    void *new_ptr = kmalloc(new_size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, block->size);
        kfree(ptr);
    }
    return new_ptr;
}

size_t heap_get_allocated_bytes(void) {
    return heap_allocated_bytes;
}

size_t heap_get_free_bytes(void) {
    if (KERNEL_HEAP_SIZE > heap_allocated_bytes) {
        return KERNEL_HEAP_SIZE - heap_allocated_bytes;
    }
    return 0;
}
