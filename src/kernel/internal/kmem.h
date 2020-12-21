#pragma once

#include <kernel/std/type.h>

extern uint32_t heap_bot;

// boop up heap_bot to the higher half
void old_heap_init();

// initialize the new heap
void heap_init(void* start, void* end, void* max);

// allocate some bytes
void* kmalloc(size_t sz);

// allocate some bytes aligned on a page boundary
void* kmalloc_a(size_t sz);
