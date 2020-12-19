#include "io.h"

#include <stdarg.h>

#include "cursor.h"
#include "kernel/bio.h"
#include "string.h"

uint8_t mkcolorattr(color_t fg, color_t bg) {
	uint8_t res = 0;
	res |= fg;
	res |= bg << 4;
	return res;
}

static int cursor_x = 0;
static int cursor_y = 0;

static uint8_t* fb = (uint8_t*)0xC00B8000;

void clear_screen() {
	// clear the screen
	memset(fb, 0, VGA_WIDTH * 25 * 2);
	cursor_x = 0;
	cursor_y = 0;
	move_cursor(cursor_x, cursor_y);
}

void scroll_screen(int rows) {
	// screen contents
	uint16_t arr[VGA_WIDTH * 25];
	// copy all contents to the array
	memcpy(arr, fb, VGA_WIDTH * 25 * sizeof(uint16_t));
	// save the cursor position
	int saved_cx = cursor_x, saved_cy = cursor_y;
	// clear the frame buffer
	clear_screen();
	// copy the old screencontents back, scrolled up by `rows` rows.
	memcpy(fb, arr + (rows * VGA_WIDTH), sizeof(arr) - (2 * rows * VGA_WIDTH));
	// restore the cursor
	cursor_x = saved_cx;
	cursor_y = saved_cy - 1;
	move_cursor(cursor_x, cursor_y);
}

void kputchar(char s) {
	kputcharc(s, mkcolorattr(WHITE, BLACK));
}

void kputcharc(char s, uint8_t color) {
	int cpos = cursor_x + cursor_y * VGA_WIDTH;

	if (s == '\n') {
		cursor_x = 0;
		cursor_y++;
	} else {
		fb[cpos * 2]	 = s;
		fb[cpos * 2 + 1] = color;
		cursor_x++;
		if (cursor_x > VGA_WIDTH) {
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

	if (cursor_y > 22) {
		scroll_screen(cursor_y - 22);
	}
}

void kputsr(const char* str) {
	size_t slen = strlen(str);

	color_t cols[6] = { LIGHT_RED, YELLOW, LIGHT_GREEN, LIGHT_BLUE, BLUE, LIGHT_PURPLE };

	for (size_t i = 0; i < slen; ++i) {
		kputcharc(str[i], cols[i % (sizeof(cols) / sizeof(cols[0]))]);
	}

	move_cursor(cursor_x, cursor_y);

	if (cursor_y > 22) {
		scroll_screen(cursor_y - 22);
	}
}

void kprintf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char res[1024];
	sprintfv(res, fmt, args);
	va_end(args);

	kputs(res);
}

void kprintfr(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char res[1024];
	sprintfv(res, fmt, args);
	va_end(args);

	kputsr(res);
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
