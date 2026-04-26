#include "../include/keyboard.h"
#include "../include/io.h"
#include "../include/k_string.h"
#include <stdbool.h>

// US Keyboard Layout (Scan Code Set 1)
const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	'9', '0', '-', '=', '\b', 
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 
   'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' ',   0
};

// US Keyboard Layout - Shift Pressed (Scan Code Set 1)
const char kbd_us_upper[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 
  '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',  0, '|', 'Z', 
   'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0, '*',   0, ' ',   0
};


void keyboard_init() {
    // Register the callback for Vector 33 (IRQ 1)
    register_interrupt_handler(INT_KEYBD, keyboard_handler);

    // unmask Keyboard (IRQ1)
    pic_unmask(0x01);
}

void keyboard_handler(interrupt_registers_t* regs) {
    static bool shift_pressed = false;
    static bool caps_lock_pressed = false;

    uint8_t scancode = k_inb(PORT_KEYBD);
    // k_printf("(sc:%d)",scancode);

    if (scancode == SHIFT_LEFT || scancode == SHIFT_RIGHT) {
        shift_pressed = true;
    }
    
    if (scancode == CAPS_LOCK) {
        caps_lock_pressed = !caps_lock_pressed;
    }
    
    // If the 7th bit is set, the key was released
    if (scancode & 0x80) {
        if (scancode == (SHIFT_LEFT | 0x80) || scancode == (SHIFT_RIGHT | 0x80)) {
            shift_pressed = false;
        }
    }  else {
        // Key was pressed
        bool upper = shift_pressed ^ caps_lock_pressed;
        char c = upper ? kbd_us_upper[scancode] : kbd_us[scancode];
        if (c > 0) {
            k_print(c, STD_ATTRIBUTE);
        }
    }
}
