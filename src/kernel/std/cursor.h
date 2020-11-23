#pragma once

#include "type.h"

void enable_cursor();
void disable_cursor();
void move_cursor(uint8_t row, uint8_t col);

void get_cursor_pos(uint8_t* x, uint8_t* y);
