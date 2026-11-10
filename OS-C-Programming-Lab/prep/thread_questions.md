# Operating Systems Viva & Exam Prep: Threads & Concurrency

## 1. What is a Thread?
A **thread** is the smallest unit of execution that can be scheduled by an operating system. Often called a **Lightweight Process (LWP)**, a thread belongs to a parent process and shares the process's code, data, heap, and open file descriptors, while maintaining its own:
- Thread ID (TID)
- Program counter (PC)
- Register set
- Call stack (local variables and call frames)

---

## 2. Process vs Thread Detailed Comparison

| Attribute | Process | Thread |
|---|---|---|
| **Definition** | An independent executing program instance | An independent execution stream within a process |
| **Address Space** | Isolated private address space | Shares address space (code, data, heap) with peer threads |
| **Creation Cost** | High (memory allocation, page tables, descriptor clones) | Low (only stack allocation and thread control block) |
| **Context Switch Overhead** | High (flushes TLB, invalidates CPU caches, swaps page tables) | Low (no page table swap; minimal cache invalidation) |
| **Communication** | Inter-Process Communication (pipes, shared memory, sockets) | Direct memory access (global/heap variables, pointers) |
| **Fault Isolation** | High: crash in one process does not crash others | Low: crash in one thread (e.g. SIGSEGV) crashes the entire process |
| **System Call** | `fork()` | `pthread_create()` / `clone()` |

---

## 3. Concurrency vs Parallelism

- **Concurrency:** The ability to handle multiple tasks by interleaving their execution over time (e.g. multiple threads time-sliced on a single CPU core).
- **Parallelism:** The ability to execute multiple tasks simultaneously at the exact same physical instant across multiple CPU cores.
> *Formula:* Concurrency is about structure; Parallelism is about physical simultaneous execution.

---

## 4. Multithreading Models

1. **Many-to-One (User-Level Threads):**
   - Many user-level threads map to one kernel thread.
   - Fast context switching in user-space, but if one thread makes a blocking system call, the entire process blocks. Cannot take advantage of multi-core CPUs.
2. **One-to-One (Kernel-Level Threads - Linux Model):**
   - Each user thread maps directly to one kernel execution context (`clone()` in Linux / NPTL).
   - Allows true multi-core hardware parallelism; blocking one thread does not affect peer threads.
3. **Many-to-Many:**
   - Multiplexes $M$ user threads onto $N$ kernel threads ($M \ge N$).
   - Complex runtime scheduler required (e.g. Go goroutines scheduler).

---

## 5. POSIX Thread (Pthread) Core Functions

### `pthread_create()`
```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);
```
- Spawns a new thread executing `start_routine(arg)`.
- Returns `0` on success; returns an error code on failure (does **not** set `errno`).

### `pthread_join()`
```c
int pthread_join(pthread_t thread, void **retval);
```
- Suspends caller until target thread finishes.
- Collects target's exit status (`retval`) and reaps its thread resources (analogous to `waitpid()` for processes).

### `pthread_detach()`
```c
int pthread_detach(pthread_t thread);
```
- Marks target thread as detached. When it finishes, its resources are automatically reclaimed by the OS without needing `pthread_join()`.

### `pthread_exit()`
```c
void pthread_exit(void *retval);
```
- Terminates the calling thread immediately and provides `retval` to any joining thread.

---

## 6. What is Thread Safety and Reentrancy?
- **Thread Safety:** Code that functions correctly and produces consistent results when executed concurrently by multiple threads without uncoordinated shared state.
- **Reentrant Function:** A function that can be safely interrupted in the middle of execution, re-entered by another thread or signal handler, and resumed without state corruption (avoids static/global variables; uses caller-supplied buffers, e.g. `strtok_r` instead of `strtok`).
