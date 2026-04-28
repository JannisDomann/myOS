#include "pmm.h"
#include "mem.h"
#include "types.h"

#include "debug.h"

static uint8_t* bitmap = (uint8_t*)BITMAP_ADDR;
static uint64_t total_bitmap_blocks;

void pmm_init() {
    uint16_t entry_count = *(uint16_t*)MEM_MAP_ADDR;
    e820_entry_t* map = (e820_entry_t*)(MEM_MAP_ADDR + 0x04);
    uint64_t max_addr = 0x00;

    // gather available RAM
    for (uint16_t i=0; i<entry_count; ++i) {
        if (map[i].type == 1) {
            uint64_t end_of_region = map[i].base_addr + map[i].length;
            if (end_of_region > max_addr) {
                max_addr = end_of_region;
            }
        }
    }

    // total blocks
    total_bitmap_blocks = (max_addr / PAGE_SIZE) / 0x40;

    // set complete RAM as locked (1)
    k_memset_u64(bitmap, 0xFFFFFFFFFFFFFFFF, total_bitmap_blocks);

    // open range from available RAM BIOS call
    for (uint16_t i=0; i<entry_count; ++i) {
        if (map[i].type == 1) {
            pmm_free_range(map[i].base_addr, map[i].length);
        }
    }

    // lock first 1MB for bootloader
    pmm_lock_range(0x00, 0x100000);

    // lock further 4MB for kernel
    pmm_lock_range(0x100000, 0x500000);

    k_printf("Physical memory management is active with %dMB of RAM.\n", total_bitmap_blocks * 0x40 * 0x1000 / (0x400 * 0x400));
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
    uint64_t end = (base + length + (PAGE_SIZE - 0x01)) & ~(PAGE_SIZE - 0x01);

    for (uint64_t addr=start; addr<end; addr+=PAGE_SIZE) {
        pmm_lock_frame(addr);
    }
}

void* pmm_alloc_frame() {
    uint64_t* bitmap64 = (uint64_t*)bitmap;
    for (uint64_t i=0x00; i<total_bitmap_blocks; ++i) {
        if (bitmap64[i] != 0xFFFFFFFFFFFFFFFF) {
            for (uint8_t j=0x00; j<0x40; ++j) {
                if (!(bitmap64[i] & (1ULL << j))) {
                    uint64_t addr = (i * 0x40 + j) * PAGE_SIZE;
                    pmm_lock_frame(addr);
                    return (void*)addr;
                }
            }
        }
    }
    return NULL;
}

void* pmm_alloc_frames(uint64_t count) {
    if (count == 0) {
        return NULL;
    }

    uint64_t total_frames = total_bitmap_blocks * 64;
    uint64_t* bitmap64 = (uint64_t*)bitmap;

    // We search for a sequence of 'count' free bits
    for (uint64_t i=0; i<total_frames; ++i) {
        
        // Fast skip: if the 64-bit block is all ones, jump 64 frames
        if (bitmap64[i / 0x40] == 0xFFFFFFFFFFFFFFFF) {
            i += (0x3F - (i % 0x40)); // Align to next block
            continue;
        }

        // Potential start found, now check the next 'count' bits
        bool sequence_found = true;
        for (uint64_t j=0; j<count; ++j) {
            uint64_t current_frame = i + j;
            
            // Check if we hit the end of memory or if bit is set (locked)
            if (current_frame >= total_frames || 
                (bitmap64[current_frame / 0x40] & (1ULL << (current_frame % 0x40)))) {
                sequence_found = false;
                break;
            }
        }

        if (sequence_found) {
            uint64_t addr = i * PAGE_SIZE;
            // Lock all frames in the found sequence
            pmm_lock_range(addr, count*PAGE_SIZE);
            return (void*)addr;
        }
    }

    return NULL; // No contiguous block of 'count' frames found
}
