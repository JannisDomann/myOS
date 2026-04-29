#pragma once

#include "idt.h"

#ifdef __cplusplus
extern "C" {
#endif

// Ports
#define PORT_KEYBD      0x60

// defines
#define SHIFT_LEFT      0x2A
#define SHIFT_RIGHT     0x36
#define CAPS_LOCK       0x3A

// Interrupt
#define INT_KEYBD       0x21  

void keyboard_init();
void keyboard_handler(interrupt_registers_t* regs);

#ifdef __cplusplus
}
#endif