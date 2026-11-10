# Operating Systems Viva & Exam Prep: Memory Management

## 1. Process Memory Layout in Linux
A compiled C program running in a Linux virtual address space is organized into distinct segments:

```
+------------------------------------+ High Memory (0xFFFFFFFFFFFFFFFF)
| Kernel Space (Page Tables, OS)     |
+------------------------------------+
| User Stack (grows downward \/)     | Local variables, function call frames
|                                    |
| vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv   |
|                                    |
| ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^   |
| Heap (grows upward /\)             | malloc, calloc, realloc (brk/mmap)
+------------------------------------+
| Uninitialized Data (.bss)          | Global/static variables initialized to 0
+------------------------------------+
| Initialized Data (.data)           | Global/static variables with non-zero initial values
+------------------------------------+
| Text / Code Segment (.text)        | Compiled machine instructions (Read-Only)
+------------------------------------+ Low Memory (0x0000000000000000)
```

---

## 2. Stack vs Heap Comparison

| Characteristic | Stack | Heap |
|---|---|---|
| **Allocation Mechanism** | Automatic by CPU compiler push/pop instructions | Explicit via programmer requests (`malloc`, `free`) |
| **Growth Direction** | Downward towards lower memory addresses | Upward towards higher memory addresses (`brk` / `sbrk`) |
| **Allocation Speed** | Extremely fast (moves stack pointer register `RSP`) | Slower (requires searching free lists, bins, metadata) |
| **Lifetime** | Tied to the enclosing scope/function call frame | Persists until explicitly `free()`d or process exits |
| **Size Limit** | Restricted (typically 8MB on Linux, configured via `ulimit -s`) | Limited only by available virtual memory / RAM |
| **Fragmentation** | None (strictly LIFO order) | Vulnerable to internal and external fragmentation |

---

## 3. Dynamic Allocation Functions in C

| Function | Signature | Purpose & Behavior |
|---|---|---|
| `malloc()` | `void* malloc(size_t size);` | Allocates `size` uninitialized bytes. Contains indeterminate garbage values. |
| `calloc()` | `void* calloc(size_t nmemb, size_t size);` | Allocates memory for `nmemb` elements of `size` bytes each and clears all bytes to zero (`0`). |
| `realloc()` | `void* realloc(void *ptr, size_t size);` | Resizes existing heap block. Expands in-place or relocates, copies data, and frees old block. |
| `free()` | `void free(void *ptr);` | Returns heap block to allocator. Passing `NULL` is safely ignored. |

---

## 4. Common Dynamic Memory Pitfalls

1. **Memory Leak:**
   - Memory is allocated with `malloc`/`calloc` but never released with `free()`. In long-running applications (servers, daemons), unreleased memory gradually consumes RAM until the Linux Out-Of-Memory (OOM) killer terminates the process.
2. **Dangling Pointer:**
   - A pointer referencing memory that has already been deallocated via `free()`. Accessing a dangling pointer causes undefined behavior or memory corruption.
   - *Best Practice Defense:* Always set pointer to `NULL` immediately after `free`:
     ```c
     free(ptr);
     ptr = NULL;
     ```
3. **Double Free:**
   - Calling `free()` twice on the exact same heap address causes heap metadata corruption and potential security vulnerabilities.
4. **Buffer Overflow:**
   - Writing beyond the boundary of an allocated array overwrites adjacent heap control headers or heap objects.

---

## 5. Heap Diagnostic Tools
- **Valgrind (Memcheck):**
  Runs program inside an emulated CPU sandbox to detect memory leaks, uninitialized memory reads, and out-of-bounds writes:
  ```bash
  valgrind --leak-check=full --show-leak-kinds=all ./my_program
  ```
- **AddressSanitizer (ASan):**
  GCC compiler instrumentation providing high-speed runtime memory error detection:
  ```bash
  gcc -fsanitize=address -g program.c -o program
  ```
