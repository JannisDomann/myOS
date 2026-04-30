#include "memory_layout.h"
#include "pmm.h"
#include "mem.h"
#include "k_string.h"
#include "types.h"

static uint8_t* bitmap = (uint8_t*)KERNEL_BITMAP;
static uint64_t total_bitmap_blocks;
uint64_t total_mem_bytes;

void pmm_init() {
    // gather kernel end from symbol and calculate virt and phys adresses
    uint64_t k_end_virt = (uint64_t)_kernel_end;
    uint64_t k_end_phys = k_end_virt - KERNEL_VIRT_BASE;

    uint16_t entry_count = *(uint16_t*)MEM_MAP_ADDR;
    // e820_entry_t* map = (e820_entry_t*)((uint8_t*)MEM_MAP_ADDR + 0x04);
    e820_entry_t* map = (e820_entry_t*)(MEM_MAP_ADDR + 0x04);
    uint64_t max_addr = 0x00;

    #ifdef DEBUG
    for (uint16_t i=0x00; i<entry_count; ++i) {
        k_printf("E820 [%d]: Base %x%x, Len %x%x, Type %d\n", 
                i, 
                (uint32_t)(map[i].base_addr >> 0x20), (uint32_t)map[i].base_addr,
                (uint32_t)(map[i].length >> 0x20), (uint32_t)map[i].length,
                map[i].type);
    }
    #endif

    // gather available RAM
    for (uint16_t i=0x00; i<entry_count; ++i) {
        if (map[i].type == 0x01) {
            max_addr = MAX(max_addr, map[i].base_addr + map[i].length);
        }
    }

    // total blocks
    total_bitmap_blocks = (max_addr / PAGE_SIZE) / 0x40;

    // set complete RAM as locked (1)
    k_memset_u64(bitmap, 0xFFFFFFFFFFFFFFFF, total_bitmap_blocks);

    // open range from available RAM BIOS call
    for (uint16_t i=0x0; i<entry_count; ++i) {
        if (map[i].type == 0x01) {
            pmm_free_range(map[i].base_addr, map[i].length);
        }
    }

    // lock first 1MB for bootloader
    pmm_lock_range(0x00, 0x100000);

    // lock further MBs for kernel
    k_printf("k end\t\t:%p\n", _kernel_end);
    k_printf("k end virt:\t%p\n", k_end_virt);
    k_printf("k end ohys:\t%p\n", k_end_phys);
    pmm_lock_range(0x100000, k_end_phys - 0x100000);

    // lock bitmap
    uint64_t bitmap_size_bytes = total_bitmap_blocks * sizeof(uint64_t);
    uint64_t bitmap_phys = (uint64_t)bitmap - PHYS_MAP_OFFSET;
    pmm_lock_range(bitmap_phys, bitmap_size_bytes);

    // calc total mem bytes
    total_mem_bytes = max_addr;

    k_printf("Physical memory management is active with %dMB of RAM.\n", total_mem_bytes / (0x400 * 0x400));
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
    uint64_t start = ALIGN_4K(base); 
    uint64_t end = ALIGN_DOWN_4K(base + length);

    for (uint64_t addr = start; addr < end; addr += PAGE_SIZE) {
        pmm_free_frame(addr);
    }
}

void pmm_lock_range(uint64_t base, uint64_t length) {
    uint64_t start = ALIGN_4K(base);
    uint64_t end = ALIGN_4K(base + length);

    for (uint64_t addr = start; addr < end; addr += PAGE_SIZE) {
        pmm_lock_frame(addr);
    }
}

void* pmm_alloc_frame() {
    uint64_t* bitmap64 = (uint64_t*)bitmap;
    for (uint64_t i=0x00; i<total_bitmap_blocks; ++i) {
        if (bitmap64[i] != 0xFFFFFFFFFFFFFFFF) {
            for (uint8_t j=0x00; j<0x40; ++j) {
                if (!(bitmap64[i] & (0x01ULL << j))) {
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
    if (count == 0x00) {
        return NULL;
    }

    uint64_t total_frames = total_bitmap_blocks * 0x40;
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
                (bitmap64[current_frame / 0x40] & (0x01ULL << (current_frame % 0x40)))) {
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




static bool pmm_is_frame_used(uint64_t frame) {
    return (bitmap[frame / 0x08] & (0x01ULL << (frame % 0x08))) != 0x00;
}
void pmm_dump_occupied_areas() {
    uint64_t total_frames = total_mem_bytes / 0x1000;
    k_printf("Occupied Physical Memory Areas:\n");
    
    uint64_t start_addr = 0;
    bool in_occupied_block = false;

    // Iterate through all frames (total_frames = RAM_SIZE / 4096)
    for (uint64_t frame=0; frame<total_frames; ++frame) {
        bool is_used = pmm_is_frame_used(frame); // Your bitmap check function

        if (is_used && !in_occupied_block) {
            // Found start of a locked area
            start_addr = frame * PAGE_SIZE;
            in_occupied_block = true;
        } else if (!is_used && in_occupied_block) {
            // Found end of a locked area
            uint64_t end_addr = (frame * PAGE_SIZE) - 0x01;
            k_printf("\t%p - %p\n", start_addr, end_addr);
            in_occupied_block = false;
        }
    }

    // Handle case where last frame is occupied
    if (in_occupied_block) {
        k_printf("\t%p - %p\n", start_addr, (total_frames * PAGE_SIZE) - 1);
    }
}