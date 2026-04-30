#pragma once

/* Central definition of the memory layout may be changed */
#define KERNEL_FILE_NAME    "KERNEL  SYS"
#define KERNEL_PHYS_BASE    0x200000
#define KERNEL_VIRT_BASE    0xFFFFFFFF80000000
#define PHYS_MAP_OFFSET     0xFFFF800000000000


/* Helper */
#define ALIGN_4K(addr) (((addr) + 0xFFF) & ~0xFFF)
#define ALIGN_DOWN_4K(addr) ((addr) & ~0xFFF)
#define MAX(a,b) ((a) > (b) ? (a) : (b))

/* Minimal address spaces may NOT be changed */
#define VGA_BUFFER_ADDR     (0xB8000 + PHYS_MAP_OFFSET)
#define BOOTLOADER_END      0x9000
#define KERNEL_PHYS_ALIGNED ALIGN_4K(KERNEL_PHYS_BASE)
#define KERNEL_PHYS_MIN     MAX(KERNEL_PHYS_ALIGNED, 0x1FF000) 
#define STAGE2_PML4_ADDR    MAX(KERNEL_PHYS_MIN - 0x1FF000, BOOTLOADER_END + 0x1000) 
#define STAGE2_PDPT_ADDR    MAX(KERNEL_PHYS_MIN - 0x1FE000, BOOTLOADER_END + 0x2000) 
#define STAGE2_PDPT_K_ADDR  MAX(KERNEL_PHYS_MIN - 0x1FD000, BOOTLOADER_END + 0x3000) 
#define STAGE2_PD_ADDR      MAX(KERNEL_PHYS_MIN - 0x1FC000, BOOTLOADER_END + 0x4000) 
#define MEM_MAP_ADDR        (STAGE2_PD_ADDR + 0x2000)
#define BITMAP_PHYS_ADDR    (MEM_MAP_ADDR + 0x2000)
#define KERNEL_BITMAP       (BITMAP_PHYS_ADDR + PHYS_MAP_OFFSET)
