/**
 * @file pthread_join.c
 * @brief Demonstrates multi-threading concurrency and result retrieval using pthread_join().
 *
 * Concepts Demonstrated:
 *  - Spawning multiple concurrent worker threads
 *  - Safe parameter passing to individual threads
 *  - Returning dynamic results from threads via pthread_exit / return
 *  - Synchronizing and collecting results using pthread_join()
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 4

typedef struct {
    int thread_id;
    int range_start;
    int range_end;
} WorkerTask;

// Worker routine: calculates sum of integers in [range_start, range_end]
void *calculate_sum(void *arg) {
    WorkerTask *task = (WorkerTask *)arg;
    printf("  [Worker %d] Starting: Summing from %d to %d (TID: %lu)\n",
           task->thread_id, task->range_start, task->range_end,
           (unsigned long)pthread_self());

    long partial_sum = 0;
    for (int i = task->range_start; i <= task->range_end; i++) {
        partial_sum += i;
    }

    // Small delay to simulate varied completion timing
    usleep(50000);

    // Allocate return memory for the result
    long *result = malloc(sizeof(long));
    if (result == NULL) {
        perror("malloc failed in thread");
        pthread_exit(NULL);
    }
    *result = partial_sum;

    printf("  [Worker %d] Finished: Partial Sum = %ld\n", task->thread_id, partial_sum);
    return (void *)result;
}

int main(void) {
    printf("====================================================\n");
    printf("     Multi-Thread Concurrency: pthread_join()       \n");
    printf("====================================================\n\n");

    pthread_t threads[NUM_THREADS];
    WorkerTask tasks[NUM_THREADS];

    // Partition numbers 1..100 across 4 threads
    int slice_size = 25;
    printf("[Main] Spawning %d worker threads to sum numbers 1 through 100...\n", NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        tasks[i].thread_id = i + 1;
        tasks[i].range_start = (i * slice_size) + 1;
        tasks[i].range_end = (i + 1) * slice_size;

        int rc = pthread_create(&threads[i], NULL, calculate_sum, (void *)&tasks[i]);
        if (rc != 0) {
            fprintf(stderr, "Failed to create thread %d: %s\n", i, strerror(rc));
            return EXIT_FAILURE;
        }
    }

    printf("[Main] All %d threads dispatched. Awaiting completion...\n\n", NUM_THREADS);

    long total_sum = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        void *thread_result = NULL;
        int rc = pthread_join(threads[i], &thread_result);
        if (rc != 0) {
            fprintf(stderr, "Failed to join thread %d: %s\n", i, strerror(rc));
            return EXIT_FAILURE;
        }

        if (thread_result != NULL) {
            long partial = *(long *)thread_result;
            total_sum += partial;
            printf("[Main] Joined Thread %d -> Received partial sum = %ld\n", i + 1, partial);
            free(thread_result); // Free dynamically allocated result
        }
    }

    printf("\n====================================================\n");
    printf("  Combined Total Sum of 1..100 : %ld\n", total_sum);
    printf("  Mathematical Check [n(n+1)/2]: %d\n", (100 * 101) / 2);
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
