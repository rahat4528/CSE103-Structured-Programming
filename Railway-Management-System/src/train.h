#ifndef TRAIN_H
#define TRAIN_H

#include <stdbool.h>

#define MAX_TRAINS 200
#define TRAIN_NAME_LEN 64
#define DISTRICT_NAME_LEN 32
#define TIME_SLOT_LEN 16

typedef struct {
    int trainNo;
    char trainName[TRAIN_NAME_LEN];
    char sourceDistrict[DISTRICT_NAME_LEN];
    char destinationDistrict[DISTRICT_NAME_LEN];
    char timeSlot[TIME_SLOT_LEN];
    int seatsAvailable;
    double pricePerSeat;
} Train;

/* Display functions */
void showInfo(const Train *t);
void printTrainTableHeader(void);
void printTrainTableFooter(void);
void showTrainDetails(const Train *t);
void viewAllTrains(const Train *trains, int trainCount);

/* Lookup functions */
const Train* findTrainByNumber(const Train *trains, int trainCount, int trainNo);
Train* findTrainByNumberMutable(Train *trains, int trainCount, int trainNo);
int findTrainIndexByNumber(const Train *trains, int trainCount, int trainNo);

/* Core operations */
bool addTrain(Train *trains, int *trainCount, const Train *newTrain);

/* Search functions (Stage 2) */
void searchTrainByNumber(const Train *trains, int trainCount, int trainNo);
void searchTrainByName(const Train *trains, int trainCount, const char *keyword);
void searchTrainByDistrict(const Train *trains, int trainCount, const char *district);
void searchTrainByTimeSlot(const Train *trains, int trainCount, const char *slot);
void searchTrainByRoute(const Train *trains, int trainCount, const char *src, const char *dst);

/* Management operations (Stage 5) */
bool updateTrain(Train *t, const char *newName, const char *newSrc, const char *newDst,
                 const char *newSlot, int newSeats, double newPrice);
bool deleteTrain(Train *trains, int *trainCount, int trainNo);
void sortTrains(Train *trains, int trainCount, int criteria, bool ascending);

#endif /* TRAIN_H */
