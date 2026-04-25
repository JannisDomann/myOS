#include "../include/k_string.h"

#include <stdarg.h>

void k_int2str(int value, char* str) {
	if (value < 0) {
		*str++ = '-';
		value = -value;
	}

	char buffer[32] = {0};
	char* p_buffer = &buffer[31];
	
	do {
		*(--p_buffer) = (value % 10)+'0';
		value /= 10;
	} while (value>0);
	
	while (*p_buffer != '\0') {
		*str++ = *p_buffer++;
	}
}

void k_hex2str(uint64_t value, char* str) {
	char buffer[32] = {0};
	char* p_buffer = &buffer[31];
	char* hex_chars = "0123456789ABCDEF";

	do {
		*(--p_buffer) = hex_chars[value % 16];
		value /= 16;
	} while (value>0);
	
	while (*p_buffer != '\0') {
		*(str++) = *(p_buffer++);
	}
}

void k_printf(const char* format, ...) {
	PRINT_STATE state = PS_NORMAL;
	uint8_t attrib = 0x02;

	va_list args;
	va_start(args, format);
	
	for (uint32_t i=0; format[i] != '\0'; ++i) {
		if (format[i] == '%') {
			if (format[i+1] == '\0') {
				k_print(format[i], attrib);
				break;
			}

			state = PS_FORMAT;
			++i;
			if (format[i] == '%') {
				state = PS_NORMAL;
			}
		}
		else {
			state = PS_NORMAL;

		}
		

		if (state == PS_FORMAT) {
			switch (format[i]) {
				case 'd': {
					int value = va_arg(args, int);
					char num[32] = {0};
					k_int2str(value, num);
					k_print_s(num, attrib);
					break;
				}
				case 'x': {
					uint64_t value = va_arg(args, uint64_t);
					char num[32] = {0};
					k_hex2str(value, num);
					k_print_s(num, attrib);
					break;
				}
				case 's': {
					char* value = va_arg(args, char*);
					k_print_s(value, attrib);
					break;
				}
			}
		}
		else {
			k_print(format[i], attrib);
		}
	}

	va_end(args);
}
