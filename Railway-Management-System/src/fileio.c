#include "fileio.h"
#include "data.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int split_csv_line(char *line, char *fields[], int max_fields) {
    if (!line || !fields || max_fields <= 0) return 0;
    int count = 0;
    char *p = line;
    while (*p && count < max_fields) {
        fields[count++] = p;
        char *comma = strchr(p, ',');
        if (comma) {
            *comma = '\0';
            p = comma + 1;
        } else {
            break;
        }
    }
    for (int i = 0; i < count; i++) {
        trim_whitespace(fields[i]);
    }
    return count;
}

bool backup_file(const char *src_file, const char *bak_file) {
    if (!src_file || !bak_file) return false;

    FILE *src = fopen(src_file, "rb");
    if (!src) {
        return true; /* Source file does not exist yet; nothing to back up */
    }

    FILE *dst = fopen(bak_file, "wb");
    if (!dst) {
        fclose(src);
        return false;
    }

    char buffer[4096];
    size_t bytes = 0;
    bool success = true;

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            success = false;
            break;
        }
    }

    fclose(src);
    if (fclose(dst) != 0) {
        success = false;
    }

    return success;
}

bool load_trains_file(const char *filename, Train *trains, int *trainCount, int maxTrains,
                      int *out_loaded, int *out_skipped) {
    if (!filename || !trains || !trainCount) return false;

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Notice: File \"%s\" could not be opened (file does not exist or unreadable).\n", filename);
        if (out_loaded) *out_loaded = 0;
        if (out_skipped) *out_skipped = 0;
        return false;
    }

    char line[512];
    int lineNum = 0;
    int loaded = 0;
    int skipped = 0;

    while (fgets(line, sizeof(line), fp)) {
        lineNum++;
        trim_whitespace(line);

        if (line[0] == '\0' || line[0] == '#') {
            continue; /* Skip blank lines and comments */
        }

        if (*trainCount >= maxTrains) {
            printf("Line %d skipped: System train capacity reached (%d trains).\n", lineNum, maxTrains);
            skipped++;
            continue;
        }

        char *fields[10];
        int fCount = split_csv_line(line, fields, 10);
        if (fCount != 7) {
            printf("Line %d skipped: Expected 7 fields, found %d.\n", lineNum, fCount);
            skipped++;
            continue;
        }

        /* 1. Train Number */
        char *endptr = NULL;
        errno = 0;
        long tNo = strtol(fields[0], &endptr, 10);
        if (errno != 0 || endptr == fields[0] || *endptr != '\0' || tNo <= 0) {
            printf("Line %d skipped: Invalid train number \"%s\".\n", lineNum, fields[0]);
            skipped++;
            continue;
        }

        /* Check duplicates */
        if (findTrainIndexByNumber(trains, *trainCount, (int)tNo) >= 0) {
            printf("Line %d skipped: Train number %ld is already loaded.\n", lineNum, tNo);
            skipped++;
            continue;
        }

        /* 2. Train Name */
        if (strlen(fields[1]) == 0 || strlen(fields[1]) >= TRAIN_NAME_LEN || contains_comma(fields[1])) {
            printf("Line %d skipped: Invalid train name \"%s\".\n", lineNum, fields[1]);
            skipped++;
            continue;
        }

        /* 3. Source District */
        char canonSrc[DISTRICT_NAME_LEN];
        if (!is_valid_district(fields[2], canonSrc, sizeof(canonSrc))) {
            printf("Line %d skipped: Unrecognized source district \"%s\".\n", lineNum, fields[2]);
            skipped++;
            continue;
        }

        /* 4. Destination District */
        char canonDst[DISTRICT_NAME_LEN];
        if (!is_valid_district(fields[3], canonDst, sizeof(canonDst))) {
            printf("Line %d skipped: Unrecognized destination district \"%s\".\n", lineNum, fields[3]);
            skipped++;
            continue;
        }

        if (str_case_cmp(canonSrc, canonDst) == 0) {
            printf("Line %d skipped: Source and Destination cannot be the same (%s).\n", lineNum, canonSrc);
            skipped++;
            continue;
        }

        /* 5. Time Slot */
        char canonSlot[TIME_SLOT_LEN];
        if (!parse_time_slot(fields[4], canonSlot, sizeof(canonSlot))) {
            printf("Line %d skipped: Invalid time slot \"%s\".\n", lineNum, fields[4]);
            skipped++;
            continue;
        }

        /* 6. Seats Available */
        errno = 0;
        long seats = strtol(fields[5], &endptr, 10);
        if (errno != 0 || endptr == fields[5] || *endptr != '\0' || seats < 0) {
            printf("Line %d skipped: Invalid available seats \"%s\".\n", lineNum, fields[5]);
            skipped++;
            continue;
        }

        /* 7. Price Per Seat */
        errno = 0;
        double price = strtod(fields[6], &endptr);
        if (errno != 0 || endptr == fields[6] || *endptr != '\0' || price <= 0.0) {
            printf("Line %d skipped: Invalid seat price \"%s\".\n", lineNum, fields[6]);
            skipped++;
            continue;
        }

        /* Populate record */
        Train *t = &trains[*trainCount];
        t->trainNo = (int)tNo;
        snprintf(t->trainName, sizeof(t->trainName), "%s", fields[1]);
        snprintf(t->sourceDistrict, sizeof(t->sourceDistrict), "%s", canonSrc);
        snprintf(t->destinationDistrict, sizeof(t->destinationDistrict), "%s", canonDst);
        snprintf(t->timeSlot, sizeof(t->timeSlot), "%s", canonSlot);
        t->seatsAvailable = (int)seats;
        t->pricePerSeat = price;

        (*trainCount)++;
        loaded++;
    }

    fclose(fp);
    if (out_loaded) *out_loaded = loaded;
    if (out_skipped) *out_skipped = skipped;

    printf("\nTrain Data Load Summary: %d loaded, %d skipped.\n\n", loaded, skipped);
    return true;
}

bool save_trains_file(const char *filename, const Train *trains, int trainCount) {
    if (!filename || !trains) return false;

    /* Create backup of existing file */
    backup_file(filename, TRAINS_BAK_FILE);

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Could not open \"%s\" for writing.\n", filename);
        return false;
    }

    for (int i = 0; i < trainCount; i++) {
        if (fprintf(fp, "%d,%s,%s,%s,%s,%d,%.2f\n",
                    trains[i].trainNo,
                    trains[i].trainName,
                    trains[i].sourceDistrict,
                    trains[i].destinationDistrict,
                    trains[i].timeSlot,
                    trains[i].seatsAvailable,
                    trains[i].pricePerSeat) < 0) {
            printf("Error: Write failed for train %d.\n", trains[i].trainNo);
            fclose(fp);
            return false;
        }
    }

    if (fclose(fp) != 0) {
        printf("Error: Could not cleanly close \"%s\".\n", filename);
        return false;
    }

    return true;
}

bool load_bookings_file(const char *filename, Booking *bookings, int *bookingCount, int maxBookings,
                        int *nextBookingId, int *out_loaded, int *out_skipped) {
    if (!filename || !bookings || !bookingCount || !nextBookingId) return false;

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        if (out_loaded) *out_loaded = 0;
        if (out_skipped) *out_skipped = 0;
        return false;
    }

    char line[512];
    int lineNum = 0;
    int loaded = 0;
    int skipped = 0;

    while (fgets(line, sizeof(line), fp)) {
        lineNum++;
        trim_whitespace(line);

        if (line[0] == '\0') continue;

        /* Check for metadata header: #NEXT_BOOKING_ID,1005 */
        if (strncmp(line, "#NEXT_BOOKING_ID,", 17) == 0) {
            int metaId = atoi(line + 17);
            if (metaId > *nextBookingId) {
                *nextBookingId = metaId;
            }
            continue;
        }

        if (line[0] == '#') continue;

        if (*bookingCount >= maxBookings) {
            printf("Line %d skipped: Booking capacity reached (%d bookings).\n", lineNum, maxBookings);
            skipped++;
            continue;
        }

        char *fields[10];
        int fCount = split_csv_line(line, fields, 10);
        if (fCount != 8) {
            printf("Booking Line %d skipped: Expected 8 fields, found %d.\n", lineNum, fCount);
            skipped++;
            continue;
        }

        /* 1. bookingId */
        if (strlen(fields[0]) == 0 || strlen(fields[0]) >= BOOKING_ID_LEN) {
            skipped++;
            continue;
        }

        /* Track nextBookingId counter from BKxxxx */
        if (strncmp(fields[0], "BK", 2) == 0) {
            int idNum = atoi(fields[0] + 2);
            if (idNum >= *nextBookingId) {
                *nextBookingId = idNum + 1;
            }
        }

        /* 2. passengerName */
        if (strlen(fields[1]) == 0 || strlen(fields[1]) >= PASSENGER_NAME_LEN || contains_comma(fields[1])) {
            skipped++;
            continue;
        }

        /* 3. trainNo */
        int tNo = atoi(fields[2]);
        if (tNo <= 0) {
            skipped++;
            continue;
        }

        /* 4. seats */
        int seats = atoi(fields[3]);
        if (seats <= 0) {
            skipped++;
            continue;
        }

        /* 5. travelDate */
        if (strlen(fields[4]) == 0 || strlen(fields[4]) >= DATE_LEN) {
            skipped++;
            continue;
        }

        /* 6. timeSlot */
        if (strlen(fields[5]) == 0 || strlen(fields[5]) >= TIME_SLOT_LEN) {
            skipped++;
            continue;
        }

        /* 7. totalPrice */
        double price = atof(fields[6]);
        if (price < 0.0) {
            skipped++;
            continue;
        }

        /* 8. status */
        if (strcmp(fields[7], "CONFIRMED") != 0 && strcmp(fields[7], "CANCELLED") != 0) {
            skipped++;
            continue;
        }

        /* Check duplicate booking ID in loaded set */
        bool duplicate = false;
        for (int i = 0; i < *bookingCount; i++) {
            if (strcmp(bookings[i].bookingId, fields[0]) == 0) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) {
            skipped++;
            continue;
        }

        /* Populate booking */
        Booking *b = &bookings[*bookingCount];
        snprintf(b->bookingId, sizeof(b->bookingId), "%s", fields[0]);
        snprintf(b->passengerName, sizeof(b->passengerName), "%s", fields[1]);
        b->trainNo = tNo;
        b->seats = seats;
        snprintf(b->travelDate, sizeof(b->travelDate), "%s", fields[4]);
        snprintf(b->timeSlot, sizeof(b->timeSlot), "%s", fields[5]);
        b->totalPrice = price;
        snprintf(b->status, sizeof(b->status), "%s", fields[7]);

        (*bookingCount)++;
        loaded++;
    }

    fclose(fp);
    if (out_loaded) *out_loaded = loaded;
    if (out_skipped) *out_skipped = skipped;

    return true;
}

bool save_bookings_file(const char *filename, const Booking *bookings, int bookingCount, int nextBookingId) {
    if (!filename || !bookings) return false;

    /* Create backup of existing file */
    backup_file(filename, BOOKINGS_BAK_FILE);

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Could not open \"%s\" for writing.\n", filename);
        return false;
    }

    /* Store next ID counter so booking IDs never repeat across runs */
    if (fprintf(fp, "#NEXT_BOOKING_ID,%d\n", nextBookingId) < 0) {
        fclose(fp);
        return false;
    }

    for (int i = 0; i < bookingCount; i++) {
        if (fprintf(fp, "%s,%s,%d,%d,%s,%s,%.2f,%s\n",
                    bookings[i].bookingId,
                    bookings[i].passengerName,
                    bookings[i].trainNo,
                    bookings[i].seats,
                    bookings[i].travelDate,
                    bookings[i].timeSlot,
                    bookings[i].totalPrice,
                    bookings[i].status) < 0) {
            printf("Error: Write failed for booking %s.\n", bookings[i].bookingId);
            fclose(fp);
            return false;
        }
    }

    if (fclose(fp) != 0) {
        printf("Error: Could not cleanly close \"%s\".\n", filename);
        return false;
    }

    return true;
}
