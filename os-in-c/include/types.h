/**
 * CSE103-OS: Freestanding Primitive Types
 * Standard integer definitions, fixed-width types, and booleans for 32-bit x86.
 */

#ifndef CSE103_TYPES_H
#define CSE103_TYPES_H

typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned short     uint16_t;
typedef signed short       int16_t;
typedef unsigned int       uint32_t;
typedef signed int         int32_t;
typedef unsigned long long uint64_t;
typedef signed long long   int64_t;

typedef unsigned int       size_t;
typedef signed int         ptrdiff_t;
typedef unsigned int       uintptr_t;
typedef signed int         intptr_t;

#define NULL ((void *)0)

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
/* In C23, bool, true, and false are built-in language keywords */
#else
#ifndef __cplusplus
#ifndef bool
typedef _Bool bool;
#define true 1
#define false 0
#endif
#endif
#endif

#endif /* CSE103_TYPES_H */
