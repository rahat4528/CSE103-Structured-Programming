# Memory Allocation Practice

This folder covers dynamic heap memory management in C using the standard C library runtime (`malloc`, `calloc`, `realloc`, and `free`), defensive memory checks, and leak prevention.

---

## 1. malloc_example.c

- **Program Name:** `malloc_example.c`
- **OS Concept:** Dynamic Memory Allocation, Heap Management, Pointer Arithmetic.
- **Objective:** Allocate uninitialized memory dynamically on the heap with `malloc()`, check for NULL pointer failures, access items safely, and deallocate with `free()`.
- **Required Headers:**
  - `<stdio.h>`: Standard I/O.
  - `<stdlib.h>`: `malloc`, `free`, `EXIT_SUCCESS`, `EXIT_FAILURE`.
- **Important Functions:**
  - `malloc(size_t size)`: Allocates `size` bytes of uninitialized memory from the process heap.
  - `free(void *ptr)`: Returns previously allocated memory back to the heap manager.
- **How It Works:**
  1. Requests `5 * sizeof(int)` bytes on the heap.
  2. Verifies `numbers != NULL`.
  3. Populates array elements with square numbers and computes their summation.
  4. Calls `free(numbers)` to release heap memory and sets `numbers = NULL` to prevent dangling pointer bugs.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic practice/memory_allocation/malloc_example.c -o build/practice/memory_allocation/malloc_example
  ```
- **Execution Command:**
  ```bash
  ./build/practice/memory_allocation/malloc_example
  ```
- **Expected Output:**
  ```text
  ====================================================
        Dynamic Memory Allocation: malloc() & free()  
  ====================================================

  [1] Requesting heap memory for 5 integers (20 bytes)...
      Allocation successful. Heap address: 0x55d78e1ac2a0

  [2] Populating array with square values:
      numbers[0] =  1  (Stored at address: 0x55d78e1ac2a0)
      numbers[1] =  4  (Stored at address: 0x55d78e1ac2a4)
      numbers[2] =  9  (Stored at address: 0x55d78e1ac2a8)
      numbers[3] = 16  (Stored at address: 0x55d78e1ac2ac)
      numbers[4] = 25  (Stored at address: 0x55d78e1ac2b0)

  [3] Computed sum of array elements: 55

  [4] Deallocating heap memory using free()...
      Memory successfully freed. Pointer reset to NULL ((nil)).
  ====================================================
  ```
- **Common Errors:**
  - Not checking if `malloc()` returns `NULL` when system memory is exhausted.
  - Forgetting to call `free()`, causing a memory leak.
  - Accessing memory after calling `free()` (use-after-free vulnerability).

---

## 2. calloc_example.c

- **Program Name:** `calloc_example.c`
- **OS Concept:** Contiguous Zero-Initialized Memory Allocation.
- **Objective:** Allocate contiguous heap memory with `calloc()`, verify automatic zero-initialization of bytes, modify contents, and cleanly deallocate.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
- **Important Functions:**
  - `calloc(size_t nmemb, size_t size)`: Allocates memory for an array of `nmemb` elements of `size` bytes each and sets all bytes to zero.
  - `free(void *ptr)`: Releases heap block.
- **How It Works:**
  1. Calls `calloc(6, sizeof(int))`.
  2. Iterates across all 6 elements immediately and validates that every integer is `0`.
  3. Updates elements with multiples of 10.
  4. Releases the allocated buffer using `free()`.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic practice/memory_allocation/calloc_example.c -o build/practice/memory_allocation/calloc_example
  ```
- **Execution Command:**
  ```bash
  ./build/practice/memory_allocation/calloc_example
  ```
- **Expected Output:**
  ```text
  ====================================================
        Zero-Initialized Allocation: calloc()         
  ====================================================

  [1] Requesting calloc() for 6 integers (24 bytes total)...
      Allocation successful. Base Address: 0x55d78e1ac2a0

  [2] Inspecting initial contents immediately after calloc():
      array[0] = 0
      array[1] = 0
      array[2] = 0
      array[3] = 0
      array[4] = 0
      array[5] = 0
      --> Verified: All 6 elements were automatically initialized to 0!

  [3] Populating array with multiples of 10:
      array[0] updated to 10
      array[1] updated to 20
      array[2] updated to 30
      array[3] updated to 40
      array[4] updated to 50
      array[5] updated to 60

  [4] Releasing allocated memory via free()...
      Memory released cleanly. Pointer set to NULL.
  ====================================================
  ```
- **Common Errors:**
  - Assuming `malloc()` zeros memory: `malloc()` leaves indeterminate garbage bits, whereas `calloc()` clears memory.
  - Forgetting that `calloc` takes two separate arguments `(nmemb, size)` rather than a single multiplication.

---

## 3. realloc_example.c

- **Program Name:** `realloc_example.c`
- **OS Concept:** Dynamic Heap Buffer Resizing, Heap Relocation, Defensive Memory Reallocation.
- **Objective:** Expand an active dynamic allocation from 3 elements to 6 elements using `realloc()`, prevent memory leaks on failure using temporary pointers, and append new values.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
- **Important Functions:**
  - `realloc(void *ptr, size_t size)`: Changes the size of the memory block pointed to by `ptr` to `size` bytes. May expand in-place or allocate a new block, copy existing data, and free the old block.
  - `free(void *ptr)`: Deallocates buffer.
- **How It Works:**
  1. Allocates an initial array of 3 integers with `malloc()`.
  2. Populates initial values `[85, 92, 78]`.
  3. Uses `int *temp = realloc(scores, 6 * sizeof(int))` to safely attempt expansion.
  4. If successful, assigns `scores = temp` and populates indices 3, 4, 5.
  5. Displays all 6 elements and frees the block.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic practice/memory_allocation/realloc_example.c -o build/practice/memory_allocation/realloc_example
  ```
- **Execution Command:**
  ```bash
  ./build/practice/memory_allocation/realloc_example
  ```
- **Expected Output:**
  ```text
  ====================================================
       Dynamic Memory Resizing: realloc() Demo        
  ====================================================

  [1] Initial allocation with malloc() for 3 integers...
      Initial buffer address: 0x55d78e1ac2a0
  [2] Initial array contents:
      scores[0] = 85
      scores[1] = 92
      scores[2] = 78

  [3] Resizing array from 3 to 6 integers using realloc()...
      Resizing successful!
      New buffer address    : 0x55d78e1ac2a0

  [4] Displaying full expanded array contents:
      scores[0] =  85  (Element address: 0x55d78e1ac2a0)
      scores[1] =  92  (Element address: 0x55d78e1ac2a4)
      scores[2] =  78  (Element address: 0x55d78e1ac2a8)
      scores[3] =  95  (Element address: 0x55d78e1ac2ac)
      scores[4] =  88  (Element address: 0x55d78e1ac2b0)
      scores[5] = 100  (Element address: 0x55d78e1ac2b4)

  [5] Freeing entire reallocated block...
      Deallocation complete. Memory safely reclaimed.
  ====================================================
  ```
- **Common Errors:**
  - Writing `ptr = realloc(ptr, new_size)`. If `realloc()` fails and returns `NULL`, the original pointer `ptr` is overwritten and its memory is orphaned (leaked permanently).
  - Accessing the old pointer after `realloc()` moved the buffer to a new heap address.

