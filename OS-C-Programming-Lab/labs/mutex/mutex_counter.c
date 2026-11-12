/**
 * @file mutex_counter.c
 * @brief Demonstration of Race Conditions and Mutual Exclusion using pthread_mutex_t.
 *
 * Concepts Demonstrated:
 *  - Race conditions arising from concurrent, non-atomic access to shared state
 *  - Protecting critical sections using POSIX mutex locks
 *  - Comparing unsynchronized vs synchronized execution
 *  - Initializing, acquiring, releasing, and destroying mutexes
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 5
#define ITERATIONS_PER_THREAD 100000

// Shared state
static long unsafe_counter = 0;
static long safe_counter = 0;
static pthread_mutex_t lock;

// Worker routine WITHOUT synchronization (Demonstrates Race Condition)
void *unsynchronized_worker(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS_PER_THREAD; i++) {
        // Non-atomic read-modify-write:
        // 1. Read counter into CPU register
        // 2. Increment register
        // 3. Write register back to memory
        unsafe_counter++;
    }
    return NULL;
}

// Worker routine WITH synchronization (Thread-Safe Critical Section)
void *synchronized_worker(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS_PER_THREAD; i++) {
        // ENTER CRITICAL SECTION
        int rc = pthread_mutex_lock(&lock);
        if (rc != 0) {
            fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(rc));
            return NULL;
        }

        safe_counter++;

        // LEAVE CRITICAL SECTION
        rc = pthread_mutex_unlock(&lock);
        if (rc != 0) {
            fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(rc));
            return NULL;
        }
    }
    return NULL;
}

int main(void) {
    printf("====================================================\n");
    printf("     Mutex Synchronization & Race Condition Demo    \n");
    printf("====================================================\n\n");

    const long expected_total = (long)NUM_THREADS * ITERATIONS_PER_THREAD;
    pthread_t threads[NUM_THREADS];

    // ----------------------------------------------------
    // TEST 1: UNSYNCHRONIZED COUNTER (RACE CONDITION)
    // ----------------------------------------------------
    printf("[Test 1] Spawning %d threads WITHOUT mutex protection...\n", NUM_THREADS);
    printf("         Each thread executes %d increments.\n", ITERATIONS_PER_THREAD);

    int created = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_create(&threads[i], NULL, unsynchronized_worker, NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(rc));
            for (int j = 0; j < created; j++) {
                pthread_join(threads[j], NULL);
            }
            return EXIT_FAILURE;
        }
        created++;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_join failed: %s\n", strerror(rc));
            return EXIT_FAILURE;
        }
    }

    printf("  Expected Count : %ld\n", expected_total);
    printf("  Actual Count   : %ld\n", unsafe_counter);
    if (unsafe_counter != expected_total) {
        printf("  [RESULT] RACE CONDITION DETECTED! Lost %ld updates due to race.\n\n",
               expected_total - unsafe_counter);
    } else {
        printf("  [RESULT] No lost updates detected (interleaving was serialized).\n\n");
    }

    // ----------------------------------------------------
    // TEST 2: SYNCHRONIZED COUNTER (MUTEX PROTECTED)
    // ----------------------------------------------------
    printf("[Test 2] Spawning %d threads WITH pthread_mutex_t protection...\n", NUM_THREADS);

    int rc = pthread_mutex_init(&lock, NULL);
    if (rc != 0) {
        fprintf(stderr, "pthread_mutex_init failed: %s\n", strerror(rc));
        return EXIT_FAILURE;
    }

    created = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        rc = pthread_create(&threads[i], NULL, synchronized_worker, NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(rc));
            for (int j = 0; j < created; j++) {
                pthread_join(threads[j], NULL);
            }
            pthread_mutex_destroy(&lock);
            return EXIT_FAILURE;
        }
        created++;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_join failed: %s\n", strerror(rc));
            pthread_mutex_destroy(&lock);
            return EXIT_FAILURE;
        }
    }

    printf("  Expected Count : %ld\n", expected_total);
    printf("  Actual Count   : %ld\n", safe_counter);
    if (safe_counter == expected_total) {
        printf("  [RESULT] SUCCESS! Mutex guaranteed mutual exclusion and atomic consistency.\n\n");
    }

    rc = pthread_mutex_destroy(&lock);
    if (rc != 0) {
        fprintf(stderr, "pthread_mutex_destroy failed: %s\n", strerror(rc));
        return EXIT_FAILURE;
    }

    printf("  Mutex cleanly destroyed via pthread_mutex_destroy().\n");
    printf("====================================================\n");

    return (safe_counter == expected_total) ? EXIT_SUCCESS : EXIT_FAILURE;
}
