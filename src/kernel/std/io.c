#include "io.h"

#include "string.h"
#include "cursor.h"
#include <stdarg.h>

#include "kernel/bio.h"

uint8_t mkcolorattr(color_t fg, color_t bg) {
	uint8_t res = 0;
	res |= fg;
	res |= bg << 4;
	return res;
}

static int cursor_x = 0;
static int cursor_y = 0;

void clear_screen() {
	char arr[80 * 25 + 1];
	memset(arr, ' ', sizeof(arr) - 1);
	arr[80 * 25] = '\0';

	kputs(arr);

	cursor_x = 0;
	cursor_y = 0;

	move_cursor(cursor_x, cursor_y);
}

void kputchar(char s) {
	volatile uint8_t* fb = (uint8_t*)0xC00B8000;
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

	move_cursor(cursor_x, cursor_y);
}

void kprintf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char res[1024];
	sprintfv(res, fmt, args);
	va_end(args);

	kputs(res);
}

bool cisempty(serial_t port) {
	return inb(port + 5) & (0x1 << 5);
}
void cputchar(serial_t port, char c) {
	while (!cisempty(port)) {}
	outb(port, c);
}
void cputs(serial_t port, const char* str) {
	size_t slen = strlen(str);

	for (size_t i = 0; i < slen; ++i) {
		cputchar(port, str[i]);
	}
}

void cprintf(serial_t port, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char res[1024];
	sprintfv(res, fmt, args);
	va_end(args);

	cputs(port, res);
}

bool cisready(serial_t port) {
	return inb(port + 5) & 1;
}
size_t cgets(serial_t port, char* dest) {
	size_t tot = 0;
	char recvd;
	do {
		while (!cisready(port)) {}
		recvd	  = inb(port);
		dest[tot] = recvd;
		tot++;
	} while (recvd != '\0');

	// minus one for the null byte
	return tot - 1;
}
