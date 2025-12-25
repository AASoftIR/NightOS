/*
 * NightOS - A Minimal Operating System
 * 
 * Common type definitions and macros
 * Inspired by Unix/Linux with a dark, minimal aesthetic
 */

#ifndef TYPES_H
#define TYPES_H

/* Standard integer types */
typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned short     uint16_t;
typedef signed short       int16_t;
typedef unsigned int       uint32_t;
typedef signed int         int32_t;
typedef unsigned long long uint64_t;
typedef signed long long   int64_t;

/* Size types */
typedef uint32_t size_t;
typedef int32_t  ssize_t;

/* Boolean type */
typedef uint8_t bool;
#define true  1
#define false 0

/* NULL pointer */
#define NULL ((void*)0)

/* Useful macros */
#define UNUSED(x) (void)(x)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* Memory alignment */
#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

/* Bit manipulation */
#define BIT(x)        (1 << (x))
#define SET_BIT(n, x) ((n) |= BIT(x))
#define CLR_BIT(n, x) ((n) &= ~BIT(x))
#define GET_BIT(n, x) (((n) >> (x)) & 1)

#endif /* TYPES_H */
