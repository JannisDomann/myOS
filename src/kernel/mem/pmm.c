#include "../include/pmm.h"
#include "../include/memory.h"

uint8_t* bitmap = (uint8_t*)BITMAP_ADDR;

void pmm_init() {
    uint16_t entry_count = *(uint16_t*)MEM_MAP_ADDR;
    e820_entry_t* map = (e820_entry_t*)(MEM_MAP_ADDR + 0x04);

    // set complete 4GB RAM as locked (1)
    k_memset_64(bitmap, 0xFFFFFFFFFFFFFFFF, 0x10 * 0x400);

    // open range from available RAM BIOS call
    for (uint16_t i=0; i<entry_count; ++i) {
        if (map[i].type == 1) {
            pmm_free_range(map[i].base_addr, map[i].length);
        }
    }
    
    // lock range from 0 to end kernel
    pmm_lock_range(0x00, 0x100000);
    pmm_lock_range(0x100000, 0x1000000);
}

void pmm_free_frame(uint64_t address) {
    uint64_t frame = address / PAGE_SIZE;
    bitmap[frame / 0x08] &= ~(0x01 << frame % 0x08);
}

void pmm_lock_frame(uint64_t address) {
    uint64_t frame = address / PAGE_SIZE;
    bitmap[frame / 0x08] |= (0x01 << frame % 0x08);
}

void pmm_free_range(uint64_t base, uint64_t length) {
    // align to 4KB
    uint64_t start = (base + (PAGE_SIZE - 0x01)) & ~(PAGE_SIZE - 0x01);
    uint64_t end = (base + length) & ~(PAGE_SIZE - 0x01);

    for (uint64_t addr=start; addr<end; addr+=PAGE_SIZE) {
        pmm_free_frame(addr);
    }
}

void pmm_lock_range(uint64_t base, uint64_t length) {
    // align to 4KB
    uint64_t start = (base + (PAGE_SIZE - 0x01)) & ~(PAGE_SIZE - 0x01);
    uint64_t end = (base + length) & ~(PAGE_SIZE - 0x01);

    for (uint64_t addr=start; addr<end; addr+=PAGE_SIZE) {
        pmm_lock_frame(addr);
    }
}
