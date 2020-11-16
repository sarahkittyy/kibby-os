#include "idt.h"

#include <std/io.h>

#include "../bio.h"

idt_entry_t idt_entries[256];
idt_t idt;

idt_entry_t mk_idt_entry(uint32_t offset, uint16_t selector, uint8_t type) {
	idt_entry_t f;
	f.offset_lo = offset & 0x0000FFFF;
	f.offset_hi = (offset >> 16) & 0x0000FFFF;
	f.type_attr = type;
	f.selector	= selector;
	f.zero		= 0;
	return f;
}

void setup_idt() {
	idt.sz	 = (sizeof(struct idt_entry) * 256) - 1;
	idt.addr = (uint32_t)idt_entries;

	// zero out idts
	for (int i = 0; i < 256; ++i) {
		idt_entries[i].offset_lo = 0;
		idt_entries[i].offset_hi = 0;
		idt_entries[i].zero		 = 0;
		idt_entries[i].selector	 = 0;
		idt_entries[i].type_attr = 0;
	}

	uint32_t isr_addrs[16];
	isr_addrs[0]  = (uint32_t)isr_0;
	isr_addrs[1]  = (uint32_t)isr_1;
	isr_addrs[2]  = (uint32_t)isr_2;
	isr_addrs[3]  = (uint32_t)isr_3;
	isr_addrs[4]  = (uint32_t)isr_4;
	isr_addrs[5]  = (uint32_t)isr_5;
	isr_addrs[6]  = (uint32_t)isr_6;
	isr_addrs[7]  = (uint32_t)isr_7;
	isr_addrs[8]  = (uint32_t)isr_8;
	isr_addrs[9]  = (uint32_t)isr_9;
	isr_addrs[10] = (uint32_t)isr_10;
	isr_addrs[11] = (uint32_t)isr_11;
	isr_addrs[12] = (uint32_t)isr_12;
	isr_addrs[13] = (uint32_t)isr_13;
	isr_addrs[14] = (uint32_t)isr_14;
	isr_addrs[15] = (uint32_t)isr_15;

	for (size_t i = 0; i < 16; ++i) {
		// 0x8E = kernel level
		// 0x08 = code data segment
		idt_entries[i] = mk_idt_entry(isr_addrs[i], 0x08, 0x8E);
	}

	sputs(COM1, "loading the idt.");
	flush_idt();
	sputs(COM1, "loaded the idt.");
}

void interrupt_handler(interrupt_state_t* state) {
	sputs(COM1, "interrupt called");
	for (;;)
		;
	pic_eoi(state->interrupt);
}

void pic_eoi(uint32_t interrupt) {
	if (interrupt >= 8)
		outb(0xA0, 0x20);
	outb(0x20, 0x20);
}
