#ifndef DATA_H
#define DATA_H

#include <stdbool.h>
#include <stddef.h>

#define TOTAL_DISTRICTS 64
#define TOTAL_DIVISIONS 8
#define TOTAL_TIME_SLOTS 4

typedef struct {
    const char *name;
    const char *division;
} District;

extern const District BANGLADESH_DISTRICTS[TOTAL_DISTRICTS];
extern const char * const DIVISIONS[TOTAL_DIVISIONS];
extern const char * const TIME_SLOTS[TOTAL_TIME_SLOTS];

/* Verify at startup that exactly 64 unique districts are present */
void init_and_verify_districts(void);

/* Case-insensitive lookup; stores canonical name if found */
bool is_valid_district(const char *name, char *canonical_out, size_t out_size);

/* Parses slot by number ("1"-"4") or slot string; stores canonical slot */
bool parse_time_slot(const char *input, char *canonical_out, size_t out_size);

/* Formatted display of all districts grouped by division */
void print_districts_by_division(void);

#endif /* DATA_H */

