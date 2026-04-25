#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// VGA Control Register Ports
#define VGA_CTRL_ADDR_PORT      0x3D4
#define VGA_CTRL_DATA_PORT      0x3D5

// VGA Register Indices
#define VGA_REG_CURSOR_HIGH     0x0E  // Index for high byte of cursor position
#define VGA_REG_CURSOR_LOW      0x0F  // Index for low byte of cursor position

// Screen Dimensions
#define VGA_MAX_COLS            80
#define VGA_MAX_ROWS            25
#define VGA_BUFFER              (uint16_t*)0xB8000

// Standard Attributes
#define STD_FOREGROUND          0x0F
#define STD_BACKGROUND          0x01
#define STD_ATTRIBUTE           (((STD_BACKGROUND << 0x04) & 0xF0) | STD_FOREGROUND & 0x0F)
#define SPACE                   0x20
#define BLANK                   (((uint16_t)STD_ATTRIBUTE<< 0x08) | (uint16_t)SPACE)

/*
Wert	Farbe (Dunkel)	        Wert	Farbe (Hell)
0x00	Black (Schwarz)	        0x08	Dark Grey (Dunkelgrau)
0x01	Blue (Blau)	            0x09	Light Blue (Hellblau)
0x02	Green (Grün)	        0x0A	Light Green (Hellgrün)
0x03	Cyan (Türkis)	        0x0B	Light Cyan (Helltürkis)
0x04	Red (Rot)	            0x0C	Light Red (Hellrot)
0x05	Magenta (Lila)	        0x0D	Light Magenta (Pink)
0x06	Brown (Braun)	        0x0E	Yellow (Gelb)
0x07	Light Grey (Standard)	0x0F	White (Weiß)
*/

#define CLR_RESET   "\x1BR"
#define CLR_ERROR   "\x1B\x4F"
#define CLR_WARNING "\x1B\x5F"
#define CLR_OK      "\x1B\x2F"

void k_clear_screen();
void k_scroll();
void k_update_cursor(uint8_t row, uint8_t col);
void k_print(char chr, uint8_t attrib);
void k_print_s(const char* txt, uint8_t attrib);

extern void outb_asm(uint16_t port, uint8_t val);

#ifdef __cplusplus
}
#endif
