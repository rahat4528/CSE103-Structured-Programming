#!/usr/bin/env bash
# ==============================================================================
# OS C Programming Lab - Automated Smoke Test Suite
# Target: Linux / POSIX Shell (bash)
# ==============================================================================

set -u

# Terminal Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
NC='\033[0m' # No Color

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

BUILD_DIR="build"

echo -e "${BOLD}${BLUE}============================================================${NC}"
echo -e "${BOLD}${BLUE}       OS C Programming Lab: Automated Smoke Test Suite     ${NC}"
echo -e "${BOLD}${BLUE}============================================================${NC}"
echo ""

# Ensure we are in project root
if [ ! -f "Makefile" ]; then
    if [ -f "../Makefile" ]; then
        cd ..
    fi
fi

# Ensure clean build
echo -e "${YELLOW}[Step 1] Building all targets via Makefile...${NC}"
make clean > /dev/null 2>&1
if ! make all > /dev/null 2>&1; then
    echo -e "${RED}[FATAL] Compilation failed during 'make all'!${NC}"
    exit 1
fi
echo -e "${GREEN}[OK] All programs compiled successfully.${NC}"
echo ""

# Helper test runner function
run_test() {
    local name="$1"
    local bin="$2"
    local args="${3:-}"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -n "  Testing: ${name}... "

    if [ ! -x "$bin" ]; then
        echo -e "${RED}[FAILED] Executable '$bin' not found or not executable!${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi

    # Run binary and capture output
    local output
    if output=$("$bin" $args 2>&1); then
        echo -e "${GREEN}[PASSED]${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    else
        echo -e "${RED}[FAILED] (Exit code: $?)${NC}"
        echo -e "    Error output:\n${output}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
}

echo -e "${YELLOW}[Step 2] Executing Category 01: Practice Tests...${NC}"
run_test "Process Fork" "$BUILD_DIR/practice/process_creation/fork_example"
run_test "Process Wait & Reap" "$BUILD_DIR/practice/process_creation/fork_wait"
run_test "Thread Create" "$BUILD_DIR/practice/threads/pthread_create"
run_test "Thread Join Concurrency" "$BUILD_DIR/practice/threads/pthread_join"
run_test "Dynamic Malloc" "$BUILD_DIR/practice/memory_allocation/malloc_example"
run_test "Zeroed Calloc" "$BUILD_DIR/practice/memory_allocation/calloc_example"
run_test "Dynamic Realloc" "$BUILD_DIR/practice/memory_allocation/realloc_example"
echo ""

echo -e "${YELLOW}[Step 3] Executing Category 02: Class Work Tests...${NC}"
run_test "Basic Pipe" "$BUILD_DIR/class_work/pipes/basic_pipe"
run_test "Parent-Child Pipe" "$BUILD_DIR/class_work/pipes/parent_child_pipe"
run_test "File Stream Write" "$BUILD_DIR/class_work/file_handling/file_write"
run_test "File Stream Append" "$BUILD_DIR/class_work/file_handling/file_append"
run_test "File Stream Read" "$BUILD_DIR/class_work/file_handling/file_read"
echo ""

echo -e "${YELLOW}[Step 4] Executing Category 03: Assignments Tests...${NC}"
# Shared Memory Write followed by Read
run_test "Shared Memory Write" "$BUILD_DIR/assignments/shared_memory/shared_memory_write"
run_test "Shared Memory Read & Unlink" "$BUILD_DIR/assignments/shared_memory/shared_memory_read"
# Clean up in case
rm -f /dev/shm/os_c_shm_demo

# CPU Scheduling algorithms with verified default dataset
run_test "FCFS Scheduling" "$BUILD_DIR/assignments/cpu_scheduling/fcfs"
run_test "SJF Scheduling" "$BUILD_DIR/assignments/cpu_scheduling/sjf"
run_test "Round Robin Scheduling" "$BUILD_DIR/assignments/cpu_scheduling/round_robin"
echo ""

echo -e "${YELLOW}[Step 5] Executing Category 04: Labs Tests...${NC}"
run_test "Mutex Counter Sync" "$BUILD_DIR/labs/mutex/mutex_counter"
run_test "Counting Semaphore" "$BUILD_DIR/labs/semaphores/semaphore_example"
run_test "Producer-Consumer Queue" "$BUILD_DIR/labs/semaphores/producer_consumer"
echo ""

echo -e "${YELLOW}[Step 6] Executing Category 06: Final Combined Examples...${NC}"
run_test "Bidirectional Duplex Pipe IPC" "$BUILD_DIR/final_examples/process_communication"
run_test "Thread Synchronization Bank" "$BUILD_DIR/final_examples/thread_synchronization"
run_test "Dynamic Vector Lifecycle" "$BUILD_DIR/final_examples/memory_management"
echo ""

echo -e "${BOLD}${BLUE}============================================================${NC}"
echo -e "${BOLD}                       TEST SUMMARY                         ${NC}"
echo -e "${BOLD}${BLUE}============================================================${NC}"
echo -e "  Total Tests Executed : ${TOTAL_TESTS}"
echo -e "  Tests Passed         : ${GREEN}${PASSED_TESTS}${NC}"
echo -e "  Tests Failed         : ${RED}${FAILED_TESTS}${NC}"

if [ "$FAILED_TESTS" -eq 0 ]; then
    echo -e "${BOLD}${GREEN}>>> ALL TESTS PASSED! Operating Systems Repository is verified. <<<${NC}"
    exit 0
else
    echo -e "${BOLD}${RED}>>> SOME TESTS FAILED! Check outputs above. <<<${NC}"
    exit 1
fi

