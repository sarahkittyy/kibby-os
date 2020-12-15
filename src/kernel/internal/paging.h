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

void setup_paging();
extern void flush_paging(uint32_t page_dir);
extern void invalidate_page(uint32_t page);

void set_page_dir();
void bind_page_dir(page_dir_t page_dir);
page_dir_t get_page_dir();

// get the page given a virtual address
page_t* get_page(void* vaddr);

// map a physical address to a virtual address (4kb)
void map_addr(void* physaddr, void* vaddr, bool kernel, bool writeable);

// get the physical address given the virtual address
void* get_phys_addr(void* vaddr);
