#include "gdt.h"

#include "kernel/std/assert.h"

gdt_t kibby_gdt[3];
gdt_table_t kibby_gdt_table;

// https://wiki.osdev.org/LGDT
void gdt_set_gate(uint32_t id, uint32_t base, uint32_t lim, uint8_t acc, uint8_t gran) {
	kibby_gdt[id].sl.base_lo  = (base & 0xFFFF);
	kibby_gdt[id].sl.base_mid = (base >> 16) & 0xFF;
	kibby_gdt[id].sl.base_hi  = (base >> 24) & 0xFF;

	kibby_gdt[id].sl.limit_lo = (lim & 0xFFFF);
	kibby_gdt[id].sl.gran	   = (lim >> 16) & 0x0F;

	kibby_gdt[id].sl.gran |= gran & 0xF0;
	kibby_gdt[id].sl.access = acc;
}

void setup_gdt() {
	assert(sizeof(gdt_t) == 8);

	kibby_gdt_table.sz	 = (sizeof(gdt_t) * 3) - 1;
	kibby_gdt_table.addr = (uint32_t)&kibby_gdt;

	gdt_set_gate(0, 0, 0, 0, 0);				  // null
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);	  // code
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);	  // data

	gdt_flush();
}
