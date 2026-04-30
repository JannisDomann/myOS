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

	uint16_t* p = k_malloc(2);
	*p = 7;
	k_printf("p=%p *p=%d\n", p, *p);

	k_printf("mem_map _addr: %p\n", MEM_MAP_ADDR);
	k_printf("mem_map Kernel_Bitmap: %p\n", KERNEL_BITMAP);
	pmm_dump_occupied_areas();

	
	halt();
}
