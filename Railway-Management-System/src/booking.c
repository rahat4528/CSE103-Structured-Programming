#include "booking.h"
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
    (void)bookings; (void)bookingCount; (void)nextBookingId;
    (void)trains; (void)trainCount; (void)passengerName;
    (void)trainNo; (void)seats; (void)travelDate;
    (void)timeSlot; (void)out_booking;
    return false;
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
