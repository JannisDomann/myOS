#include "kernel.h"


void kernel_main() {
	k_clear_screen();

	// inits
	idt_init();
	keyboard_init();
	ata_init();
	sti();
    k_printf("Interrupts enabled. Keyboard and ATA drivers are active.\n");
    
	uint64_t total_blocks = pmm_init();
    k_printf("Physical memory management is active with %dMB of RAM.\n", total_blocks * 0x40 * 0x1000 / (0x400 * 0x400));

	halt();
}
