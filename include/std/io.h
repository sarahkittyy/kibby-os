#pragma once

#include <std/type.h>

typedef enum COLOR {
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	PURPLE,
	BROWN,
	GRAY,
	DARK_GRAY,
	LIGHT_BLUE,
	LIGHT_GREEN,
	LIGHT_CYAN,
	LIGHT_RED,
	LIGHT_PURPLE,
	YELLOW,
	WHITE
} color_t;

uint8_t mkcolorattr(color_t fg, color_t bg);

typedef enum SERIAL {
	COM1 = 0x3F8,
	COM2 = 0x2F8,
	COM3 = 0x3E8,
	COM4 = 0x2E8
} serial_t;

void puts(const char* str);
