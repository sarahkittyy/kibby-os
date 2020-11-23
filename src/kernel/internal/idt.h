#pragma once

#include <kernel/std/type.h>

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

typedef struct regs {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;	// from 'pusha'
	uint32_t id, code;
	uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) regs_t;

idt_entry_t mk_idt_entry(uint32_t offset, uint16_t selector, uint8_t type);

void setup_idt();

void irq_add_callback(uint32_t irq, void (*cb)(void));
void irq_rm_callback(uint32_t irq);

void interrupt_handler(regs_t* regs);

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
extern void isr_16();
extern void isr_17();
extern void isr_18();
extern void isr_19();
extern void isr_20();
extern void isr_21();
extern void isr_22();
extern void isr_23();
extern void isr_24();
extern void isr_25();
extern void isr_26();
extern void isr_27();
extern void isr_28();
extern void isr_29();
extern void isr_30();
extern void isr_31();
// pic interrupts
extern void isr_32();
extern void isr_33();
extern void isr_34();
extern void isr_35();
extern void isr_36();
extern void isr_37();
extern void isr_38();
extern void isr_39();
extern void isr_40();
extern void isr_41();
extern void isr_42();
extern void isr_43();
extern void isr_44();
extern void isr_45();
extern void isr_46();
extern void isr_47();

void pic_eoi(uint32_t interrupt);
