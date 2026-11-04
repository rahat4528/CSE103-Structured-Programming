#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/* String manipulation */
void trim_whitespace(char *str);
int str_case_cmp(const char *s1, const char *s2);
bool str_case_contains(const char *haystack, const char *needle);
bool contains_comma(const char *str);

/* Robust input functions */
bool safe_read_line(char *buffer, size_t size, FILE *stream);
bool safe_read_int(int *out_val, int min_val, int max_val);
bool safe_read_double(double *out_val, double min_val, double max_val);
bool safe_read_name(char *buffer, size_t size, const char *prompt_label);

/* Date validation */
bool is_leap_year(int year);
bool is_valid_date(int day, int month, int year);
bool is_date_future_or_today(int day, int month, int year);
bool parse_and_validate_date(const char *date_str, char *canonical_out, size_t out_size);

/* Cross-platform directory creation */
int portable_mkdir(const char *dir_path);

#endif /* UTILS_H */
