# File Handling Practice and Class Work

This folder covers file stream operations in C (`fopen`, `fgets`, `fprintf`, `fputs`, `fclose`), append modes, error recovery, and the fundamental differences between Standard C stream I/O and POSIX system call I/O.

---

## Standard C Streams (`FILE*`) vs Low-Level POSIX System Calls (`int fd`)

| Feature | Standard C Library Streams (`FILE*`) | POSIX System Calls (`int fd`) |
|---|---|---|
| **Underlying Type** | Opaque structure pointer (`FILE*`) | Integer file descriptor (`int fd`) |
| **Header File** | `<stdio.h>` | `<fcntl.h>`, `<unistd.h>`, `<sys/stat.h>` |
| **Key Functions** | `fopen()`, `fclose()`, `fread()`, `fwrite()`, `fgets()`, `fprintf()` | `open()`, `close()`, `read()`, `write()`, `lseek()`, `ioctl()` |
| **Buffering** | **User-space buffered** (reduces expensive context switches into kernel) | **Unbuffered** (every call triggers an immediate trap into kernel space) |
| **Portability** | ANSI/ISO C standard (works on Windows, Linux, macOS, bare metal) | POSIX-compliant operating systems (UNIX, Linux, macOS) |
| **Efficiency for Small I/O**| High: aggregates small transfers into 4KB/8KB buffer | Low: multiple system call traps incur high CPU overhead |
| **Direct Hardware / IPC Control** | Limited to filesystem streams | Direct: pipes, sockets, device nodes (`/dev`), terminal settings (`termios`) |

---

## 1. file_read.c

- **Program Name:** `file_read.c`
- **OS Concept:** Buffered Stream Reading, File Access Checks, End-of-File (EOF) Detection.
- **Objective:** Open an existing text file in read mode (`"r"`), gracefully handle missing files, read line-by-line using `fgets()`, and close the stream.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<string.h>`
- **Important Functions:**
  - `fopen(const char *path, const char *mode)`: Opens stream for file at `path`.
  - `fgets(char *s, int size, FILE *stream)`: Reads at most `size - 1` characters until newline or EOF.
  - `fread(void *ptr, size_t size, size_t nmemb, FILE *stream)`: Reads buffered byte/record blocks directly into memory.
  - `ferror(FILE *stream)`: Checks whether a stream read/write error occurred.
  - `fclose(FILE *stream)`: Flushes and releases the file stream.
- **How It Works:**
  1. Checks if a file path was passed in `argv[1]` or defaults to `class_work/file_handling/sample_input.txt`.
  2. Calls `fopen(filepath, "r")`. If the file does not exist, outputs an explanatory error and exits.
  3. Loops with `fgets()` until EOF is reached, strips trailing `\r`/`\n`, and prints each numbered line.
  4. Rewinds and performs a buffered `fread()` pass to report the total file byte count.
  5. Closes the file with `fclose()`.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic class_work/file_handling/file_read.c -o build/class_work/file_handling/file_read
  ```
- **Execution Command:**
  ```bash
  ./build/class_work/file_handling/file_read
  ```
- **Expected Output:**
  ```text
  ====================================================
          File Stream Reading: file_read.c            
  ====================================================

  [1] Attempting to open file: "class_work/file_handling/sample_input.txt" in read mode ("r")...
      File opened successfully! Reading contents line by line:

  -------------------- FILE CONTENTS --------------------
    Line 01: Operating Systems Laboratory
    Line 02: Course: Operating Systems & System Programming
    Line 03: Topic: File Subsystems and Stream Buffering
    Line 04: Key Concept: Standard C Streams vs POSIX File Descriptors
    Line 05: File Handling Status: Ready
  -------------------------------------------------------

  [2] Closing file stream with fclose()...
      File stream closed cleanly.
  ====================================================
  ```
- **Common Errors:**
  - Omitting the check for `file == NULL`, resulting in segmentation faults when calling `fgets()` on a NULL pointer.
  - Using `gets()` (unsafe, obsolete, removed in C11) instead of `fgets()`.

---

## 2. file_write.c

- **Program Name:** `file_write.c`
- **OS Concept:** File Creation, Truncation, Formatted File Stream Serialization.
- **Objective:** Create a new file or truncate an existing file using `"w"` mode, write formatted text records with `fprintf()`, and flush changes to disk.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<string.h>`
  - `<sys/stat.h>`
- **Important Functions:**
  - `fopen(path, "w")`: Opens file for writing (truncating existing contents).
  - `fprintf(stream, format, ...)`: Writes formatted string into stream buffer.
  - `fputs(str, stream)`: Writes unformatted string to stream.
  - `fwrite(ptr, size, nmemb, stream)`: Writes fixed-size memory blocks directly to the stream buffer.
  - `fclose(stream)`: Flushes internal C library buffer into the kernel and closes descriptor.
- **How It Works:**
  1. Opens `class_work/file_handling/demo_output.txt` with mode `"w"`.
  2. Writes structured lab header and status metrics using `fputs()`, `fprintf()`, and a block footer via `fwrite()`.
  3. Closes stream with `fclose()`, ensuring all buffered bytes are committed to the filesystem.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic class_work/file_handling/file_write.c -o build/class_work/file_handling/file_write
  ```
- **Execution Command:**
  ```bash
  ./build/class_work/file_handling/file_write
  ```
- **Expected Output:**
  ```text
  ====================================================
          File Stream Writing: file_write.c           
  ====================================================

  [1] Opening destination file: "class_work/file_handling/demo_output.txt" in write mode ("w")...
      Note: Mode "w" creates a new file or truncates an existing file.
      File opened successfully for writing.

  [2] Writing records to file stream...
      Records written successfully.

  [3] Closing file stream via fclose()...
      File stream flushed to disk and closed.
      You can verify contents using: cat class_work/file_handling/demo_output.txt
  ====================================================
  ```
- **Common Errors:**
  - Using `"w"` mode when intending to add to an existing file, unintentionally wiping out previous data.
  - Not checking `fclose()` error status on storage full conditions.

---

## 3. file_append.c

- **Program Name:** `file_append.c`
- **OS Concept:** File Offset Positioning at EOF, Non-Destructive Appending, File Log Maintenance.
- **Objective:** Open an existing file in append mode (`"a"`), add timestamped log records to the end of the file, and read back the updated contents to verify.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<time.h>`: Timestamp formatting (`time`, `localtime`, `strftime`).
- **Important Functions:**
  - `fopen(path, "a")`: Opens file for appending (writes always occur at the end of file).
  - `fclose()`: Flushes appended records.
- **How It Works:**
  1. Opens `class_work/file_handling/demo_output.txt` with mode `"a"`.
  2. Formats a current timestamp string.
  3. Writes an appended log entry and closes the write stream.
  4. Re-opens in `"r"` mode to print the entire updated file to the terminal.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic class_work/file_handling/file_append.c -o build/class_work/file_handling/file_append
  ```
- **Execution Command:**
  ```bash
  ./build/class_work/file_handling/file_append
  ```
- **Expected Output:**
  ```text
  ====================================================
          File Stream Appending: file_append.c        
  ====================================================

  [1] Opening file "class_work/file_handling/demo_output.txt" in append mode ("a")...
  [2] Appending new log entry to "class_work/file_handling/demo_output.txt"...
      Entry appended and buffer flushed.

  [3] Reading and displaying updated file contents:
  ---------------- UPDATED FILE CONTENTS ----------------
    [01] # Operating Systems Lab Records
    [02] ----------------------------------------
    [03] Process ID  : 1001
    [04] Module Name : Process Scheduler Simulation
    [05] Execution   : Successful
    [06] CPU Slices  : 4
    [07] ----------------------------------------
    [08] [LOG ENTRY] Timestamp: 2026-11-11 02:45:10 | Action: Appending new OS record
  -------------------------------------------------------
  ====================================================
  ```
- **Common Errors:**
  - Assuming `fseek()` can change the write position in append mode: POSIX guarantees that all writes in `"a"` mode are placed at the actual end of the file regardless of previous seek operations.

---

## Crash-Safe Atomic Persistence Note (Connecting to CSE103 Systems)

Writing directly over an existing data file with `"w"` mode truncates the file immediately—if the process crashes mid-write, the old data is lost. In production systems (and in the sibling CSE103 projects `Railway-Management-System` and `c-blood-group-management`), safe persistence writes to a temporary `.tmp` file (checking `fwrite`/`fprintf`, `fflush`, and `fclose`) and only replaces the target file via `rename(tmp_path, target_path)` (optionally keeping a `.bak` copy via a buffered `fread`/`fwrite` loop).

