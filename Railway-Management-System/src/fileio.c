#include "fileio.h"
#include <stdio.h>

bool backup_file(const char *src_file, const char *bak_file) {
    (void)src_file; (void)bak_file;
    return false;
}

bool load_trains_file(const char *filename, Train *trains, int *trainCount, int maxTrains, int *out_loaded, int *out_skipped) {
    (void)filename; (void)trains; (void)trainCount; (void)maxTrains;
    if (out_loaded) *out_loaded = 0;
    if (out_skipped) *out_skipped = 0;
    return false;
}

bool save_trains_file(const char *filename, const Train *trains, int trainCount) {
    (void)filename; (void)trains; (void)trainCount;
    return false;
}

bool load_bookings_file(const char *filename, Booking *bookings, int *bookingCount, int maxBookings, int *nextBookingId, int *out_loaded, int *out_skipped) {
    (void)filename; (void)bookings; (void)bookingCount; (void)maxBookings; (void)nextBookingId;
    if (out_loaded) *out_loaded = 0;
    if (out_skipped) *out_skipped = 0;
    return false;
}

bool save_bookings_file(const char *filename, const Booking *bookings, int bookingCount, int nextBookingId) {
    (void)filename; (void)bookings; (void)bookingCount; (void)nextBookingId;
    return false;
}
