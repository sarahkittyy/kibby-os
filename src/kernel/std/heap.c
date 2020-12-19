#include "heap.h"

#include <kernel/std/assert.h>

#define HEAP_DATA_SIZE 0x20000
#define HEAP_MAGIC 0xBACADACA

static bool header_lt_pred(void* a, void* b) {
	header_t* ha = (header_t*)a;
	header_t* hb = (header_t*)b;

	return ha->size < hb->size;
}

heap_t* heap_make(void* start, void* end, void* max, bool kernel, bool writeable) {
	heap_t* h = (heap_t*)kmalloc_old(sizeof(heap_t));

	// heap is page aligned
	assert((uint32_t)start % 4096 == 0);
	assert((uint32_t)end % 4096 == 0);
	assert((uint32_t)max % 4096 == 0);

	// create the data for the heap block table
	h->data = ol_place(start, HEAP_DATA_SIZE, header_lt_pred);

	// move the start since the first HEAP_DATA_SIZE bytes are for the block table
	start = (void*)((uint32_t)start + (sizeof(void*) * HEAP_DATA_SIZE));

	// page align start
	if ((uint32_t)start % 4096 != 0) {
		start = (void*)(((uint32_t)start & 0xFFFFF000) + 4096);
	}

	h->start_addr = start;
	h->end_addr	  = end;
	h->max_addr	  = max;
	h->kernel	  = kernel;
	h->writeable  = writeable;

	// create one, giant hole :3
	header_t* f = (header_t*)start;
	f->is_hole	= true;
	f->magic	= HEAP_MAGIC;
	f->size		= (uint32_t)end - (uint32_t)start;

	ol_insert(&h->data, (void*)f);

	return h;
}

static size_t find_smallest_hole(heap_t* h, size_t sz) {
	return 0;
}

void* kmalloc(size_t sz) {
	return (void*)0;
}

void kfree(void* addr) {
}
