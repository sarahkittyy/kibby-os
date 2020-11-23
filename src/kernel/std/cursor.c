#include "cursor.h"

#include "kernel/bio.h"

static const uint16_t CURSOR_COM_PORT = 0x3D4;
static const uint16_t CURSOR_DAT_PORT = 0x3D5;

static const uint8_t VGA_WIDTH = 80;

void enable_cursor() {
	outb(CURSOR_COM_PORT, 0x0A);
	outb(CURSOR_DAT_PORT, (inb(CURSOR_DAT_PORT) & 0xC0) | 0);

	outb(CURSOR_COM_PORT, 0x0B);
	outb(CURSOR_DAT_PORT, (inb(CURSOR_DAT_PORT) & 0xE0) | 15);
}

void disable_cursor() {
	outb(CURSOR_COM_PORT, 0x0A);
	outb(CURSOR_DAT_PORT, 0x20);
}

void move_cursor(uint8_t row, uint8_t col) {
	uint16_t pos = col * VGA_WIDTH + row;
	
	outb(CURSOR_COM_PORT, 0x0F);
	outb(CURSOR_DAT_PORT, (uint8_t)(pos & 0xFF));

	outb(CURSOR_COM_PORT, 0x0E);
	outb(CURSOR_DAT_PORT, (uint8_t)((pos >> 8) & 0xFF));
}

void get_cursor_pos(uint8_t* x, uint8_t* y) {
	uint16_t pos = 0;
	outb(CURSOR_COM_PORT, 0x0F);
	pos |= inb(CURSOR_DAT_PORT);
	outb(CURSOR_COM_PORT, 0x0E);
	pos |= ((uint16_t)inb(CURSOR_DAT_PORT)) << 8;

	if (x) {
		*x = pos % VGA_WIDTH;
	}
	if (y) {
		*y = pos / VGA_WIDTH;
	}
}
