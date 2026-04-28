#include "screen.h"
#include "mem.h"
#include "io.h"
#include "lib.h"

void k_clear_screen() {
	k_memset_u16(VGA_BUFFER, BLANK, (VGA_MAX_ROWS * VGA_MAX_COLS));
}

void k_scroll() {
	k_memcpy_u16(VGA_BUFFER, VGA_BUFFER + VGA_MAX_COLS, (VGA_MAX_ROWS - 1) * VGA_MAX_COLS);
	k_memset_u16(VGA_BUFFER + (VGA_MAX_ROWS - 1) * VGA_MAX_COLS, BLANK, VGA_MAX_COLS);
}

void k_update_cursor(uint8_t row, uint8_t col) {
    // Calculate the 1D position on the screen
    uint16_t pos = row * VGA_MAX_COLS + col;

    // Send the high byte (bits 8-15)
    k_outb(VGA_CTRL_ADDR_PORT, VGA_REG_CURSOR_HIGH);
    k_outb(VGA_CTRL_DATA_PORT, (uint8_t)((pos >> 8) & 0xFF));

    // Send the low byte (bits 0-7)
    k_outb(VGA_CTRL_ADDR_PORT, VGA_REG_CURSOR_LOW);
    k_outb(VGA_CTRL_DATA_PORT, (uint8_t)(pos & 0xFF));
}

void k_print(char chr, uint8_t attrib) {
	uint16_t* videoBuffer = VGA_BUFFER;
	static uint8_t row = 0;
	static uint8_t col = 0;
	
	if (chr == '\n') {
		++row;
		col = 0;
	}
	else if (chr == '\t') {
		uint8_t new_col = min(col+4, VGA_MAX_COLS);
		k_memset_u16(VGA_BUFFER + row * VGA_MAX_COLS + col, BLANK, new_col);
		col = new_col;
	}
	else if (chr == '\b') {
		if (col > 0) {
			--col;
		} else if (row > 0) {
			--row;
			col = VGA_MAX_COLS - 1;

			while (col > 0 && (videoBuffer[row * VGA_MAX_COLS + col] & 0xFF) == SPACE) {
				--col;
			}
		}

		k_memset_u16(VGA_BUFFER + row * VGA_MAX_COLS + col, BLANK, 0x01);
	}
	else {
		videoBuffer[row * VGA_MAX_COLS + col] = (uint16_t)(chr | attrib << 8);
        ++col;
	}

	if (col >= VGA_MAX_COLS) {
		col = 0;
		++row;
	}

	if (row >= VGA_MAX_ROWS) {
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
