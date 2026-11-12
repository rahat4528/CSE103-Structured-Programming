# Semaphores Lab

This folder covers POSIX counting and binary semaphores, resource pool throttling, and solving the classic Bounded-Buffer Producer-Consumer synchronization problem.

---

## 1. semaphore_example.c

- **Program Name:** `semaphore_example.c`
- **OS Concept:** POSIX Counting Semaphores, Resource Throttling, Signaled Permits.
- **Objective:** Initialize a semaphore with 2 permits for 5 competing threads, demonstrate throttling with `sem_wait()` and `sem_post()`, track active occupancy, and deallocate with `sem_destroy()`.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<string.h>`
  - `<pthread.h>`
  - `<semaphore.h>`
  - `<time.h>`: POSIX.1-2008 `nanosleep()` delay helper.
- **Important Functions:**
  - `sem_init(sem_t *sem, int pshared, unsigned int value)`: Initializes an unnamed semaphore with initial value `value`.
  - `sem_wait(sem_t *sem)`: Decrements (locks) the semaphore; if value is 0, thread blocks until value > 0.
  - `sem_post(sem_t *sem)`: Increments (unlocks) the semaphore, awakening a waiting thread.
  - `sem_destroy(sem_t *sem)`: Destroys the semaphore object.
- **How It Works:**
  1. `sem_init(&resource_semaphore, 0, 2)` gives the semaphore an initial capacity of 2 permits.
  2. 5 threads compete to enter the restricted section.
  3. The first 2 threads acquire permits immediately; threads 3, 4, 5 block.
  4. As running threads finish and call `sem_post()`, the OS unblocks waiting threads one by one.
  5. The program destroys the semaphore via `sem_destroy()`.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic -pthread labs/semaphores/semaphore_example.c -o build/labs/semaphores/semaphore_example
  ```
- **Execution Command:**
  ```bash
  ./build/labs/semaphores/semaphore_example
  ```
- **Expected Output:**
  ```text
  ====================================================
       POSIX Counting Semaphore: semaphore_example.c  
  ====================================================

  [Init] Initializing semaphore with 2 permits for 5 competing threads...
  [Thread 1] Arrived and waiting for semaphore permit...
  [Thread 2] Arrived and waiting for semaphore permit...
  [Thread 3] Arrived and waiting for semaphore permit...
    >>> [Thread 1] ACQUIRED PERMIT! (Currently active in resource: 1 / 2)
    >>> [Thread 2] ACQUIRED PERMIT! (Currently active in resource: 2 / 2)
  [Thread 4] Arrived and waiting for semaphore permit...
  [Thread 5] Arrived and waiting for semaphore permit...
    <<< [Thread 1] Releasing permit and exiting resource...
    >>> [Thread 3] ACQUIRED PERMIT! (Currently active in resource: 2 / 2)
  ...
  [Done] All threads serviced. Semaphore destroyed successfully.
  ====================================================
  ```
- **Common Errors:**
  - Initializing `pshared` incorrectly: pass `0` for intra-process thread synchronization; pass non-zero only when sharing semaphores across processes mapped in shared memory.

---

## 2. producer_consumer.c

- **Program Name:** `producer_consumer.c`
- **OS Concept:** Bounded-Buffer Problem, Asymmetric Coordination, Deadlock Avoidance.
- **Objective:** Implement a circular shared buffer of capacity 5, coordinate 2 producer threads and 2 consumer threads using two counting semaphores (`empty_slots`, `full_slots`) and one mutex, and cleanly join and destroy primitives.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<string.h>`
  - `<pthread.h>`
  - `<semaphore.h>`
  - `<time.h>`: POSIX.1-2008 `nanosleep()` delay helper.
- **Important Functions:**
  - `sem_init()`: Initializes `empty_slots = 5` and `full_slots = 0`.
  - `sem_wait(&empty_slots)`: Blocks producer if buffer is completely full.
  - `sem_wait(&full_slots)`: Blocks consumer if buffer is completely empty.
  - `pthread_mutex_lock(&buffer_mutex)`: Protects array indices `in` and `out`.
- **How It Works:**
  1. Producers wait on `empty_slots`, lock the buffer mutex, write item, advance `in = (in + 1) % 5`, unlock mutex, and post `full_slots`.
  2. Consumers wait on `full_slots`, lock the buffer mutex, extract item, advance `out = (out + 1) % 5`, unlock mutex, and post `empty_slots`.
  3. **Deadlock Avoidance Rule:** Always acquire the counting semaphore *before* acquiring the mutex!
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic -pthread labs/semaphores/producer_consumer.c -o build/labs/semaphores/producer_consumer
  ```
- **Execution Command:**
  ```bash
  ./build/labs/semaphores/producer_consumer
  ```
- **Expected Output:**
  ```text
  ====================================================
     Producer-Consumer Simulation: producer_consumer   
  ====================================================

  [Init] Buffer Size = 5, Total Items = 10
  [Init] Producers = 2, Consumers = 2

    [Producer 1] Produced Item 01 -> Buffer Slot [0]
    [Producer 2] Produced Item 02 -> Buffer Slot [1]
      [Consumer 1] Consumed Item 01 <- Buffer Slot [0]
      [Consumer 2] Consumed Item 02 <- Buffer Slot [1]
    [Producer 1] Produced Item 03 -> Buffer Slot [2]
    ...
  [Cleanup] All 10 items successfully produced and consumed.
  [Cleanup] Mutex and Semaphores destroyed safely.
  ====================================================
  ```
- **Common Errors:**
  - Inverting lock order (locking `buffer_mutex` *before* `sem_wait(&empty_slots)`): If the buffer is full, the producer sleeps holding the mutex, preventing the consumer from ever entering to consume an item, deadlocking the system.

