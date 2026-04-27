#include "../include/ata.h"
#include "../include/pmm.h"
#include "../include/idt.h"
#include "../include/io.h"
#include "../include/types.h"
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
}

void* ata_read_sector(uint64_t lba, uint64_t total_bitmap_blocks) {
    /* 1. Allocate a physical frame for the data */
    void* buffer = pmm_alloc_frame(total_bitmap_blocks);
    if (buffer == NULL)  {
        return NULL;
    }

    ata_irq_fired = false;

    /* 2. Select drive and highest 4 bits of LBA */
    while (k_inb(ATA_CONTROL_PORT) & (ATA_STATUS_BSY | ATA_STATUS_DRQ));
    k_outb(ATA_DRIVE_SELECT_PORT, ATA_DRIVE_MASTER | ((lba >> 0x18) & 0x0F));
    k_io_wait();

    /* 3. Send sector count and rest of LBA */
    k_outb(ATA_SECTOR_COUNT_PORT, 0x01);
    k_outb(ATA_LBA_LOW_PORT, (uint8_t)lba);
    k_outb(ATA_LBA_MID_PORT, (uint8_t)(lba >> 0x08));
    k_outb(ATA_LBA_HIGH_PORT, (uint8_t)(lba >> 0x10));

    /* 4. Issue the READ command */
    while (k_inb(ATA_CONTROL_PORT) & (ATA_STATUS_BSY | ATA_STATUS_DRQ));
    k_outb(ATA_COMMAND_STAT_PORT, ATA_CMD_READ_PIO);

    /* 5. Wait for the interrupt to signal data is ready */
    while (!ata_irq_fired);

    /* 6. Transfer 256 words (512 bytes) from data port to buffer */
    uint8_t status;
    do {
        status = k_inb(ATA_CONTROL_PORT);
    }
    while ((status & ATA_STATUS_BSY) || (!(status & ATA_STATUS_DRQ)));
    k_insw(ATA_DATA_PORT, buffer, 0x100);

    return buffer;
}
