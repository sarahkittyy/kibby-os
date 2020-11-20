#include "paging.h"

#include <std/assert.h>
#include <std/string.h>
#include <std/io.h>

uint32_t page_dir[1024] __attribute__((aligned(4096)));

void setup_paging() {
	// zero out the page directory
	for(volatile size_t i = 0; i < 1024; ++i) {
		// rw, not present, kernel-only
		page_dir[i] = 0;
	}

	// first entry is 0
	page_dir[0] = 0x83;
	page_dir[768] = 0x83;

	flush_paging();
}
