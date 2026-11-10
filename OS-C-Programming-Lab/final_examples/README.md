# Final Integration Examples

This folder combines multiple core Operating Systems concepts into robust, cohesive C programs demonstrating IPC pipelines, multithreaded mutual exclusion, and complete dynamic memory lifecycles.

---

## 1. process_communication.c

- **Program Name:** `process_communication.c`
- **OS Concept:** Bidirectional Inter-Process Pipeline, Fork-Join Concurrency, Coordinated IPC.
- **Objective:** Establish a duplex communication channel using two distinct UNIX pipes across parent and child processes. The parent sends an array of numbers; the child computes aggregate statistics (sum, average, min, max) and streams the result structure back to the parent.
- **Required Headers:** `<stdio.h>`, `<stdlib.h>`, `<unistd.h>`, `<sys/types.h>`, `<sys/wait.h>`
- **Important Functions:**
  - `pipe()`: Creates two pipes (`pipe_p2c` and `pipe_c2p`).
  - `fork()`: Spawns the worker child process.
  - `write()` / `read()`: Streams structured binary payloads across processes.
  - `close()`: Closes 4 unused descriptor ends to prevent descriptor leaks and pipe deadlocks.
  - `waitpid()`: Reaps the child process upon completion.
- **How It Works:**
  1. Parent creates two pipes before calling `fork()`.
  2. Child closes `pipe_p2c[1]` and `pipe_c2p[0]`.
  3. Parent closes `pipe_p2c[0]` and `pipe_c2p[1]`.
  4. Parent writes `TaskPayload` to child and closes write end.
  5. Child reads payload, computes statistics, writes `TaskResult` to parent, and exits.
  6. Parent reads results, prints audit summary, and reaps the child.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic final_examples/process_communication.c -o build/final_examples/process_communication
  ```
- **Execution Command:**
  ```bash
  ./build/final_examples/process_communication
  ```
- **Expected Output:**
  ```text
  ====================================================
   Integrated OS Example: Bidirectional Pipe IPC      
  ====================================================

  [Parent] Pipes created:
    Parent-to-Child Pipe: read_fd=3, write_fd=4
    Child-to-Parent Pipe: read_fd=5, write_fd=6

  [Parent] Dispatching task with 5 numbers to Child PID 13420...
  [Parent] Waiting for computation results from child...
    [Child (PID 13420)] Descriptors configured. Awaiting task payload...
    [Child] Received 5 integers: [42, 17, 88, 63, 29]
    [Child] Calculation complete. Sending response back to parent...
    [Child] Closed descriptors. Exiting.

  [Parent] Results received from child:
    Sum     : 239
    Average : 47.80
    Minimum : 17
    Maximum : 88

  [Parent] Child reaped. Bidirectional IPC test passed successfully.
  ====================================================
  ```
- **Common Errors:**
  - Forgetting to close the write end of `pipe_p2c` in the parent, causing child's subsequent `read()` to block waiting for more data instead of recognizing end-of-payload.

---

## 2. thread_synchronization.c

- **Program Name:** `thread_synchronization.c`
- **OS Concept:** Thread-Safe Invariant Protection, Critical Section Locking with Mutex.
- **Objective:** Simulate a multi-worker banking system where multiple concurrent threads execute thousands of deposit and withdrawal operations, protected by a `pthread_mutex_t`, verifying financial invariants.
- **Required Headers:** `<stdio.h>`, `<stdlib.h>`, `<pthread.h>`, `<unistd.h>`
- **Important Functions:**
  - `pthread_mutex_init()`
  - `pthread_mutex_lock()`
  - `pthread_mutex_unlock()`
  - `pthread_mutex_destroy()`
  - `pthread_create()`, `pthread_join()`
- **How It Works:**
  1. Initializes a shared `BankAccount` structure with balance and transaction tallies.
  2. Spawns 4 worker threads performing 1,000 deposits and withdrawals.
  3. Every transaction is enveloped in `pthread_mutex_lock` / `pthread_mutex_unlock`.
  4. After all threads join, audits: $\text{Final Balance} == \text{Initial Balance} + \text{Deposits} - \text{Withdrawals}$.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic -pthread final_examples/thread_synchronization.c -o build/final_examples/thread_synchronization
  ```
- **Execution Command:**
  ```bash
  ./build/final_examples/thread_synchronization
  ```
- **Expected Output:**
  ```text
  ====================================================
   Integrated OS Example: Thread-Safe Bank Account    
  ====================================================

  [Init] Initial Account Balance: $5000
  [Init] Dispatching 4 worker threads (1000 operations each)...

  ---------------- AUDIT STATEMENT ----------------
    Initial Balance         : $5000
    Total Deposits Made     : +$40000
    Total Withdrawals Made  : -$20000
    Expected Final Balance  : $25000
    Actual Final Balance    : $25000
    Processed Transactions  : 4000
  -------------------------------------------------
  [SUCCESS] Invariant verified: Mutex prevented all race conditions!
  ====================================================
  ```
- **Common Errors:**
  - Deadlock caused by unhandled error paths returning without releasing `account.lock`.

---

## 3. memory_management.c

- **Program Name:** `memory_management.c`
- **OS Concept:** Complete Dynamic Memory Lifecycle, Heap Buffer Geometric Growth, Leak Prevention.
- **Objective:** Construct a dynamically resizable integer Vector data structure in C demonstrating allocation (`malloc`), zero-initialization (`calloc`), geometric capacity expansion (`realloc`), and clean reclamation (`free`).
- **Required Headers:** `<stdio.h>`, `<stdlib.h>`
- **Important Functions:**
  - `vector_create()`: Uses `malloc()` for vector struct and `calloc()` for dynamic buffer.
  - `vector_push()`: Doubles capacity using `realloc()` when vector is full.
  - `vector_free()`: Frees buffer, frees container, and sets caller pointer to `NULL`.
- **How It Works:**
  1. Creates vector with initial capacity 2.
  2. Pushes 6 integers, automatically triggering reallocations from 2 $\to$ 4 $\to$ 8.
  3. Uses temporary pointer during `realloc()` to preserve data on allocation failure.
  4. Fully reclaims all heap blocks via `vector_free()`.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic final_examples/memory_management.c -o build/final_examples/memory_management
  ```
- **Execution Command:**
  ```bash
  ./build/final_examples/memory_management
  ```
- **Expected Output:**
  ```text
  ====================================================
   Integrated OS Example: Dynamic Memory Lifecycle    
  ====================================================

  [1] Creating dynamic vector with initial capacity = 2 (using malloc + calloc)...
      Vector created. Buffer Base: 0x55b1a82c, Capacity: 2, Size: 0

  [2] Inserting 6 elements sequentially to trigger dynamic reallocations:
    Pushing element: 10
    Pushing element: 25
    Pushing element: 40
      [Vector Notice] Expanding capacity: 2 -> 4 elements...
    Pushing element: 55
    Pushing element: 70
      [Vector Notice] Expanding capacity: 4 -> 8 elements...
    Pushing element: 85

  [3] Final Vector Contents (Size = 6, Capacity = 8):
    [ 10, 25, 40, 55, 70, 85 ]

  [4] Reclaiming all dynamic memory via vector_free()...
      Vector successfully deallocated. Pointer reset to NULL.
      Zero memory leaks remaining.
  ====================================================
  ```
- **Common Errors:**
  - Freeing the struct container before freeing the nested `vec->data` buffer, causing an orphaned heap memory leak.

