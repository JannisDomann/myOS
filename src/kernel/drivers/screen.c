#include "../include/screen.h"
#include "../include/memory.h"

void k_clear_screen() {
	char* videoBufferStart = (char*)0xb8000;
	char* videoBuffer = videoBufferStart;
	char cols = 80;
	char rows = 25;
	char bg = 0x00;
	char fg = 0x07;

	while (videoBuffer < (char*)videoBufferStart+cols*rows*0x02) {
		*videoBuffer++ = 0x20; // space
		*videoBuffer++ = ((bg << 0x04) & 0xf0) | fg & 0x0f;
	}
}

void k_scroll() {
	volatile uint16_t* videoBufferStart = (volatile uint16_t*)0xb8000;
	const uint8_t MAX_ROWS = 25;
	const uint8_t MAX_COLS = 80;

	k_memcpy(videoBufferStart, videoBufferStart + MAX_COLS, (MAX_ROWS - 1) * MAX_COLS * sizeof(uint16_t) / sizeof(uint64_t));
	k_memset(videoBufferStart + (MAX_ROWS - 1) * MAX_COLS, 0x00, MAX_COLS * sizeof(uint16_t) / sizeof(uint64_t));
}

void k_update_cursor(uint8_t row, uint8_t col) {
    // Calculate the 1D position on the screen
    uint16_t pos = row * VGA_MAX_COLS + col;

    // Send the high byte (bits 8-15)
    outb_asm(VGA_CTRL_ADDR_PORT, VGA_REG_CURSOR_HIGH);
    outb_asm(VGA_CTRL_DATA_PORT, (uint8_t)((pos >> 8) & 0xFF));

    // Send the low byte (bits 0-7)
    outb_asm(VGA_CTRL_ADDR_PORT, VGA_REG_CURSOR_LOW);
    outb_asm(VGA_CTRL_DATA_PORT, (uint8_t)(pos & 0xFF));
}


void k_print(char chr, uint8_t attrib) {
	volatile uint16_t* videoBufferStart = (volatile uint16_t*)0xb8000;
	const uint8_t MAX_ROWS = 25;
	const uint8_t MAX_COLS = 80;

	static uint8_t row = 0;
	static uint8_t col = 0;
	

	if (chr == '\n') {
		++row;
		col = 0;
	}
	else {
		videoBufferStart[row * MAX_COLS + col] = (uint16_t)(chr | attrib << 8);
        ++col;
	}

	if (col >= MAX_COLS) {
		col = 0;
		++row;
	}

	if (row >= MAX_ROWS) {
		k_scroll();
		--row;
	}
	
	k_update_cursor(row, col);
}

void k_print_s(const char* txt, uint8_t attrib) {
	while (*txt != '\0') {
		k_print(*txt++, attrib);
	}
}
