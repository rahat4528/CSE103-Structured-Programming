/**
 * CSE103-OS: String and Memory Library
 * Standard freestanding C implementations of memory copying, setting, comparison,
 * and string manipulation functions.
 */

#include "string.h"

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;
    uint8_t val = (uint8_t)c;
    for (size_t i = 0; i < n; i++) {
        p[i] = val;
    }
    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    if (d < s) {
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else if (d > s) {
        for (size_t i = n; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return (int)p1[i] - (int)p2[i];
        }
    }
    return 0;
}

size_t strlen(const char *s) {
    size_t len = 0;
    while (s && s[len] != '\0') {
        len++;
    }
    return len;
}

int strcmp(const char *s1, const char *s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (int)((unsigned char)*s1) - (int)((unsigned char)*s2);
}

int strncmp(const char *s1, const char *s2, size_t n) {
    if (n == 0) return 0;
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    while (n > 1 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    return (int)((unsigned char)*s1) - (int)((unsigned char)*s2);
}

char *strcpy(char *dest, const char *src) {
    if (!dest || !src) return dest;
    char *d = dest;
    while ((*d++ = *src++) != '\0') {
        /* copy loop */
    }
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    if (!dest) return NULL;
    size_t i = 0;
    for (; i < n && src && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

char *strchr(const char *s, int c) {
    if (!s) return NULL;
    char target = (char)c;
    while (*s != '\0') {
        if (*s == target) {
            return (char *)s;
        }
        s++;
    }
    if (target == '\0') {
        return (char *)s;
    }
    return NULL;
}
