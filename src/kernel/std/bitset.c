#include "bitset.h"

#include <kernel/internal/kmem.h>

static size_t bitset_major_index(size_t b) {
	return b / 8;
}

static size_t bitset_minor_index(size_t b) {
	return b % 8;
}

bitset_t* bitset_create(size_t bits) {
	bitset_t* b = (bitset_t*)kmalloc(sizeof(bitset_t));
	// 1 byte / 8 bits
	b->data = (uint8_t*)kmalloc(bits / 8 + 1);
	b->sz	= bits;
	return b;
}

void bitset_set(bitset_t* s, size_t bit) {
	uint8_t* b = &s->data[bitset_major_index(bit)];
	*b |= (1 << bitset_minor_index(bit));
}

bool bitset_test(bitset_t* s, size_t bit) {
	uint8_t* b = &s->data[bitset_major_index(bit)];
	return !!(*b & (1 << bitset_minor_index(bit)));
}

void bitset_clear(bitset_t* s, size_t bit) {
	uint8_t* b = &s->data[bitset_major_index(bit)];
	*b &= ~(1 << bitset_minor_index(bit));
}

size_t bitset_first_unset(bitset_t* s) {
	// for every byte
	for (size_t i = 0; i < s->sz / 8 + 1; ++i) {
		// if none are set in the whole byte, ignore
		if (!(i & 0xFF)) {
			continue;
		} else {
			// check all bits in this byte
			for (size_t j = 0; j < 8; ++j) {
				size_t ij = i * 8 + j;
				if (!bitset_test(s, ij)) {
					return ij;
				}
			}
		}
	}
	return SIZE_MAX;
}
