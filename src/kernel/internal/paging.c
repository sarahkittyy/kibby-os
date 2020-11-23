#include "paging.h"

#include <kernel/std/assert.h>
#include <kernel/std/string.h>
#include <kernel/std/io.h>

#define KERNEL_V_ADDR 0xC0000000

page_dir_t page_dir[1024] __attribute__((aligned(4096)));
page_t page_table_identity[1024] __attribute__((aligned(4096)));

void setup_paging() {
	assert(sizeof(page_dir_t) == 4);
	assert(sizeof(page_t) == 4);

	memset(page_dir, 0, 1024 * sizeof(uint32_t));
	memset(page_table_identity, 0, 1024 * sizeof(uint32_t));

	// higher half kernel directory mappings
	page_dir[0].s.p = 1;
	page_dir[0].s.rw = 1;
	page_dir[0].s.addr = addr_shift((uint32_t)page_table_identity - KERNEL_V_ADDR);
	page_dir[768].s.p = 1;
	page_dir[768].s.rw = 1;
	page_dir[768].s.addr = addr_shift((uint32_t)page_table_identity - KERNEL_V_ADDR);

	//// we map the last entry in the page directory to the page directory itself.
	page_dir[1023].s.p = 1;
	page_dir[1023].s.rw = 1;
	page_dir[1023].s.addr = addr_shift((uint32_t)page_dir - KERNEL_V_ADDR);

	for(size_t i = 0; i < 1024; ++i) {
		page_table_identity[i].s.p = 1; 
		page_table_identity[i].s.rw = 1; 
		page_table_identity[i].s.addr = addr_shift(i * 0x1000);
	}

	flush_paging();
}

uint32_t addr_shift(uint32_t addr) {
	return addr/0x1000;
}


