#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* I/O Ports for Primary Bus */
#define ATA_DATA_PORT           0x1F0
#define ATA_FEATURES_PORT       0x1F1
#define ATA_SECTOR_COUNT_PORT   0x1F2
#define ATA_LBA_LOW_PORT        0x1F3
#define ATA_LBA_MID_PORT        0x1F4
#define ATA_LBA_HIGH_PORT       0x1F5
#define ATA_DRIVE_SELECT_PORT   0x1F6
#define ATA_COMMAND_STAT_PORT   0x1F7
#define ATA_CONTROL_PORT        0x3F6

/* ATA Commands */
#define ATA_CMD_READ_PIO        0x20
#define ATA_CMD_WRITE_PIO       0x30
#define ATA_CMD_IDENTIFY        0xEC

/* Drive Select Bits */
#define ATA_DRIVE_MASTER        0xE0

/* Control Register Bits */
#define nIEN                    0x00

/* Status Register Bits */
#define ATA_STATUS_BSY          0x80
#define ATA_STATUS_DRQ          0x08
#define ATA_STATUS_ERR          0x01

/* Interrupt */
#define INT_PRIMARY_ATA         0x2E

/* Function prototypes */
void ata_init();
void ata_soft_reset();
void ata_read_sector(uint64_t lba, void* buffer);
void ata_write_sector(uint64_t lba, void* buffer) ;

#ifdef __cplusplus
}
#endif
