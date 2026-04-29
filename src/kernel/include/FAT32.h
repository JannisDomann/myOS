#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t  jmp[3];
    char     oem[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_count;
    uint16_t root_entries;
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t fat_size_16;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;

    // FAT32 spezifisch ab hier
    uint32_t fat_size_32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
    uint16_t fs_info;
    uint16_t backup_boot_sector;
    uint8_t  reserved[12];
    uint8_t  drive_number;
    uint8_t  reserved1;
    uint8_t  boot_signature;
    uint32_t volume_id;
    char     volume_label[11];
    char     fs_type[8];
} __attribute__((packed)) fat32_bpb_t;

typedef struct {
    char     name[11];      // 8.3 Format: "KERNEL  SYS"
    uint8_t  attr;
    uint8_t  nt_res;
    uint8_t  create_time_tens;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t last_access_date;
    uint16_t cluster_high;  // Obere 16 Bit der Cluster-Nummer
    uint16_t write_time;
    uint16_t write_date;
    uint16_t cluster_low;   // Untere 16 Bit der Cluster-Nummer
    uint32_t file_size;
} __attribute__((packed)) fat32_dir_t;

// functions
void fat32_init();
uint32_t find_partition_start();
uint32_t fat32_get_next_cluster(uint32_t current_cluster);
uint32_t fat32_find_file(const char* name, uint32_t* file_size_bytes);
void fat32_read_file(uint32_t start_cluster, void* buffer);

#ifdef __cplusplus
}
#endif
