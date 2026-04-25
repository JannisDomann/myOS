#pragma once

#include "screen.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PS_NORMAL,
    PS_FORMAT
} PRINT_STATE;


void k_int2str(int value, char* str);
void k_hex2str(uint64_t value, char* str);
void k_printf(const char* format, ...);

#ifdef __cplusplus
}
#endif
