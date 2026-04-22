#include "kernel.h"

void kClearScreen() {
	char* videoBufferStart = (char*)0xb8000;
	char* videoBuffer = videoBufferStart;
	char cols = 0x50;	// 80 cols
	char rows = 0x19;	// 25 rows
	char bg = 0x00;
	char fg = 0x07;

	while (videoBuffer < (char*)videoBufferStart+cols*rows*0x02) {
		*videoBuffer++ = 0x20; // space
		*videoBuffer++ = (bg << 0x04) & 0xf0 | fg & 0x0f;
	}
}

void kPrintf(char* txt, char row, char attrib) {
	char* videoBufferStart = (char*)0xb8000;
	char cols = 0x50;	// 80 cols
	char* videoBuffer = (char*)videoBufferStart+row*cols*0x02;

	while (*txt != 0) {
		if (*txt == '\n') {
			++row;
			++txt;
			videoBuffer = (char*)videoBufferStart+row*cols*0x02;
			continue;
		}
		*videoBuffer++ = (char)*txt;
		*videoBuffer++ = attrib;
		++txt;
	}
}

void kUpdateCursor(char row, char col) {
	unsigned short pos = row*0x50+col;

	outb_asm(0x3d4, 0x0e);
	outb_asm(0x3d5, (char)((pos>>0x08)&0xff));
	outb_asm(0x3d4, 0x0f);
	outb_asm(0x3d5, (char)(pos&0xff));
}

void kernel_main() {
	kClearScreen();
	kPrintf("Hello World!\nHow are you?", 0x00, 0x02);
	kUpdateCursor(0x02, 0x00);

    while (1) {
        __asm("hlt");
    }
}
