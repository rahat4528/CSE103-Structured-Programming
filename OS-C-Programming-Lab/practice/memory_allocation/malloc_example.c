/**
 * @file malloc_example.c
 * @brief Dynamic heap memory allocation using malloc() and safe deallocation with free().
 *
 * Concepts Demonstrated:
 *  - Requesting uninitialized memory blocks on the heap
 *  - Checking for memory allocation failure (NULL pointer check)
 *  - Accessing allocated heap memory via pointer indexing
 *  - Releasing resources to prevent memory leaks and zeroing dangling pointers
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("====================================================\n");
    printf("      Dynamic Memory Allocation: malloc() & free()  \n");
    printf("====================================================\n\n");

    const int element_count = 5;
    size_t total_bytes = element_count * sizeof(int);

    printf("[1] Requesting heap memory for %d integers (%zu bytes)...\n", 
           element_count, total_bytes);

    int *numbers = (int *)malloc(total_bytes);

    // Always verify the pointer is non-NULL
    if (numbers == NULL) {
        fprintf(stderr, "[Error] Memory allocation failed! Out of memory.\n");
        return EXIT_FAILURE;
    }

    printf("    Allocation successful. Heap address: %p\n\n", (void *)numbers);

    printf("[2] Populating array with square values:\n");
    for (int i = 0; i < element_count; i++) {
        numbers[i] = (i + 1) * (i + 1);
        printf("    numbers[%d] = %2d  (Stored at address: %p)\n", 
               i, numbers[i], (void *)&numbers[i]);
    }

    int sum = 0;
    for (int i = 0; i < element_count; i++) {
        sum += numbers[i];
    }
    printf("\n[3] Computed sum of array elements: %d\n", sum);

    printf("\n[4] Deallocating heap memory using free()...\n");
    free(numbers);
    numbers = NULL; // Defend against dangling pointer access

    printf("    Memory successfully freed. Pointer reset to NULL (%p).\n", (void *)numbers);
    printf("====================================================\n");

    return EXIT_SUCCESS;
}

