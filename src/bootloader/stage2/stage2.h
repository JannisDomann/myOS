#pragma once

#include <stdint.h>

#define CR0_PG          (1 << 31)       // Paging Enable
#define CR0_PE          (1 << 0)        // Protection Enable

#define CR4_PAE         (1 << 5)        // Physical Address Extension
#define CR4_PSE         (1 << 4)        // Page Size Extensions (2MB Pages)

#define EFER_MSR        0xC0000080      // Register Address
#define EFER_LME        (1 << 8)        // Long Mode Enable

// Paging Flags
#define PAGE_PRESENT    (1ULL << 0)
#define PAGE_WRITE      (1ULL << 1)
#define PAGE_USER       (1ULL << 2)     // User Mode
#define PAGE_HUGE       (1ULL << 7)

// Each table level with 512 entries
typedef uint64_t* page_table_p;
typedef uint8_t bool;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t memcmp(const void* s1, const void* s2, uint32_t n);
void setup_paging();
void read_sectors_ata(uint32_t lba, uint8_t count, uint32_t dest_addr);
uint32_t find_partition_start();
uint32_t find_kernel_cluster(uint32_t partition_lba);
void load_kernel_file(uint32_t partition_lba, uint32_t start_cluster, uint32_t dest);

extern void load_paging_and_jump(uint32_t pml4, uint32_t efer_msr, uint32_t kernel_entry);

extern void outb_asm(uint16_t port, uint8_t val);
extern uint8_t inb_asm(uint16_t port);
extern void outw_asm(uint16_t port, uint16_t val);
extern uint16_t inw_asm(uint16_t port);

#ifdef __cplusplus
}
#endif