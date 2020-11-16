#pragma once

#include <std/type.h>

typedef struct idt_entry {
	uint16_t offset_lo;
	uint16_t selector;	 // gdt code segment selector
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_hi;
} __attribute__((packed)) idt_entry_t;

typedef struct idt {
	uint16_t sz;
	uint32_t addr;
} __attribute__((packed)) idt_t;

// this is all stolen from here http://www.osdever.net/bkerndev/Docs/isrs.htm
// i don't know how these arguments are pushed onto the stack in asm
typedef struct interrupt_state {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;   // from 'pushad'
	uint32_t interrupt, code;
	uint32_t eip, cs, eflags, useresp, ss;	 // pushed automagically
} __attribute__((packed)) interrupt_state_t;

idt_entry_t mk_idt_entry(uint32_t offset, uint16_t selector, uint8_t type);

void setup_idt();

void interrupt_handler(interrupt_state_t* state);

extern void flush_idt();
extern void isr_0();
extern void isr_1();
extern void isr_2();
extern void isr_3();
extern void isr_4();
extern void isr_5();
extern void isr_6();
extern void isr_7();
extern void isr_8();
extern void isr_9();
extern void isr_10();
extern void isr_11();
extern void isr_12();
extern void isr_13();
extern void isr_14();
extern void isr_15();

void pic_eoi();
