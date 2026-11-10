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
#include <pthread.h>
#include <unistd.h>

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
        pthread_mutex_lock(&lock);

        safe_counter++;

        // LEAVE CRITICAL SECTION
        pthread_mutex_unlock(&lock);
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

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, unsynchronized_worker, NULL) != 0) {
            perror("pthread_create failed");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
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

    // Initialize mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        perror("pthread_mutex_init failed");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, synchronized_worker, NULL) != 0) {
            perror("pthread_create failed");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("  Expected Count : %ld\n", expected_total);
    printf("  Actual Count   : %ld\n", safe_counter);
    if (safe_counter == expected_total) {
        printf("  [RESULT] SUCCESS! Mutex guaranteed mutual exclusion and atomic consistency.\n\n");
    }

    // Destroy mutex
    pthread_mutex_destroy(&lock);
    printf("  Mutex cleanly destroyed via pthread_mutex_destroy().\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}

