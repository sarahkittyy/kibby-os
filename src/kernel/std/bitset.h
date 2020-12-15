#pragma once

#include <kernel/std/type.h>

typedef struct {
	uint8_t* data;
	size_t sz;
} bitset_t;

bitset_t* bitset_create(size_t bits);

void bitset_set(bitset_t* s, size_t bit);
bool bitset_test(bitset_t* s, size_t bit);
void bitset_clear(bitset_t* s, size_t bit);

size_t bitset_first_unset(bitset_t* s);
