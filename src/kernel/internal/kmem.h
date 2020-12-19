#pragma once

#include <kernel/std/type.h>

extern uint32_t heap_bot;

// boop up heap_bot to the higher half
void heap_init();

// allocate some bytes
uint32_t kmalloc_old(size_t sz);

// allocate some bytes aligned on a page boundary
uint32_t kmalloc_old_a(size_t sz);
