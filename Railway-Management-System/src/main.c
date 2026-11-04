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

    (void)bookings;
    (void)bookingCount;
    (void)nextBookingId;

    int choice = 0;
    while (1) {
        printMainMenu();
        if (!safe_read_int(&choice, 1, 16)) {
            printf("\nInvalid menu option. Please enter a number between 1 and 16.\n\n");
            /* If EOF detected on stdin, exit loop */
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
                printf("\n[Feature: Search Train will be unlocked in Stage 2]\n\n");
                break;
            case 4:
                printf("\n[Feature: Book Ticket will be unlocked in Stage 3]\n\n");
                break;
            case 5:
                viewAllBookings(bookings, bookingCount);
                break;
            case 6:
                printf("\n[Feature: Load Data will be unlocked in Stage 4]\n\n");
                break;
            case 7:
                printf("\n[Feature: Save Data will be unlocked in Stage 4]\n\n");
                break;
            case 8:
                printf("\nExiting Railway Management System. Goodbye!\n");
                return 0;
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
