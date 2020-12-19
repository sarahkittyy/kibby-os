#include "kmem.h"

void heap_init() {
	heap_bot += 0xC0000000;
}

uint32_t kmalloc_old(size_t sz) {
	uint32_t tmp = heap_bot;
	heap_bot += sz;
	return tmp;
}

// 4096b alignment
uint32_t kmalloc_old_a(size_t sz) {
	heap_bot &= 0xFFFFF000;
	heap_bot += 4096;

	return kmalloc_old(sz);
}
