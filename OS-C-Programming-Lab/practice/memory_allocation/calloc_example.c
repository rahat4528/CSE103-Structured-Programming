/**
 * @file calloc_example.c
 * @brief Dynamic heap memory allocation using calloc() with zero-initialization.
 *
 * Concepts Demonstrated:
 *  - Allocating contiguous memory blocks with automatic zero initialization
 *  - Comparing calloc() semantics with malloc()
 *  - Safe traversal, modification, and freeing of calloc() allocated arrays
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("====================================================\n");
    printf("      Zero-Initialized Allocation: calloc()         \n");
    printf("====================================================\n\n");

    const size_t num_elements = 6;

    printf("[1] Requesting calloc() for %zu integers (%zu bytes total)...\n",
           num_elements, num_elements * sizeof(int));

    // calloc takes (number of elements, size of each element)
    int *array = (int *)calloc(num_elements, sizeof(int));

    if (array == NULL) {
        fprintf(stderr, "[Error] calloc failed to allocate memory.\n");
        return EXIT_FAILURE;
    }

    printf("    Allocation successful. Base Address: %p\n\n", (void *)array);

    // Verify and display zero-initialization
    printf("[2] Inspecting initial contents immediately after calloc():\n");
    int all_zeros = 1;
    for (size_t i = 0; i < num_elements; i++) {
        printf("    array[%zu] = %d\n", i, array[i]);
        if (array[i] != 0) {
            all_zeros = 0;
        }
    }

    if (all_zeros) {
        printf("    --> Verified: All %zu elements were automatically initialized to 0!\n\n", 
               num_elements);
    }

    // Populate with meaningful data (multiples of 10)
    printf("[3] Populating array with multiples of 10:\n");
    for (size_t i = 0; i < num_elements; i++) {
        array[i] = (int)((i + 1) * 10);
        printf("    array[%zu] updated to %d\n", i, array[i]);
    }

    // Free the allocated memory
    printf("\n[4] Releasing allocated memory via free()...\n");
    free(array);
    array = NULL;

    printf("    Memory released cleanly. Pointer set to NULL.\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
