#include "paging.h"

#include <std/assert.h>
#include <std/string.h>
#include <std/io.h>

#define KERNEL_V_ADDR 0xC0000000

uint32_t page_dir[1024] __attribute__((aligned(4096)));
uint32_t page_table_identity[1024] __attribute__((aligned(4096)));

void setup_paging() {
	memset(page_dir, 0, 1024 * sizeof(uint32_t));
	memset(page_table_identity, 0, 1024 * sizeof(uint32_t));

	// higher half kernel directory mappings
	page_dir[0] = ((uint32_t)page_table_identity - KERNEL_V_ADDR) | 0x03;
	page_dir[768] = ((uint32_t)page_table_identity - KERNEL_V_ADDR) | 0x03;

	//// we map the last entry in the page directory to the page directory itself.
	//page_dir[1023] = (uint32_t)page_dir | 0x03;

	for(size_t i = 0; i < 1024; ++i) {
		page_table_identity[i] = (i * 0x1000) | 0x03;
	}

	flush_paging();
}
