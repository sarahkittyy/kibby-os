#include "paging.h"

#include <kernel/internal/kmem.h>
#include <kernel/std/assert.h>
#include <kernel/std/bitset.h>
#include <kernel/std/io.h>
#include <kernel/std/string.h>

#define KERNEL_V_ADDR 0xC0000000
// place it at kernel + 4 mb (past the standard kernel mapping)
#define KERNEL_HEAP_ADDR (0xC0000000 + (4 * 1024 * 1024))
// make it big ig
#define KERNEL_HEAP_ISIZE 0x100000

static page_dir_t cur_page_dir;
static bitset_t* page_bitset;

void setup_paging() {
	assert(sizeof(page_t) == 4);
	assert(sizeof(page_table_t) == 4);

	// available vmem goes from 0-this
	size_t mem_end = 0xBFFFF000;
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
		// example of code that does the same as ^^
		/*alloc_page_exists(get_page((void*)(0xC0000000 + i * 0x1000), true), true, true);*/
	}

	// we map the last entry in the page directory to the page directory itself.
	map_addr((void*)((uint32_t)page_dir - KERNEL_V_ADDR), (void*)0xFFFFF000, true, true);

	// place the heap at KERNEL_V_ADDR + 4MB
	// and allocate space for 0x20000 void*s
	// the heap's initial size all needs to be paged :3
	// it expands itself
	for (size_t i = 0; i < (KERNEL_HEAP_ISIZE / 0x1000); ++i) {
		map_addr(
			(void*)(KERNEL_HEAP_ADDR - KERNEL_V_ADDR + (i * 0x1000)),
			(void*)(KERNEL_HEAP_ADDR + (i * 0x1000)),
			true,
			true);
	}

	// flush
	set_page_dir();

	// set up the heap
	heap_init((void*)KERNEL_HEAP_ADDR, (void*)(KERNEL_HEAP_ADDR + KERNEL_HEAP_ISIZE), (void*)0xFFFFE000);
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
page_t* get_page(void* vaddr, bool make) {
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
	page_table_t* tbl = &dir[pti];

	// assert the table is present.
	if (!tbl->s.p) {
		if (make) {
			// make the table
			tbl->s.p   = 1;
			tbl->s.rw  = 1;
			tbl->s.sup = 1;

			page_t* tbl_data = (page_t*)kmalloc_a(1024 * sizeof(page_t));
			memset(tbl_data, 0, 1024 * sizeof(page_t));
			tbl->s.addr = addr_shift((uint32_t)tbl_data - KERNEL_V_ADDR);
		} else {
			return NULL;
		}
	}

	// get all the pages in that table
	page_t* p = (page_t*)((uint32_t)tbl->b & (~0xFFF));
	p		  = (page_t*)((uint32_t)p + KERNEL_V_ADDR);
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
	/*
		i think this works(?)
		for example, when we map the kernel to the higher half
		we are using the pages for the lower half of memory
		and so pti and pi are accurate because that lower half
		of physmem is allocated i think :3
	*/
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

void alloc_page_exists(page_t* p, bool kernel, bool writeable) {
	/*	so we have a page at a certain position already
		meaning that the virtual address will be the same regardless

		this won't intersect with the bitset because the bitset
		marks free physical addresses :3
	*/

	// find the first free physical address in 0-BFFFF000
	size_t idx = bitset_first_unset(page_bitset);
	assert(idx != SIZE_MAX);

	// set that as stolen
	bitset_set(page_bitset, idx);

	// then we're gonna put that physaddr into the page
	p->s.addr = addr_shift(idx * 0x1000);
	// and set all the other data
	p->s.p	 = 1;
	p->s.rw	 = writeable;
	p->s.sup = !kernel;

	// and then that's it i think
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
	page_t* p = get_page(vaddr, false);
	if (p == NULL) {
		return NULL;
	}
	// get the address the page points to
	uint32_t addr = (uint32_t)p & (~0xFFF);
	// add the bottom 4096 of the vaddr to get the paddr
	addr += (uint32_t)vaddr & 0xFFF;

	return (void*)addr;
}
