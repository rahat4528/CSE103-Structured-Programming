#include "data.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

const char * const DIVISIONS[TOTAL_DIVISIONS] = {
    "Dhaka",
    "Chattogram",
    "Rajshahi",
    "Khulna",
    "Barishal",
    "Sylhet",
    "Rangpur",
    "Mymensingh" /* Note: in the guide, districts are grouped under 7-8 divisions. In the prompt:
                    Dhaka (17), Chattogram (11), Rajshahi (8), Khulna (10),
                    Barishal (6), Sylhet (4), Rangpur (8) -> total 64 across the 7 division headers */
};

const char * const TIME_SLOTS[TOTAL_TIME_SLOTS] = {
    "08:00 AM",
    "12:00 PM",
    "04:00 PM",
    "08:00 PM"
};

const District BANGLADESH_DISTRICTS[TOTAL_DISTRICTS] = {
    /* Dhaka Division (17 districts) */
    {"Dhaka", "Dhaka"},
    {"Gazipur", "Dhaka"},
    {"Kishoreganj", "Dhaka"},
    {"Manikganj", "Dhaka"},
    {"Munshiganj", "Dhaka"},
    {"Narayanganj", "Dhaka"},
    {"Narsingdi", "Dhaka"},
    {"Tangail", "Dhaka"},
    {"Faridpur", "Dhaka"},
    {"Gopalganj", "Dhaka"},
    {"Madaripur", "Dhaka"},
    {"Rajbari", "Dhaka"},
    {"Shariatpur", "Dhaka"},
    {"Mymensingh", "Dhaka"},
    {"Jamalpur", "Dhaka"},
    {"Sherpur", "Dhaka"},
    {"Netrokona", "Dhaka"},

    /* Chattogram Division (11 districts) */
    {"Chattogram", "Chattogram"},
    {"Cox's Bazar", "Chattogram"},
    {"Bandarban", "Chattogram"},
    {"Brahmanbaria", "Chattogram"},
    {"Chandpur", "Chattogram"},
    {"Cumilla", "Chattogram"},
    {"Feni", "Chattogram"},
    {"Khagrachhari", "Chattogram"},
    {"Lakshmipur", "Chattogram"},
    {"Noakhali", "Chattogram"},
    {"Rangamati", "Chattogram"},

    /* Rajshahi Division (8 districts) */
    {"Rajshahi", "Rajshahi"},
    {"Bogura", "Rajshahi"},
    {"Joypurhat", "Rajshahi"},
    {"Naogaon", "Rajshahi"},
    {"Natore", "Rajshahi"},
    {"Chapainawabganj", "Rajshahi"},
    {"Pabna", "Rajshahi"},
    {"Sirajganj", "Rajshahi"},

    /* Khulna Division (10 districts) */
    {"Khulna", "Khulna"},
    {"Bagerhat", "Khulna"},
    {"Chuadanga", "Khulna"},
    {"Jashore", "Khulna"},
    {"Jhenaidah", "Khulna"},
    {"Kushtia", "Khulna"},
    {"Magura", "Khulna"},
    {"Meherpur", "Khulna"},
    {"Narail", "Khulna"},
    {"Satkhira", "Khulna"},

    /* Barishal Division (6 districts) */
    {"Barishal", "Barishal"},
    {"Barguna", "Barishal"},
    {"Bhola", "Barishal"},
    {"Jhalokati", "Barishal"},
    {"Patuakhali", "Barishal"},
    {"Pirojpur", "Barishal"},

    /* Sylhet Division (4 districts) */
    {"Sylhet", "Sylhet"},
    {"Habiganj", "Sylhet"},
    {"Moulvibazar", "Sylhet"},
    {"Sunamganj", "Sylhet"},

    /* Rangpur Division (8 districts) */
    {"Rangpur", "Rangpur"},
    {"Dinajpur", "Rangpur"},
    {"Gaibandha", "Rangpur"},
    {"Kurigram", "Rangpur"},
    {"Lalmonirhat", "Rangpur"},
    {"Nilphamari", "Rangpur"},
    {"Panchagarh", "Rangpur"},
    {"Thakurgaon", "Rangpur"}
};

void init_and_verify_districts(void) {
    /* Startup assertion/self-check that the array has exactly 64 unique entries */
    assert(TOTAL_DISTRICTS == 64);

    for (int i = 0; i < TOTAL_DISTRICTS; i++) {
        assert(BANGLADESH_DISTRICTS[i].name != NULL);
        assert(BANGLADESH_DISTRICTS[i].division != NULL);
        assert(strlen(BANGLADESH_DISTRICTS[i].name) > 0);

        for (int j = i + 1; j < TOTAL_DISTRICTS; j++) {
            if (str_case_cmp(BANGLADESH_DISTRICTS[i].name, BANGLADESH_DISTRICTS[j].name) == 0) {
                fprintf(stderr, "Fatal Error: Duplicate district detected: %s\n", BANGLADESH_DISTRICTS[i].name);
                assert(false && "Duplicate district found in BANGLADESH_DISTRICTS");
            }
        }
    }
}

bool is_valid_district(const char *name, char *canonical_out, size_t out_size) {
    if (!name || *name == '\0') return false;

    char cleaned[64];
    strncpy(cleaned, name, sizeof(cleaned) - 1);
    cleaned[sizeof(cleaned) - 1] = '\0';
    trim_whitespace(cleaned);

    for (int i = 0; i < TOTAL_DISTRICTS; i++) {
        if (str_case_cmp(cleaned, BANGLADESH_DISTRICTS[i].name) == 0) {
            if (canonical_out && out_size > 0) {
                strncpy(canonical_out, BANGLADESH_DISTRICTS[i].name, out_size - 1);
                canonical_out[out_size - 1] = '\0';
            }
            return true;
        }
    }
    return false;
}

bool parse_time_slot(const char *input, char *canonical_out, size_t out_size) {
    if (!input || *input == '\0') return false;

    char cleaned[32];
    strncpy(cleaned, input, sizeof(cleaned) - 1);
    cleaned[sizeof(cleaned) - 1] = '\0';
    trim_whitespace(cleaned);

    /* Allow picking by number 1-4 */
    if (strcmp(cleaned, "1") == 0) {
        if (canonical_out && out_size > 0) {
            strncpy(canonical_out, TIME_SLOTS[0], out_size - 1);
            canonical_out[out_size - 1] = '\0';
        }
        return true;
    } else if (strcmp(cleaned, "2") == 0) {
        if (canonical_out && out_size > 0) {
            strncpy(canonical_out, TIME_SLOTS[1], out_size - 1);
            canonical_out[out_size - 1] = '\0';
        }
        return true;
    } else if (strcmp(cleaned, "3") == 0) {
        if (canonical_out && out_size > 0) {
            strncpy(canonical_out, TIME_SLOTS[2], out_size - 1);
            canonical_out[out_size - 1] = '\0';
        }
        return true;
    } else if (strcmp(cleaned, "4") == 0) {
        if (canonical_out && out_size > 0) {
            strncpy(canonical_out, TIME_SLOTS[3], out_size - 1);
            canonical_out[out_size - 1] = '\0';
        }
        return true;
    }

    /* Allow typing the slot (case-insensitive, e.g. "08:00 AM", "8:00 AM") */
    for (int i = 0; i < TOTAL_TIME_SLOTS; i++) {
        if (str_case_cmp(cleaned, TIME_SLOTS[i]) == 0) {
            if (canonical_out && out_size > 0) {
                strncpy(canonical_out, TIME_SLOTS[i], out_size - 1);
                canonical_out[out_size - 1] = '\0';
            }
            return true;
        }
    }

    /* Check alternate formats like "8:00 AM" for "08:00 AM" or "4:00 PM" for "04:00 PM" */
    if (str_case_cmp(cleaned, "8:00 AM") == 0 || str_case_cmp(cleaned, "8:00AM") == 0) {
        if (canonical_out && out_size > 0) {
            strncpy(canonical_out, TIME_SLOTS[0], out_size - 1);
            canonical_out[out_size - 1] = '\0';
        }
        return true;
    }
    if (str_case_cmp(cleaned, "12:00PM") == 0) {
        if (canonical_out && out_size > 0) {
            strncpy(canonical_out, TIME_SLOTS[1], out_size - 1);
            canonical_out[out_size - 1] = '\0';
        }
        return true;
    }
    if (str_case_cmp(cleaned, "4:00 PM") == 0 || str_case_cmp(cleaned, "4:00PM") == 0) {
        if (canonical_out && out_size > 0) {
            strncpy(canonical_out, TIME_SLOTS[2], out_size - 1);
            canonical_out[out_size - 1] = '\0';
        }
        return true;
    }
    if (str_case_cmp(cleaned, "8:00 PM") == 0 || str_case_cmp(cleaned, "8:00PM") == 0) {
        if (canonical_out && out_size > 0) {
            strncpy(canonical_out, TIME_SLOTS[3], out_size - 1);
            canonical_out[out_size - 1] = '\0';
        }
        return true;
    }

    return false;
}

void print_districts_by_division(void) {
    const char *divisions[] = {
        "Dhaka", "Chattogram", "Rajshahi", "Khulna", "Barishal", "Sylhet", "Rangpur"
    };
    int numDivisions = 7;

    printf("\n===============================================================\n");
    printf("         BANGLADESH DISTRICTS BY DIVISION (TOTAL: 64)         \n");
    printf("===============================================================\n");

    for (int d = 0; d < numDivisions; d++) {
        int count = 0;
        printf("\n[%s Division]\n", divisions[d]);
        for (int i = 0; i < TOTAL_DISTRICTS; i++) {
            if (strcmp(BANGLADESH_DISTRICTS[i].division, divisions[d]) == 0) {
                count++;
                printf("  %2d. %-18s", count, BANGLADESH_DISTRICTS[i].name);
                if (count % 3 == 0) printf("\n");
            }
        }
        if (count % 3 != 0) printf("\n");
        printf("  Subtotal: %d districts\n", count);
    }
    printf("===============================================================\n\n");
}

