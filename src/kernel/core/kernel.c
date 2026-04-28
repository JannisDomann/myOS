#include "kernel.h"

void kernel_main() {
	k_clear_screen();

	// inits
	idt_init();
	keyboard_init();
	ata_init();
	sti();
	fat32_init();
    k_printf("Interrupts enabled.\n");
	pmm_init();
    
	
	halt();
}
