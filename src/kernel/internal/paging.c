#include "paging.h"

#include <kernel/internal/kmem.h>
#include <kernel/std/assert.h>
#include <kernel/std/bitset.h>
#include <kernel/std/io.h>
#include <kernel/std/string.h>

#define KERNEL_V_ADDR 0xC0000000

static page_dir_t cur_page_dir;
static bitset_t* page_bitset;

void setup_paging() {
	assert(sizeof(page_t) == 4);
	assert(sizeof(page_table_t) == 4);

	// available vmem goes from 0-this
	size_t mem_end = 0x8FFFFFFF;
	// each page is 4kb, so this is how many pages we have.
	page_bitset = bitset_create(mem_end / 0x1000);

	// create the main page directory, and bind it.
	page_dir_t page_dir = (page_table_t*)kmalloc_a(1024 * sizeof(page_table_t));
	bind_page_dir(page_dir);
	memset(page_dir, 0, 1024 * sizeof(page_table_t));

	// map 4 MB of the kernel to the higher half
	for (size_t i = 0; i < 1024; ++i) {
		map_addr((void*)(0x00000000 + i * 0x1000),
				 (void*)(0xC0000000 + i * 0x1000), true, true);
	}

	// we map the last entry in the page directory to the page directory itself.
	map_addr((void*)((uint32_t)page_dir - KERNEL_V_ADDR), (void*)0xFFFFF000, true, true);

	// flush
	set_page_dir();
}

void set_page_dir() {
	flush_paging((uint32_t)get_page_dir());
}

void bind_page_dir(page_dir_t page_dir) {
	cur_page_dir = page_dir;
}

page_dir_t get_page_dir() {
	// we always map the last entries to the table itself
	return cur_page_dir;
}

uint32_t addr_shift(uint32_t addr) {
	return addr / 0x1000;
}
page_t* get_page(void* vaddr) {
	// each page maps 4 KiB
	// each table maps 4 MiB
	// the whole dir maps 4 GiB

	// vaddr is in [0, 0xFFFFFFFF] (0-4 GiB)

	// find the table we're in
	uint32_t pti = (uint32_t)vaddr / (4 * 1024 * 1024);
	// find the page we're in
	uint32_t pi = ((uint32_t)vaddr / (4 * 1024)) % 1024;

	// get the directory
	page_dir_t dir = get_page_dir();
	// get the table in that directory
	page_table_t tbl = dir[pti];

	// assert the table is present.
	if (!tbl.s.p) {
		return NULL;
	}

	// get all the pages in that table
	page_t* p = (page_t*)((uint32_t)tbl.b & (~0xFFF));
	p		  = (page_t*)((uint32_t)p + KERNEL_V_ADDR);
	// check if the page needed is present
	if (!p[pi].s.p) {
		return NULL;
	}
	// return the page
	return p + pi;
}

page_t* map_addr(void* physaddr, void* vaddr, bool kernel, bool writeable) {
	assert((uint32_t)vaddr % 4096 == 0);

	// table the vaddr is in
	uint32_t pti = (uint32_t)vaddr / (4 * 1024 * 1024);
	// page in the table that the vaddr is in
	uint32_t pi = ((uint32_t)vaddr / (4 * 1024)) % 1024;

	page_dir_t dir = get_page_dir();

	// does the page table exist?
	if (!dir[pti].s.p) {
		// if not, create it
		dir[pti].s.p   = 1;
		dir[pti].s.rw  = 1;
		dir[pti].s.sup = 1;

		page_t* new_table = (page_t*)kmalloc_a(1024 * sizeof(page_t));
		memset(new_table, 0, 1024 * sizeof(page_t));
		dir[pti].s.addr = addr_shift((uint32_t)new_table - KERNEL_V_ADDR);
	}

	// get the page table
	page_t* tbl = (page_t*)((uint32_t)dir[pti].b & ~(0xFFF));
	tbl			= (page_t*)((uint32_t)tbl + KERNEL_V_ADDR);

	// assert the page doesn't exist
	assert(!tbl[pi].s.p);

	// grab the page
	page_t* p = tbl + pi;
	// set values
	p->s.p	  = 1;
	p->s.rw	  = writeable;
	p->s.sup  = !kernel;
	p->s.addr = addr_shift((uint32_t)physaddr);

	// set the bitset flag to indicate this page is used.
	bitset_set(page_bitset, pti * 1024 + pi);

	invalidate_page(pti);

	return p;
}

page_t* alloc_page(bool kernel, bool writeable) {
	// get the first available frame.
	size_t idx = bitset_first_unset(page_bitset);
	assert(idx != SIZE_MAX);

	// so now we know that we want the 4kb zone @ i*0x1000 to be mapped
	// and the kernel is physically in the lower half, so
	// we're gonna move all mappings to the upper half :3
	uint32_t physaddr = KERNEL_V_ADDR + idx * 0x1000;
	// and then the virtual address is gonna be in the lower half just fine
	uint32_t vaddr = idx * 0x1000;
	// and THEN we map_addr
	return map_addr((void*)physaddr, (void*)vaddr, kernel, writeable);
}

void free_page(page_t* page) {
	// get the frame this page is in.
	uint32_t physaddr = page->b & (~0xFFF);
	physaddr -= KERNEL_V_ADDR;
	size_t frame = physaddr / 0x1000;

	// clear the bit
	bitset_clear(page_bitset, frame);

	// mark the page as not present
	page->b = 0;
}

void* get_phys_addr(void* vaddr) {
	// find the page handing this virtual address
	page_t* p = get_page(vaddr);
	if (p == NULL) {
		return NULL;
	}
	// get the address the page points to
	uint32_t addr = (uint32_t)p & (~0xFFF);
	// add the bottom 4096 of the vaddr to get the paddr
	addr += (uint32_t)vaddr & 0xFFF;

	return (void*)addr;
}
