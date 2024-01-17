/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ide_ops.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 15:04:08 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/17 16:13:57 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ide.h>
#include <system/io.h>

static int ide_inb(IDEChannelRegisters *regs, uint8_t reg) {
    if (reg > 0x07 && reg < 0x0C) {
        outb(regs->ctrl, 0x80 | regs->nIEN);
    }
    int result = inb(regs->base + reg);
    if (reg > 0x07 && reg < 0x0C) {
        outb(regs->ctrl, regs->nIEN);
    }
    return (result);
}

static void ide_wait(IDEChannelRegisters *regs) {
    for (int i = 0; i < 4; ++i) {
        ide_inb(regs, ATA_REG_ALTSTATUS);
    }
    while (ide_inb(regs, ATA_REG_STATUS) & ATA_SR_BSY) {
    }
}

static void ide_send_command(IDEDevice *dev, uint8_t cmd, uint8_t rw, uint32_t lba, uint8_t sectors) {
    IDEChannelRegisters *regs = &dev->regs;
    IDELBA lba_mode = __ide_get_lba(lba);
    IDELBAMode lbamode = dev->lba_mode;

    if (lbamode == IDE_LBA48) {
        outb(regs->base + ATA_REG_CONTROL, 0x40 | 0x08);
        outb(regs->base + ATA_REG_SECCOUNT1, 0);
        outb(regs->base + ATA_REG_LBA3, lba_mode.lba_mid);
        outb(regs->base + ATA_REG_LBA4, lba_mode.lba_high);
        outb(regs->base + ATA_REG_LBA5, lba_mode.lba);
    }

    outb(regs->base + ATA_REG_SECCOUNT0, sectors);
    outb(regs->base + ATA_REG_LBA0, lba_mode.lba);
    outb(regs->base + ATA_REG_LBA1, lba_mode.lba_high);
    outb(regs->base + ATA_REG_LBA2, lba_mode.lba_mid);
    outb(regs->base + ATA_REG_HDDEVSEL, 0xE0 | (dev->drive << 4) | ((lba_mode.device & 0x0F) << 0));
}

static void ide_flush_cache(IDEDevice *dev) {
    IDEChannelRegisters *regs = &dev->regs;

    outb(regs->base + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    ide_wait(regs);
}

void ide_select_drive(IDEDevice *dev) {
    IDEChannelRegisters *regs = &dev->regs;
    outb(regs->base + ATA_REG_HDDEVSEL, 0xE0 | (dev->drive << 4));
}

int ide_read_sectors(IDEDevice *dev, uint32_t lba, uint8_t sectors, void *buf) {
    IDEChannelRegisters *regs = &dev->regs;

    ide_select_drive(dev);
    ide_send_command(dev, ATA_CMD_READ_PIO, ATA_READ, lba, sectors);
    ide_display_disk_state(dev);

    for (int i = 0; i < sectors; ++i) {
        int timeout = 10000;
        while ((ide_inb(regs, ATA_REG_STATUS) & ATA_SR_DRQ) != 0) {
            if (--timeout <= 0) {
                printk("Timeout waiting for DRQ\n");
                return -1;
            }
            int st = 0;
            if ((st = (inb(regs->base + ATA_REG_STATUS) & ATA_SR_ERR))) {
                printk("Error while reading sector\n");
                ide_error_msg(st, true);
                return -1;
            }
        }
        memcpy((uint8_t *)buf + (i * SECTOR_SIZE), (void *)(regs->base + ATA_REG_DATA), SECTOR_SIZE);
    }

    ide_wait(regs);

    return (0);
}

int ide_write_sectors(IDEDevice *dev, uint32_t lba, uint8_t sectors, const void *buf) {
    IDEChannelRegisters *regs = &dev->regs;

    ide_select_drive(dev);
    ide_send_command(dev, ATA_CMD_WRITE_PIO, ATA_WRITE, lba, sectors);
    ide_display_disk_state(dev);

    for (int i = 0; i < sectors; ++i) {
        int timeout = 10000;
        while ((ide_inb(regs, ATA_REG_STATUS) & ATA_SR_DRQ) != 0) {
            if (--timeout <= 0) {
                printk("Timeout waiting for DRQ\n");
                return -1;
            }

            int st = 0;
            if ((st = (inb(regs->base + ATA_REG_STATUS) & ATA_SR_ERR))) {
                printk("Error while writing sector\n");
                ide_error_msg(st, true);
                return -1;
            }
        }

        memcpy((void *)(regs->base + ATA_REG_DATA), (const uint8_t *)buf + (i * SECTOR_SIZE), SECTOR_SIZE);
    }
    ide_flush_cache(dev);

    ide_wait(regs);
    return (0);
}