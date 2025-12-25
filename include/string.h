/*
 * NightOS - String Utilities
 * 
 * Basic string manipulation functions
 */

#ifndef STRING_H
#define STRING_H

#include "types.h"

/* Get string length */
size_t strlen(const char* str);

/* Copy string */
char* strcpy(char* dest, const char* src);

/* Copy n characters */
char* strncpy(char* dest, const char* src, size_t n);

/* Compare strings */
int strcmp(const char* s1, const char* s2);

/* Compare n characters */
int strncmp(const char* s1, const char* s2, size_t n);

/* Find character in string */
char* strchr(const char* str, int c);

/* Concatenate strings */
char* strcat(char* dest, const char* src);

/* Memory operations */
void* memset(void* ptr, int value, size_t num);
void* memcpy(void* dest, const void* src, size_t num);
int memcmp(const void* ptr1, const void* ptr2, size_t num);
void* memmove(void* dest, const void* src, size_t num);

/* Convert integer to string */
void itoa(int value, char* str, int base);
void utoa(uint32_t value, char* str, int base);

/* Convert string to integer */
int atoi(const char* str);

#endif /* STRING_H */
