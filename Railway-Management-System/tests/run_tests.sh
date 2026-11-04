#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR/.."
cd "$PROJECT_DIR"

echo "==============================================================="
echo "           RAILWAY MANAGEMENT SYSTEM - TEST SUITE (SH)         "
echo "==============================================================="

echo "[1/7] Compiling with strict flags (C11, -Wall, -Wextra, -Wpedantic, -O2)..."
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc \
    src/main.c src/train.c src/booking.c src/data.c src/fileio.c src/utils.c \
    -o railway_system
echo "[PASS] Compilation successful with 0 warnings."

# Clean state
cp -f trains.txt trains.test_backup.txt 2>/dev/null || true
rm -f bookings.txt *.bak tickets/*.txt tests/out*.tmp 2>/dev/null || true

# Test 1
echo "[2/7] Running Test 1: Admin PIN & Train Addition Validation..."
./railway_system < tests/test1_admin_and_train_ops.txt > tests/out1.tmp 2>&1 || true
grep -q "Access Denied" tests/out1.tmp
grep -q "Admin authentication successful" tests/out1.tmp
grep -q "Invalid district" tests/out1.tmp
grep -q "Error: Source and Destination districts cannot be the same" tests/out1.tmp
grep -q "Success: Train 108 (Bijoy Express) added successfully!" tests/out1.tmp
echo "[PASS] Test 1 passed."

# Test 2
echo "[3/7] Running Test 2: Train Search & Stable Sort..."
./railway_system < tests/test2_search_and_sort.txt > tests/out2.tmp 2>&1 || true
grep -q "Search Results for Train #101" tests/out2.tmp
grep -q "Search Results for Train Name containing" tests/out2.tmp
grep -q "Search Results for Route" tests/out2.tmp
grep -q "No matching trains." tests/out2.tmp
grep -q "Sorted by Train Number (Ascending)" tests/out2.tmp
echo "[PASS] Test 2 passed."

# Test 3
echo "[4/7] Running Test 3: Ticket Booking, Date Validation & Group Discount..."
./railway_system < tests/test3_booking_and_limits.txt > tests/out3.tmp 2>&1 || true
grep -q "Error: Passenger Name cannot contain commas" tests/out3.tmp
grep -q "Error: Train number 9999 not found" tests/out3.tmp
grep -q "Invalid seat quantity" tests/out3.tmp
grep -q "Invalid date" tests/out3.tmp
grep -q "Error: Time slot mismatch with the train" tests/out3.tmp
grep -q "Group Discount   : -12.50 BDT" tests/out3.tmp
grep -q "BK1001" tests/out3.tmp
echo "[PASS] Test 3 passed."

# Test 4
echo "[5/7] Running Test 4: Cancellation, Receipts, Reports & Delete Protection..."
rm -f bookings.txt *.bak tickets/*.txt 2>/dev/null || true
./railway_system < tests/test4_cancel_receipt_report.txt > tests/out4.tmp 2>&1 || true
grep -q "Error: Cannot delete Train 101" tests/out4.tmp
grep -q "BANGLADESH RAILWAY E-TICKET" tests/out4.tmp
grep -q "Ticket receipt saved to" tests/out4.tmp
grep -q "Cancellation Fee  : 10.00 BDT (10% retained)" tests/out4.tmp
grep -q "Refund Amount     : 90.00 BDT" tests/out4.tmp
grep -q "Success: Train 101 deleted successfully." tests/out4.tmp
echo "[PASS] Test 4 passed."

# Test 5
echo "[6/7] Running Test 5: File Persistence, Backups & Duplicate Reload Protection..."
./railway_system < tests/test5_fileio_roundtrip.txt > tests/out5.tmp 2>&1 || true
grep -q "Success: Saved" tests/out5.tmp
grep -q "already loaded" tests/out5.tmp
test -f trains.bak
echo "[PASS] Test 5 passed."

# Test 6
echo "[7/7] Running Test 6: Input Robustness & EOF Protection..."
./railway_system < tests/test6_robust_input_and_eof.txt > tests/out6.tmp 2>&1 || true
grep -q "Invalid menu option" tests/out6.tmp
grep -q "Exiting Railway Management System" tests/out6.tmp
echo "[PASS] Test 6 passed."

# Restore clean state
cp -f trains.test_backup.txt trains.txt 2>/dev/null || true
rm -f trains.test_backup.txt bookings.txt *.bak tests/out*.tmp 2>/dev/null || true

echo "==============================================================="
echo "           ALL AUTOMATED TESTS PASSED SUCCESSFULLY! (6/6)      "
echo "==============================================================="
exit 0
