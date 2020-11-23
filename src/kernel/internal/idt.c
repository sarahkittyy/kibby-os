#include "idt.h"

#include <kernel/std/io.h>
#include <kernel/std/string.h>

#include "../bio.h"

volatile idt_entry_t idt_entries[256];
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

static const int PIC_MASTER_COM	 = 0x20;
static const int PIC_MASTER_DATA = 0x21;
static const int PIC_SLAVE_COM	 = 0xA0;
static const int PIC_SLAVE_DATA	 = 0xA1;
static const int PIC_EOI		 = 0x20;
static const int IRQ_START		 = 32;

// move the master pic's vector offset to 32 and the slave pic's to 40
void program_pic() {
	// initialize command (now they wait for 3 additional bytes on the data port)
	outb(PIC_MASTER_COM, 0x11);
	outb(PIC_SLAVE_COM, 0x11);

	// expects offset -> how it is wired -> env info
	outb(PIC_MASTER_DATA, IRQ_START);
	outb(PIC_SLAVE_DATA, IRQ_START + 8);

	outb(PIC_MASTER_DATA, 0x4);	  // b100 tells it that IRQ2 is for the slave pic
	outb(PIC_SLAVE_DATA, 0x2);	  // slave pic cascades towards irq2
	// extra
	outb(PIC_MASTER_DATA, 0x01);   // MCS-80/85 mode
	outb(PIC_SLAVE_DATA, 0x01);

	outb(PIC_MASTER_DATA, 0x00);
	outb(PIC_SLAVE_DATA, 0x00);
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

	uint32_t isr_addrs[48];
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
	isr_addrs[16] = (uint32_t)isr_16;
	isr_addrs[17] = (uint32_t)isr_17;
	isr_addrs[18] = (uint32_t)isr_18;
	isr_addrs[19] = (uint32_t)isr_19;
	isr_addrs[20] = (uint32_t)isr_20;
	isr_addrs[21] = (uint32_t)isr_21;
	isr_addrs[22] = (uint32_t)isr_22;
	isr_addrs[23] = (uint32_t)isr_23;
	isr_addrs[24] = (uint32_t)isr_24;
	isr_addrs[25] = (uint32_t)isr_25;
	isr_addrs[26] = (uint32_t)isr_26;
	isr_addrs[27] = (uint32_t)isr_27;
	isr_addrs[28] = (uint32_t)isr_28;
	isr_addrs[29] = (uint32_t)isr_29;
	isr_addrs[30] = (uint32_t)isr_30;
	isr_addrs[31] = (uint32_t)isr_31;
	isr_addrs[32] = (uint32_t)isr_32;
	isr_addrs[33] = (uint32_t)isr_33;
	isr_addrs[34] = (uint32_t)isr_34;
	isr_addrs[35] = (uint32_t)isr_35;
	isr_addrs[36] = (uint32_t)isr_36;
	isr_addrs[37] = (uint32_t)isr_37;
	isr_addrs[38] = (uint32_t)isr_38;
	isr_addrs[39] = (uint32_t)isr_39;
	isr_addrs[40] = (uint32_t)isr_40;
	isr_addrs[41] = (uint32_t)isr_41;
	isr_addrs[42] = (uint32_t)isr_42;
	isr_addrs[43] = (uint32_t)isr_43;
	isr_addrs[44] = (uint32_t)isr_44;
	isr_addrs[45] = (uint32_t)isr_45;
	isr_addrs[46] = (uint32_t)isr_46;
	isr_addrs[47] = (uint32_t)isr_47;

	for (size_t i = 0; i < 47; ++i) {
		// 0x8E = kernel level
		// 0x08 = code data segment
		idt_entries[i] = mk_idt_entry(isr_addrs[i], 0x08, 0x8E);
	}

	program_pic();

	flush_idt();
}

static const char* error_strings[] = {
	"DIVIDE BY ZERO",
	"DEBUG",
	"NMI",
	"BREAKPOINT",
	"OVERFLOW",
	"BOUND RANGE EXCEEDED",
	"INVALID OPCODE",
	"DEVICE NOT AVAILABLE",
	"DOUBLE FAULT",
	"COPROCESSOR SEGMENT OVERRUN",
	"INVALID TSS",
	"SEGMENT NOT PRESENT",
	"STACK-SEGMENT FAULT",
	"GENERAL PROTECTION FAULT",
	"PAGE FAULT",
	"RESERVED",
	"x87 FPE",
	"ALIGNMENT CHECK",
	"MACHINE CHECK",
	"SIMD FPE",
	"VIRTUALIZATION EXCEPTION",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"SECURITY EXCEPTION",
	"RESERVED",
	"TRIPLE FAULT",
	"FPU ERROR INTERRUPT",
};

static void (*handlers[16])(void) = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

void irq_add_callback(uint32_t irq, void (*cb)(void)) {
	handlers[irq] = cb;
}

void irq_rm_callback(uint32_t irq) {
	handlers[irq] = NULL;
}

void interrupt_handler(regs_t* s) {
	// if the interrupt is a system interrupt, handle it here
	if (s->id < 32) {
		kprintf("INTERRUPT %d (code %x): %s\n", s->id, s->code, error_strings[s->id]);
		kprintf("eax: %x\nebx: %x\necx: %x\nedx: %x\n", s->eax, s->ebx, s->ecx, s->edx);
		for (;;)
			;
		return;
	} else if (s->id >= 32 && s->id < 48) {
		// interrupts 32-39 are from PIC1, 40-47 are PIC2
		uint32_t irq = s->id - 32;	 // 0-15
		// irq 0 is called a lot so we print debug info when it's *not* irq 0
		/*if (irq != 0)*/
			/*cprintf(COM1, "Got IRQ %x\n", irq);*/

		// if the handler exists for the irq, run it.
		if (handlers[irq]) {
			handlers[irq]();
		} else {
			// kbd IRQ requires reading a byte regardless so we have a fallback
			if (irq == 1) {
				inb(0x60);
			}
		}
		pic_eoi(irq);
		return;
	} else {
		// for higher level interrupts
	}
}

void pic_eoi(uint32_t interrupt) {
	if (interrupt >= 8) {
		outb(PIC_SLAVE_COM, PIC_EOI);
	}
	outb(PIC_MASTER_COM, PIC_EOI);
}
