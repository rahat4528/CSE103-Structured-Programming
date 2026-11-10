/**
 * @file semaphore_example.c
 * @brief Demonstration of POSIX Counting Semaphores using sem_init, sem_wait, and sem_post.
 *
 * Concepts Demonstrated:
 *  - Initializing unnamed POSIX semaphores via sem_init()
 *  - Regulating concurrent access to a finite pool of resources
 *  - Decrementing semaphore with sem_wait() and incrementing with sem_post()
 *  - Destroying semaphores via sem_destroy()
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 5
#define MAX_CONCURRENT_ACCESS 2

// Global semaphore and active tracker
static sem_t resource_semaphore;
static int active_in_resource = 0;
static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg) {
    int id = *(int *)arg;

    printf("[Thread %d] Arrived and waiting for semaphore permit...\n", id);

    // Acquire permit (decrement semaphore)
    if (sem_wait(&resource_semaphore) != 0) {
        perror("sem_wait failed");
        return NULL;
    }

    // Track active threads inside resource
    pthread_mutex_lock(&count_mutex);
    active_in_resource++;
    int current_active = active_in_resource;
    pthread_mutex_unlock(&count_mutex);

    printf("  >>> [Thread %d] ACQUIRED PERMIT! (Currently active in resource: %d / %d)\n",
           id, current_active, MAX_CONCURRENT_ACCESS);

    // Simulate work inside the limited resource
    usleep(200000); // 200 ms

    pthread_mutex_lock(&count_mutex);
    active_in_resource--;
    pthread_mutex_unlock(&count_mutex);

    printf("  <<< [Thread %d] Releasing permit and exiting resource...\n", id);

    // Release permit (increment semaphore)
    if (sem_post(&resource_semaphore) != 0) {
        perror("sem_post failed");
    }

    return NULL;
}

int main(void) {
    printf("====================================================\n");
    printf("     POSIX Counting Semaphore: semaphore_example.c  \n");
    printf("====================================================\n\n");

    printf("[Init] Initializing semaphore with %d permits for %d competing threads...\n",
           MAX_CONCURRENT_ACCESS, NUM_THREADS);

    // sem_init(sem_t *sem, int pshared, unsigned int value)
    // pshared = 0: shared between threads of the calling process
    // value = MAX_CONCURRENT_ACCESS: number of concurrent tokens
    if (sem_init(&resource_semaphore, 0, MAX_CONCURRENT_ACCESS) != 0) {
        perror("sem_init failed");
        return EXIT_FAILURE;
    }

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, worker, &thread_ids[i]) != 0) {
            perror("pthread_create failed");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    sem_destroy(&resource_semaphore);
    pthread_mutex_destroy(&count_mutex);

    printf("\n[Done] All threads serviced. Semaphore destroyed successfully.\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}

