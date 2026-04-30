#include "stage2.h"
#include "MBR.h"
#include "FAT32.h"
#include "ATA_driver.h"
#include "memory_layout.h"

/*
    PML4 (Page Map Level 4)
    PDPT (Page Directory Pointer Table)
    PD (Page Directory)
    PT (Page Table)
*/

// fixed addresses for tables (4KB aligned)
page_table_p pml4           = (page_table_p)STAGE2_PML4_ADDR;
page_table_p pdpt           = (page_table_p)STAGE2_PDPT_ADDR;
page_table_p pdpt_kernel    = (page_table_p)STAGE2_PDPT_K_ADDR;
page_table_p pd             = (page_table_p)STAGE2_PD_ADDR;

void memcpy(const void* dest, const void* src, uint64_t count) {
    uint8_t* p_dest = (uint8_t*)dest;
    uint8_t* p_src = (uint8_t*)src;
    for (uint64_t i=0x00; i<count; ++i) {
        *p_dest++ = *p_src++;
    }
}

void memset(const void* dest, uint8_t value, uint64_t size) {
    uint8_t* p = (uint8_t*)dest;
    for (uint64_t i=0x00; i<size; ++i) {
        p[i] = value;
    }
}

uint32_t memcmp(const void* s1, const void* s2, uint32_t n) {
    const uint8_t* p1 = (uint8_t*)s1;
    const uint8_t* p2 = (uint8_t*)s2;
    for (uint32_t i= 0x00; i<n; ++i) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}

void setup_paging() {
    memset((void*)pml4, 0x00, 0x1000);
    memset((void*)pdpt, 0x00, 0x1000);
    memset((void*)pdpt_kernel, 0x00, 0x1000);
    memset((void*)pd, 0x00, 0x1000);

    // 1. Identity Mapping (Lower Half)
    // map first pdpt entry to pd
    pml4[0x00] = (uint32_t)pdpt | PAGE_PRESENT | PAGE_WRITE;
    pdpt[0x00] = (uint32_t)pd   | PAGE_PRESENT | PAGE_WRITE;

    /*  2. Direct Mapping (Higher Half)
        We map the SAME PDPT to the higher half entry as well.
        0xFFFF800000000000 corresponds to PML4 Index 256
    */
    pml4[0x100] = (uint32_t)pdpt | PAGE_PRESENT | PAGE_WRITE;

    /*  3. Kernel Mapping (Higher Half)
        We map the kernel PDPT to the higher half entry as well.
        0xFFFFFFFF800000000000 corresponds to PML4 Index 511
    */
    pml4[0x1FF] = (uint32_t)pdpt_kernel | PAGE_PRESENT | PAGE_WRITE;

    // PDPT maps index at 510 to space from -2GB (0xFFFFFFFF80000000)
    pdpt_kernel[0x1FE] = (uint32_t)pd | PAGE_PRESENT | PAGE_WRITE;


    // identity mapping for first 1GB (512 * 2MB) (kernel area)
    for (uint16_t i=0x00; i<0x200; ++i) {
        pd[i] = (uint64_t)(i * 0x200000) | PAGE_PRESENT | PAGE_WRITE | PAGE_HUGE;
    }
}

void read_sectors_ata(uint32_t lba, uint8_t count, uint32_t dest_addr) {
    uint16_t* ptr = (uint16_t*)dest_addr;

    // Select drive and send the highest 4 bits of the LBA address
    outb_asm(ATA_DRIVE_SELECT_PORT, ATA_DRIVE_MASTER | ((lba >> 0x18) & 0x0F));
    
    // Set the number of sectors to be read
    outb_asm(ATA_SECTOR_COUNT_PORT, count);
    
    // Send the remaining 24 bits of the LBA address
    outb_asm(ATA_LBA_LOW_PORT,  (uint8_t)lba);
    outb_asm(ATA_LBA_MID_PORT,  (uint8_t)(lba >> 0x08));
    outb_asm(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 0x10));
    
    // Issue the PIO Read command
    outb_asm(ATA_COMMAND_STAT_PORT, ATA_CMD_READ_PIO);

    for (uint8_t i=0x0; i<count; ++i) {
        // Poll status: Wait until BSY is cleared AND DRQ is set
        while ((inb_asm(ATA_COMMAND_STAT_PORT) & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) != ATA_STATUS_DRQ);

        // Transfer one sector (256 words = 512 bytes) into RAM
        for (uint16_t j=0; j<0x100; ++j) {
            *ptr++ = inw_asm(ATA_DATA_PORT);
        }
    }
}



uint32_t find_partition_start() {
    uint8_t sector_buffer[0x200];

    // read MBR (sector 0)
    read_sectors_ata(0x00, 0x01, (uint32_t)sector_buffer);

    // partition table begins at offset 446 (0x1BE)
    mbr_partition_t* first_partition = (mbr_partition_t*)&sector_buffer[0x1BE];

    // type check FAT32 (0x0B or 0x0C)
    if (first_partition->partition_type == 0x0B || first_partition->partition_type == 0x0C) {
        return first_partition->lba_start;
    }

    return 0;
}

uint32_t find_kernel_cluster(uint32_t partition_lba) {  // 2048secs
    uint8_t bpb_buffer[0x200];
    read_sectors_ata(partition_lba, 0x01, (uint32_t)bpb_buffer);
    fat32_bpb_t* bpb = (fat32_bpb_t*)bpb_buffer;

    // Where does data segment start
    uint32_t fat_start = partition_lba + bpb->reserved_sectors; // 2048+32=2080secs
    uint32_t data_start = fat_start + (bpb->fat_count * bpb->fat_size_32); // 2080+2*923=3926

    // Read root cluster
    uint32_t root_lba = data_start + (bpb->root_cluster - 0x02) * bpb->sectors_per_cluster;
    // 3926+(2-2)*1=3926 (0x1EAC00)

    uint8_t dir_buffer[512];
    read_sectors_ata(root_lba, 1, (uint32_t)dir_buffer);
    fat32_dir_t* dir = (fat32_dir_t*)dir_buffer;

    // search entries in sector (16 entries per 512 byte sector)
    for (uint8_t i=0; i<0x10; ++i) {
        // compare to "KERNEL   SYS"
        if (memcmp(dir[i].name, KERNEL_FILE_NAME, 0x0B) == 0) {
            // merge cluster number high and low (0x0003)
            return (uint32_t)((uint32_t)dir[i].cluster_low | (uint32_t)dir[i].cluster_high << 0x10);
        }
    }

    return 0x00;
}

void load_kernel_file(uint32_t partition_lba, uint32_t start_cluster, uint32_t dest) {
    uint8_t bpb_buffer[0x200];
    read_sectors_ata(partition_lba, 0x01, (uint32_t)bpb_buffer);
    fat32_bpb_t* bpb = (fat32_bpb_t*)bpb_buffer;

    uint32_t data_start = partition_lba + bpb->reserved_sectors + (bpb->fat_count * bpb->fat_size_32);   
    uint32_t cluster_lba = data_start + (start_cluster - 0x02) * bpb->sectors_per_cluster;

    read_sectors_ata(cluster_lba, 0x80, dest);
} 


void stage2_main() {
     /* Temporary buffer to load the raw ELF file from disk */
    /* We load it at doubled kernel entry to avoid overlapping with the final kernel destination */
    uint32_t kernel_raw_buffer = KERNEL_PHYS_MIN * 0x02; 
    
    setup_paging();

    uint32_t partition_start = find_partition_start();
    if (partition_start) {
        uint32_t kernel_cluster = find_kernel_cluster(partition_start);
        if (kernel_cluster) {
            /* 1. Load the raw ELF file into our temporary buffer */
            load_kernel_file(partition_start, kernel_cluster, kernel_raw_buffer);
            
            Elf64_Ehdr* ehdr = (Elf64_Ehdr*)kernel_raw_buffer;

            /* 2. Verify ELF Magic: 0x7F 'E' 'L' 'F' */
            uint8_t* elf_magic = (uint8_t*)kernel_raw_buffer;
            if (elf_magic[0x00] != 0x7F || memcmp(&elf_magic[0x01], "ELF", 0x03) != 0x00) {
                /* Not a valid ELF */
                halt();
            }

            /* 3. Unpack segments from buffer to final physical destination (LMA) */
            /* Phdr table is at ehdr->e_phoff inside the raw buffer */
            Elf64_Phdr* phdr = (Elf64_Phdr*)(kernel_raw_buffer + (uint32_t)ehdr->e_phoff);
            
            for (uint16_t i=0; i<ehdr->e_phnum; ++i) {
                if (phdr[i].p_type == 0x01) { /* PT_LOAD segment */
                    /* Copy from buffer offset to the physical address specified by the linker (LMA) */
                    memcpy((void*)(uint32_t)phdr[i].p_paddr, 
                           (void*)(kernel_raw_buffer + (uint32_t)phdr[i].p_offset), 
                           (uint32_t)phdr[i].p_filesz);
                    
                    /* Zero out BSS area (memsz - filesz) */
                    if (phdr[i].p_memsz > phdr[i].p_filesz) {
                        memset((void*)(uint32_t)(phdr[i].p_paddr + phdr[i].p_filesz), 0x00, 
                               (uint64_t)(phdr[i].p_memsz - phdr[i].p_filesz));
                    }
                }
            }

            /* 4. Transition to Higher Half! */
            /* Paging is set up to map ehdr->e_entry (High) to p_paddr (Low) */
            load_paging_and_jump((uint32_t)pml4, EFER_MSR, ehdr->e_entry);
        }
    }
    
    /* Should never be reached */
    halt();
}
