#include "std/string.h"

#include <stdarg.h>

size_t strlen(const char* str) {
	size_t i = 0;
	for (; *str != '\0'; ++i, ++str)
		;
	return i;
}

void strcpy(char* dest, const char* src) {
	while (*src) { *dest++ = *src++; }
}

void strcat(char* dest, const char* src) {
	size_t len = strlen(dest);
	dest += len;
	strcpy(dest, src);
}

int strcmp(const char* a, const char* b) {
	while (*a || *b) {
		if (!*a) {
			return -1;
		} else if (!*b) {
			return 1;
		}
		if (*a > *b) {
			return 1;
		} else if (*a < *b) {
			return -1;
		} else {
			a++;
			b++;
		}
	}
	return 0;
}

size_t sprintf(char* out, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	size_t slen = strlen(fmt);

	size_t ct = 0;

	for (size_t i = 0; i < slen;) {
		char ch = fmt[i];
		if (ch == '%') {
			if (i + 1 >= slen) { return ct; }

			char mod = fmt[i + 1];
			switch (mod) {
			default:
				break;
			case '%':
				*out = '%';
				break;
			}
			ct++;
			out++;
			i++;
		} else {
			*out = ch;
			out++;
			i++;
		}
	}

	va_end(args);

	return ct;
}
