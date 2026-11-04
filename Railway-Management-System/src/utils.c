#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

void trim_whitespace(char *str) {
    if (!str) return;

    /* Trim leading whitespace */
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    /* Trim trailing whitespace and newlines */
    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }
    *end = '\0';

    if (start != str) {
        memmove(str, start, (size_t)(end - start + 1));
    }
}

int str_case_cmp(const char *s1, const char *s2) {
    if (!s1 || !s2) {
        return (s1 == s2) ? 0 : (s1 ? 1 : -1);
    }
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

bool str_case_contains(const char *haystack, const char *needle) {
    if (!haystack || !needle) return false;
    if (*needle == '\0') return true;

    size_t n_len = strlen(needle);
    size_t h_len = strlen(haystack);
    if (n_len > h_len) return false;

    for (size_t i = 0; i <= h_len - n_len; i++) {
        size_t j = 0;
        for (; j < n_len; j++) {
            if (tolower((unsigned char)haystack[i + j]) != tolower((unsigned char)needle[j])) {
                break;
            }
        }
        if (j == n_len) return true;
    }
    return false;
}

bool contains_comma(const char *str) {
    if (!str) return false;
    return strchr(str, ',') != NULL;
}

bool safe_read_line(char *buffer, size_t size, FILE *stream) {
    if (!buffer || size == 0 || !stream) return false;

    if (fgets(buffer, (int)size, stream) == NULL) {
        buffer[0] = '\0';
        return false;
    }

    size_t len = strlen(buffer);
    if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[--len] = '\0';
        }
    } else if (len == size - 1) {
        /* Line was truncated; flush rest of line until \n or EOF */
        int c;
        while ((c = fgetc(stream)) != '\n' && c != EOF) {}
    }

    trim_whitespace(buffer);
    return true;
}

bool safe_read_int(int *out_val, int min_val, int max_val) {
    if (!out_val) return false;

    char line[64];
    if (!safe_read_line(line, sizeof(line), stdin)) {
        return false;
    }

    if (line[0] == '\0') {
        return false;
    }

    char *endptr = NULL;
    errno = 0;
    long val = strtol(line, &endptr, 10);
    if (errno != 0 || endptr == line || *endptr != '\0') {
        return false;
    }

    if (val < min_val || val > max_val) {
        return false;
    }

    *out_val = (int)val;
    return true;
}

bool safe_read_double(double *out_val, double min_val, double max_val) {
    if (!out_val) return false;

    char line[64];
    if (!safe_read_line(line, sizeof(line), stdin)) {
        return false;
    }

    if (line[0] == '\0') {
        return false;
    }

    char *endptr = NULL;
    errno = 0;
    double val = strtod(line, &endptr);
    if (errno != 0 || endptr == line || *endptr != '\0') {
        return false;
    }

    if (val < min_val || val > max_val) {
        return false;
    }

    *out_val = val;
    return true;
}

bool safe_read_name(char *buffer, size_t size, const char *prompt_label) {
    if (!buffer || size == 0) return false;

    while (1) {
        printf("Enter %s: ", prompt_label);
        if (!safe_read_line(buffer, size, stdin)) {
            return false;
        }

        if (buffer[0] == '\0') {
            printf("Error: %s cannot be empty. Please re-enter.\n", prompt_label);
            continue;
        }

        if (contains_comma(buffer)) {
            printf("Error: %s cannot contain commas. Please re-enter.\n", prompt_label);
            continue;
        }

        return true;
    }
}

bool is_leap_year(int year) {
    if (year % 400 == 0) return true;
    if (year % 100 == 0) return false;
    return (year % 4 == 0);
}

bool is_valid_date(int day, int month, int year) {
    if (year < 2000 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1) return false;

    static const int days_in_month[] = {
        0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    int max_days = days_in_month[month];
    if (month == 2 && is_leap_year(year)) {
        max_days = 29;
    }

    return (day <= max_days);
}

bool is_date_future_or_today(int day, int month, int year) {
    time_t now = time(NULL);
    struct tm *cur = localtime(&now);
    if (!cur) return true;

    int cur_year = cur->tm_year + 1900;
    int cur_mon = cur->tm_mon + 1;
    int cur_day = cur->tm_mday;

    if (year > cur_year) return true;
    if (year < cur_year) return false;

    if (month > cur_mon) return true;
    if (month < cur_mon) return false;

    return (day >= cur_day);
}

bool parse_and_validate_date(const char *date_str, char *canonical_out, size_t out_size) {
    if (!date_str || strlen(date_str) != 10) return false;

    if (date_str[2] != '-' || date_str[5] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit((unsigned char)date_str[i])) return false;
    }

    int day = (date_str[0] - '0') * 10 + (date_str[1] - '0');
    int month = (date_str[3] - '0') * 10 + (date_str[4] - '0');
    int year = (date_str[6] - '0') * 1000 + (date_str[7] - '0') * 100 +
               (date_str[8] - '0') * 10 + (date_str[9] - '0');

    if (!is_valid_date(day, month, year)) return false;
    if (!is_date_future_or_today(day, month, year)) return false;

    if (canonical_out && out_size >= 11) {
        snprintf(canonical_out, out_size, "%02d-%02d-%04d", day, month, year);
    }
    return true;
}

int portable_mkdir(const char *dir_path) {
    if (!dir_path || *dir_path == '\0') return -1;
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    return _mkdir(dir_path);
#else
    return mkdir(dir_path, 0755);
#endif
}
