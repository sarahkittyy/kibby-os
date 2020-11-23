#include "paging.h"

#include <std/assert.h>
#include <std/string.h>
#include <std/io.h>

uint32_t page_dir[1024] __attribute__((aligned(4096)));

void setup_paging() {
	memset(page_dir, 0, 1024 * sizeof(uint32_t));

	// first entry is 0
	page_dir[0] = 0x83;
	page_dir[768] = 0x83;

	flush_paging();
}
