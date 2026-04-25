#pragma once

// ATA I/O Ports
#define ATA_DATA_PORT         0x1F0
#define ATA_SECTOR_COUNT_PORT 0x1F2
#define ATA_LBA_LOW_PORT      0x1F3
#define ATA_LBA_MID_PORT      0x1F4
#define ATA_LBA_HIGH_PORT     0x1F5
#define ATA_DRIVE_SELECT_PORT 0x1F6
#define ATA_COMMAND_STAT_PORT 0x1F7

// ATA Commands & Control Flags
#define ATA_CMD_READ_PIO      0x20
#define ATA_DRIVE_MASTER      0xE0  // LBA mode + Master drive selection

// Status Register Bits
#define ATA_STATUS_BSY        0x80  // Busy: drive is preparing data
#define ATA_STATUS_DRQ        0x08  // Data Request: drive is ready to transfer data
