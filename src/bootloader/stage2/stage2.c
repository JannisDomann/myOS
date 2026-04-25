#include "stage2.h"
#include "MBR.h"
#include "FAT32.h"
#include "ATA_driver.h"

/*
    PML4 (Page Map Level 4)
    PDPT (Page Directory Pointer Table)
    PD (Page Directory)
    PT (Page Table)
*/

// fixed addresses for tables (4KB aligned)
page_table_p pml4 = (page_table_p)0x1000;
page_table_p pdpt = (page_table_p)0x2000;
page_table_p pd   = (page_table_p)0x3000;

void memset(const void* dest, uint8_t value, uint32_t size) {
    uint8_t* p = (uint8_t*)dest;
    for (uint32_t i=0; i<size; ++i) {
        p[i] = value;
    }
}

uint32_t memcmp(const void* s1, const void* s2, uint32_t n) {
    const uint8_t* p1 = (uint8_t*)s1;
    const uint8_t* p2 = (uint8_t*)s2;
    for (uint32_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}

void setup_paging() {
    memset(pml4, 0, 4096);
    memset(pdpt, 0, 4096);
    memset(pd, 0, 4096);

    // map first pml4 entry to pdpt
    pml4[0] = (uint32_t)pdpt | PAGE_PRESENT | PAGE_WRITE;

    // map first pdpt entry to pd
    pdpt[0] = (uint32_t)pd   | PAGE_PRESENT | PAGE_WRITE;

    // identity mapping for first 1GB (512 * 2MB) (kernel area)
    for (uint16_t i=0; i<512; ++i) {
        pd[i] = (uint64_t)(i * 0x200000) | PAGE_PRESENT | PAGE_WRITE | PAGE_HUGE;
    }
}

void read_sectors_ata(uint32_t lba, uint8_t count, uint32_t dest_addr) {
    uint16_t* ptr = (uint16_t*)dest_addr;

    // Select drive and send the highest 4 bits of the LBA address
    outb_asm(ATA_DRIVE_SELECT_PORT, ATA_DRIVE_MASTER | ((lba >> 24) & 0x0F));
    
    // Set the number of sectors to be read
    outb_asm(ATA_SECTOR_COUNT_PORT, count);
    
    // Send the remaining 24 bits of the LBA address
    outb_asm(ATA_LBA_LOW_PORT,  (uint8_t)lba);
    outb_asm(ATA_LBA_MID_PORT,  (uint8_t)(lba >> 8));
    outb_asm(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 16));
    
    // Issue the PIO Read command
    outb_asm(ATA_COMMAND_STAT_PORT, ATA_CMD_READ_PIO);

    for (int i = 0; i < count; i++) {
        // Poll status: Wait until BSY is cleared AND DRQ is set
        while ((inb_asm(ATA_COMMAND_STAT_PORT) & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) != ATA_STATUS_DRQ);

        // Transfer one sector (256 words = 512 bytes) into RAM
        for (int j = 0; j < 256; j++) {
            *ptr++ = inw_asm(ATA_DATA_PORT);
        }
    }
}



uint32_t find_partition_start() {
    uint8_t sector_buffer[512];

    // read MBR (sector 0)
    read_sectors_ata(0, 1, (uint32_t)sector_buffer);

    // partition table begins at offset 446 (0x1BE)
    mbr_partition_t* first_partition = (mbr_partition_t*)&sector_buffer[446];

    // type check FAT32 (0x0B or 0x0C)
    if (first_partition->partition_type == 0x0B || first_partition->partition_type == 0x0C) {
        return first_partition->lba_start;
    }

    return 0;
}

uint32_t find_kernel_cluster(uint32_t partition_lba) {  // 2048secs
    uint8_t bpb_buffer[512];
    read_sectors_ata(partition_lba, 1, (uint32_t)bpb_buffer);
    fat32_bpb_t* bpb = (fat32_bpb_t*)bpb_buffer;

    // Where does data segment start
    uint32_t fat_start = partition_lba + bpb->reserved_sectors; // 2048+32=2080secs
    uint32_t data_start = fat_start + (bpb->fat_count * bpb->fat_size_32); // 2080+2*923=3926

    // Read root cluster
    uint32_t root_lba = data_start + (bpb->root_cluster - 2) * bpb->sectors_per_cluster;
    // 3926+(2-2)*1=3926 (0x1EAC00)

    uint8_t dir_buffer[512];
    read_sectors_ata(root_lba, 1, (uint32_t)dir_buffer);
    fat32_dir_t* dir = (fat32_dir_t*)dir_buffer;

    // search entries in sector (16 entries per 512 byte sector)
    for (uint8_t i=0; i<16; ++i) {
        // compare to "KERNEL   SYS"
        if (memcmp(dir[i].name, "KERNEL  SYS", 11) == 0) {
            // merge cluster number high and low (0x0003)
            return (uint32_t)((uint32_t)dir[i].cluster_low | (uint32_t)dir[i].cluster_high << 16);
        }
    }

    return 0;
}

void load_kernel_file(uint32_t partition_lba, uint32_t start_cluster, uint32_t dest) {
    uint8_t bpb_buffer[512];
    read_sectors_ata(partition_lba, 1, (uint32_t)bpb_buffer);
    fat32_bpb_t* bpb = (fat32_bpb_t*)bpb_buffer;

    uint32_t data_start = partition_lba + bpb->reserved_sectors + (bpb->fat_count * bpb->fat_size_32);   
    uint32_t cluster_lba = data_start + (start_cluster - 2) * bpb->sectors_per_cluster;

    read_sectors_ata(cluster_lba, 128, dest);
} 


void stage2_main() {
    uint32_t kernel_ram_entry = 0x200000;
    uint32_t* KERNEL_MAGIC = (uint32_t*)kernel_ram_entry;

    setup_paging();

    uint32_t partition_start = find_partition_start();
    if (partition_start) {
        uint32_t kernel_cluster = find_kernel_cluster(partition_start);
        if (kernel_cluster) {
            load_kernel_file(partition_start, kernel_cluster, kernel_ram_entry);
            if (*KERNEL_MAGIC != 0xBEBAFECA) {
                while(1) {
                    __asm("hlt");
                }
            }
            load_paging_and_jump((uint32_t)pml4, EFER_MSR, kernel_ram_entry+0x04);
        }
    }
}
