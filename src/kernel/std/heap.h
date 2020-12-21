#pragma once

/*
	= heap implementation =
	referenced from:
	http://www.jamesmolloy.co.uk/tutorial_html/7.-The%20Heap.html
	- ordered list of blocks & holes
	- blocks are contiguous used memory
	- holes are free memory
	- initialize with one big hole
*/

#include <kernel/internal/kmem.h>
#include <kernel/std/ordered_list.h>
#include <kernel/std/type.h>

typedef struct {
	uint32_t magic;	  // HEAP_MAGIC for error checking
	bool is_hole;	  // 0 if block, 1 if hole
	uint32_t size;	  // size of header + footer + available
} header_t;

typedef struct {
	uint32_t magic;	  // HEAP_MAGIC for error checking
	header_t* hdr;	  // pointer to the relevant header
} footer_t;

typedef struct {
	ordered_list_t data;   // for storing the blocks & holes
	void* start_addr;	   // heap start address
	void* end_addr;		   // heap end address, can be expanded
	void* max_addr;		   // the max address
	// page opts
	bool kernel;
	bool writeable;
} heap_t;

// start is where the heap should be placed
// end is where the heap initially ends
// max is the maximum size of the heap
heap_t* heap_make(void* start, void* end, void* max, bool kernel, bool writeable);

// allocation functions
void* heap_alloc(heap_t* h, size_t sz);
void heap_free(heap_t* h, void* addr);
