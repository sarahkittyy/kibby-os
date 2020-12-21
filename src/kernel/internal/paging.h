#pragma once

#include <kernel/std/type.h>

typedef union {
	struct {
		uint8_t p : 1;		  // present
		uint8_t rw : 1;		  // read-write
		uint8_t sup : 1;	  // user/supervisor
		uint8_t wt : 1;		  // write-through
		uint8_t cd : 1;		  // cache-disabled
		uint8_t accd : 1;	  // accessed
		uint8_t dirty : 2;	  // set if the page has been written to
		uint8_t avail : 4;	  // available for the OS
		uint32_t addr : 20;	  // physical page address
	} __attribute__((packed)) s;
	uint32_t b;
} page_t;

typedef union {
	struct {
		uint8_t p : 1;		  // present
		uint8_t rw : 1;		  // read-write
		uint8_t sup : 1;	  // user/supervisor
		uint8_t wt : 1;		  // write-through
		uint8_t cd : 1;		  // cache-disabled
		uint8_t accd : 2;	  // accessed
		uint8_t size : 1;	  // page size (set for 4MiB)
		uint8_t avail : 4;	  // available for the OS
		uint32_t addr : 20;	  // address of the page table
	} __attribute__((packed)) s;
	uint32_t b;
} page_table_t;

// for readability
typedef page_table_t* page_dir_t;

uint32_t addr_shift(uint32_t addr);

// main paging initialization
void setup_paging();

// move the page dir address given into cr3
extern void flush_paging(uint32_t page_dir);
// invlpg wrapper
extern void invalidate_page(uint32_t page);

// use the bound page directory to flush
void set_page_dir();
// bind a page directory for map_addr() and get_page()
void bind_page_dir(page_dir_t page_dir);
// get the bound page directory
page_dir_t get_page_dir();

// get the page given a virtual address
// if make is true, we create the page table if it doesn't exist
page_t* get_page(void* vaddr, bool make);

// map a physical address to a virtual address (4kb)
// NOTE: when mapping note that the physical address of the kernel is at 0x00000000
page_t* map_addr(void* physaddr, void* vaddr, bool kernel, bool writeable);

// allocate a new 4kb page in the nearest available slot
page_t* alloc_page(bool kernel, bool writeable);
// allocate a page
void alloc_page_exists(page_t* p, bool kernel, bool writeable);

// free the given page from use
void free_page(page_t* page);

// looks up the page table and tries to find the physical address that a virtual address points to.
// NULL if that address isn't present.
void* get_phys_addr(void* vaddr);
