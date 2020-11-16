#pragma once

#include <std/type.h>

// compute the length of a string
size_t strlen(const char* str);

// copy strings
void strcpy(char* dest, const char* src);

// concatenate strings
void strcat(char* dest, const char* src);

// compare strings
int strcmp(const char* a, const char* b);

// print to string
size_t sprintf(char* out, const char* fmt, ...);
