/**
 * @file realloc_example.c
 * @brief Dynamic memory reallocation and expansion using realloc().
 *
 * Concepts Demonstrated:
 *  - Expanding an existing dynamic heap allocation
 *  - Defensive memory programming: preserving original pointer if realloc() fails
 *  - Observing whether memory was expanded in-place or relocated to a new address
 *  - Proper deallocation of resized memory blocks
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("====================================================\n");
    printf("     Dynamic Memory Resizing: realloc() Demo        \n");
    printf("====================================================\n\n");

    size_t initial_capacity = 3;
    printf("[1] Initial allocation with malloc() for %zu integers...\n", initial_capacity);

    int *scores = (int *)malloc(initial_capacity * sizeof(int));
    if (scores == NULL) {
        perror("Initial malloc failed");
        return EXIT_FAILURE;
    }

    printf("    Initial buffer address: %p\n", (void *)scores);

    // Populate initial items
    scores[0] = 85;
    scores[1] = 92;
    scores[2] = 78;

    printf("[2] Initial array contents:\n");
    for (size_t i = 0; i < initial_capacity; i++) {
        printf("    scores[%zu] = %d\n", i, scores[i]);
    }

    // Resize array to hold 6 integers
    size_t expanded_capacity = 6;
    printf("\n[3] Resizing array from %zu to %zu integers using realloc()...\n",
           initial_capacity, expanded_capacity);

    /*
     * DEFENSIVE BEST PRACTICE:
     * Never assign realloc directly to the original pointer:
     *     scores = realloc(scores, ...); // WRONG: leaks if realloc returns NULL!
     * Always use a temporary pointer.
     */
    int *temp = (int *)realloc(scores, expanded_capacity * sizeof(int));
    if (temp == NULL) {
        fprintf(stderr, "[Error] realloc failed! Retaining original allocation.\n");
        free(scores);
        return EXIT_FAILURE;
    }

    // Assign safe reallocated pointer
    scores = temp;
    printf("    Resizing successful!\n");
    printf("    New buffer address    : %p\n", (void *)scores);

    // Append newly available elements
    scores[3] = 95;
    scores[4] = 88;
    scores[5] = 100;

    printf("\n[4] Displaying full expanded array contents:\n");
    for (size_t i = 0; i < expanded_capacity; i++) {
        printf("    scores[%zu] = %3d  (Element address: %p)\n", 
               i, scores[i], (void *)&scores[i]);
    }

    // Cleanup
    printf("\n[5] Freeing entire reallocated block...\n");
    free(scores);
    scores = NULL;

    printf("    Deallocation complete. Memory safely reclaimed.\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
