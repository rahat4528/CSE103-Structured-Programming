# Operating Systems Lab Troubleshooting & Debugging Guide

This guide provides practical solutions for common runtime, compilation, and system-level issues encountered during OS laboratory exercises.

---

## 1. Stale Shared Memory Objects in `/dev/shm`

### Problem:
Running `shared_memory_read` fails with corrupted data, or previous runs leave uncleaned memory objects.

### Root Cause:
POSIX shared memory persists in the kernel under the tmpfs filesystem at `/dev/shm/` until system reboot or until explicitly unlinked with `shm_unlink()`.

### Solution:
Inspect and remove active shared memory files directly:
```bash
# List active shared memory files
ls -l /dev/shm/

# Clean up our lab object manually if unlinking was interrupted
rm -f /dev/shm/os_c_shm_demo
```

---

## 2. Linking & Compilation Errors

### A. `undefined reference to 'pthread_create'` or `'pthread_join'`
- **Fix:** Add `-pthread` flag to the gcc command.
  ```bash
  gcc -pthread program.c -o program
  ```

### B. `undefined reference to 'shm_open'` or `'shm_unlink'`
- **Fix:** Link the realtime extensions library by appending `-lrt` to the command line:
  ```bash
  gcc program.c -o program -lrt
  ```

### C. `warning: implicit declaration of function 'waitpid'` or `'usleep'`
- **Fix:** Expose POSIX.1-2008 prototypes by adding `#define _POSIX_C_SOURCE 200809L` at the very top of the `.c` file before any header includes, or pass `-D_POSIX_C_SOURCE=200809L` to GCC.

---

## 3. Pipe Communication Pitfalls

### A. Process Hangs Indefinitely (Pipe Deadlock)
- **Symptom:** The reader process never finishes and remains blocked in `read()`.
- **Cause:** The reader process (or parent) kept a clone of the pipe's write descriptor open. As long as any open write descriptor exists anywhere in the system, `read()` will not return EOF ($0$).
- **Rule:** **Always close unused pipe ends immediately after calling `fork()`!**

### B. Program Terminates Abruptly with "Broken pipe" (SIGPIPE)
- **Symptom:** Process crashes without printing error messages.
- **Cause:** A process called `write()` on a pipe whose read end has been closed by all reading processes. The kernel sends signal `SIGPIPE` (signal 13), whose default action is process termination.

---

## 4. Process Lifecycles & Zombie Defuncts

### A. Zombie Processes Accumulating
- **Check via terminal:**
  ```bash
  ps aux | grep Z
  ```
- **Cause:** A parent process spawned children but never invoked `wait()` or `waitpid()`.
- **Fix:** Always reap children in the parent process:
  ```c
  int status;
  waitpid(child_pid, &status, 0);
  ```

---

## 5. Segmentation Faults & Memory Bugs

### Diagnosing with AddressSanitizer:
Recompile with `-fsanitize=address -g`:
```bash
gcc -fsanitize=address -g program.c -o program
./program
```
AddressSanitizer will output a readable stack trace pinpointing the exact line number of out-of-bounds array access, use-after-free, or NULL pointer dereference.

### Diagnosing with GDB (GNU Debugger):
```bash
gcc -g program.c -o program
gdb ./program
(gdb) run
# Once crash occurs:
(gdb) backtrace
```
