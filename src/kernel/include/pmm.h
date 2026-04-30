#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PAGE_SIZE       0x1000      // Page Size (4096)

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;                  // 1 = available RAM, else reserved
    uint32_t acpi;                  // ACPI reserviert
} __attribute__((packed)) e820_entry_t;

extern uint64_t total_mem_bytes;
extern uint8_t _kernel_end[];

void pmm_init();
void pmm_free_frame(uint64_t address);
void pmm_lock_frame(uint64_t address);
void pmm_free_range(uint64_t base, uint64_t length);
void pmm_lock_range(uint64_t base, uint64_t length);
void* pmm_alloc_frame();
void* pmm_alloc_frames(uint64_t count);

void pmm_dump_occupied_areas();

#ifdef __cplusplus
}
#endif
