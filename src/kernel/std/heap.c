#include "heap.h"

#include <kernel/internal/paging.h>
#include <kernel/std/assert.h>

// how much is dedicated to the heap block & hole table
#define HEAP_DATA_SIZE 0x20000
// magic number for magical things
#define HEAP_MAGIC 0xBACADACA
// the minimum size of the heap
#define HEAP_MIN_SIZE 0x40000

static bool header_lt_pred(void* a, void* b) {
	header_t* ha = (header_t*)a;
	header_t* hb = (header_t*)b;

	return ha->size < hb->size;
}

heap_t* heap_make(void* start, void* end, void* max, bool kernel, bool writeable) {
	heap_t* h = (heap_t*)kmalloc(sizeof(heap_t));

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

static size_t heap_smallest_hole(heap_t* h, size_t sz) {
	// for every hole/block
	for (size_t i = 0; i < h->data.size; ++i) {
		header_t* hdr = (header_t*)ol_get(&h->data, i);
		// the blocks are sorted in ascending size so we just grab the first fitting block
		if (hdr->size >= sz) {
			return i;
		}
	}
	// nothing was found.
	return SIZE_MAX;
}

static void heap_expand(heap_t* h, size_t new_sz) {
	// the new size has to be bigger lmao
	assert(new_sz > ((uint32_t)h->end_addr - (uint32_t)h->start_addr));

	// get the closest page boundary
	if (new_sz % 4096 != 0) {
		new_sz &= 0xFFFFF000;
		new_sz += 4096;
	}

	// there's a max size so we make sure we don't go past that
	assert((uint32_t)h->start_addr + new_sz <= (uint32_t)h->max_addr);

	size_t old_size = (uint32_t)h->end_addr - (uint32_t)h->start_addr;
	// for every page between the end of the heap and the new size
	for (size_t i = old_size; i < new_sz; i += 4096) {
		// allocate it
		alloc_page_exists(
			get_page((void*)((uint32_t)h->start_addr + i), 1), h->kernel, h->writeable);
	}

	h->end_addr = (void*)((uint32_t)h->start_addr + new_sz);
}

static void heap_contract(heap_t* h, size_t new_sz) {
	// heap's gotta be smoller
	assert(new_sz < ((uint32_t)h->end_addr - (uint32_t)h->start_addr));

	// closest page boundary
	if (new_sz % 4096 != 0) {
		new_sz &= 0xFFFFF000;
		new_sz += 4096;
	}

	new_sz = new_sz < HEAP_MIN_SIZE ? HEAP_MIN_SIZE : new_sz;

	size_t old_sz = (uint32_t)h->end_addr - (uint32_t)h->start_addr;
	for (size_t i = old_sz; i < new_sz; i -= 4096) {
		// free all the pages that are unnecessary now
		free_page(get_page((page_t*)((uint32_t)h->start_addr + i), 0));
	}
	h->end_addr = (void*)((uint32_t)h->start_addr + new_sz);
}

void* heap_alloc(heap_t* h, size_t sz) {
	// size + header & footer :3
	size_t nsz = sz + sizeof(header_t) + sizeof(footer_t);

	size_t hole_i = heap_smallest_hole(h, nsz);
	if (hole_i == SIZE_MAX) {
		// no hole was found, so we expand the heap and then recursively call heap_alloc
		uint32_t old_len =
			(uint32_t)h->end_addr - (uint32_t)h->start_addr;
		uint32_t old_end_addr =
			(uint32_t)h->end_addr;
		heap_expand(h, old_len + nsz);
		uint32_t new_len =
			(uint32_t)h->end_addr - (uint32_t)h->start_addr;

		// find the endmost header in the index
		// furthest header storage
		int32_t fhdr_i	= -1;
		uint32_t fhdr_v = 0x0;
		for (size_t i = 0; i < h->data.size; ++i) {
			header_t* hdr = (header_t*)ol_get(&h->data, i);
			if ((uint32_t)hdr > fhdr_v) {
				fhdr_v = (uint32_t)hdr;
				fhdr_i = i;
			}
		}

		if (fhdr_i == -1) {	  // if no headers are found at all
			// create one
			header_t* hdr = (header_t*)old_end_addr;
			hdr->magic	  = HEAP_MAGIC;
			hdr->size	  = new_len - old_len;
			hdr->is_hole  = 1;

			footer_t* ftr =
				(footer_t*)(old_end_addr + hdr->size - sizeof(footer_t));
			ftr->magic = HEAP_MAGIC;
			ftr->hdr   = hdr;

			ol_insert(&h->data, (void*)hdr);
		} else {   // update the last header otherwise
			header_t* hdr = (header_t*)ol_get(&h->data, fhdr_i);
			hdr->size += new_len - old_len;
			// remake the footer
			footer_t* ftr =
				(footer_t*)((uint32_t)hdr + hdr->size - sizeof(footer_t));
			ftr->magic = HEAP_MAGIC;
			ftr->hdr   = hdr;
		}

		// there's enough size now, recurse
		return heap_alloc(h, sz);
	}

	// the header of the hole to use
	header_t* orig_hole	  = (header_t*)ol_get(&h->data, hole_i);
	uint32_t orig_hole_sz = orig_hole->size;

	/*
		do we split the hole into two parts (i.e. is the hole too big?)
		because if the leftover space is less than the overhead of header+footer
		,,we should just increase the allocated size
	*/
	if (orig_hole->size - nsz < sizeof(header_t) + sizeof(footer_t)) {
		// there's not enough to split into a block and a hole so we just make the whole thing a block
		sz += orig_hole->size - nsz;
		nsz = orig_hole->size;
	}

	// remove the hole now cuz it's not a hole anyamore
	ol_remove(&h->data, hole_i);

	header_t* new_hdr = orig_hole;
	new_hdr->magic	  = HEAP_MAGIC;
	new_hdr->is_hole  = 0;
	new_hdr->size	  = nsz;
	// associated footer
	footer_t* new_ftr =
		(footer_t*)((uint32_t)orig_hole + sizeof(header_t) + sz);
	new_ftr->magic = HEAP_MAGIC;
	new_ftr->hdr   = new_hdr;

	// do we need to write a hole after the allocated block?
	// cuz there might be space left over :3
	if (orig_hole_sz - nsz > 0) {
		// make a new hole
		header_t* hole_hdr =
			(header_t*)((uint32_t)orig_hole + sizeof(header_t) + sz + sizeof(footer_t));
		hole_hdr->magic	  = HEAP_MAGIC;
		hole_hdr->is_hole = 1;
		hole_hdr->size	  = orig_hole_sz - nsz;
		// get the footer
		footer_t* hole_ftr =
			(footer_t*)((uint32_t)hole_hdr + orig_hole_sz - nsz - sizeof(footer_t));
		// ignore if it's the end addr? TODO why?
		if ((uint32_t)hole_hdr < (uint32_t)h->end_addr) {
			hole_ftr->magic = HEAP_MAGIC;
			hole_ftr->hdr	= hole_hdr;
		}

		// back into the block/heap table it goes
		ol_insert(&h->data, (void*)hole_hdr);
	}

	return (void*)((uint32_t)new_hdr + sizeof(header_t));
}

void heap_free(heap_t* h, void* addr) {
}
