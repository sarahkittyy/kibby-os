#pragma once

#include "kernel/std/type.h"
#include <stdarg.h>

// compute the length of a string
size_t strlen(const char* str);

// copy strings
void strcpy(char* dest, const char* src);

// concatenate strings
void strcat(char* dest, const char* src);

// compare strings
int strcmp(const char* a, const char* b);

// reverse a string in-place
void strrev(char* str);

// int to string.
void itoa(char* dest, uint32_t x, int base);

// print to string
size_t sprintf(char* out, const char* fmt, ...);
size_t sprintfv(char* out, const char* fmt, va_list l);

// standard memset
void* memset(void* data, uint8_t b, size_t n);
// volatile memset
volatile void* memset_v(volatile void* data, uint8_t b, size_t n);
