#include "train.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

void printTrainTableHeader(void) {
    printf("+----------+-------------------------+-------------------+-------------------+------------+-------+------------+\n");
    printf("| Train No | Train Name              | Source            | Destination       | Time Slot  | Seats | Price(BDT) |\n");
    printf("+----------+-------------------------+-------------------+-------------------+------------+-------+------------+\n");
}

void printTrainTableFooter(void) {
    printf("+----------+-------------------------+-------------------+-------------------+------------+-------+------------+\n");
}

void showInfo(const Train *t) {
    if (!t) return;
    printf("| %-8d | %-23s | %-17s | %-17s | %-10s | %5d | %10.2f |\n",
           t->trainNo, t->trainName, t->sourceDistrict, t->destinationDistrict,
           t->timeSlot, t->seatsAvailable, t->pricePerSeat);
}

void showTrainDetails(const Train *t) {
    if (!t) return;
    printf("--------------------------------------------------\n");
    printf("Train Number       : %d\n", t->trainNo);
    printf("Train Name         : %s\n", t->trainName);
    printf("Source District    : %s\n", t->sourceDistrict);
    printf("Destination District: %s\n", t->destinationDistrict);
    printf("Time Slot          : %s\n", t->timeSlot);
    printf("Available Seats    : %d\n", t->seatsAvailable);
    printf("Price Per Seat     : %.2f BDT\n", t->pricePerSeat);
    printf("--------------------------------------------------\n");
}

void viewAllTrains(const Train *trains, int trainCount) {
    if (trainCount <= 0 || !trains) {
        printf("\nNo trains available.\n\n");
        return;
    }

    printf("\n--- All Available Trains (%d total) ---\n", trainCount);
    printTrainTableHeader();
    for (int i = 0; i < trainCount; i++) {
        showInfo(&trains[i]);
    }
    printTrainTableFooter();
    printf("\n");
}

int findTrainIndexByNumber(const Train *trains, int trainCount, int trainNo) {
    if (!trains || trainCount <= 0) return -1;
    for (int i = 0; i < trainCount; i++) {
        if (trains[i].trainNo == trainNo) {
            return i;
        }
    }
    return -1;
}

const Train* findTrainByNumber(const Train *trains, int trainCount, int trainNo) {
    int idx = findTrainIndexByNumber(trains, trainCount, trainNo);
    if (idx >= 0) {
        return &trains[idx];
    }
    return NULL;
}

Train* findTrainByNumberMutable(Train *trains, int trainCount, int trainNo) {
    int idx = findTrainIndexByNumber(trains, trainCount, trainNo);
    if (idx >= 0) {
        return &trains[idx];
    }
    return NULL;
}

bool addTrain(Train *trains, int *trainCount, const Train *newTrain) {
    if (!trains || !trainCount || !newTrain) return false;

    if (*trainCount >= MAX_TRAINS) {
        printf("Error: Train capacity reached (maximum %d trains).\n", MAX_TRAINS);
        return false;
    }

    if (findTrainIndexByNumber(trains, *trainCount, newTrain->trainNo) >= 0) {
        printf("Error: Train number %d already exists.\n", newTrain->trainNo);
        return false;
    }

    trains[*trainCount] = *newTrain;
    (*trainCount)++;
    return true;
}

void searchTrainByNumber(const Train *trains, int trainCount, int trainNo) {
    if (!trains || trainCount <= 0) {
        printf("\nNo matching trains.\n\n");
        return;
    }

    int matches = 0;
    for (int i = 0; i < trainCount; i++) {
        if (trains[i].trainNo == trainNo) {
            if (matches == 0) {
                printf("\n--- Search Results for Train #%d ---\n", trainNo);
                printTrainTableHeader();
            }
            showInfo(&trains[i]);
            matches++;
        }
    }

    if (matches == 0) {
        printf("\nNo matching trains.\n\n");
    } else {
        printTrainTableFooter();
        printf("Found %d matching train.\n\n", matches);
    }
}

void searchTrainByName(const Train *trains, int trainCount, const char *keyword) {
    if (!trains || trainCount <= 0 || !keyword || *keyword == '\0') {
        printf("\nNo matching trains.\n\n");
        return;
    }

    int matches = 0;
    for (int i = 0; i < trainCount; i++) {
        if (str_case_contains(trains[i].trainName, keyword)) {
            if (matches == 0) {
                printf("\n--- Search Results for Train Name containing \"%s\" ---\n", keyword);
                printTrainTableHeader();
            }
            showInfo(&trains[i]);
            matches++;
        }
    }

    if (matches == 0) {
        printf("\nNo matching trains.\n\n");
    } else {
        printTrainTableFooter();
        printf("Found %d matching train(s).\n\n", matches);
    }
}

void searchTrainByDistrict(const Train *trains, int trainCount, const char *district) {
    if (!trains || trainCount <= 0 || !district || *district == '\0') {
        printf("\nNo matching trains.\n\n");
        return;
    }

    int matches = 0;
    for (int i = 0; i < trainCount; i++) {
        if (str_case_cmp(trains[i].sourceDistrict, district) == 0 ||
            str_case_cmp(trains[i].destinationDistrict, district) == 0) {
            if (matches == 0) {
                printf("\n--- Search Results for District \"%s\" ---\n", district);
                printTrainTableHeader();
            }
            showInfo(&trains[i]);
            matches++;
        }
    }

    if (matches == 0) {
        printf("\nNo matching trains.\n\n");
    } else {
        printTrainTableFooter();
        printf("Found %d matching train(s).\n\n", matches);
    }
}

void searchTrainByTimeSlot(const Train *trains, int trainCount, const char *slot) {
    if (!trains || trainCount <= 0 || !slot || *slot == '\0') {
        printf("\nNo matching trains.\n\n");
        return;
    }

    int matches = 0;
    for (int i = 0; i < trainCount; i++) {
        if (str_case_cmp(trains[i].timeSlot, slot) == 0) {
            if (matches == 0) {
                printf("\n--- Search Results for Time Slot \"%s\" ---\n", slot);
                printTrainTableHeader();
            }
            showInfo(&trains[i]);
            matches++;
        }
    }

    if (matches == 0) {
        printf("\nNo matching trains.\n\n");
    } else {
        printTrainTableFooter();
        printf("Found %d matching train(s).\n\n", matches);
    }
}

void searchTrainByRoute(const Train *trains, int trainCount, const char *src, const char *dst) {
    if (!trains || trainCount <= 0 || !src || !dst) {
        printf("\nNo matching trains.\n\n");
        return;
    }

    int matches = 0;
    for (int i = 0; i < trainCount; i++) {
        if (str_case_cmp(trains[i].sourceDistrict, src) == 0 &&
            str_case_cmp(trains[i].destinationDistrict, dst) == 0) {
            if (matches == 0) {
                printf("\n--- Search Results for Route \"%s\" -> \"%s\" ---\n", src, dst);
                printTrainTableHeader();
            }
            showInfo(&trains[i]);
            matches++;
        }
    }

    if (matches == 0) {
        printf("\nNo matching trains.\n\n");
    } else {
        printTrainTableFooter();
        printf("Found %d matching train(s).\n\n", matches);
    }
}

static int get_time_slot_order(const char *slot) {
    if (str_case_cmp(slot, "08:00 AM") == 0 || str_case_cmp(slot, "8:00 AM") == 0) return 1;
    if (str_case_cmp(slot, "12:00 PM") == 0) return 2;
    if (str_case_cmp(slot, "04:00 PM") == 0 || str_case_cmp(slot, "4:00 PM") == 0) return 3;
    if (str_case_cmp(slot, "08:00 PM") == 0 || str_case_cmp(slot, "8:00 PM") == 0) return 4;
    return 99;
}

static int compare_trains(const Train *a, const Train *b, int criteria, bool ascending) {
    int cmp = 0;
    switch (criteria) {
        case 1: /* Train Number */
            cmp = (a->trainNo > b->trainNo) - (a->trainNo < b->trainNo);
            break;
        case 2: /* Time Slot Chronological */
            cmp = get_time_slot_order(a->timeSlot) - get_time_slot_order(b->timeSlot);
            break;
        case 3: /* Price */
            if (a->pricePerSeat > b->pricePerSeat) cmp = 1;
            else if (a->pricePerSeat < b->pricePerSeat) cmp = -1;
            else cmp = 0;
            break;
        case 4: /* Seats Available */
            cmp = (a->seatsAvailable > b->seatsAvailable) - (a->seatsAvailable < b->seatsAvailable);
            break;
        default:
            cmp = 0;
            break;
    }
    return ascending ? cmp : -cmp;
}

void sortTrains(Train *trains, int trainCount, int criteria, bool ascending) {
    if (!trains || trainCount <= 1) return;

    /* Stable Insertion Sort */
    for (int i = 1; i < trainCount; i++) {
        Train key = trains[i];
        int j = i - 1;
        while (j >= 0 && compare_trains(&trains[j], &key, criteria, ascending) > 0) {
            trains[j + 1] = trains[j];
            j--;
        }
        trains[j + 1] = key;
    }
}

bool updateTrain(Train *t, const char *newName, const char *newSrc, const char *newDst,
                 const char *newSlot, int newSeats, double newPrice) {
    if (!t || !newName || !newSrc || !newDst || !newSlot) return false;
    if (newSeats < 0 || newPrice <= 0.0) return false;

    snprintf(t->trainName, sizeof(t->trainName), "%s", newName);
    snprintf(t->sourceDistrict, sizeof(t->sourceDistrict), "%s", newSrc);
    snprintf(t->destinationDistrict, sizeof(t->destinationDistrict), "%s", newDst);
    snprintf(t->timeSlot, sizeof(t->timeSlot), "%s", newSlot);
    t->seatsAvailable = newSeats;
    t->pricePerSeat = newPrice;
    return true;
}

bool deleteTrain(Train *trains, int *trainCount, int trainNo) {
    if (!trains || !trainCount || *trainCount <= 0) return false;
    int idx = findTrainIndexByNumber(trains, *trainCount, trainNo);
    if (idx < 0) return false;

    for (int i = idx; i < *trainCount - 1; i++) {
        trains[i] = trains[i + 1];
    }
    (*trainCount)--;
    return true;
}

