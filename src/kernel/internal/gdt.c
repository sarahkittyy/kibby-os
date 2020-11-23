#include "gdt.h"

#include "kernel/std/assert.h"
#include "kernel/std/string.h"
#include "tss.h"

gdt_t kibby_gdt[6];
gdt_table_t kibby_gdt_table;

tss_t tss;

// https://wiki.osdev.org/LGDT
void gdt_set_gate_l(uint32_t id, uint32_t base, uint32_t lim, uint8_t acc, uint8_t gran) {
	kibby_gdt[id].sl.base_lo  = (base & 0xFFFF);
	kibby_gdt[id].sl.base_mid = (base >> 16) & 0xFF;
	kibby_gdt[id].sl.base_hi  = (base >> 24) & 0xFF;

	kibby_gdt[id].sl.limit_lo = (lim & 0xFFFF);
	kibby_gdt[id].sl.gran	  = (lim >> 16) & 0x0F;

	kibby_gdt[id].sl.gran |= gran & 0xF0;
	kibby_gdt[id].sl.access = acc;
}

void gdt_set_gate_s(uint32_t id, gdt_t entry) {
	kibby_gdt[id] = entry;
}

void setup_gdt() {
	kibby_gdt_table.sz	 = (sizeof(gdt_t) * 6) - 1;
	kibby_gdt_table.addr = (uint32_t)&kibby_gdt;

	gdt_set_gate_l(0, 0, 0, 0, 0);					// null
	gdt_set_gate_l(1, 0, 0xFFFFFFFF, 0x9A, 0xC0);	// code
	gdt_set_gate_l(2, 0, 0xFFFFFFFF, 0x92, 0xC0);	// data

	// user code
	gdt_t uc;
	memset(&uc, 0, sizeof(gdt_t));
	uc.ss.limit_lo = 0xFFFF;
	uc.ss.limit_hi = 0xF;
	uc.ss.base_lo  = 0;
	uc.ss.base_hi  = 0;

	uc.ss.rw   = 1;
	uc.ss.ex   = 1;
	uc.ss.s	   = 1;
	uc.ss.priv = 3;	  // ring 3
	uc.ss.p	   = 1;
	uc.ss.gran = 1;
	uc.ss.sz   = 1;
	gdt_set_gate_s(3, uc);

	// user data
	gdt_t ud = uc;
	ud.ss.ex = 0;	// not executable
	gdt_set_gate_s(4, ud);

	// tss
	uint32_t tss_base = (uint32_t)&tss;
	uint32_t tss_limit = sizeof(tss_t);
	// present, executable, accessed, byte granularity :D
	gdt_set_gate_l(5, tss_base, tss_base + tss_limit, 0x89, 0x00);

	gdt_flush();
	setup_tss(0x10, 0x0);
}

void setup_tss(uint16_t ss0, uint32_t esp0) {
	assert(sizeof(tss_t) == 104);

	memset(&tss, 0, sizeof(tss));
	tss.ss0	 = ss0;	  // kernel data descriptor
	tss.esp0 = esp0;

	// code segment, with the last two bits set, (to allow access from ring 3)
	tss.cs = 0x08 | 0x03;
	// data segment
	tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x10 | 0x03;

	flush_tss();
}
