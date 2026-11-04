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
    (void)bookings; (void)bookingCount; (void)trains; (void)trainCount;
    (void)bookingId; (void)out_refund; (void)out_fee;
    return false;
}

void searchBookings(const Booking *bookings, int bookingCount, int criteria,
                    const char *queryStr, int queryInt) {
    (void)bookings; (void)bookingCount; (void)criteria;
    (void)queryStr; (void)queryInt;
}

bool printTicketReceipt(const Booking *b, const Train *t, bool writeToFile) {
    (void)b; (void)t; (void)writeToFile;
    return false;
}

void generateReportsAndStatistics(const Train *trains, int trainCount,
                                  const Booking *bookings, int bookingCount) {
    (void)trains; (void)trainCount; (void)bookings; (void)bookingCount;
}

