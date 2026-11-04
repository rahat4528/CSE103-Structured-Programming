#ifndef FILEIO_H
#define FILEIO_H

#include "train.h"
#include "booking.h"
#include <stdbool.h>

#define TRAINS_DATA_FILE "trains.txt"
#define TRAINS_BAK_FILE "trains.bak"
#define BOOKINGS_DATA_FILE "bookings.txt"
#define BOOKINGS_BAK_FILE "bookings.bak"

bool backup_file(const char *src_file, const char *bak_file);
bool load_trains_file(const char *filename, Train *trains, int *trainCount, int maxTrains, int *out_loaded, int *out_skipped);
bool save_trains_file(const char *filename, const Train *trains, int trainCount);
bool load_bookings_file(const char *filename, Booking *bookings, int *bookingCount, int maxBookings, int *nextBookingId, int *out_loaded, int *out_skipped);
bool save_bookings_file(const char *filename, const Booking *bookings, int bookingCount, int nextBookingId);

#endif /* FILEIO_H */

