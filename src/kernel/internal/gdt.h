#pragma once

#include <std/type.h>

// main kernel gdt
void setup_gdt();

// set an entry in the main gdt table
void gdt_set_gate(uint32_t id, uint32_t base, uint32_t lim, uint8_t acc, uint8_t gran);

// gdt entries are 8-byte entries
struct gdt {
	uint16_t limit_lo;

	uint16_t base_lo;
	uint8_t base_mid;

	uint8_t access;

	uint8_t gran;

	uint8_t base_hi;
} __attribute__((packed));
typedef struct gdt gdt_t;

struct gdt_table {
	uint16_t sz;
	uint32_t addr;
} __attribute__((packed));
typedef struct gdt_table gdt_table_t;

// defined in gdt.asm
extern void gdt_flush();
