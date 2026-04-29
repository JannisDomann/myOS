#include "kernel.h"

void kernel_main() {
	k_clear_screen();

	// inits
	gdt_init();
	pmm_init();
	vmm_init();    
	idt_init();
	keyboard_init();
	ata_init();
	sti();
	fat32_init();
    k_printf("Interrupts enabled.\n");


	halt();
}
