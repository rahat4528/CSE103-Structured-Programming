#include "train.h"
#include "booking.h"
#include "data.h"
#include "fileio.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void handleAddTrain(Train *trains, int *trainCount) {
    if (*trainCount >= MAX_TRAINS) {
        printf("\nError: Train capacity reached (maximum %d trains).\n\n", MAX_TRAINS);
        return;
    }

    printf("\n--- Add New Train ---\n");
    Train newTrain;
    memset(&newTrain, 0, sizeof(Train));

    /* 1. Train Number */
    while (1) {
        printf("Enter Train Number (positive integer): ");
        int no = 0;
        if (!safe_read_int(&no, 1, 999999)) {
            printf("Invalid input. Please enter a positive integer greater than 0.\n");
            continue;
        }
        if (findTrainIndexByNumber(trains, *trainCount, no) >= 0) {
            printf("Error: Train number %d already exists. Enter a unique number.\n", no);
            continue;
        }
        newTrain.trainNo = no;
        break;
    }

    /* 2. Train Name */
    if (!safe_read_name(newTrain.trainName, sizeof(newTrain.trainName), "Train Name")) {
        printf("\nAdd Train cancelled.\n");
        return;
    }

    /* 3. Source District */
    while (1) {
        char buf[64];
        printf("Enter Source District (one of 64 BD districts): ");
        if (!safe_read_line(buf, sizeof(buf), stdin)) {
            printf("\nAdd Train cancelled.\n");
            return;
        }
        if (!is_valid_district(buf, newTrain.sourceDistrict, sizeof(newTrain.sourceDistrict))) {
            printf("Invalid district: \"%s\". Please enter a valid Bangladesh district (Option 16 lists all).\n", buf);
            continue;
        }
        break;
    }

    /* 4. Destination District */
    while (1) {
        char buf[64];
        printf("Enter Destination District: ");
        if (!safe_read_line(buf, sizeof(buf), stdin)) {
            printf("\nAdd Train cancelled.\n");
            return;
        }
        if (!is_valid_district(buf, newTrain.destinationDistrict, sizeof(newTrain.destinationDistrict))) {
            printf("Invalid district: \"%s\". Please enter a valid Bangladesh district.\n", buf);
            continue;
        }
        if (str_case_cmp(newTrain.sourceDistrict, newTrain.destinationDistrict) == 0) {
            printf("Error: Source and Destination districts cannot be the same (%s).\n", newTrain.sourceDistrict);
            continue;
        }
        break;
    }

    /* 5. Time Slot */
    while (1) {
        char buf[64];
        printf("Select Time Slot:\n");
        printf("  1. 08:00 AM\n  2. 12:00 PM\n  3. 04:00 PM\n  4. 08:00 PM\n");
        printf("Enter Choice (1-4 or slot name): ");
        if (!safe_read_line(buf, sizeof(buf), stdin)) {
            printf("\nAdd Train cancelled.\n");
            return;
        }
        if (!parse_time_slot(buf, newTrain.timeSlot, sizeof(newTrain.timeSlot))) {
            printf("Invalid time slot: \"%s\". Please enter 1-4 or an exact slot like \"08:00 AM\".\n", buf);
            continue;
        }
        break;
    }

    /* 6. Seats Available */
    while (1) {
        printf("Enter Available Seats (0 - 2000): ");
        int seats = 0;
        if (!safe_read_int(&seats, 0, 2000)) {
            printf("Invalid input. Seats must be between 0 and 2000.\n");
            continue;
        }
        newTrain.seatsAvailable = seats;
        break;
    }

    /* 7. Price Per Seat */
    while (1) {
        printf("Enter Price Per Seat in BDT (> 0.0): ");
        double price = 0.0;
        if (!safe_read_double(&price, 0.01, 50000.0)) {
            printf("Invalid input. Price must be a positive number greater than 0.\n");
            continue;
        }
        newTrain.pricePerSeat = price;
        break;
    }

    if (addTrain(trains, trainCount, &newTrain)) {
        printf("\nSuccess: Train %d (%s) added successfully!\n\n", newTrain.trainNo, newTrain.trainName);
    }
}

static void handleSearchTrain(const Train *trains, int trainCount) {
    if (trainCount <= 0) {
        printf("\nNo trains available to search.\n\n");
        return;
    }

    while (1) {
        printf("\n--- Search Train Menu ---\n");
        printf(" 1. Search by Train Number\n");
        printf(" 2. Search by Name Keyword\n");
        printf(" 3. Search by District (Source or Destination)\n");
        printf(" 4. Search by Time Slot\n");
        printf(" 5. Search by Route (Source -> Destination)\n");
        printf(" 6. Return to Main Menu\n");
        printf("Enter search choice (1-6): ");

        int subChoice = 0;
        if (!safe_read_int(&subChoice, 1, 6)) {
            printf("Invalid choice. Please enter a number between 1 and 6.\n");
            if (feof(stdin)) break;
            continue;
        }

        if (subChoice == 6) {
            break;
        }

        switch (subChoice) {
            case 1: {
                printf("Enter Train Number to search: ");
                int no = 0;
                if (safe_read_int(&no, 1, 999999)) {
                    searchTrainByNumber(trains, trainCount, no);
                } else {
                    printf("Invalid train number format.\n");
                }
                break;
            }
            case 2: {
                char kw[64];
                printf("Enter train name keyword: ");
                if (safe_read_line(kw, sizeof(kw), stdin) && kw[0] != '\0') {
                    searchTrainByName(trains, trainCount, kw);
                } else {
                    printf("Invalid or empty keyword.\n");
                }
                break;
            }
            case 3: {
                char dist[64];
                printf("Enter district name: ");
                if (safe_read_line(dist, sizeof(dist), stdin) && dist[0] != '\0') {
                    char canon[32];
                    if (is_valid_district(dist, canon, sizeof(canon))) {
                        searchTrainByDistrict(trains, trainCount, canon);
                    } else {
                        searchTrainByDistrict(trains, trainCount, dist);
                    }
                } else {
                    printf("Invalid district input.\n");
                }
                break;
            }
            case 4: {
                char slotBuf[64];
                printf("Select Time Slot to search:\n");
                printf("  1. 08:00 AM\n  2. 12:00 PM\n  3. 04:00 PM\n  4. 08:00 PM\n");
                printf("Enter choice (1-4 or slot string): ");
                if (safe_read_line(slotBuf, sizeof(slotBuf), stdin) && slotBuf[0] != '\0') {
                    char canonSlot[16];
                    if (parse_time_slot(slotBuf, canonSlot, sizeof(canonSlot))) {
                        searchTrainByTimeSlot(trains, trainCount, canonSlot);
                    } else {
                        printf("Invalid time slot entered.\n");
                    }
                }
                break;
            }
            case 5: {
                char srcBuf[64], dstBuf[64];
                char canonSrc[32], canonDst[32];
                printf("Enter Source District: ");
                if (!safe_read_line(srcBuf, sizeof(srcBuf), stdin) || srcBuf[0] == '\0') {
                    printf("Invalid source district.\n");
                    break;
                }
                printf("Enter Destination District: ");
                if (!safe_read_line(dstBuf, sizeof(dstBuf), stdin) || dstBuf[0] == '\0') {
                    printf("Invalid destination district.\n");
                    break;
                }

                const char *srcQuery = is_valid_district(srcBuf, canonSrc, sizeof(canonSrc)) ? canonSrc : srcBuf;
                const char *dstQuery = is_valid_district(dstBuf, canonDst, sizeof(canonDst)) ? canonDst : dstBuf;
                searchTrainByRoute(trains, trainCount, srcQuery, dstQuery);
                break;
            }
            default:
                break;
        }
    }
}

static void handleBookTicket(Booking *bookings, int *bookingCount, int *nextBookingId,
                             Train *trains, int trainCount) {
    if (trainCount <= 0) {
        printf("\nNo trains available to book. Please add or load trains first.\n\n");
        return;
    }

    if (*bookingCount >= MAX_BOOKINGS) {
        printf("\nError: Maximum booking limit reached (%d bookings).\n\n", MAX_BOOKINGS);
        return;
    }

    printf("\n--- Book Train Ticket ---\n");

    /* 1. Passenger Name */
    char passengerName[PASSENGER_NAME_LEN];
    if (!safe_read_name(passengerName, sizeof(passengerName), "Passenger Name")) {
        printf("\nBooking cancelled.\n");
        return;
    }

    /* 2. Train Number */
    int trainNo = 0;
    const Train *selectedTrain = NULL;
    while (1) {
        printf("Enter Train Number: ");
        if (!safe_read_int(&trainNo, 1, 999999)) {
            printf("Invalid train number format. Enter a positive number.\n");
            continue;
        }

        selectedTrain = findTrainByNumber(trains, trainCount, trainNo);
        if (!selectedTrain) {
            printf("Error: Train number %d not found. Please check available trains.\n", trainNo);
            printf("Do you want to try another train number? (1=Yes, 2=Cancel): ");
            int retry = 0;
            if (!safe_read_int(&retry, 1, 2) || retry == 2) {
                printf("\nBooking cancelled.\n");
                return;
            }
            continue;
        }

        if (selectedTrain->seatsAvailable <= 0) {
            printf("Error: Train %d (%s) is fully booked (0 seats available).\n",
                   selectedTrain->trainNo, selectedTrain->trainName);
            return;
        }
        break;
    }

    printf("\nSelected Train: %d - %s\n", selectedTrain->trainNo, selectedTrain->trainName);
    printf("Route: %s -> %s | Time: %s | Seats Available: %d | Price/Seat: %.2f BDT\n\n",
           selectedTrain->sourceDistrict, selectedTrain->destinationDistrict,
           selectedTrain->timeSlot, selectedTrain->seatsAvailable, selectedTrain->pricePerSeat);

    /* 3. Number of seats (1 to 10) */
    int seats = 0;
    while (1) {
        printf("Enter Number of Seats (1 - %d): ", MAX_SEATS_PER_BOOKING);
        if (!safe_read_int(&seats, 1, MAX_SEATS_PER_BOOKING)) {
            printf("Invalid seat quantity. You can book between 1 and %d seats.\n", MAX_SEATS_PER_BOOKING);
            continue;
        }
        if (seats > selectedTrain->seatsAvailable) {
            printf("Error: Not enough seats available. Requested: %d, Available: %d.\n",
                   seats, selectedTrain->seatsAvailable);
            continue;
        }
        break;
    }

    /* 4. Travel Date (DD-MM-YYYY, real date, not in past) */
    char travelDate[DATE_LEN];
    while (1) {
        char dateBuf[64];
        printf("Enter Travel Date (DD-MM-YYYY, e.g. 15-11-2026): ");
        if (!safe_read_line(dateBuf, sizeof(dateBuf), stdin)) {
            printf("\nBooking cancelled.\n");
            return;
        }
        if (!parse_and_validate_date(dateBuf, travelDate, sizeof(travelDate))) {
            printf("Invalid date. Must be in DD-MM-YYYY format, valid calendar day, and not in the past.\n");
            continue;
        }
        break;
    }

    /* 5. Time Slot (must match train's time slot) */
    char slotChoice[64];
    while (1) {
        printf("Confirm Time Slot for this train (scheduled at %s):\n", selectedTrain->timeSlot);
        printf("  1. 08:00 AM\n  2. 12:00 PM\n  3. 04:00 PM\n  4. 08:00 PM\n");
        printf("Enter Choice (1-4 or slot string): ");
        if (!safe_read_line(slotChoice, sizeof(slotChoice), stdin)) {
            printf("\nBooking cancelled.\n");
            return;
        }

        char parsedSlot[TIME_SLOT_LEN];
        if (!parse_time_slot(slotChoice, parsedSlot, sizeof(parsedSlot))) {
            printf("Invalid time slot entered. Try again.\n");
            continue;
        }

        if (str_case_cmp(selectedTrain->timeSlot, parsedSlot) != 0) {
            printf("Error: Time slot mismatch with the train. Train runs at \"%s\", not \"%s\".\n",
                   selectedTrain->timeSlot, parsedSlot);
            continue;
        }
        break;
    }

    /* 6. Execute booking */
    Booking newBooking;
    if (bookTicket(bookings, bookingCount, nextBookingId,
                   trains, trainCount,
                   passengerName, trainNo, seats,
                   travelDate, selectedTrain->timeSlot,
                   &newBooking)) {
        double baseTotal = seats * selectedTrain->pricePerSeat;
        double discount = (seats >= 5) ? (baseTotal * GROUP_DISCOUNT_PERCENT / 100.0) : 0.0;

        printf("\n===============================================================\n");
        printf("                  BOOKING CONFIRMATION RECEIPT                 \n");
        printf("===============================================================\n");
        printf(" Booking ID       : %s\n", newBooking.bookingId);
        printf(" Status           : %s\n", newBooking.status);
        printf(" Passenger Name   : %s\n", newBooking.passengerName);
        printf(" Train Number     : %d (%s)\n", selectedTrain->trainNo, selectedTrain->trainName);
        printf(" Route            : %s -> %s\n", selectedTrain->sourceDistrict, selectedTrain->destinationDistrict);
        printf(" Travel Date      : %s\n", newBooking.travelDate);
        printf(" Time Slot        : %s\n", newBooking.timeSlot);
        printf(" Seats Booked     : %d\n", newBooking.seats);
        printf(" Base Fare        : %.2f BDT\n", baseTotal);
        if (seats >= 5) {
            printf(" Group Discount   : -%.2f BDT (%d%% applied for %d seats)\n",
                   discount, GROUP_DISCOUNT_PERCENT, seats);
        }
        printf(" Total Paid       : %.2f BDT\n", newBooking.totalPrice);
        printf(" Seats Remaining  : %d\n", selectedTrain->seatsAvailable);
        printf("===============================================================\n\n");
    }
}

static void printMainMenu(void) {
    printf("===============================================================\n");
    printf("                  RAILWAY MANAGEMENT SYSTEM                    \n");
    printf("===============================================================\n");
    printf(" 1. Add Train\n");
    printf(" 2. View All Trains\n");
    printf(" 3. Search Train\n");
    printf(" 4. Book Ticket\n");
    printf(" 5. View All Bookings\n");
    printf(" 6. Load Train Data from File\n");
    printf(" 7. Save Train Data to File\n");
    printf(" 8. Exit\n");
    printf("---------------------------------------------------------------\n");
    printf(" [More Features]\n");
    printf(" 9. Cancel Booking\n");
    printf(" 10. Update Train\n");
    printf(" 11. Delete Train\n");
    printf(" 12. Sort Trains\n");
    printf(" 13. Search Bookings\n");
    printf(" 14. Print Ticket Receipt\n");
    printf(" 15. Reports & Statistics\n");
    printf(" 16. List Districts by Division\n");
    printf("===============================================================\n");
    printf("Enter your choice (1-16): ");
}

int main(void) {
    /* Startup assertion/self-check that the district array has exactly 64 unique entries */
    init_and_verify_districts();

    Train trains[MAX_TRAINS];
    int trainCount = 0;

    Booking bookings[MAX_BOOKINGS];
    int bookingCount = 0;
    int nextBookingId = 1001;

    /* Auto-load trains.txt and bookings.txt at startup if they exist (silently ignore if missing) */
    FILE *testTrains = fopen(TRAINS_DATA_FILE, "r");
    if (testTrains) {
        fclose(testTrains);
        load_trains_file(TRAINS_DATA_FILE, trains, &trainCount, MAX_TRAINS, NULL, NULL);
    }

    FILE *testBookings = fopen(BOOKINGS_DATA_FILE, "r");
    if (testBookings) {
        fclose(testBookings);
        load_bookings_file(BOOKINGS_DATA_FILE, bookings, &bookingCount, MAX_BOOKINGS, &nextBookingId, NULL, NULL);
    }

    int choice = 0;
    while (1) {
        printMainMenu();
        if (!safe_read_int(&choice, 1, 16)) {
            printf("\nInvalid menu option. Please enter a number between 1 and 16.\n\n");
            if (feof(stdin)) {
                printf("\nEOF encountered. Exiting Railway Management System.\n");
                break;
            }
            continue;
        }

        switch (choice) {
            case 1:
                handleAddTrain(trains, &trainCount);
                break;
            case 2:
                viewAllTrains(trains, trainCount);
                break;
            case 3:
                handleSearchTrain(trains, trainCount);
                break;
            case 4:
                handleBookTicket(bookings, &bookingCount, &nextBookingId, trains, trainCount);
                break;
            case 5:
                viewAllBookings(bookings, bookingCount);
                break;
            case 6: {
                printf("\n--- Load Train & Booking Data ---\n");
                int loaded = 0, skipped = 0;
                load_trains_file(TRAINS_DATA_FILE, trains, &trainCount, MAX_TRAINS, &loaded, &skipped);
                int bLoaded = 0, bSkipped = 0;
                load_bookings_file(BOOKINGS_DATA_FILE, bookings, &bookingCount, MAX_BOOKINGS, &nextBookingId, &bLoaded, &bSkipped);
                if (bLoaded > 0 || bSkipped > 0) {
                    printf("Booking Data Load Summary: %d loaded, %d skipped.\n\n", bLoaded, bSkipped);
                }
                break;
            }
            case 7: {
                printf("\n--- Save Train & Booking Data ---\n");
                bool okT = save_trains_file(TRAINS_DATA_FILE, trains, trainCount);
                bool okB = save_bookings_file(BOOKINGS_DATA_FILE, bookings, bookingCount, nextBookingId);
                if (okT && okB) {
                    printf("Success: Saved %d trains and %d bookings to file (backups created).\n\n",
                           trainCount, bookingCount);
                } else {
                    printf("Warning: An error occurred while saving files.\n\n");
                }
                break;
            }
            case 8: {
                printf("\nDo you want to save train and booking data before exiting? (y/n): ");
                char ans[16];
                if (safe_read_line(ans, sizeof(ans), stdin) && (ans[0] == 'y' || ans[0] == 'Y')) {
                    save_trains_file(TRAINS_DATA_FILE, trains, trainCount);
                    save_bookings_file(BOOKINGS_DATA_FILE, bookings, bookingCount, nextBookingId);
                    printf("All data successfully saved to file.\n");
                }
                printf("\nExiting Railway Management System. Goodbye!\n");
                return 0;
            }
            case 9:
                printf("\n[Feature: Cancel Booking will be unlocked in Stage 5]\n\n");
                break;
            case 10:
                printf("\n[Feature: Update Train will be unlocked in Stage 5]\n\n");
                break;
            case 11:
                printf("\n[Feature: Delete Train will be unlocked in Stage 5]\n\n");
                break;
            case 12:
                printf("\n[Feature: Sort Trains will be unlocked in Stage 5]\n\n");
                break;
            case 13:
                printf("\n[Feature: Search Bookings will be unlocked in Stage 5]\n\n");
                break;
            case 14:
                printf("\n[Feature: Print Receipt will be unlocked in Stage 5]\n\n");
                break;
            case 15:
                printf("\n[Feature: Reports will be unlocked in Stage 5]\n\n");
                break;
            case 16:
                print_districts_by_division();
                break;
            default:
                printf("Invalid selection. Try again.\n");
                break;
        }
    }

    return 0;
}
