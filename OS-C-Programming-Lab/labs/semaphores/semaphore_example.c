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
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_THREADS 5
#define MAX_CONCURRENT_ACCESS 2

// Global semaphore and active tracker
static sem_t resource_semaphore;
static int active_in_resource = 0;
static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

static void sleep_ms(long milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

void *worker(void *arg) {
    int id = *(int *)arg;

    printf("[Thread %d] Arrived and waiting for semaphore permit...\n", id);

    // Acquire permit (decrement semaphore)
    if (sem_wait(&resource_semaphore) != 0) {
        perror("sem_wait failed");
        return NULL;
    }

    // Track active threads inside resource
    int rc = pthread_mutex_lock(&count_mutex);
    if (rc != 0) {
        fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(rc));
        sem_post(&resource_semaphore);
        return NULL;
    }
    active_in_resource++;
    int current_active = active_in_resource;
    pthread_mutex_unlock(&count_mutex);

    printf("  >>> [Thread %d] ACQUIRED PERMIT! (Currently active in resource: %d / %d)\n",
           id, current_active, MAX_CONCURRENT_ACCESS);

    // Simulate work inside the limited resource
    sleep_ms(200);

    if (pthread_mutex_lock(&count_mutex) == 0) {
        active_in_resource--;
        pthread_mutex_unlock(&count_mutex);
    }

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
        pthread_mutex_destroy(&count_mutex);
        return EXIT_FAILURE;
    }

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    int created = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        int rc = pthread_create(&threads[i], NULL, worker, &thread_ids[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(rc));
            for (int j = 0; j < created; j++) {
                pthread_join(threads[j], NULL);
            }
            sem_destroy(&resource_semaphore);
            pthread_mutex_destroy(&count_mutex);
            return EXIT_FAILURE;
        }
        created++;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_join failed: %s\n", strerror(rc));
            sem_destroy(&resource_semaphore);
            pthread_mutex_destroy(&count_mutex);
            return EXIT_FAILURE;
        }
    }

    // Clean up
    if (sem_destroy(&resource_semaphore) != 0) {
        perror("sem_destroy failed");
        pthread_mutex_destroy(&count_mutex);
        return EXIT_FAILURE;
    }

    int rc = pthread_mutex_destroy(&count_mutex);
    if (rc != 0) {
        fprintf(stderr, "pthread_mutex_destroy failed: %s\n", strerror(rc));
        return EXIT_FAILURE;
    }

    printf("\n[Done] All threads serviced. Semaphore destroyed successfully.\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
