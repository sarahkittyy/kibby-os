#pragma once

#include <std/type.h>

/// operations to write / read bytes from io devices
extern void outb(uint32_t addr, uint8_t byte);
extern uint8_t inb(uint32_t addr);
