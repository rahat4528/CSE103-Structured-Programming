/**
 * @file producer_consumer.c
 * @brief Solution to the classic Bounded-Buffer Producer-Consumer Problem.
 *
 * Concepts Demonstrated:
 *  - Coordinating cooperating threads using counting semaphores
 *  - empty_slots semaphore: counts available space in circular buffer
 *  - full_slots semaphore: counts ready items ready for consumption
 *  - pthread_mutex_t: protects circular buffer index manipulation
 *  - Deadlock avoidance: acquiring semaphores BEFORE acquiring mutexes
 *  - Proper reclamation of synchronization primitives
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 10
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2
#define ITEMS_PER_PRODUCER (TOTAL_ITEMS / NUM_PRODUCERS)
#define ITEMS_PER_CONSUMER (TOTAL_ITEMS / NUM_CONSUMERS)

// Shared Bounded Buffer
static int buffer[BUFFER_SIZE];
static int in = 0;
static int out = 0;
static int item_counter = 1;

// Synchronization Primitives
static sem_t empty_slots;
static sem_t full_slots;
static pthread_mutex_t buffer_mutex;

// Producer Thread Routine
void *producer(void *arg) {
    int producer_id = *(int *)arg;

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        // Wait for an empty slot in buffer
        sem_wait(&empty_slots);

        // LOCK MUTEX to update buffer state
        pthread_mutex_lock(&buffer_mutex);

        int item = item_counter++;
        buffer[in] = item;
        printf("  [Producer %d] Produced Item %02d -> Buffer Slot [%d]\n", 
               producer_id, item, in);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&buffer_mutex);

        // Signal that a full slot is available
        sem_post(&full_slots);

        usleep(80000); // 80 ms
    }

    return NULL;
}

// Consumer Thread Routine
void *consumer(void *arg) {
    int consumer_id = *(int *)arg;

    for (int i = 0; i < ITEMS_PER_CONSUMER; i++) {
        // Wait for a full slot in buffer
        sem_wait(&full_slots);

        // LOCK MUTEX to consume from buffer
        pthread_mutex_lock(&buffer_mutex);

        int item = buffer[out];
        printf("    [Consumer %d] Consumed Item %02d <- Buffer Slot [%d]\n", 
               consumer_id, item, out);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&buffer_mutex);

        // Signal that an empty slot is now available
        sem_post(&empty_slots);

        usleep(120000); // 120 ms
    }

    return NULL;
}

int main(void) {
    printf("====================================================\n");
    printf("   Producer-Consumer Simulation: producer_consumer   \n");
    printf("====================================================\n\n");

    printf("[Init] Buffer Size = %d, Total Items = %d\n", BUFFER_SIZE, TOTAL_ITEMS);
    printf("[Init] Producers = %d, Consumers = %d\n\n", NUM_PRODUCERS, NUM_CONSUMERS);

    // Initialize semaphores
    // empty_slots starts at BUFFER_SIZE (all slots free)
    if (sem_init(&empty_slots, 0, BUFFER_SIZE) != 0) {
        perror("sem_init empty_slots failed");
        return EXIT_FAILURE;
    }

    // full_slots starts at 0 (no items in buffer)
    if (sem_init(&full_slots, 0, 0) != 0) {
        perror("sem_init full_slots failed");
        sem_destroy(&empty_slots);
        return EXIT_FAILURE;
    }

    // Initialize buffer protection mutex
    if (pthread_mutex_init(&buffer_mutex, NULL) != 0) {
        perror("pthread_mutex_init failed");
        sem_destroy(&empty_slots);
        sem_destroy(&full_slots);
        return EXIT_FAILURE;
    }

    pthread_t prod_threads[NUM_PRODUCERS];
    pthread_t cons_threads[NUM_CONSUMERS];
    int prod_ids[NUM_PRODUCERS];
    int cons_ids[NUM_CONSUMERS];

    // Spawn producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        prod_ids[i] = i + 1;
        pthread_create(&prod_threads[i], NULL, producer, &prod_ids[i]);
    }

    // Spawn consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cons_ids[i] = i + 1;
        pthread_create(&cons_threads[i], NULL, consumer, &cons_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(prod_threads[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(cons_threads[i], NULL);
    }

    // Clean up synchronization primitives
    pthread_mutex_destroy(&buffer_mutex);
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);

    printf("\n[Cleanup] All %d items successfully produced and consumed.\n", TOTAL_ITEMS);
    printf("[Cleanup] Mutex and Semaphores destroyed safely.\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
