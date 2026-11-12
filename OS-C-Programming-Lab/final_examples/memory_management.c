/**
 * @file memory_management.c
 * @brief Complete dynamic memory lifecycle demonstrating malloc, calloc, realloc, and free.
 *
 * Concepts Demonstrated:
 *  - Implementing a dynamically resizable integer Vector data structure
 *  - Initializing with calloc() for zeroed metadata
 *  - Dynamic capacity doubling using realloc() with defensive pointer caching
 *  - Clean deallocation and dangling pointer nullification via free()
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} IntVector;

// Vector initialization using malloc + calloc
IntVector *vector_create(size_t initial_capacity) {
    if (initial_capacity == 0) {
        fprintf(stderr, "[Error] Initial vector capacity must be greater than zero.\n");
        return NULL;
    }

    IntVector *vec = (IntVector *)malloc(sizeof(IntVector));
    if (vec == NULL) {
        perror("Failed to allocate vector container");
        return NULL;
    }

    // Allocate zero-initialized memory for buffer
    vec->data = (int *)calloc(initial_capacity, sizeof(int));
    if (vec->data == NULL) {
        perror("Failed to allocate vector buffer");
        free(vec);
        return NULL;
    }

    vec->size = 0;
    vec->capacity = initial_capacity;
    return vec;
}

// Vector append with dynamic reallocation and overflow check
int vector_push(IntVector *vec, int value) {
    if (vec == NULL) return -1;

    // Expand buffer if capacity is reached
    if (vec->size >= vec->capacity) {
        if (vec->capacity > (SIZE_MAX / 2) / sizeof(int)) {
            fprintf(stderr, "[Error] Vector capacity overflow!\n");
            return -1;
        }
        size_t new_capacity = vec->capacity * 2;
        printf("    [Vector Notice] Expanding capacity: %zu -> %zu elements...\n",
               vec->capacity, new_capacity);

        int *temp = (int *)realloc(vec->data, new_capacity * sizeof(int));
        if (temp == NULL) {
            fprintf(stderr, "[Error] realloc failed during vector expansion!\n");
            return -1;
        }

        vec->data = temp;
        vec->capacity = new_capacity;
    }

    vec->data[vec->size++] = value;
    return 0;
}

// Vector deallocation
void vector_free(IntVector **vec_ptr) {
    if (vec_ptr == NULL || *vec_ptr == NULL) return;

    IntVector *vec = *vec_ptr;
    if (vec->data != NULL) {
        free(vec->data);
        vec->data = NULL;
    }

    free(vec);
    *vec_ptr = NULL; // Defend caller against dangling pointer
}

int main(void) {
    printf("====================================================\n");
    printf(" Integrated OS Example: Dynamic Memory Lifecycle    \n");
    printf("====================================================\n\n");

    const size_t initial_cap = 2;
    printf("[1] Creating dynamic vector with initial capacity = %zu (using malloc + calloc)...\n",
           initial_cap);

    IntVector *my_vec = vector_create(initial_cap);
    if (my_vec == NULL) {
        return EXIT_FAILURE;
    }

    printf("    Vector created. Buffer Base: %p, Capacity: %zu, Size: %zu\n\n",
           (void *)my_vec->data, my_vec->capacity, my_vec->size);

    printf("[2] Inserting 6 elements sequentially to trigger dynamic reallocations:\n");
    int elements[] = {10, 25, 40, 55, 70, 85};
    int count = (int)(sizeof(elements) / sizeof(elements[0]));

    for (int i = 0; i < count; i++) {
        printf("  Pushing element: %d\n", elements[i]);
        if (vector_push(my_vec, elements[i]) != 0) {
            fprintf(stderr, "Push failed for %d\n", elements[i]);
            vector_free(&my_vec);
            return EXIT_FAILURE;
        }
    }

    printf("\n[3] Final Vector Contents (Size = %zu, Capacity = %zu):\n  [ ",
           my_vec->size, my_vec->capacity);
    for (size_t i = 0; i < my_vec->size; i++) {
        printf("%d%s", my_vec->data[i], (i == my_vec->size - 1) ? "" : ", ");
    }
    printf(" ]\n\n");

    printf("[4] Reclaiming all dynamic memory via vector_free()...\n");
    vector_free(&my_vec);

    if (my_vec == NULL) {
        printf("    Vector successfully deallocated. Pointer reset to NULL.\n");
        printf("    Zero memory leaks remaining.\n");
    }

    printf("====================================================\n");
    return EXIT_SUCCESS;
}
