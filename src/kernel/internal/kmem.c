#include "kmem.h"

#include <kernel/std/heap.h>

static heap_t* kheap;

void old_heap_init() {
	kheap = NULL;
	heap_bot += 0xC0000000;
}

void heap_init(void* start, void* end, void* max) {
	kheap = heap_make(start, end, max, true, true);
}

void* kmalloc(size_t sz) {
	if (kheap != NULL) {
		return heap_alloc(kheap, sz);
	}

	uint32_t tmp = heap_bot;
	heap_bot += sz;
	return (void*)tmp;
}

// 4096b alignment
void* kmalloc_a(size_t sz) {
	heap_bot &= 0xFFFFF000;
	heap_bot += 4096;

	return kmalloc(sz);
}
