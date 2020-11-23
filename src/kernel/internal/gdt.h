#pragma once

#include <kernel/std/type.h>

// main kernel gdt
void setup_gdt();

// set an entry in the main gdt table
void gdt_set_gate(uint32_t id, uint32_t base, uint32_t lim, uint8_t acc, uint8_t gran);

// gdt entries are 8-byte entries
typedef union {
	struct {
		uint16_t limit_lo;

		uint16_t base_lo;
		uint8_t base_mid;

		uint8_t access;

		uint8_t gran;

		uint8_t base_hi;
	} __attribute__((packed)) sl;
	struct {
		uint16_t limit_lo: 16; // lower 16 bits of limit
		uint32_t base_lo: 24; // lower 24 bits of base

		uint8_t accd: 1; // cpu sets this to one when accessed
		uint8_t rw: 1; // allow read for code selectors, allow write for data selectors
		uint8_t dc: 1; // data: 0 grows up, 1 grows down
		uint8_t ex: 1; // executable bit
		uint8_t s: 1; // set for code/data segments, cleared for sys segs
		uint8_t priv: 2; // ring level 0-3
		uint8_t p: 1; // present bit (must be 1

		uint8_t lim_hi: 4; // high nibble of limit

		uint8_t zero: 2;
		uint8_t gran: 1; // 1 for 4KiB granularity, 0 for byte granularity
		uint8_t sz: 1; // 0 - 16 bit, 1 = 32 bit

		uint8_t base_hi: 8;
	} __attribute__((packed)) ss;
	uint64_t b;
} gdt_t;

struct gdt_table {
	uint16_t sz;
	uint32_t addr;
} __attribute__((packed));
typedef struct gdt_table gdt_table_t;

// defined in gdt.asm
extern void gdt_flush();
