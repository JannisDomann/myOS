#pragma once

#include <stdint.h>

typedef struct {
    uint8_t  boot_drive;
    uint8_t  start_head;
    uint8_t  start_sector;   // Bits 0-5 Sector, 6-7 Cylinder High
    uint8_t  start_cylinder;
    uint8_t  partition_type; // FAT32 ist 0x0B oder 0x0C
    uint8_t  end_head;
    uint8_t  end_sector;
    uint8_t  end_cylinder;
    uint32_t lba_start;      // <--- DAS brauchen wir (wird 2048 sein)
    uint32_t lba_length;
} __attribute__((packed)) mbr_partition_t;
