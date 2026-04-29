#include "FAT32.h"
#include "MBR.h"
#include "ata.h"
#include "mem.h"
#include "k_string.h"

static uint32_t partition_lba;

static uint8_t bpb_buffer[512] __attribute__((aligned(16)));
static uint8_t fat_scratch_buffer[512] __attribute__((aligned(16)));

static fat32_bpb_t* bpb = (fat32_bpb_t*)bpb_buffer;
static fat32_dir_t* fat_scratch = (fat32_dir_t*)fat_scratch_buffer;


void fat32_init() {
    partition_lba = find_partition_start();
    if (partition_lba == 0x00) {
        return;
    }

    // read bpb
    ata_read_sector(partition_lba, bpb_buffer);

    k_printf("FAT32 intialized. Sectors/Clusters: %d\n", bpb->sectors_per_cluster);
}

uint32_t find_partition_start() {
    // misuse bpb buffer for reading MBR (sector 0) one time 
    uint8_t* mbr = bpb_buffer;
    ata_read_sector(0x00, mbr);

    // partition table begins at offset 446 (0x1BE)
    mbr_partition_t* first_partition = (mbr_partition_t*)&mbr[446];

    // type check FAT32 (0x0B or 0x0C)
    if (first_partition->partition_type == 0x0B || first_partition->partition_type == 0x0C) {
        return first_partition->lba_start;
    }

    return 0;
}

uint32_t fat32_get_next_cluster(uint32_t current_cluster) {
    /* 1. Calculate the LBA of the FAT sector containing the entry */
    uint32_t fat_start_lba = partition_lba + bpb->reserved_sectors;
    uint32_t fat_offset = current_cluster * 0x04;
    uint32_t fat_sector = fat_start_lba + (fat_offset / 0x200);
    uint32_t ent_offset  = fat_offset % 0x200;

    /* 2. Read the FAT sector into the scratch buffer */
    ata_read_sector(fat_sector, fat_scratch_buffer);

    /* 3. Extract the next cluster (FAT32 uses only the lower 28 bits) */
    uint32_t* table = (uint32_t*)fat_scratch_buffer;
    uint32_t next_cluster = table[ent_offset / 0x04] & 0x0FFFFFFF;

    return next_cluster;
}

uint32_t fat32_find_file(const char* name, uint32_t* file_size_bytes) {
    uint32_t fat_start = partition_lba + bpb->reserved_sectors;
    uint32_t data_start = fat_start + (bpb->fat_count * bpb->fat_size_32);
    
    /* Root directory LBA (typically Cluster 2 in FAT32) */
    uint32_t root_lba = data_start + (bpb->root_cluster - 2) * bpb->sectors_per_cluster;

    /* Read the first sector of the root directory into scratch */
    ata_read_sector(root_lba, fat_scratch_buffer);

    /* Search through the 16 entries of the 512-byte sector */
    for (uint32_t i=0; i<16; ++i) {
        /* End of directory? */
        if (fat_scratch[i].name[0] == 0x00) break;
        /* Deleted entry? */
        if (fat_scratch[i].name[0] == 0xE5) continue;

        /* Compare 8.3 filename (exactly 11 characters) */
        if (k_memcmp(fat_scratch[i].name, name, 11) == 0) {
            /* Reconstruct starting cluster from high and low parts */
            *file_size_bytes = fat_scratch[i].file_size;
            return (uint32_t)((uint32_t)fat_scratch[i].cluster_low | ((uint32_t)fat_scratch[i].cluster_high << 0x10));
        }
    }

    /* File not found */
    *file_size_bytes = 0;
    return 0;
}


void fat32_read_file(uint32_t start_cluster, void* buffer) {
    uint32_t current_cluster = start_cluster;
    uint8_t* current_ptr = (uint8_t*)buffer;

    uint32_t fat_start = partition_lba + bpb->reserved_sectors;
    uint32_t data_start = fat_start + (bpb->fat_count * bpb->fat_size_32);

    /* Follow the cluster chain until End of Chain (EOC) marker is reached */
    while (current_cluster < 0x0FFFFFF8 && current_cluster >= 2) {
        /* Calculate LBA for the current cluster */
        uint32_t cluster_lba = data_start + (current_cluster - 2) * bpb->sectors_per_cluster;

        /* Read all sectors belonging to this cluster */
        for (uint32_t i=0; i<bpb->sectors_per_cluster; ++i) {
            ata_read_sector(cluster_lba + i, current_ptr);
            current_ptr += 0x200;
        }

        /* Fetch next cluster from the File Allocation Table */
        current_cluster = fat32_get_next_cluster(current_cluster);
    }
}
