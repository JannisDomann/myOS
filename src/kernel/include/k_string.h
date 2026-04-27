#pragma once

#include "screen.h"
#include <stdint.h>

#define hex_prefix  0x7830

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PS_NORMAL,
    PS_FORMAT,
    PS_COLOR,
} PRINT_STATE;


void k_int2str(int value, char* str);
void k_hex2str(uint64_t value, char* str);
void k_printf(const char* format, ...);

#ifdef __cplusplus
}
#endif
