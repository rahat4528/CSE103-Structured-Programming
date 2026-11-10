# POSIX & Linux System Calls Reference

This document provides signatures, header requirements, operational behaviors, return values, and error codes for all system calls and POSIX APIs utilized in the OS C Programming Lab.

---

## 1. Process Management System Calls

### `fork`
```c
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);
```
- **Description:** Duplicates the calling process, creating an identical child process.
- **Return Value:**
  - `0`: In the child process.
  - `> 0` (Child PID): In the parent process.
  - `-1`: On error (sets `errno` to `EAGAIN` or `ENOMEM`).

### `getpid` / `getppid`
```c
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void);
pid_t getppid(void);
```
- **Description:** Returns the process ID (`getpid`) or parent process ID (`getppid`) of the caller. Always successful.

### `wait` / `waitpid`
```c
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
```
- **Description:** Suspends caller until child process state changes (terminates, stops).
- **Inspection Macros:**
  - `WIFEXITED(status)`: Evaluates to non-zero if child terminated normally.
  - `WEXITSTATUS(status)`: Extracts lower 8 bits of child's exit status.
  - `WIFSIGNALED(status)`: True if child terminated by uncaught signal.

---

## 2. Pipe & File Descriptor System Calls

### `pipe`
```c
#include <unistd.h>

int pipe(int pipefd[2]);
```
- **Description:** Allocates a unidirectional data channel in kernel memory.
- **Descriptors:**
  - `pipefd[0]`: Read end.
  - `pipefd[1]`: Write end.
- **Return Value:** `0` on success; `-1` on error.

### `read`
```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
```
- **Description:** Reads up to `count` bytes from file descriptor `fd` into buffer `buf`.
- **Return Value:** Number of bytes read, `0` on EOF, or `-1` on error.

### `write`
```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
```
- **Description:** Writes up to `count` bytes from buffer `buf` to file descriptor `fd`.
- **Return Value:** Number of bytes written, or `-1` on error.

### `close`
```c
#include <unistd.h>

int close(int fd);
```
- **Description:** Closes a file descriptor, releasing underlying kernel resources.

---

## 3. POSIX Shared Memory APIs

### `shm_open`
```c
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int shm_open(const char *name, int oflag, mode_t mode);
```
- **Description:** Creates or opens a POSIX shared memory object under `/dev/shm`.
- **Flags:** `O_CREAT` (create if absent), `O_RDWR` (read/write), `O_RDONLY` (read-only).
- **Linking:** Append `-lrt` on Linux.

### `ftruncate`
```c
#include <unistd.h>

int ftruncate(int fd, off_t length);
```
- **Description:** Truncates or extends the shared memory segment to `length` bytes.

### `mmap` / `munmap`
```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);
```
- **Description:** Maps files or shared memory into process virtual address space.
- **Protection:** `PROT_READ`, `PROT_WRITE`.
- **Flags:** `MAP_SHARED` (updates visible to other processes), `MAP_PRIVATE` (copy-on-write).

### `shm_unlink`
```c
#include <sys/mman.h>

int shm_unlink(const char *name);
```
- **Description:** Removes the shared memory object name from `/dev/shm`.

---

## 4. POSIX Thread (Pthread) APIs

Linking Flag: `-pthread`

### `pthread_create` / `pthread_join`
```c
#include <pthread.h>

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);
int pthread_join(pthread_t thread, void **retval);
```
- **Description:** Spawns a thread (`pthread_create`) or awaits thread completion (`pthread_join`).
- **Return:** Returns `0` on success; returns positive integer error code directly on failure.

### Mutex Operations
```c
#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```
- **Description:** Manages mutual exclusion locks for protecting critical sections.

---

## 5. POSIX Semaphore APIs

Linking Flag: `-pthread`

### `sem_init` / `sem_wait` / `sem_post` / `sem_destroy`
```c
#include <semaphore.h>

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);
int sem_destroy(sem_t *sem);
```
- **Description:** Manages counting semaphores for thread/process coordination.
- **`pshared` parameter:** Pass `0` for intra-process thread synchronization.
- **`value` parameter:** Initial permit count.
