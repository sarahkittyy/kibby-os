#include <std/io.h>
#include <std/string.h>

#include "bio.h"
#include "internal/gdt.h"

void setup_serial() {
}

void kernel_main() {
	// disable NMI
	outb(0x70, inb(0x70) | 0x80);

	// instantly set up the gdt
	// NOTE: this line caused me so much pain
	setup_gdt();

	// com port setup
	setup_serial();

	// NOTE: GRUB auto-enables a20 line. be sure to enable it around here if you need to not use grub for some reason
	puts("booted, protected mode enabled!");
}
