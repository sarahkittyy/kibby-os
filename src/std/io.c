#include <std/io.h>
#include <std/string.h>
#include <stdarg.h>

uint8_t mkcolorattr(color_t fg, color_t bg) {
	uint8_t res = 0;
	res |= fg;
	res |= bg << 4;
	return res;
}

void puts(const char* str) {
	uint8_t col			 = 0x0F;
	volatile uint8_t* fb = (uint8_t*)0xB8000;

	size_t slen	  = strlen(str);
	uint8_t color = mkcolorattr(WHITE, BLACK);

	for (size_t i = 0; i < slen; ++i) {
		fb[0] = str[i];
		fb[1] = color;
		fb += 2;
	}
}
