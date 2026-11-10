/**
 * @file pthread_create.c
 * @brief Demonstrates creating and executing a POSIX thread with pthread_create().
 *
 * Concepts Demonstrated:
 *  - Thread definition and entry-point routine
 *  - Passing parameters to worker threads safely
 *  - Checking return values of pthread API functions
 *  - Synchronizing thread termination with pthread_join()
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Structure to pass structured parameters to the thread
typedef struct {
    int thread_number;
    char message[64];
} ThreadArg;

// Thread entry function
void *thread_worker(void *arg) {
    ThreadArg *data = (ThreadArg *)arg;

    printf("  [Thread Worker] Started! Thread ID: %lu\n", (unsigned long)pthread_self());
    printf("  [Thread Worker] Received payload: ID=%d, Msg=\"%s\"\n", 
           data->thread_number, data->message);

    // Simulate some computational work
    printf("  [Thread Worker] Performing computation...\n");
    sleep(1);

    printf("  [Thread Worker] Task completed successfully.\n");
    return NULL;
}

int main(void) {
    printf("====================================================\n");
    printf("       Thread Creation Demo: pthread_create()       \n");
    printf("====================================================\n\n");

    pthread_t thread_id;
    ThreadArg task_data;
    task_data.thread_number = 1;
    strncpy(task_data.message, "Greetings from main thread", sizeof(task_data.message) - 1);
    task_data.message[sizeof(task_data.message) - 1] = '\0';

    printf("[Main Thread] Creating worker thread...\n");
    int rc = pthread_create(&thread_id, NULL, thread_worker, (void *)&task_data);

    // pthread functions return 0 on success, or an error number directly
    if (rc != 0) {
        fprintf(stderr, "[Error] pthread_create failed: %s\n", strerror(rc));
        return EXIT_FAILURE;
    }

    printf("[Main Thread] Thread created successfully (Assigned TID: %lu)\n", 
           (unsigned long)thread_id);
    printf("[Main Thread] Waiting for thread to finish execution...\n");

    rc = pthread_join(thread_id, NULL);
    if (rc != 0) {
        fprintf(stderr, "[Error] pthread_join failed: %s\n", strerror(rc));
        return EXIT_FAILURE;
    }

    printf("[Main Thread] Worker thread has joined. Execution finished cleanly.\n");
    return EXIT_SUCCESS;
}

