#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void kClearScreen();
void kPrintf(char* txt, char row, char attrib);
void kUpdateCursor(char row, char col);

extern void outb_asm(uint16_t port, uint8_t val);

#ifdef __cplusplus
}
#endif
