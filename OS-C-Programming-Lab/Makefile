# ==============================================================================
# OS C PROGRAMMING LAB - MASTER MAKEFILE
# Target: Linux (POSIX.1-2008 / C11)
# Compiler: GCC
# ==============================================================================

CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
PTHREAD_FLAGS ?= -pthread
RT_FLAGS ?= -lrt

BUILD_DIR ?= build

# ------------------------------------------------------------------------------
# Source Groups
# ------------------------------------------------------------------------------

# Practice targets
PRACTICE_SRCS := \
    practice/process_creation/fork_example.c \
    practice/process_creation/fork_wait.c \
    practice/threads/pthread_create.c \
    practice/threads/pthread_join.c \
    practice/memory_allocation/malloc_example.c \
    practice/memory_allocation/calloc_example.c \
    practice/memory_allocation/realloc_example.c

# Class work targets
CLASS_WORK_SRCS := \
    class_work/pipes/basic_pipe.c \
    class_work/pipes/parent_child_pipe.c \
    class_work/file_handling/file_read.c \
    class_work/file_handling/file_write.c \
    class_work/file_handling/file_append.c

# Assignments targets
ASSIGNMENTS_SRCS := \
    assignments/shared_memory/shared_memory_write.c \
    assignments/shared_memory/shared_memory_read.c \
    assignments/cpu_scheduling/fcfs.c \
    assignments/cpu_scheduling/sjf.c \
    assignments/cpu_scheduling/round_robin.c

# Labs targets
LABS_SRCS := \
    labs/mutex/mutex_counter.c \
    labs/semaphores/semaphore_example.c \
    labs/semaphores/producer_consumer.c

# Final combined examples
FINAL_EXAMPLES_SRCS := \
    final_examples/process_communication.c \
    final_examples/thread_synchronization.c \
    final_examples/memory_management.c

# All C sources
ALL_SRCS := $(PRACTICE_SRCS) $(CLASS_WORK_SRCS) $(ASSIGNMENTS_SRCS) $(LABS_SRCS) $(FINAL_EXAMPLES_SRCS)

# Map sources to corresponding build targets
PRACTICE_BINS       := $(patsubst %.c,$(BUILD_DIR)/%,$(PRACTICE_SRCS))
CLASS_WORK_BINS     := $(patsubst %.c,$(BUILD_DIR)/%,$(CLASS_WORK_SRCS))
ASSIGNMENTS_BINS    := $(patsubst %.c,$(BUILD_DIR)/%,$(ASSIGNMENTS_SRCS))
LABS_BINS           := $(patsubst %.c,$(BUILD_DIR)/%,$(LABS_SRCS))
FINAL_EXAMPLES_BINS := $(patsubst %.c,$(BUILD_DIR)/%,$(FINAL_EXAMPLES_SRCS))
ALL_BINS            := $(PRACTICE_BINS) $(CLASS_WORK_BINS) $(ASSIGNMENTS_BINS) $(LABS_BINS) $(FINAL_EXAMPLES_BINS)

# Prep documentation files
PREP_DOCS := \
    prep/process_questions.md \
    prep/thread_questions.md \
    prep/memory_questions.md \
    prep/synchronization_questions.md \
    prep/scheduling_questions.md \
    prep/viva_revision.md

# ------------------------------------------------------------------------------
# Phony Targets
# ------------------------------------------------------------------------------
.PHONY: all practice class_work assignments labs prep final_examples test sanitize valgrind clean help dirs

# Default target
all: dirs practice class_work assignments labs final_examples
	@echo ""
	@echo "============================================================"
	@echo "  OS C Programming Lab: All binaries compiled successfully! "
	@echo "  Binaries are located in the '$(BUILD_DIR)/' directory.     "
	@echo "============================================================"

# Ensure output directory structure exists
dirs:
	@mkdir -p $(BUILD_DIR)/practice/process_creation
	@mkdir -p $(BUILD_DIR)/practice/threads
	@mkdir -p $(BUILD_DIR)/practice/memory_allocation
	@mkdir -p $(BUILD_DIR)/class_work/pipes
	@mkdir -p $(BUILD_DIR)/class_work/file_handling
	@mkdir -p $(BUILD_DIR)/assignments/shared_memory
	@mkdir -p $(BUILD_DIR)/assignments/cpu_scheduling
	@mkdir -p $(BUILD_DIR)/labs/mutex
	@mkdir -p $(BUILD_DIR)/labs/semaphores
	@mkdir -p $(BUILD_DIR)/final_examples

practice: dirs $(PRACTICE_BINS)
	@echo "[OK] Practice category built."

class_work: dirs $(CLASS_WORK_BINS)
	@echo "[OK] Class Work category built."

assignments: dirs $(ASSIGNMENTS_BINS)
	@echo "[OK] Assignments category built."

labs: dirs $(LABS_BINS)
	@echo "[OK] Labs category built."

final_examples: dirs $(FINAL_EXAMPLES_BINS)
	@echo "[OK] Final Examples category built."

# Prep target validates viva notes and questions existence
prep:
	@echo "Validating OS Exam and Viva preparation files..."
	@for doc in $(PREP_DOCS); do \
		if [ -f "$$doc" ]; then \
			echo "  [FOUND] $$doc"; \
		else \
			echo "  [MISSING] $$doc"; exit 1; \
		fi \
	done
	@echo "[OK] All 6 preparation guides verified."

# Automated smoke test suite
test: all
	@echo "Running automated smoke test suite..."
	@chmod +x scripts/smoke_test.sh
	@./scripts/smoke_test.sh

# Build with AddressSanitizer + UndefinedBehaviorSanitizer (Linux/GCC diagnostic run)
sanitize: CFLAGS += -fsanitize=address,undefined -g -O1
sanitize: clean all
	@echo "[OK] Sanitizer-instrumented binaries built in '$(BUILD_DIR)/'."
	@echo "Run individual programs or './scripts/smoke_test.sh' to check for leaks and UB."

# Run Valgrind leak checks on dynamic memory and thread synchronization binaries
valgrind: all
	@command -v valgrind >/dev/null 2>&1 || { echo "[ERROR] valgrind is not installed."; exit 1; }
	@echo "Running Valgrind leak checks on memory and synchronization examples..."
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 $(BUILD_DIR)/practice/memory_allocation/malloc_example >/dev/null
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 $(BUILD_DIR)/practice/memory_allocation/calloc_example >/dev/null
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 $(BUILD_DIR)/practice/memory_allocation/realloc_example >/dev/null
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 $(BUILD_DIR)/final_examples/memory_management >/dev/null
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 $(BUILD_DIR)/practice/threads/pthread_join >/dev/null
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 $(BUILD_DIR)/final_examples/thread_synchronization >/dev/null
	@echo "[OK] Valgrind leak checks passed with 0 leaks."

# Clean build artifacts (preserve sample_input.txt)
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)/*
	@touch $(BUILD_DIR)/.gitkeep
	@rm -f class_work/file_handling/demo_output.txt
	@echo "[OK] Clean complete."

# ------------------------------------------------------------------------------
# Compilation Rules with Category-Specific Flags
# ------------------------------------------------------------------------------

# Threads: requires -pthread
$(BUILD_DIR)/practice/threads/%: practice/threads/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(PTHREAD_FLAGS)
	@echo "  CC (pthread) $< -> $@"

# Process creation
$(BUILD_DIR)/practice/process_creation/%: practice/process_creation/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@
	@echo "  CC $< -> $@"

# Memory allocation
$(BUILD_DIR)/practice/memory_allocation/%: practice/memory_allocation/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@
	@echo "  CC $< -> $@"

# Pipes
$(BUILD_DIR)/class_work/pipes/%: class_work/pipes/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@
	@echo "  CC $< -> $@"

# File handling
$(BUILD_DIR)/class_work/file_handling/%: class_work/file_handling/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@
	@echo "  CC $< -> $@"

# Shared memory: links -lrt on Linux for POSIX shm_open/mmap
$(BUILD_DIR)/assignments/shared_memory/%: assignments/shared_memory/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(RT_FLAGS)
	@echo "  CC (shm) $< -> $@"

# CPU scheduling
$(BUILD_DIR)/assignments/cpu_scheduling/%: assignments/cpu_scheduling/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@
	@echo "  CC $< -> $@"

# Mutex: requires -pthread
$(BUILD_DIR)/labs/mutex/%: labs/mutex/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(PTHREAD_FLAGS)
	@echo "  CC (pthread) $< -> $@"

# Semaphores: requires -pthread
$(BUILD_DIR)/labs/semaphores/%: labs/semaphores/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(PTHREAD_FLAGS)
	@echo "  CC (pthread) $< -> $@"

# Final examples: thread_synchronization needs -pthread
$(BUILD_DIR)/final_examples/thread_synchronization: final_examples/thread_synchronization.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(PTHREAD_FLAGS)
	@echo "  CC (pthread) $< -> $@"

$(BUILD_DIR)/final_examples/%: final_examples/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@
	@echo "  CC $< -> $@"

# ------------------------------------------------------------------------------
# Help Target
# ------------------------------------------------------------------------------
help:
	@echo "======================================================================"
	@echo "               OS C Programming Lab - Build System Help               "
	@echo "======================================================================"
	@echo "Available Targets:"
	@echo "  make              - Build all programs across all categories (default)"
	@echo "  make all          - Same as 'make'"
	@echo "  make practice     - Build Folder 01: Process, Threads, Memory programs"
	@echo "  make class_work   - Build Folder 02: Pipes & File Handling programs"
	@echo "  make assignments  - Build Folder 03: POSIX Shared Memory & Scheduling"
	@echo "  make labs         - Build Folder 04: Mutex & Semaphore sync programs"
	@echo "  make prep         - Verify Folder 05: Exam & Viva Markdown guides"
	@echo "  make final_examples - Build Folder 06: Combined integration programs"
	@echo "  make test         - Run automated smoke test suite (scripts/smoke_test.sh)"
	@echo "  make sanitize     - Build binaries with AddressSanitizer & UBSan (-fsanitize=address,undefined)"
	@echo "  make valgrind     - Run Valgrind memory-leak checks on memory & thread targets"
	@echo "  make clean        - Remove all compiled binaries from build/"
	@echo "  make help         - Display this help information"
	@echo ""
	@echo "Building an Individual Program:"
	@echo "  make build/practice/process_creation/fork_example"
	@echo "  make build/assignments/cpu_scheduling/fcfs"
	@echo "======================================================================"

