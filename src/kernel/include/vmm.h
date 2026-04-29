#pragma once

#include "types.h"
#include <stdint.h>
#include <stdbool.h>

/*
    PML4 (Page Map Level 4)
    PDPT (Page Directory Pointer Table)
    PD (Page Directory)
    PT (Page Table)
*/

#ifdef __cplusplus
extern "C" {
#endif

#define PAGE_SIZE       0x1000
#define PHYS_OFFSET     0xFFFF800000000000 // Higher Half Address Space
#define PAGE_FRAME_MASK ~0xFFFULL

// Page Table Entry Flags
#define PTE_PRESENT     (0x1ULL << 0x00)
#define PTE_WRITABLE    (0x1ULL << 0x01)
#define PTE_USER        (0x1ULL << 0x02)

typedef uint64_t pt_entry;

// Helper to get indices from a virtual address
#define PML4_IDX(addr) (((addr) >> 39) & 0x1FF)
#define PDPT_IDX(addr) (((addr) >> 30) & 0x1FF)
#define PD_IDX(addr)   (((addr) >> 21) & 0x1FF)
#define PT_IDX(addr)   (((addr) >> 12) & 0x1FF)

// Functions
void vmm_init();
bool vmm_map(uint64_t virt, uint64_t phys, uint64_t* pml4, uint64_t flags);
bool vmm_unmap(uint64_t virt, pt_entry* pml4);
void* vmm_allocate_kernel_pages(uint64_t count, uint64_t start_vaddr);

#ifdef __cplusplus
}
#endif
