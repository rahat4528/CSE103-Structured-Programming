#ifndef BOOKING_H
#define BOOKING_H

#include "train.h"
#include <stdbool.h>

#define MAX_BOOKINGS 1000
#define BOOKING_ID_LEN 16
#define PASSENGER_NAME_LEN 64
#define DATE_LEN 16
#define STATUS_LEN 16

#define CANCEL_FEE_PERCENT 10
#define GROUP_DISCOUNT_PERCENT 5
#define MAX_SEATS_PER_BOOKING 10
#define ADMIN_PIN "1234"

typedef struct {
    char bookingId[BOOKING_ID_LEN];
    char passengerName[PASSENGER_NAME_LEN];
    int trainNo;
    int seats;
    char travelDate[DATE_LEN];
    char timeSlot[TIME_SLOT_LEN];
    double totalPrice;
    char status[STATUS_LEN]; /* "CONFIRMED" or "CANCELLED" */
} Booking;

/* Display functions */
void showBooking(const Booking *b);
void printBookingTableHeader(void);
void printBookingTableFooter(void);
void viewAllBookings(const Booking *bookings, int bookingCount);

/* Booking operations (Stage 3) */
bool bookTicket(Booking *bookings, int *bookingCount, int *nextBookingId,
                Train *trains, int trainCount,
                const char *passengerName, int trainNo, int seats,
                const char *travelDate, const char *timeSlot,
                Booking *out_booking);

int countActiveBookingsForTrain(const Booking *bookings, int bookingCount, int trainNo);

/* Cancellation, Search, Receipts & Reports (Stage 5) */
bool cancelBookingById(Booking *bookings, int bookingCount, Train *trains, int trainCount,
                       const char *bookingId, double *out_refund, double *out_fee);

void searchBookings(const Booking *bookings, int bookingCount, int criteria,
                    const char *queryStr, int queryInt);

bool printTicketReceipt(const Booking *b, const Train *t, bool writeToFile);

void generateReportsAndStatistics(const Train *trains, int trainCount,
                                  const Booking *bookings, int bookingCount);

#endif /* BOOKING_H */

