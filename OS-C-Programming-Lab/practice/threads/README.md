# Threads Practice

This folder covers multithreading principles, POSIX thread creation, parameter passing, and synchronization using `pthread_create()` and `pthread_join()`.

---

## 1. pthread_create.c

- **Program Name:** `pthread_create.c`
- **OS Concept:** POSIX Threads (pthreads), Lightweight Processes, Thread Execution Routines.
- **Objective:** Create a single worker thread, pass structured arguments to the worker, display thread metadata, and handle potential creation errors safely.
- **Required Headers:**
  - `<stdio.h>`: Standard I/O.
  - `<stdlib.h>`: Utility macros (`EXIT_SUCCESS`, `EXIT_FAILURE`).
  - `<string.h>`: String copying and error reporting (`strerror`).
  - `<pthread.h>`: POSIX Threads API.
  - `<unistd.h>`: System calls (`sleep`).
- **Important Functions:**
  - `pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)`: Spawns a new execution thread in the calling process.
  - `pthread_self()`: Obtains the unique ID of the executing thread.
  - `pthread_join(pthread_t thread, void **retval)`: Blocks calling thread until target thread terminates.
- **How It Works:**
  1. Main thread initializes a `ThreadArg` structure with a thread ID and greeting message.
  2. `pthread_create()` initiates the `thread_worker` routine with a pointer to `task_data`.
  3. The worker displays its thread ID (`pthread_self()`), reads the arguments, simulates work with `sleep(1)`, and finishes.
  4. The main thread calls `pthread_join()` to avoid exiting prematurely while worker runs.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic -pthread practice/threads/pthread_create.c -o build/practice/threads/pthread_create
  ```
- **Execution Command:**
  ```bash
  ./build/practice/threads/pthread_create
  ```
- **Expected Output:**
  ```text
  ====================================================
         Thread Creation Demo: pthread_create()       
  ====================================================

  [Main Thread] Creating worker thread...
  [Main Thread] Thread created successfully (Assigned TID: 140234123)
  [Main Thread] Waiting for thread to finish execution...
    [Thread Worker] Started! Thread ID: 140234123
    [Thread Worker] Received payload: ID=1, Msg="Greetings from main thread"
    [Thread Worker] Performing computation...
    [Thread Worker] Task completed successfully.
  [Main Thread] Worker thread has joined. Execution finished cleanly.
  ```
- **Common Errors:**
  - Omitting the `-pthread` compiler flag, causing linker errors (`undefined reference to pthread_create`).
  - Passing a pointer to a local stack variable that goes out of scope before the thread reads it.
  - Checking `errno` instead of the return value of `pthread_create()` (pthread functions return error numbers directly).

---

## 2. pthread_join.c

- **Program Name:** `pthread_join.c`
- **OS Concept:** Multithreaded Parallel Decomposition, Fork-Join Model, Dynamic Return Values.
- **Objective:** Distribute a computation (summing integers 1 to 100) across 4 worker threads, await their completion with `pthread_join()`, collect returned values, and assemble the final result.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<string.h>`
  - `<pthread.h>`
  - `<unistd.h>`
- **Important Functions:**
  - `pthread_create()`: Spawns worker threads.
  - `pthread_join()`: Blocks until target terminates and captures its returned `void*` pointer.
  - `pthread_exit()`: Terminates calling thread and returns a status pointer.
  - `free()`: Deallocates dynamically allocated memory returned by threads.
- **How It Works:**
  1. Main splits the range 1..100 into 4 slices: [1..25], [26..50], [51..75], and [76..100].
  2. Each slice is assigned to a `WorkerTask` and dispatched via `pthread_create()`.
  3. Threads execute concurrently, computing their respective partial sums.
  4. Each thread allocates a `long*` on the heap and returns it via `return (void*)result`.
  5. Main joins each thread, reads the partial sum, adds it to `total_sum`, and frees the allocated memory.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic -pthread practice/threads/pthread_join.c -o build/practice/threads/pthread_join
  ```
- **Execution Command:**
  ```bash
  ./build/practice/threads/pthread_join
  ```
- **Expected Output:**
  ```text
  ====================================================
       Multi-Thread Concurrency: pthread_join()       
  ====================================================

  [Main] Spawning 4 worker threads to sum numbers 1 through 100...
    [Worker 1] Starting: Summing from 1 to 25 (TID: 140123)
    [Worker 2] Starting: Summing from 26 to 50 (TID: 140124)
    [Worker 3] Starting: Summing from 51 to 75 (TID: 140125)
    [Worker 4] Starting: Summing from 76 to 100 (TID: 140126)
  [Main] All 4 threads dispatched. Awaiting completion...

    [Worker 1] Finished: Partial Sum = 325
    [Worker 2] Finished: Partial Sum = 950
    [Worker 3] Finished: Partial Sum = 1575
    [Worker 4] Finished: Partial Sum = 2200
  [Main] Joined Thread 1 -> Received partial sum = 325
  [Main] Joined Thread 2 -> Received partial sum = 950
  [Main] Joined Thread 3 -> Received partial sum = 1575
  [Main] Joined Thread 4 -> Received partial sum = 2200

  ====================================================
    Combined Total Sum of 1..100 : 5050
    Mathematical Check [n(n+1)/2]: 5050
  ====================================================
  ```
- **Common Errors:**
  - Returning the address of a local automatic stack variable from the thread function (leads to undefined behavior / garbage values). Heap memory or pre-allocated caller structs must be used.
  - Reusing a single loop index pointer `&i` across all threads in `pthread_create()`, causing a race condition where multiple threads read the same modified index.

