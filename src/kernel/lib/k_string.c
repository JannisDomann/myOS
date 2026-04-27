#include "../include/k_string.h"
#include "../include/memory.h"

#include <stdarg.h>

void k_int2str(int value, char* str) {
	uint32_t val = (uint32_t)value;

	if (value < 0) {
		*str++ = '-';
		val = -value;
	}

	char buffer[32] = {0};
	char* p_buffer = &buffer[31];
	*p_buffer = '\0';
	
	do {
		*(--p_buffer) = (val % 10)+'0';
		val /= 10;
	} while (val>0);
	
	while (*p_buffer != '\0') {
		*str++ = *p_buffer++;
	}

	*str = '\0';
}

void k_hex2str(uint64_t value, char* str) {
	char* hex_chars = "0123456789ABCDEF";
	char buffer[32] = {0};
	char* p_buffer = &buffer[31];
	*p_buffer = '\0';

	do {
		*(--p_buffer) = hex_chars[value % 16];
		value /= 16;
	} while (value>0);
	
	while (*p_buffer != '\0') {
		*(str++) = *(p_buffer++);
	}

	*str = '\0';
}

void k_printf(const char* format, ...) {
	PRINT_STATE state = PS_NORMAL;
	uint8_t attrib = STD_ATTRIBUTE;

	va_list args;
	va_start(args, format);
	
	for (uint32_t i=0; format[i] != '\0'; ++i) {

		if (format[i] == '\x1B') {
			state = PS_COLOR;
			continue;
		}
		else if (format[i] == '\t') {
			k_print_s(TAB, attrib);
			continue;
		}

		if (state == PS_COLOR) {
			if (format[i] == 'R') {
				attrib = STD_ATTRIBUTE;
			} else {
				attrib = (uint8_t)format[i];
			}
			state = PS_NORMAL;
			continue;
		}

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
				case 'p': {
					uint64_t value = va_arg(args, uint64_t);
					char num[32] = {'0'};
					k_memset_u16(num, hex_prefix, 0x01);
					k_hex2str(value, &num[2]);
					k_print_s(num, attrib);
					break;
				}
				case 's': {
					char* value = va_arg(args, char*);
					k_print_s(value, attrib);
					break;
				}
				case 'c': {
					char value = va_arg(args, int);
					k_print(value, attrib);
					break;
				}
				default: {
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
