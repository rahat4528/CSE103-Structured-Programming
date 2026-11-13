/**
 * CSE103-OS: Freestanding String and Memory Manipulation Library
 * Implements minimal libc standard memory and string functions without hosted libc.
 */

#ifndef CSE103_STRING_H
#define CSE103_STRING_H

#include "types.h"

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int   memcmp(const void *s1, const void *s2, size_t n);

size_t strlen(const char *s);
int    strcmp(const char *s1, const char *s2);
int    strncmp(const char *s1, const char *s2, size_t n);
char  *strcpy(char *dest, const char *src);
char  *strncpy(char *dest, const char *src, size_t n);
char  *strchr(const char *s, int c);

#endif /* CSE103_STRING_H */
