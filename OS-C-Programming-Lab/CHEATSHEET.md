# OS C Programming Lab — Practical Cheatsheet

A concise, high-yield reference guide for compiling, running, and understanding every Operating Systems program in this repository.

---

## 1. Quick Master Program Directory

| Category | File Path | Primary OS Concept | Purpose |
|---|---|---|---|
| **Practice** | `practice/process_creation/fork_example.c` | Process Creation | Demonstrates `fork()`, PID/PPID hierarchy, and memory isolation |
| **Practice** | `practice/process_creation/fork_wait.c` | Process Reaping | Uses `wait()` to capture exit status and prevent zombie processes |
| **Practice** | `practice/threads/pthread_create.c` | POSIX Threads | Spawns worker thread, passes payload, handles creation errors |
| **Practice** | `practice/threads/pthread_join.c` | Concurrent Workers | Partitions workload across 4 threads, joins and aggregates sums |
| **Practice** | `practice/memory_allocation/malloc_example.c` | Dynamic Memory | Allocates uninitialized heap array, checks NULL, frees memory |
| **Practice** | `practice/memory_allocation/calloc_example.c` | Zeroed Memory | Demonstrates contiguous zero-initialized allocation with `calloc()` |
| **Practice** | `practice/memory_allocation/realloc_example.c` | Heap Resizing | Safely expands heap buffer using defensive pointer caching |
| **Class Work**| `class_work/pipes/basic_pipe.c` | Unidirectional IPC | Writes and reads stream data through kernel pipe descriptors |
| **Class Work**| `class_work/pipes/parent_child_pipe.c` | Pipeline IPC | Transmits message between parent & child; closes unused ends |
| **Class Work**| `class_work/file_handling/file_read.c` | Stream Reading | Line-by-line file reading with `fopen("r")` and `fgets()` |
| **Class Work**| `class_work/file_handling/file_write.c` | Stream Writing | Formatted writing and buffer flushing with `fopen("w")` and `fprintf()`|
| **Class Work**| `class_work/file_handling/file_append.c` | Stream Appending | Non-destructive log writing with `fopen("a")` |
| **Assignments**| `assignments/shared_memory/shared_memory_write.c` | POSIX Shared Memory | Creates `/dev/shm` segment, sets size with `ftruncate`, maps via `mmap`|
| **Assignments**| `assignments/shared_memory/shared_memory_read.c` | POSIX Shared Memory | Maps shared memory, reads payload, and cleans up with `shm_unlink` |
| **Assignments**| `assignments/cpu_scheduling/fcfs.c` | CPU Scheduling | Simulates First Come First Served scheduling at $t=0$ |
| **Assignments**| `assignments/cpu_scheduling/sjf.c` | CPU Scheduling | Simulates Shortest Job First (Non-preemptive) scheduling |
| **Assignments**| `assignments/cpu_scheduling/round_robin.c` | CPU Scheduling | Simulates Round Robin time-sliced queue with quantum $Q=4$ |
| **Labs** | `labs/mutex/mutex_counter.c` | Mutex Synchronization| Demonstrates race condition vs `pthread_mutex_t` safety |
| **Labs** | `labs/semaphores/semaphore_example.c` | Counting Semaphore | Throttles concurrent thread access using `sem_wait` and `sem_post`|
| **Labs** | `labs/semaphores/producer_consumer.c` | Bounded Buffer | Coordinates producers and consumers with semaphores + mutex |
| **Final** | `final_examples/process_communication.c` | Duplex Pipeline IPC | Two-way communication pipeline with data processing and reaping |
| **Final** | `final_examples/thread_synchronization.c` | Bank Simulation | Multi-threaded bank account deposit/withdrawal with mutex audit |
| **Final** | `final_examples/memory_management.c` | Dynamic Vector | Complete lifecycle: `malloc`, `calloc`, `realloc`, and `free` |

---

## 2. Exact Manual Compilation Commands

```bash
# Standard compilation flags
FLAGS="-std=c11 -Wall -Wextra -Wpedantic"

# 1. Process Creation
gcc $FLAGS practice/process_creation/fork_example.c -o fork_example
gcc $FLAGS practice/process_creation/fork_wait.c -o fork_wait

# 2. Threads (Must link -pthread)
gcc $FLAGS -pthread practice/threads/pthread_create.c -o pthread_create
gcc $FLAGS -pthread practice/threads/pthread_join.c -o pthread_join

# 3. Dynamic Memory Allocation
gcc $FLAGS practice/memory_allocation/malloc_example.c -o malloc_example
gcc $FLAGS practice/memory_allocation/calloc_example.c -o calloc_example
gcc $FLAGS practice/memory_allocation/realloc_example.c -o realloc_example

# 4. Pipes
gcc $FLAGS class_work/pipes/basic_pipe.c -o basic_pipe
gcc $FLAGS class_work/pipes/parent_child_pipe.c -o parent_child_pipe

# 5. File Handling
gcc $FLAGS class_work/file_handling/file_read.c -o file_read
gcc $FLAGS class_work/file_handling/file_write.c -o file_write
gcc $FLAGS class_work/file_handling/file_append.c -o file_append

# 6. POSIX Shared Memory (Link -lrt on Linux)
gcc $FLAGS assignments/shared_memory/shared_memory_write.c -o shared_memory_write -lrt
gcc $FLAGS assignments/shared_memory/shared_memory_read.c -o shared_memory_read -lrt

# 7. CPU Scheduling
gcc $FLAGS assignments/cpu_scheduling/fcfs.c -o fcfs
gcc $FLAGS assignments/cpu_scheduling/sjf.c -o sjf
gcc $FLAGS assignments/cpu_scheduling/round_robin.c -o round_robin

# 8. Mutex & Semaphores (Link -pthread)
gcc $FLAGS -pthread labs/mutex/mutex_counter.c -o mutex_counter
gcc $FLAGS -pthread labs/semaphores/semaphore_example.c -o semaphore_example
gcc $FLAGS -pthread labs/semaphores/producer_consumer.c -o producer_consumer

# 9. Final Integration Examples
gcc $FLAGS final_examples/process_communication.c -o process_communication
gcc $FLAGS -pthread final_examples/thread_synchronization.c -o thread_synchronization
gcc $FLAGS final_examples/memory_management.c -o memory_management
```

---

## 3. Exact Execution Commands

```bash
# Process creation
./build/practice/process_creation/fork_example
./build/practice/process_creation/fork_wait

# Threads
./build/practice/threads/pthread_create
./build/practice/threads/pthread_join

# Dynamic memory
./build/practice/memory_allocation/malloc_example
./build/practice/memory_allocation/calloc_example
./build/practice/memory_allocation/realloc_example

# Pipes
./build/class_work/pipes/basic_pipe
./build/class_work/pipes/parent_child_pipe

# File handling
./build/class_work/file_handling/file_write
./build/class_work/file_handling/file_append
./build/class_work/file_handling/file_read

# Shared memory (Must run writer first, then reader!)
./build/assignments/shared_memory/shared_memory_write
./build/assignments/shared_memory/shared_memory_read

# Scheduling algorithms
./build/assignments/cpu_scheduling/fcfs
./build/assignments/cpu_scheduling/sjf
./build/assignments/cpu_scheduling/round_robin

# Synchronization labs
./build/labs/mutex/mutex_counter
./build/labs/semaphores/semaphore_example
./build/labs/semaphores/producer_consumer

# Final integrated examples
./build/final_examples/process_communication
./build/final_examples/thread_synchronization
./build/final_examples/memory_management
```

---

## 4. Key Linux System Calls & Header Mapping

| Function | Header | Key Purpose | Return on Success | Return on Error |
|---|---|---|---|---|
| `fork()` | `<unistd.h>` | Duplicates process | Child PID (in parent) / `0` (in child) | `-1` |
| `wait()` | `<sys/wait.h>` | Waits for child state change | Terminated Child PID | `-1` |
| `pipe()` | `<unistd.h>` | Creates kernel pipe | `0` | `-1` |
| `read()` | `<unistd.h>` | Reads from file descriptor | Bytes read / `0` on EOF | `-1` |
| `write()` | `<unistd.h>` | Writes to file descriptor | Bytes written | `-1` |
| `close()` | `<unistd.h>` | Closes file descriptor | `0` | `-1` |
| `shm_open()` | `<sys/mman.h>` | Opens shared memory | File descriptor | `-1` |
| `mmap()` | `<sys/mman.h>` | Maps memory to address space | Mapped virtual pointer | `MAP_FAILED` |
| `munmap()` | `<sys/mman.h>` | Unmaps memory | `0` | `-1` |
| `shm_unlink()`| `<sys/mman.h>`| Deletes shared memory object | `0` | `-1` |

---

## 5. Important Standard C Library Functions

| Function | Header | Description |
|---|---|---|
| `malloc(size)` | `<stdlib.h>` | Allocates `size` uninitialized bytes on heap. Returns `NULL` on failure. |
| `calloc(n, size)` | `<stdlib.h>` | Allocates zero-initialized array on heap. |
| `realloc(ptr, size)`| `<stdlib.h>` | Resizes heap block; returns new address or `NULL`. |
| `free(ptr)` | `<stdlib.h>` | Deallocates heap memory. Safe with `NULL`. |
| `fopen(path, mode)` | `<stdio.h>` | Opens standard C buffered stream (`"r"`, `"w"`, `"a"`). |
| `fclose(stream)` | `<stdio.h>` | Flushes buffer and closes stream. |
| `fgets(s, sz, st)` | `<stdio.h>` | Safely reads string line until newline or EOF. |
| `fprintf(st, ...)` | `<stdio.h>` | Formatted output to file stream. |

---

## 6. Pthread & Semaphore Compilation Syntax

### POSIX Threads:
- Always compile with the `-pthread` flag.
- Prototype headers: `#include <pthread.h>`
- Mutex initialization: `pthread_mutex_init(&lock, NULL)`
- Mutex destruction: `pthread_mutex_destroy(&lock)`
- Mutex locking: `pthread_mutex_lock(&lock)` / `pthread_mutex_unlock(&lock)`

### POSIX Semaphores:
- Header: `#include <semaphore.h>`
- Initialization: `sem_init(&sem, 0, value)` (Pass `0` in middle argument for threads)
- Wait (Decrement/P): `sem_wait(&sem)`
- Post (Increment/V): `sem_post(&sem)`
- Destruction: `sem_destroy(&sem)`

---

## 7. Shared Memory Execution Order & Cleanup

1. **Step 1:** Run `shared_memory_write`
   - Allocates object `/dev/shm/os_c_shm_demo`.
   - Populates content.
2. **Step 2:** Run `shared_memory_read`
   - Opens and displays message.
   - Automatically executes `shm_unlink("/os_c_shm_demo")`.
3. **Manual Recovery (if interrupted):**
   ```bash
   rm -f /dev/shm/os_c_shm_demo
   ```

---

## 8. CPU Scheduling Input & Verified Outputs ($AT=0$)

Sample processes: $P_1(6), P_2(8), P_3(7), P_4(3)$

```text
FCFS:
- Sequence : P1 -> P2 -> P3 -> P4
- Avg WT   : 10.25 ms
- Avg TAT  : 16.25 ms

SJF (Non-preemptive):
- Sequence : P4 -> P1 -> P3 -> P2
- Avg WT   : 7.00 ms (Optimal)
- Avg TAT  : 13.00 ms (Optimal)

Round Robin (Quantum = 4 ms):
- Sequence : P1 -> P2 -> P3 -> P4 -> P1 -> P2 -> P3
- Avg WT   : 13.25 ms
- Avg TAT  : 19.25 ms
```

---

## 9. Common Compilation & Runtime Errors

| Error | Root Cause | Solution |
|---|---|---|
| `undefined reference to pthread_create` | Missing `-pthread` flag | Add `-pthread` to gcc command |
| `undefined reference to shm_open` | Missing `-lrt` library | Add `-lrt` after the source file |
| `Segmentation fault (core dumped)` | NULL pointer dereference or out-of-bounds access | Recompile with `-fsanitize=address -g` and run with gdb |
| `Bus error (core dumped)` | `mmap` called on 0-byte shared memory object | Call `ftruncate(fd, size)` before `mmap` |
| Process hangs in `read()` | Pipe deadlock due to open write descriptor | Close unused write descriptor in reader process |
| `Permission denied` on test script | Execution bit not set | Run `chmod +x scripts/smoke_test.sh` |

