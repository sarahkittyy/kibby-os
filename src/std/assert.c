#include "std/assert.h"

#include <std/io.h>

void m_assert(bool res, const char* expr_str) {
	if (!res) {
		kprintf("assertion fail: %s\n", expr_str);
		for(;;);
	}
}
