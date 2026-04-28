#include "ata.h"
#include "pmm.h"
#include "idt.h"
#include "io.h"
#include "types.h"
#include <stdbool.h>

/* Global flag for IRQ synchronization */
static volatile bool ata_irq_fired = false;


/* ISR Callback for IRQ 14 (Vector 46) */
void ata_handler(interrupt_registers_t* regs) {
    /* Read status to acknowledge the interrupt at the controller */
    k_inb(ATA_COMMAND_STAT_PORT);
    ata_irq_fired = true;
}

void ata_init() {
    /* Register handler for IRQ 14 (Offset 32 + 14 = 46) */
    register_interrupt_handler(INT_PRIMARY_ATA, ata_handler);

    /* Unmask IRQ 14 in the PIC */
    pic_unmask(0x0E);

    /* Enable interrupts at the drive (nIEN bit = 0) */
    k_outb(ATA_CONTROL_PORT, nIEN);

    k_printf("ATA driver is active.\n");
}

void ata_read_sector(uint64_t lba, void* buffer) {
    if (buffer == NULL) {
        return;
    }
    
    ata_irq_fired = false;

    /* 1. Select drive and highest 4 bits of LBA */
    while (k_inb(ATA_CONTROL_PORT) & (ATA_STATUS_BSY | ATA_STATUS_DRQ));
    k_outb(ATA_DRIVE_SELECT_PORT, ATA_DRIVE_MASTER | ((lba >> 0x18) & 0x0F));
    k_io_wait();

    /* 2. Send sector count and rest of LBA */
    k_outb(ATA_SECTOR_COUNT_PORT, 0x01);
    k_outb(ATA_LBA_LOW_PORT, (uint8_t)lba);
    k_outb(ATA_LBA_MID_PORT, (uint8_t)(lba >> 0x08));
    k_outb(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 0x10));

    /* 3. Issue the READ command */
    while (k_inb(ATA_CONTROL_PORT) & (ATA_STATUS_BSY | ATA_STATUS_DRQ));
    k_outb(ATA_COMMAND_STAT_PORT, ATA_CMD_READ_PIO);

    /* 4. Wait for the interrupt to signal data is ready */
    while (!ata_irq_fired);

    /* 5. Transfer 256 words (512 bytes) from data port to buffer */
    uint8_t status;
    do {
        status = k_inb(ATA_CONTROL_PORT);
    }
    while ((status & ATA_STATUS_BSY) || (!(status & ATA_STATUS_DRQ)));
    k_insw(ATA_DATA_PORT, buffer, 0x100);
}

void ata_write_sector(uint64_t lba, void* buffer) {
    uint16_t* data = (uint16_t*)buffer;

    ata_irq_fired = false;

    /* 1. Select drive and highest 4 bits of LBA */
    while (k_inb(ATA_COMMAND_STAT_PORT) & (ATA_STATUS_BSY | ATA_STATUS_DRQ));
    k_outb(ATA_DRIVE_SELECT_PORT, ATA_DRIVE_MASTER | ((lba >> 0x18) & 0x0F));
    k_io_wait();

    /* 2. Send sector count and rest of LBA */
    k_outb(ATA_SECTOR_COUNT_PORT, 0x01);
    k_outb(ATA_LBA_LOW_PORT, (uint8_t)lba);
    k_outb(ATA_LBA_MID_PORT, (uint8_t)(lba >> 0x08));
    k_outb(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 0x10));

    /* 3. Issue the WRITE command */
    k_outb(ATA_COMMAND_STAT_PORT, ATA_CMD_WRITE_PIO);
    while (!(k_inb(ATA_COMMAND_STAT_PORT) & ATA_STATUS_DRQ));

    /* 4. Transfer 256 words (512 bytes) from buffer to data port */
    uint8_t status;
    do {
        status = k_inb(ATA_CONTROL_PORT);
    }
    while ((status & ATA_STATUS_BSY) || (!(status & ATA_STATUS_DRQ)));
    k_outsw(ATA_DATA_PORT, data, 0x100);

    /* 5. Wait for the interrupt to signal data is written */
    while (!ata_irq_fired);
}
