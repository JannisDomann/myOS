#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CR0_PG          (1 << 31)       // Paging Enable
#define CR0_PE          (1 << 0)        // Protection Enable

#define CR4_PAE         (1 << 5)        // Physical Address Extension
#define CR4_PSE         (1 << 4)        // Page Size Extensions (2MB Pages)

#define EFER_MSR        0xC0000080      // Register Address
#define EFER_LME        (1 << 8)        // Long Mode Enable

// Paging Flags
#define PAGE_PRESENT    (0x1ULL << 0)
#define PAGE_WRITE      (0x1ULL << 1)
#define PAGE_USER       (0x1ULL << 2)     // User Mode
#define PAGE_HUGE       (0x1ULL << 7)

// Each table level with 512 entries
typedef uint64_t* page_table_p;
typedef uint8_t bool;

// ELF64 header to determine ELF entry point
typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;       /* Entry point virtual address */
    uint64_t e_phoff;       /* Program header table file offset */
    uint64_t e_shoff;       /* Section header table file offset */
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;       /* Number of program headers */
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf64_Ehdr;

// ELF64 program header to determine kernel entry point
typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;      /* Offset in file */
    uint64_t p_vaddr;       /* Virtual Address (Higher Half) */
    uint64_t p_paddr;       /* Physikalische Adresse (LMA) */
    uint64_t p_filesz;      /* Size in file */
    uint64_t p_memsz;       /* Size in RAM */
    uint64_t p_align;
} Elf64_Phdr;


void memcpy(const void* dest, const void* src, uint64_t count);
void memset(const void* dest, uint8_t value, uint64_t size);
uint32_t memcmp(const void* s1, const void* s2, uint32_t n);
void setup_paging();
void read_sectors_ata(uint32_t lba, uint8_t count, uint32_t dest_addr);
uint32_t find_partition_start();
uint32_t find_kernel_cluster(uint32_t partition_lba);
void load_kernel_file(uint32_t partition_lba, uint32_t start_cluster, uint32_t dest);

extern volatile void load_paging_and_jump(uint32_t pml4, uint32_t efer_msr, uint64_t kernel_entry);

extern volatile void outb_asm(uint16_t port, uint8_t val);
extern volatile uint8_t inb_asm(uint16_t port);
extern volatile void outw_asm(uint16_t port, uint16_t val);
extern volatile uint16_t inw_asm(uint16_t port);
extern volatile void halt();

#ifdef __cplusplus
}
#endif