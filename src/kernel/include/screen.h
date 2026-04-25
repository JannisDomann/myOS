#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// VGA Control Register Ports
#define VGA_CTRL_ADDR_PORT     0x3D4
#define VGA_CTRL_DATA_PORT     0x3D5

// VGA Register Indices
#define VGA_REG_CURSOR_HIGH    0x0E  // Index for high byte of cursor position
#define VGA_REG_CURSOR_LOW     0x0F  // Index for low byte of cursor position

// Screen Dimensions
#define VGA_MAX_COLS           80

void k_clear_screen();
void k_scroll();
void k_update_cursor(uint8_t row, uint8_t col);
void k_print(char chr, uint8_t attrib);
void k_print_s(const char* txt, uint8_t attrib);

extern void outb_asm(uint16_t port, uint8_t val);

#ifdef __cplusplus
}
#endif
