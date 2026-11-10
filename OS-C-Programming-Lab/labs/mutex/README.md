# Mutex Synchronization Lab

This laboratory demonstrates mutual exclusion in concurrent programs, explains how race conditions manifest in uncoordinated threads, and verifies critical section protection using `pthread_mutex_t`.

---

## 1. mutex_counter.c

- **Program Name:** `mutex_counter.c`
- **OS Concept:** Critical Sections, Race Conditions, Mutual Exclusion, POSIX Mutexes.
- **Objective:** Create 5 threads that simultaneously increment a counter 100,000 times each. Compare unsynchronized execution (demonstrating update losses) against mutex-locked synchronization (guaranteeing exact results).
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<pthread.h>`
  - `<unistd.h>`
- **Important Functions:**
  - `pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)`: Initializes a mutex.
  - `pthread_mutex_lock(pthread_mutex_t *mutex)`: Locks the mutex; blocks calling thread if already locked by another thread.
  - `pthread_mutex_unlock(pthread_mutex_t *mutex)`: Releases the mutex, allowing waiting threads to proceed.
  - `pthread_mutex_destroy(pthread_mutex_t *mutex)`: Destroys and cleans up the mutex object.
- **How It Works:**
  1. **Phase 1 (Unsynchronized):** 5 threads execute `counter++` concurrently. Because `counter++` compiles into three machine instructions (read memory into register, add 1, write back to memory), context switches between these steps cause lost increments.
  2. **Phase 2 (Synchronized):** The increment is wrapped inside `pthread_mutex_lock()` and `pthread_mutex_unlock()`. Exactly one thread can enter the critical section at any given time, ensuring atomic modification.
  3. The mutex is cleaned up using `pthread_mutex_destroy()`.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic -pthread labs/mutex/mutex_counter.c -o build/labs/mutex/mutex_counter
  ```
- **Execution Command:**
  ```bash
  ./build/labs/mutex/mutex_counter
  ```
- **Expected Output:**
  ```text
  ====================================================
       Mutex Synchronization & Race Condition Demo    
  ====================================================

  [Test 1] Spawning 5 threads WITHOUT mutex protection...
           Each thread executes 100000 increments.
    Expected Count : 500000
    Actual Count   : 341289
    [RESULT] RACE CONDITION DETECTED! Lost 158711 updates due to race.

  [Test 2] Spawning 5 threads WITH pthread_mutex_t protection...
    Expected Count : 500000
    Actual Count   : 500000
    [RESULT] SUCCESS! Mutex guaranteed mutual exclusion and atomic consistency.

    Mutex cleanly destroyed via pthread_mutex_destroy().
  ====================================================
  ```
- **Common Errors:**
  - Forgetting to unlock a mutex before exiting a function or thread, creating a permanent deadlock.
  - Attempting to destroy a locked mutex with `pthread_mutex_destroy()` (returns `EBUSY`).
