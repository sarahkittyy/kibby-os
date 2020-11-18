#include <std/io.h>
#include <std/string.h>

#include "bio.h"
#include "internal/gdt.h"
#include "internal/idt.h"

void setup_serial() {
	serial_t ports[4] = { COM1, COM2, COM3, COM4 };
	for (int i = 0; i < 4; ++i) {
		serial_t port = ports[i];

		// disable interrupts
		outb(port + 1, 0x00);

		// set highest bit of DLAB for divisor registers
		outb(port + 3, 0x1 << 7);
		// divisor value
		outb(port, 0x3);
		outb(port + 1, 0x0);
		uint8_t lcr = 0;
		// ascii chars only need 7 bits but i'll send 8 to be safe :D
		// 1 stop bit
		// no parity
		lcr = 0x3;
		outb(port + 3, lcr);
		// enable fifo
		outb(port + 2, 0xC7);
		// configure the modem (enable rts and dtr)
		outb(port + 4, 0x3);
	}
}

void on_kbd(void) {
	uint8_t sc = inb(0x60);
}

void kernel_main() {
	// NOTE: GRUB auto-enables a20 line. be sure to enable it around here if you need to not use grub for some reason

	// disable NMI
	outb(0x70, inb(0x70) | 0x80);

	// instantly set up the gdt
	// NOTE: this line caused me so much pain
	setup_gdt();
	// setup the idt
	setup_idt();

	// enable NMI
	outb(0x70, inb(0x70) & 0x7F);

	// com port setup
	setup_serial();

	// booted!
	kprintf("booted, protected mode enabled!\n");
}
