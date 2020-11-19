#include "paging.h"

#include <std/assert.h>
#include <std/string.h>
#include <std/io.h>

uint32_t page_dir[1024] __attribute__((aligned(4096)));
uint32_t page_table1[1024] __attribute__((aligned(4096)));

void setup_paging() {
	// zero out the page directory
	for(volatile size_t i = 0; i < 1024; ++i) {
		// rw, not present, kernel-only
		page_dir[i] = 0;
	}

	// identity page the entire thing
	for(volatile size_t i = 0; i < 1024; ++i) {
		page_table1[i] = (4096 * i) | 0x3;
	}

	page_dir[0] = ((uint32_t)page_table1) | 3;

	flush_paging();
}
