#include "bio.h"
#include "internal/gdt.h"

void kernel_main() {
	// disable NMI
	outb(0x70, inb(0x70) | 0x80);

	// instantly set up the gdt
	//NOTE: this line caused me so much pain
	setup_gdt();

	// NOTE: GRUB auto-enables a20 line. be sure to enable it around here if you need to not use grub for some reason

	// for debugging
	uint8_t col			 = 0x0F;
	volatile uint8_t* fb = (uint8_t*)0xB8000;

	fb[0] = 'd';
	fb[1] = col;
}
