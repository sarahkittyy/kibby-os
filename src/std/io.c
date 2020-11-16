#include <std/io.h>
#include <std/string.h>
#include <stdarg.h>

#include "../kernel/bio.h"

uint8_t mkcolorattr(color_t fg, color_t bg) {
	uint8_t res = 0;
	res |= fg;
	res |= bg << 4;
	return res;
}

static int cursor_x = 0;
static int cursor_y = 0;

void kputchar(char s) {
	volatile uint8_t* fb = (uint8_t*)0xB8000;
	uint8_t color		 = mkcolorattr(WHITE, BLACK);

	int cpos = cursor_x + cursor_y * 80;

	if (s == '\n') {
		cursor_x = 0;
		cursor_y++;
	} else {
		fb[cpos * 2]	 = s;
		fb[cpos * 2 + 1] = color;
		cursor_x++;
		if (cursor_x > 80) {
			cursor_x = 0;
			cursor_y++;
		}
	}
}

void kputs(const char* str) {
	size_t slen = strlen(str);

	for (size_t i = 0; i < slen; ++i) {
		kputchar(str[i]);
	}
	kputchar('\n');
}

bool sisempty(serial_t port) {
	return inb(port + 5) & (0x1 << 5);
}
void sputchar(serial_t port, char c) {
	while (!sisempty(port)) {}
	outb(port, c);
}
void sputs(serial_t port, const char* str) {
	size_t slen = strlen(str);

	for (size_t i = 0; i < slen; ++i) {
		sputchar(port, str[i]);
	}
	sputchar(port, '\n');
}

bool sisready(serial_t port) {
	return inb(port + 5) & 1;
}
size_t sgets(serial_t port, char* dest) {
	size_t tot = 0;
	char recvd;
	do {
		while (!sisready(port)) {}
		recvd	  = inb(port);
		dest[tot] = recvd;
		tot++;
	} while (recvd != '\0');

	// minus one for the null byte
	return tot - 1;
}
