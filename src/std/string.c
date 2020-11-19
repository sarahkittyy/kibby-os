#include "std/string.h"

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

void strrev(char* str) {
	size_t slen = strlen(str);
	for (size_t i = 0; i < slen / 2; ++i) {
		char tmp		  = str[i];
		str[i]			  = str[slen - i - 1];
		str[slen - i - 1] = tmp;
	}
}

void itoa(char* dest, int x, int base) {
	const char* ntod = "0123456789ABCDEF";

	char* orig = dest;
	do {
		int digit = x % base;
		x /= base;
		*dest = ntod[digit];
		dest++;
	} while (x != 0);
	*dest = '\0';
	strrev(orig);
}

size_t sprintf(char* out, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	size_t res = sprintfv(out, fmt, args);
	va_end(args);
	return res;
}

size_t sprintfv(char* out, const char* fmt, va_list args) {
	size_t slen = strlen(fmt);

	size_t ct = 0;

	for (size_t i = 0; i < slen;) {
		char ch = fmt[i];
		if (ch == '%') {
			if (i + 1 >= slen) { break; }

			i++;
			char mod = fmt[i];
			i++;
			switch (mod) {
			default:
				break;
			case '%':
				*out = '%';
				out++;
				break;
			case 'd':
				itoa(out, va_arg(args, int), 10);
				out += strlen(out);
				break;
			case 'x':
				itoa(out, va_arg(args, int), 16);
				out += strlen(out);
				break;
			case 's':
				strcpy(out, va_arg(args, const char*));
				out += strlen(out);
				break;
			}
			ct++;
		} else {
			*out = ch;
			out++;
			i++;
		}
	}
	*out = '\0';

	return ct;
}

void* memset(void* data, uint8_t b, size_t n) {
	for(size_t i = 0; i < n; ++i) {
		*((uint8_t*)data) = b;
	}
	return data;
}
