#include "booking.h"
#include "data.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

void printBookingTableHeader(void) {
    printf("+----------+-------------------------+----------+-------+------------+------------+------------+------------+\n");
    printf("| Book ID  | Passenger Name          | Train No | Seats | Date       | Time Slot  | Total(BDT) | Status     |\n");
    printf("+----------+-------------------------+----------+-------+------------+------------+------------+------------+\n");
}

void printBookingTableFooter(void) {
    printf("+----------+-------------------------+----------+-------+------------+------------+------------+------------+\n");
}

void showBooking(const Booking *b) {
    if (!b) return;
    printf("| %-8s | %-23s | %-8d | %-5d | %-10s | %-10s | %10.2f | %-10s |\n",
           b->bookingId, b->passengerName, b->trainNo, b->seats,
           b->travelDate, b->timeSlot, b->totalPrice, b->status);
}

void viewAllBookings(const Booking *bookings, int bookingCount) {
    if (bookingCount <= 0 || !bookings) {
        printf("\nNo bookings made.\n\n");
        return;
    }

    printf("\n--- All Customer Bookings (%d total) ---\n", bookingCount);
    printBookingTableHeader();
    for (int i = 0; i < bookingCount; i++) {
        showBooking(&bookings[i]);
    }
    printBookingTableFooter();
    printf("\n");
}

int countActiveBookingsForTrain(const Booking *bookings, int bookingCount, int trainNo) {
    if (!bookings || bookingCount <= 0) return 0;
    int active = 0;
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].trainNo == trainNo && strcmp(bookings[i].status, "CONFIRMED") == 0) {
            active++;
        }
    }
    return active;
}

/* Stubs for Stage 3 & Stage 5 */
bool bookTicket(Booking *bookings, int *bookingCount, int *nextBookingId,
                Train *trains, int trainCount,
                const char *passengerName, int trainNo, int seats,
                const char *travelDate, const char *timeSlot,
                Booking *out_booking) {
    if (!bookings || !bookingCount || !nextBookingId || !trains || !passengerName || !travelDate || !timeSlot) {
        return false;
    }

    if (*bookingCount >= MAX_BOOKINGS) {
        printf("Error: Maximum booking capacity reached (%d bookings).\n", MAX_BOOKINGS);
        return false;
    }

    /* 1. Find train */
    int trainIdx = findTrainIndexByNumber(trains, trainCount, trainNo);
    if (trainIdx < 0) {
        printf("Error: Train number %d not found.\n", trainNo);
        return false;
    }

    /* 2. Validate seats count (1 to 10) */
    if (seats < 1 || seats > MAX_SEATS_PER_BOOKING) {
        printf("Error: Invalid seat quantity %d (allowed: 1 to %d seats per booking).\n",
               seats, MAX_SEATS_PER_BOOKING);
        return false;
    }

    /* 3. Check seats availability (never reduce seats on failure) */
    if (trains[trainIdx].seatsAvailable < seats) {
        printf("Error: Not enough seats available on Train %d. Requested: %d, Available: %d.\n",
               trainNo, seats, trains[trainIdx].seatsAvailable);
        return false;
    }

    /* 4. Validate time slot matches the train */
    char canonicalSlot[TIME_SLOT_LEN];
    if (!parse_time_slot(timeSlot, canonicalSlot, sizeof(canonicalSlot))) {
        printf("Error: Invalid time slot format: \"%s\".\n", timeSlot);
        return false;
    }

    if (str_case_cmp(trains[trainIdx].timeSlot, canonicalSlot) != 0) {
        printf("Error: Time slot mismatch with the train. Train runs at \"%s\", but \"%s\" was requested.\n",
               trains[trainIdx].timeSlot, canonicalSlot);
        return false;
    }

    /* 5. Validate travel date */
    char canonicalDate[DATE_LEN];
    if (!parse_and_validate_date(travelDate, canonicalDate, sizeof(canonicalDate))) {
        printf("Error: Invalid travel date \"%s\". Date must be valid DD-MM-YYYY and not in the past.\n", travelDate);
        return false;
    }

    /* 6. Pricing & Group Discount calculation */
    double basePrice = seats * trains[trainIdx].pricePerSeat;
    double discount = 0.0;
    if (seats >= 5) {
        discount = basePrice * ((double)GROUP_DISCOUNT_PERCENT / 100.0);
    }
    double totalPrice = basePrice - discount;

    /* 7. Deduct seats from train only upon confirmed validation */
    trains[trainIdx].seatsAvailable -= seats;

    /* 8. Construct booking record */
    Booking *b = &bookings[*bookingCount];
    memset(b, 0, sizeof(Booking));
    snprintf(b->bookingId, sizeof(b->bookingId), "BK%04d", *nextBookingId);
    (*nextBookingId)++;

    snprintf(b->passengerName, sizeof(b->passengerName), "%s", passengerName);
    b->trainNo = trainNo;
    b->seats = seats;
    snprintf(b->travelDate, sizeof(b->travelDate), "%s", canonicalDate);
    snprintf(b->timeSlot, sizeof(b->timeSlot), "%s", trains[trainIdx].timeSlot);
    b->totalPrice = totalPrice;
    snprintf(b->status, sizeof(b->status), "%s", "CONFIRMED");

    (*bookingCount)++;

    if (out_booking) {
        *out_booking = *b;
    }

    return true;
}

bool cancelBookingById(Booking *bookings, int bookingCount, Train *trains, int trainCount,
                       const char *bookingId, double *out_refund, double *out_fee) {
    if (!bookings || bookingCount <= 0 || !bookingId) return false;

    for (int i = 0; i < bookingCount; i++) {
        if (str_case_cmp(bookings[i].bookingId, bookingId) == 0) {
            if (strcmp(bookings[i].status, "CANCELLED") == 0) {
                printf("Notice: Booking %s is already marked CANCELLED.\n", bookings[i].bookingId);
                return false;
            }

            /* 1. Restore train seats */
            int tIdx = findTrainIndexByNumber(trains, trainCount, bookings[i].trainNo);
            if (tIdx >= 0) {
                trains[tIdx].seatsAvailable += bookings[i].seats;
            }

            /* 2. Update status */
            snprintf(bookings[i].status, sizeof(bookings[i].status), "%s", "CANCELLED");

            /* 3. Compute refund with 10% cancellation fee */
            double fee = bookings[i].totalPrice * ((double)CANCEL_FEE_PERCENT / 100.0);
            double refund = bookings[i].totalPrice - fee;

            if (out_fee) *out_fee = fee;
            if (out_refund) *out_refund = refund;

            return true;
        }
    }

    printf("Error: Booking ID \"%s\" not found.\n", bookingId);
    return false;
}

void searchBookings(const Booking *bookings, int bookingCount, int criteria,
                    const char *queryStr, int queryInt) {
    if (!bookings || bookingCount <= 0) {
        printf("\nNo matching bookings found.\n\n");
        return;
    }

    int matches = 0;
    for (int i = 0; i < bookingCount; i++) {
        bool match = false;
        switch (criteria) {
            case 1: /* By Booking ID */
                if (queryStr && str_case_contains(bookings[i].bookingId, queryStr)) {
                    match = true;
                }
                break;
            case 2: /* By Passenger Name */
                if (queryStr && str_case_contains(bookings[i].passengerName, queryStr)) {
                    match = true;
                }
                break;
            case 3: /* By Train Number */
                if (bookings[i].trainNo == queryInt) {
                    match = true;
                }
                break;
            default:
                break;
        }

        if (match) {
            if (matches == 0) {
                printf("\n--- Search Results for Bookings ---\n");
                printBookingTableHeader();
            }
            showBooking(&bookings[i]);
            matches++;
        }
    }

    if (matches == 0) {
        printf("\nNo matching bookings found.\n\n");
    } else {
        printBookingTableFooter();
        printf("Found %d matching booking(s).\n\n", matches);
    }
}

bool printTicketReceipt(const Booking *b, const Train *t, bool writeToFile) {
    if (!b) return false;

    const char *trainName = t ? t->trainName : "Unknown Train";
    const char *src = t ? t->sourceDistrict : "N/A";
    const char *dst = t ? t->destinationDistrict : "N/A";
    double pricePerSeat = t ? t->pricePerSeat : 0.0;

    /* Display receipt on screen */
    printf("\n===============================================================\n");
    printf("                  BANGLADESH RAILWAY E-TICKET                  \n");
    printf("===============================================================\n");
    printf(" Booking ID        : %s\n", b->bookingId);
    printf(" Booking Status    : %s\n", b->status);
    printf("---------------------------------------------------------------\n");
    printf(" PASSENGER INFORMATION\n");
    printf(" Passenger Name    : %s\n", b->passengerName);
    printf(" Seats Booked      : %d\n", b->seats);
    printf("---------------------------------------------------------------\n");
    printf(" JOURNEY DETAILS\n");
    printf(" Train Number      : %d (%s)\n", b->trainNo, trainName);
    printf(" Route             : %s -> %s\n", src, dst);
    printf(" Travel Date       : %s\n", b->travelDate);
    printf(" Departure Slot    : %s\n", b->timeSlot);
    printf("---------------------------------------------------------------\n");
    printf(" FARE DETAILS\n");
    if (pricePerSeat > 0.0) {
        printf(" Fare Per Seat     : %.2f BDT\n", pricePerSeat);
    }
    printf(" Total Fare Paid   : %.2f BDT\n", b->totalPrice);
    printf("===============================================================\n");
    printf("       Thank you for traveling with Bangladesh Railway!        \n");
    printf("===============================================================\n\n");

    if (writeToFile) {
        portable_mkdir("tickets");
        char filePath[128];
        snprintf(filePath, sizeof(filePath), "tickets/%s.txt", b->bookingId);

        FILE *fp = fopen(filePath, "w");
        if (!fp) {
            printf("Notice: Could not write ticket file to \"%s\".\n", filePath);
            return false;
        }

        fprintf(fp, "===============================================================\n");
        fprintf(fp, "                  BANGLADESH RAILWAY E-TICKET                  \n");
        fprintf(fp, "===============================================================\n");
        fprintf(fp, " Booking ID        : %s\n", b->bookingId);
        fprintf(fp, " Booking Status    : %s\n", b->status);
        fprintf(fp, "---------------------------------------------------------------\n");
        fprintf(fp, " PASSENGER INFORMATION\n");
        fprintf(fp, " Passenger Name    : %s\n", b->passengerName);
        fprintf(fp, " Seats Booked      : %d\n", b->seats);
        fprintf(fp, "---------------------------------------------------------------\n");
        fprintf(fp, " JOURNEY DETAILS\n");
        fprintf(fp, " Train Number      : %d (%s)\n", b->trainNo, trainName);
        fprintf(fp, " Route             : %s -> %s\n", src, dst);
        fprintf(fp, " Travel Date       : %s\n", b->travelDate);
        fprintf(fp, " Departure Slot    : %s\n", b->timeSlot);
        fprintf(fp, "---------------------------------------------------------------\n");
        fprintf(fp, " FARE DETAILS\n");
        if (pricePerSeat > 0.0) {
            fprintf(fp, " Fare Per Seat     : %.2f BDT\n", pricePerSeat);
        }
        fprintf(fp, " Total Fare Paid   : %.2f BDT\n", b->totalPrice);
        fprintf(fp, "===============================================================\n");
        fprintf(fp, "       Thank you for traveling with Bangladesh Railway!        \n");
        fprintf(fp, "===============================================================\n");

        fclose(fp);
        printf("Ticket receipt saved to \"%s\".\n\n", filePath);
    }

    return true;
}

void generateReportsAndStatistics(const Train *trains, int trainCount,
                                  const Booking *bookings, int bookingCount) {
    printf("\n===============================================================\n");
    printf("               RAILWAY SYSTEM REPORTS & STATISTICS             \n");
    printf("===============================================================\n");

    int activeBookings = 0;
    int cancelledBookings = 0;
    int totalSeatsSold = 0;
    double confirmedRevenue = 0.0;
    double cancellationFeesRetained = 0.0;

    for (int i = 0; i < bookingCount; i++) {
        if (strcmp(bookings[i].status, "CONFIRMED") == 0) {
            activeBookings++;
            totalSeatsSold += bookings[i].seats;
            confirmedRevenue += bookings[i].totalPrice;
        } else if (strcmp(bookings[i].status, "CANCELLED") == 0) {
            cancelledBookings++;
            cancellationFeesRetained += (bookings[i].totalPrice * ((double)CANCEL_FEE_PERCENT / 100.0));
        }
    }

    double netRevenue = confirmedRevenue + cancellationFeesRetained;

    printf(" Fleet Overview:\n");
    printf("   Total Trains In Service       : %d\n", trainCount);
    printf(" Booking Activity:\n");
    printf("   Total Active Bookings         : %d\n", activeBookings);
    printf("   Total Cancelled Bookings      : %d\n", cancelledBookings);
    printf("   Total Tickets Issued          : %d\n", bookingCount);
    printf("   Total Passenger Seats Sold    : %d\n", totalSeatsSold);
    printf(" Financial Summary:\n");
    printf("   Confirmed Ticket Revenue      : %.2f BDT\n", confirmedRevenue);
    printf("   Retained Cancellation Fees    : %.2f BDT (%d%% rate)\n",
           cancellationFeesRetained, CANCEL_FEE_PERCENT);
    printf("   Net Total Revenue             : %.2f BDT\n", netRevenue);
    printf("---------------------------------------------------------------\n");

    /* Busiest route identification */
    char busiestRoute[128] = "N/A";
    int maxRouteSeats = 0;

    for (int t = 0; t < trainCount; t++) {
        int routeSeats = 0;
        for (int b = 0; b < bookingCount; b++) {
            if (bookings[b].trainNo == trains[t].trainNo &&
                strcmp(bookings[b].status, "CONFIRMED") == 0) {
                routeSeats += bookings[b].seats;
            }
        }
        if (routeSeats > maxRouteSeats) {
            maxRouteSeats = routeSeats;
            snprintf(busiestRoute, sizeof(busiestRoute), "%s -> %s (%d seats booked)",
                     trains[t].sourceDistrict, trains[t].destinationDistrict, maxRouteSeats);
        }
    }

    printf(" Key Operational Metrics:\n");
    printf("   Busiest Route                 : %s\n", busiestRoute);

    /* Train with fewest seats remaining */
    if (trainCount > 0 && trains) {
        int minSeats = trains[0].seatsAvailable;
        int minIdx = 0;
        for (int i = 1; i < trainCount; i++) {
            if (trains[i].seatsAvailable < minSeats) {
                minSeats = trains[i].seatsAvailable;
                minIdx = i;
            }
        }
        printf("   Train with Fewest Seats Left  : Train %d (%s) with %d seat(s) remaining\n",
               trains[minIdx].trainNo, trains[minIdx].trainName, trains[minIdx].seatsAvailable);
    } else {
        printf("   Train with Fewest Seats Left  : N/A\n");
    }

    printf("===============================================================\n\n");
}

