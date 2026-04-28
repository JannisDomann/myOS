#include "debug.h"
#include "k_string.h"
#include "idt.h"

void kernel_panic(const char* file, int line, const char* msg) {
    cli();
    k_printf(CLR_ERROR "\n--- KERNEL PANIC ---\n" CLR_RESET);
    k_printf("\tReason: %s\n", msg);
    k_printf("\tLocation: %s:%d\n", file, line);
    k_printf("\tSystem halted.\n");
    halt();
}
