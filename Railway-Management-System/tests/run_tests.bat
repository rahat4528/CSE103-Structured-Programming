@echo off
setlocal enabledelayedexpansion

echo ===============================================================
echo            RAILWAY MANAGEMENT SYSTEM - TEST SUITE             
echo ===============================================================

set TEST_FAILED=0
set PROJECT_DIR=%~dp0..
cd /d "%PROJECT_DIR%"

echo [1/7] Compiling with strict flags (C11, -Wall, -Wextra, -Wpedantic, -O2)...
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc src/main.c src/train.c src/booking.c src/data.c src/fileio.c src/utils.c -o railway_system.exe
if errorlevel 1 (
    echo [FAIL] Compilation failed with errors!
    exit /b 1
)
echo [PASS] Clean compilation with 0 warnings.
echo.

:: Ensure clean state for test run
copy /y trains.txt trains.test_backup.txt >nul 2>&1
del /q bookings.txt *.bak tickets\*.txt tests\out*.tmp >nul 2>&1

:: Test 1: Admin Authentication, Validation, District Constraints and Add Train
echo [2/7] Running Test 1: Admin PIN and Train Addition Validation...
railway_system.exe < tests\test1_admin_and_train_ops.txt > tests\out1.tmp 2>&1
findstr /C:"Access Denied" tests\out1.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 1: Missing Access Denied for invalid PIN
    set TEST_FAILED=1
)
findstr /C:"Admin authentication successful" tests\out1.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 1: Missing Admin success
    set TEST_FAILED=1
)
findstr /C:"Invalid district" tests\out1.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 1: Missing invalid district detection
    set TEST_FAILED=1
)
findstr /C:"Error: Source and Destination districts cannot be the same" tests\out1.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 1: Missing same src/dst rejection
    set TEST_FAILED=1
)
findstr /C:"Success: Train 108 (Bijoy Express) added successfully!" tests\out1.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 1: Missing train addition confirmation
    set TEST_FAILED=1
)
findstr /C:"BANGLADESH DISTRICTS BY DIVISION" tests\out1.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 1: Missing District table output
    set TEST_FAILED=1
)
if "!TEST_FAILED!"=="0" (
    echo [PASS] Test 1 passed successfully.
)
echo.

:: Test 2: Train Search (Number, Keyword, District, Slot, Route) and Sorting
echo [3/7] Running Test 2: Train Search and Stable Sort...
railway_system.exe < tests\test2_search_and_sort.txt > tests\out2.tmp 2>&1
findstr /C:"Search Results for Train #101" tests\out2.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 2: Missing search by number
    set TEST_FAILED=1
)
findstr /C:"Search Results for Train Name containing" tests\out2.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 2: Missing search by keyword
    set TEST_FAILED=1
)
findstr /C:"Search Results for Route" tests\out2.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 2: Missing route search
    set TEST_FAILED=1
)
findstr /C:"No matching trains." tests\out2.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 2: Missing zero-match handling
    set TEST_FAILED=1
)
findstr /C:"Sorted by Train Number (Ascending)" tests\out2.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 2: Missing train number sort
    set TEST_FAILED=1
)
findstr /C:"Sorted by Price" tests\out2.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 2: Missing price sort
    set TEST_FAILED=1
)
if "!TEST_FAILED!"=="0" (
    echo [PASS] Test 2 passed successfully.
)
echo.

:: Test 3: Booking, Date Validation, Seat Limits, and 5%% Group Discount
echo [4/7] Running Test 3: Ticket Booking, Date Validation and Group Discount...
railway_system.exe < tests\test3_booking_and_limits.txt > tests\out3.tmp 2>&1
findstr /C:"Error: Passenger Name cannot contain commas" tests\out3.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 3: Missing comma rejection in name
    set TEST_FAILED=1
)
findstr /C:"Error: Train number 9999 not found" tests\out3.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 3: Missing non-existent train check
    set TEST_FAILED=1
)
findstr /C:"Invalid seat quantity" tests\out3.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 3: Missing seat limit check
    set TEST_FAILED=1
)
findstr /C:"Invalid date" tests\out3.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 3: Missing date validation
    set TEST_FAILED=1
)
findstr /C:"Error: Time slot mismatch with the train" tests\out3.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 3: Missing slot mismatch check
    set TEST_FAILED=1
)
findstr /C:"Group Discount   : -12.50 BDT" tests\out3.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 3: Missing group discount calculation
    set TEST_FAILED=1
)
findstr /C:"BK1001" tests\out3.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 3: Missing BK1001 booking ID
    set TEST_FAILED=1
)
if "!TEST_FAILED!"=="0" (
    echo [PASS] Test 3 passed successfully.
)
echo.

:: Test 4: Cancellation, Refund/Fee, Receipts, Reports, Delete Blocked
echo [5/7] Running Test 4: Cancellation, Receipts, Reports and Delete Protection...
del /q bookings.txt *.bak tickets\*.txt >nul 2>&1
railway_system.exe < tests\test4_cancel_receipt_report.txt > tests\out4.tmp 2>&1
findstr /C:"Error: Cannot delete Train 101" tests\out4.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 4: Missing active booking deletion block
    set TEST_FAILED=1
)
findstr /C:"BANGLADESH RAILWAY E-TICKET" tests\out4.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 4: Missing ticket receipt header
    set TEST_FAILED=1
)
findstr /C:"Ticket receipt saved to" tests\out4.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 4: Missing ticket file write
    set TEST_FAILED=1
)
findstr /C:"Cancellation Fee  : 10.00 BDT" tests\out4.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 4: Missing 10%% cancellation fee deduction
    set TEST_FAILED=1
)
findstr /C:"Refund Amount     : 90.00 BDT" tests\out4.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 4: Missing net refund amount
    set TEST_FAILED=1
)
findstr /C:"Success: Train 101 deleted successfully." tests\out4.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 4: Missing train deletion after booking cancel
    set TEST_FAILED=1
)
if "!TEST_FAILED!"=="0" (
    echo [PASS] Test 4 passed successfully.
)
echo.

:: Test 5: File I/O, Save/Load Roundtrip, and Backups
echo [6/7] Running Test 5: File Persistence, Backups and Duplicate Reload Protection...
railway_system.exe < tests\test5_fileio_roundtrip.txt > tests\out5.tmp 2>&1
findstr /C:"Success: Saved" tests\out5.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 5: Missing save confirmation
    set TEST_FAILED=1
)
findstr /C:"already loaded" tests\out5.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 5: Missing duplicate reload detection
    set TEST_FAILED=1
)
if exist trains.bak (
    echo [PASS] Backup file trains.bak verified.
) else (
    echo [FAIL] Test 5: trains.bak backup not created!
    set TEST_FAILED=1
)
if "!TEST_FAILED!"=="0" (
    echo [PASS] Test 5 passed successfully.
)
echo.

:: Test 6: Malformed Input, Over-long Strings and Graceful EOF
echo [7/7] Running Test 6: Input Robustness and EOF Protection...
railway_system.exe < tests\test6_robust_input_and_eof.txt > tests\out6.tmp 2>&1
findstr /C:"Invalid menu option" tests\out6.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 6: Missing invalid menu handling
    set TEST_FAILED=1
)
findstr /C:"Exiting Railway Management System" tests\out6.tmp >nul
if errorlevel 1 (
    echo [FAIL] Test 6: Missing clean exit on EOF
    set TEST_FAILED=1
)
if "!TEST_FAILED!"=="0" (
    echo [PASS] Test 6 passed successfully.
)
echo.

:: Clean up temporary test files
copy /y trains.test_backup.txt trains.txt >nul 2>&1
del /q trains.test_backup.txt bookings.txt *.bak tests\out*.tmp >nul 2>&1

echo ===============================================================
if "!TEST_FAILED!"=="0" (
    echo            ALL AUTOMATED TESTS PASSED SUCCESSFULLY! (6/6)      
    echo ===============================================================
    exit /b 0
) else (
    echo            SOME AUTOMATED TESTS FAILED. INVESTIGATE LOGS.     
    echo ===============================================================
    exit /b 1
)
