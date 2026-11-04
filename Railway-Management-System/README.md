# Railway Management System

[![Language](https://img.shields.io/badge/Language-C11-00599C?logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Standard](https://img.shields.io/badge/Standard-ISO%20C11%20Standard%20Library-green.svg)](https://en.wikipedia.org/wiki/C_standard_library)
[![Compiler](https://img.shields.io/badge/Compiler-GCC%2016%2B%20%7C%20Clang-blue.svg)](https://gcc.gnu.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-orange.svg)](https://github.com/rahat4528/CSE103-Structured-Programming)

A high-performance, resilient, console-based **Railway Management System** built in **ISO C11** using only standard C libraries and pure structured programming paradigms (structs, pointers, and pure functional decomposition). Translated faithfully from an architectural Java specification into standard portable C, replacing Java exceptions with bounded `fgets` stream ingestion, `strtol`/`strtod` numerical parsing, zero dynamic memory leaks, robust defensive validations, and non-blocking recovery.

---

## Key Highlights & Features

### Core Operations (Guide Options 1–8)
1. **Add Train**: Prompt-guided addition of trains with complete domain validation. Enforces positive train numbers, unique numbers (no duplicates), non-empty names without commas, valid Bangladesh districts for source and destination, non-identical source/destination, 4 official time slots, non-negative seat counts, and positive seat prices. Secured by Admin PIN authentication.
2. **View All Trains**: Formatted tabular representation of all scheduled trains rendered via `showInfo(const Train*)`. Displays train number, name, source, destination, departure time slot, seats available, and seat fare.
3. **Search Train**: Multi-criteria lookup engine supporting:
   - By Train Number (exact ID search)
   - By Name Keyword (case-insensitive substring match)
   - By District (matching either source or destination)
   - By Departure Time Slot
   - By Route (Source -> Destination pair)
4. **Book Ticket**: Interactive ticket reservation with bounded capacity (1 to 10 seats per booking). Validates travel date (format `DD-MM-YYYY`, leap-year checks, calendar days, not in the past), time slot schedule match, and seat availability. Decrements available seats only upon atomic validation success, assigns auto-incrementing booking IDs (`BK1001`, `BK1002`, ...), and applies a **5% Group Discount** for bookings of 5 or more seats.
5. **View All Bookings**: Comprehensive audit table of all customer bookings displayed via `showBooking(const Booking*)`.
6. **Load Train Data from File**: High-speed CSV parser for `trains.txt`. Validates 7 fields per record, reports exact line numbers and syntax reasons for skipped records, prevents duplicate train IDs on reload, and reports a final load summary (`X loaded, Y skipped`).
7. **Save Train Data to File**: Serializes train fleet state to `trains.txt` and bookings to `bookings.txt`. Automatically creates atomic backups (`trains.bak`, `bookings.bak`) before writes.
8. **Exit**: Graceful shutdown prompt offering to flush and persist uncommitted state to disk.

### Advanced Capabilities (Options 9–16)
9. **Cancel Booking**: Lookup by Booking ID or passenger name (with disambiguation listing). Restores reserved seats to the train's available pool, marks status as `CANCELLED`, computes a **10% cancellation fee** (`CANCEL_FEE_PERCENT 10`), and issues a detailed refund receipt.
10. **Update Train**: Modify existing train attributes (name, route, time slot, available seats, fare) while maintaining structural integrity. Blocked if reducing available seats below 0. Protected by Admin PIN.
11. **Delete Train**: Decommissions train records from fleet memory. Strictly blocked with descriptive diagnostic warnings if any active (`CONFIRMED`) customer bookings exist. Protected by Admin PIN.
12. **Sort Trains**: In-place, stable insertion sort preventing data corruption. Sort criteria:
    - By Train Number (Ascending)
    - By Time Slot (Chronological: 08:00 AM < 12:00 PM < 04:00 PM < 08:00 PM)
    - By Seat Fare (Ascending: Low to High)
    - By Seat Fare (Descending: High to Low)
    - By Available Seats (Descending: Most to Least)
13. **Search Bookings**: Filter customer reservations by Booking ID, passenger name substring, or train number.
14. **Print Ticket Receipt**: Renders an ASCII e-ticket on the console and saves an immutable ticket voucher to `tickets/<bookingId>.txt` (creates `tickets/` directory safely across platforms).
15. **Reports & Statistics**: Executive operational dashboard calculating:
    - Fleet size and total train units
    - Active vs. cancelled booking tallies
    - Total passenger seats sold
    - Confirmed revenue, retained cancellation fee revenue, and net total revenue
    - Busiest route across all booking history
    - Critical train with the fewest remaining seats
16. **List Districts by Division**: Formatted directory of all **64 Bangladesh districts** organized under their respective 7-8 administrative divisions, backed by a startup assertion verifying exactly 64 unique district entries.

---

## Security: Admin PIN Authentication

Operations that modify the train inventory (**Option 1: Add Train**, **Option 10: Update Train**, **Option 11: Delete Train**) are guarded by administrative authentication:

* **Default Admin PIN**: `1234` (`#define ADMIN_PIN "1234"`)
* **Throttling**: Maximum 3 consecutive attempts allowed. Exceeding 3 failed attempts immediately terminates the operation and returns to the main menu without state modification.

---

## Bangladesh District & Time Slot Constraints

* **64 Bangladesh Districts**:
  - **Dhaka (17)**: Dhaka, Gazipur, Kishoreganj, Manikganj, Munshiganj, Narayanganj, Narsingdi, Tangail, Faridpur, Gopalganj, Madaripur, Rajbari, Shariatpur, Mymensingh, Jamalpur, Sherpur, Netrokona
  - **Chattogram (11)**: Chattogram, Cox's Bazar, Bandarban, Brahmanbaria, Chandpur, Cumilla, Feni, Khagrachhari, Lakshmipur, Noakhali, Rangamati
  - **Rajshahi (8)**: Rajshahi, Bogura, Joypurhat, Naogaon, Natore, Chapainawabganj, Pabna, Sirajganj
  - **Khulna (10)**: Khulna, Bagerhat, Chuadanga, Jashore, Jhenaidah, Kushtia, Magura, Meherpur, Narail, Satkhira
  - **Barishal (6)**: Barishal, Barguna, Bhola, Jhalokati, Patuakhali, Pirojpur
  - **Sylhet (4)**: Sylhet, Habiganj, Moulvibazar, Sunamganj
  - **Rangpur (8)**: Rangpur, Dinajpur, Gaibandha, Kurigram, Lalmonirhat, Nilphamari, Panchagarh, Thakurgaon
* **Time Slots (Exactly 4)**:
  - `1. 08:00 AM`
  - `2. 12:00 PM`
  - `3. 04:00 PM`
  - `4. 08:00 PM`
  *(Users can enter slot numbers 1–4 or type the slot text case-insensitively).*

---

## File Persistence Formats

### 1. `trains.txt`
Comma-separated CSV with 7 fields:
```csv
trainNo,trainName,sourceDistrict,destinationDistrict,timeSlot,seatsAvailable,pricePerSeat
101,Ekota Express,Dhaka,Rangpur,08:00 AM,120,50.00
102,Suborno Express,Chattogram,Dhaka,12:00 PM,200,80.00
103,Silk City Express,Rajshahi,Dhaka,04:00 PM,150,65.00
104,Sundarban Express,Khulna,Dhaka,08:00 PM,140,75.00
105,Parabat Express,Sylhet,Dhaka,08:00 AM,160,70.00
106,Benapole Express,Jashore,Dhaka,12:00 PM,180,60.00
107,Kurigram Express,Kurigram,Dhaka,08:00 PM,130,55.00
```

### 2. `bookings.txt`
Comma-separated CSV storing persistent next ID counters and reservation records:
```csv
#NEXT_BOOKING_ID,1002
BK1001,Rahat Hasan,101,2,15-11-2026,08:00 AM,100.00,CONFIRMED
```

### 3. Generated Tickets (`tickets/<bookingId>.txt`)
```text
===============================================================
                  BANGLADESH RAILWAY E-TICKET                  
===============================================================
 Booking ID        : BK1001
 Booking Status    : CONFIRMED
---------------------------------------------------------------
 PASSENGER INFORMATION
 Passenger Name    : Rahat Hasan
 Seats Booked      : 2
---------------------------------------------------------------
 JOURNEY DETAILS
 Train Number      : 101 (Ekota Express)
 Route             : Dhaka -> Rangpur
 Travel Date       : 15-11-2026
 Departure Slot    : 08:00 AM
---------------------------------------------------------------
 FARE DETAILS
 Fare Per Seat     : 50.00 BDT
 Total Fare Paid   : 100.00 BDT
===============================================================
       Thank you for traveling with Bangladesh Railway!        
===============================================================
```

---

## Project Structure Tree

```
Railway-Management-System/
â”œâ”€â”€ Makefile                     # Multi-target build and test automation
â”œâ”€â”€ README.md                    # System architecture and technical documentation
â”œâ”€â”€ .gitignore                   # Executable, backup, and build artifacts filter
â”œâ”€â”€ trains.txt                   # Default operational fleet dataset (7 trains)
â”œâ”€â”€ bookings.txt                 # Persistent reservation database (generated)
â”œâ”€â”€ tickets/                     # Electronic ticket receipts repository (generated)
â”‚   â””â”€â”€ BK1001.txt
â”œâ”€â”€ src/
â”‚   â”œâ”€â”€ main.c                   # CLI orchestration, menu loop, admin verification
â”‚   â”œâ”€â”€ train.h                  # Train model declarations, search & sort prototypes
â”‚   â”œâ”€â”€ train.c                  # Fleet management, stable sort, train queries
â”‚   â”œâ”€â”€ booking.h                # Booking struct, limits, discount & reports prototypes
â”‚   â”œâ”€â”€ booking.c                # Reservation engine, cancellation, receipts, reporting
â”‚   â”œâ”€â”€ data.h                   # District directory and time slot declarations
â”‚   â”œâ”€â”€ data.c                   # 64 BD districts data table, validation & assertions
â”‚   â”œâ”€â”€ fileio.h                 # File persistence declarations and backup constants
â”‚   â”œâ”€â”€ fileio.c                 # CSV stream serialization, parsing, atomic backup
â”‚   â”œâ”€â”€ utils.h                  # Safe input routines, date checks & string helpers
â”‚   â””â”€â”€ utils.c                  # Portable safe I/O, case-insensitive logic, leap years
â””â”€â”€ tests/
    â”œâ”€â”€ run_tests.bat            # Automated Windows test suite runner
    â”œâ”€â”€ run_tests.sh             # Automated Linux/macOS bash test runner
    â”œâ”€â”€ test1_admin_and_train_ops.txt # Test 1: PIN auth, validation & train addition
    â”œâ”€â”€ test2_search_and_sort.txt     # Test 2: Multi-criteria search & stable sort
    â”œâ”€â”€ test3_booking_and_limits.txt  # Test 3: Booking bounds, date validation, group discount
    â”œâ”€â”€ test4_cancel_receipt_report.txt # Test 4: Cancellation, fee calculation, receipt & reports
    â”œâ”€â”€ test5_fileio_roundtrip.txt    # Test 5: Save/load roundtrip, backups, duplicate prevention
    â”œâ”€â”€ test6_robust_input_and_eof.txt # Test 6: Input fuzzing, over-long strings & clean EOF
    â””â”€â”€ malformed_trains.txt     # Syntax-error regression dataset
```

---

## How to Build

### Option 1: Direct Plain GCC Command (Universal)
```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc src/main.c src/train.c src/booking.c src/data.c src/fileio.c src/utils.c -o railway_system
```

On Windows (MinGW / MSYS2):
```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc src/main.c src/train.c src/booking.c src/data.c src/fileio.c src/utils.c -o railway_system.exe
```

### Option 2: Using Make
```bash
make
```
To clean build artifacts:
```bash
make clean
```

---

## How to Run

### Windows:
```powershell
.\railway_system.exe
```

### Linux / macOS:
```bash
./railway_system
```

---

## Running the Automated Test Suite

The test suite validates input sanitization, error boundaries, date math, group discounts, cancellation fees, atomic backups, and EOF protection:

### On Windows:
```powershell
.\tests\run_tests.bat
```

### On Linux / macOS:
```bash
bash tests/run_tests.sh
```
*(or run `make test`)*
