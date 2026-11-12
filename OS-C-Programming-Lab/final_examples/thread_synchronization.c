/**
 * @file thread_synchronization.c
 * @brief Integrated multi-threaded banking transaction simulation protected by a mutex.
 *
 * Concepts Demonstrated:
 *  - Coordinating multiple reader/writer worker threads
 *  - Critical section encapsulation with pthread_mutex_t
 *  - Verifying invariant consistency across concurrent deposits and withdrawals
 *  - Thread joining and mutex cleanup
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_WORKERS 4
#define OPS_PER_WORKER 1000

typedef struct {
    long balance;
    long total_deposits;
    long total_withdrawals;
    int successful_transactions;
    pthread_mutex_t lock;
} BankAccount;

static BankAccount account;

typedef struct {
    int worker_id;
    int is_deposit; // 1 = deposit, 0 = withdrawal
    long amount;
} WorkerJob;

void *account_worker(void *arg) {
    WorkerJob *job = (WorkerJob *)arg;

    for (int i = 0; i < OPS_PER_WORKER; i++) {
        int rc = pthread_mutex_lock(&account.lock);
        if (rc != 0) {
            fprintf(stderr, "Worker %d mutex_lock failed: %s\n", job->worker_id, strerror(rc));
            return NULL;
        }

        if (job->is_deposit) {
            account.balance += job->amount;
            account.total_deposits += job->amount;
            account.successful_transactions++;
        } else {
            if (account.balance >= job->amount) {
                account.balance -= job->amount;
                account.total_withdrawals += job->amount;
                account.successful_transactions++;
            }
        }

        rc = pthread_mutex_unlock(&account.lock);
        if (rc != 0) {
            fprintf(stderr, "Worker %d mutex_unlock failed: %s\n", job->worker_id, strerror(rc));
            return NULL;
        }
    }

    return NULL;
}

int main(void) {
    printf("====================================================\n");
    printf(" Integrated OS Example: Thread-Safe Bank Account    \n");
    printf("====================================================\n\n");

    const long initial_balance = 5000;
    account.balance = initial_balance;
    account.total_deposits = 0;
    account.total_withdrawals = 0;
    account.successful_transactions = 0;

    int rc = pthread_mutex_init(&account.lock, NULL);
    if (rc != 0) {
        fprintf(stderr, "Failed to init mutex: %s\n", strerror(rc));
        return EXIT_FAILURE;
    }

    printf("[Init] Initial Account Balance: $%ld\n", initial_balance);
    printf("[Init] Dispatching %d worker threads (%d operations each)...\n\n",
           NUM_WORKERS, OPS_PER_WORKER);

    pthread_t threads[NUM_WORKERS];
    WorkerJob jobs[NUM_WORKERS];

    // Configure 2 deposit workers ($20 each) and 2 withdrawal workers ($10 each)
    jobs[0] = (WorkerJob){.worker_id = 1, .is_deposit = 1, .amount = 20};
    jobs[1] = (WorkerJob){.worker_id = 2, .is_deposit = 1, .amount = 20};
    jobs[2] = (WorkerJob){.worker_id = 3, .is_deposit = 0, .amount = 10};
    jobs[3] = (WorkerJob){.worker_id = 4, .is_deposit = 0, .amount = 10};

    int created = 0;
    for (int i = 0; i < NUM_WORKERS; i++) {
        rc = pthread_create(&threads[i], NULL, account_worker, &jobs[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(rc));
            for (int j = 0; j < created; j++) {
                pthread_join(threads[j], NULL);
            }
            pthread_mutex_destroy(&account.lock);
            return EXIT_FAILURE;
        }
        created++;
    }

    for (int i = 0; i < NUM_WORKERS; i++) {
        rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_join failed: %s\n", strerror(rc));
            pthread_mutex_destroy(&account.lock);
            return EXIT_FAILURE;
        }
    }

    // Verify mathematical invariant
    long expected_balance = initial_balance + account.total_deposits - account.total_withdrawals;

    printf("---------------- AUDIT STATEMENT ----------------\n");
    printf("  Initial Balance         : $%ld\n", initial_balance);
    printf("  Total Deposits Made     : +$%ld\n", account.total_deposits);
    printf("  Total Withdrawals Made  : -$%ld\n", account.total_withdrawals);
    printf("  Expected Final Balance  : $%ld\n", expected_balance);
    printf("  Actual Final Balance    : $%ld\n", account.balance);
    printf("  Processed Transactions  : %d\n", account.successful_transactions);
    printf("-------------------------------------------------\n");

    int exit_status = EXIT_SUCCESS;
    if (account.balance == expected_balance) {
        printf("[SUCCESS] Invariant verified: Mutex prevented all race conditions!\n");
    } else {
        printf("[FAILURE] Data corruption detected!\n");
        exit_status = EXIT_FAILURE;
    }

    rc = pthread_mutex_destroy(&account.lock);
    if (rc != 0) {
        fprintf(stderr, "pthread_mutex_destroy failed: %s\n", strerror(rc));
        exit_status = EXIT_FAILURE;
    }
    printf("====================================================\n");

    return exit_status;
}
