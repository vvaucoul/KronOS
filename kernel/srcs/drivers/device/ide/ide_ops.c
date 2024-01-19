/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ide_ops.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 15:04:08 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/19 16:26:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ide.h>
#include <system/io.h>

#include <system/pit.h>

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

static void ide_wait_drq(IDEChannelRegisters *regs) {
    while (!(ide_inb(regs, ATA_REG_STATUS) & ATA_SR_DRQ)) {
    }
}

static void ide_send_command(IDEDevice *dev, uint8_t cmd, uint32_t lba, uint8_t sectors) {
    // IDEChannelRegisters *regs = &dev->regs;
    // IDELBA lba_mode = __ide_get_lba(lba);
    // IDELBAMode lbamode = dev->lba_mode;

    // printk("lbamode: %d\n", lbamode);
    // printk("lba_mode.lba_mid: %d\n", lba_mode.lba_mid);
    // printk("lba_mode.lba_high: %d\n", lba_mode.lba_high);
    // printk("lba_mode.lba: %d\n", lba_mode.lba);
    // printk("sectors: %d\n", sectors);
    // printk("cmd: %d\n", cmd);

    // // Check if the device supports LBA48 addressing
    // if (lbamode == IDE_LBA48) {
    //     // Send the LBA48 address to the controller
    //     outb(regs->base + ATA_REG_CONTROL, 0x40 | 0x08);
    //     outb(regs->base + ATA_REG_SECCOUNT1, 0);
    //     outb(regs->base + ATA_REG_LBA3, lba_mode.lba_mid);
    //     outb(regs->base + ATA_REG_LBA4, lba_mode.lba_high);
    //     outb(regs->base + ATA_REG_LBA5, lba_mode.lba);
    // } else {
    //     // Send the LBA28 address to the controller
    //     outb(regs->base + ATA_REG_LBA0, lba_mode.lba);
    //     outb(regs->base + ATA_REG_LBA1, lba_mode.lba_high);
    //     outb(regs->base + ATA_REG_LBA2, lba_mode.lba_mid);
    // }

    // // Send the sector count to the controller
    // outb(regs->base + ATA_REG_SECCOUNT0, sectors);

    // // Send the command to the controller
    // switch (cmd) {
    // case ATA_CMD_READ_PIO:
    //     outb(regs->base + ATA_REG_COMMAND, ATA_CMD_READ_PIO | rw);
    //     break;
    // case ATA_CMD_WRITE_PIO:
    //     outb(regs->base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO | rw);
    //     break;
    // case ATA_CMD_READ_PIO_EXT:
    //     outb(regs->base + ATA_REG_SECCOUNT1, (sectors >> 8) & 0xFF);
    //     outb(regs->base + ATA_REG_COMMAND, ATA_CMD_READ_PIO_EXT | rw);
    //     break;
    // case ATA_CMD_WRITE_PIO_EXT:
    //     outb(regs->base + ATA_REG_FEATURES, 0);
    //     outb(regs->base + ATA_REG_SECCOUNT1, 0);
    //     outb(regs->base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO_EXT | rw);
    //     break;
    // default:
    //     // Invalid command
    //     printk("Error: Invalid command 0x%02x\n", cmd);
    //     return;
    // }

    // // Send the device/head/sector number to the controller
    // outb(regs->base + ATA_REG_HDDEVSEL, 0xE0 | (dev->drive << 4) | ((lba_mode.device & 0x0F) << 0));

    // while (ide_inb(regs, ATA_REG_STATUS) & ATA_SR_BSY) {
    // }
    // while (ide_inb(regs, ATA_REG_STATUS) & ATA_SR_DRQ) {
    // }
    // }
    IDEChannelRegisters *regs = &dev->regs;
    IDELBA lba_mode = __ide_get_lba(lba);

    if (dev->lba_mode == IDE_LBA48) {
        outb(regs->base + ATA_REG_CONTROL, 0x40 | 0x08);
        outb(regs->base + ATA_REG_SECCOUNT1, 0);
        outb(regs->base + ATA_REG_LBA3, lba_mode.lba_mid);
        outb(regs->base + ATA_REG_LBA4, lba_mode.lba_high);
        outb(regs->base + ATA_REG_LBA5, lba_mode.lba);
    } else {
        outb(regs->base + ATA_REG_LBA0, lba_mode.lba);
        outb(regs->base + ATA_REG_LBA1, lba_mode.lba_high);
        outb(regs->base + ATA_REG_LBA2, lba_mode.lba_mid);
    }

    outb(regs->base + ATA_REG_SECCOUNT0, sectors);

    switch (cmd) {
    case ATA_CMD_READ_PIO: {
        uint8_t sec_count1 = (sectors >> 8) & 0xFF;
        outb(regs->base + ATA_REG_SECCOUNT1, sec_count1);
        break;
    }

    case ATA_CMD_WRITE_PIO: {
        uint8_t sec_count1 = (sectors >> 8) & 0xFF;
        outb(regs->base + ATA_REG_SECCOUNT1, sec_count1);
        break;
    }

    case ATA_CMD_READ_PIO_EXT: {
        uint8_t sec_count1 = (sectors >> 8) & 0xFF;
        outb(regs->base + ATA_REG_SECCOUNT1, sec_count1);
        outb(regs->base + ATA_REG_LBA3, lba_mode.lba_mid);
        outb(regs->base + ATA_REG_LBA4, lba_mode.lba_high);
        outb(regs->base + ATA_REG_LBA5, lba_mode.lba);
        break;
    }

    case ATA_CMD_WRITE_PIO_EXT: {
        outb(regs->base + ATA_REG_FEATURES, 0);
        outb(regs->base + ATA_REG_SECCOUNT1, 0);
        outb(regs->base + ATA_REG_LBA3, lba_mode.lba_mid);
        outb(regs->base + ATA_REG_LBA4, lba_mode.lba_high);
        outb(regs->base + ATA_REG_LBA5, lba_mode.lba);
        break;
    }
    default:
        break;
    }

    outb(regs->base + ATA_REG_SECCOUNT0, sectors);
    outb(regs->base + ATA_REG_LBA0, lba_mode.lba);
    outb(regs->base + ATA_REG_LBA1, lba_mode.lba_high);
    outb(regs->base + ATA_REG_LBA2, lba_mode.lba_mid);
    outb(regs->base + ATA_REG_HDDEVSEL, 0xE0 | (dev->drive << 4) | ((lba_mode.device & 0x0F) << 0));
}

static void ide_flush_cache(IDEDevice *dev) {
    IDEChannelRegisters *regs = &dev->regs;

    ide_wait(regs);
    outb(regs->base + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    ide_wait(regs);
}

void ide_select_drive(IDEDevice *dev) {
    IDEChannelRegisters *regs = &dev->regs;
    outb(regs->base + ATA_REG_HDDEVSEL, 0xE0 | (dev->drive << 4));
    ide_wait(regs);
}

int ide_read(IDEDevice *dev, uint32_t lba, uint8_t sectors, void *buf) {
    ide_select_drive(dev);

    if (lba + sectors > dev->size || sectors == 0 || sectors > dev->sector_count) {
        return -1;
    }

    // Send LBA and sector count (LBA 28)
    outb(dev->regs.base + ATA_REG_LBA0, (uint8_t)lba);
    outb(dev->regs.base + ATA_REG_LBA1, (uint8_t)(lba >> 8));
    outb(dev->regs.base + ATA_REG_LBA2, (uint8_t)(lba >> 16));
    outb(dev->regs.base + ATA_REG_SECCOUNT0, sectors);

    // Send read command
    outb(dev->regs.base + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    ide_wait_drq(&dev->regs);

    for (int i = 0; i < sectors; ++i) {
        // Read data from the drive's data register into the buffer
        insw(dev->regs.base + ATA_REG_DATA, buf + (i * SECTOR_SIZE), SECTOR_SIZE / 2);
        ide_wait(&dev->regs);
    }

    ide_wait(&dev->regs);
    return (0);
}

int ide_write(IDEDevice *dev, uint32_t lba, uint8_t sectors, const void *buf) {
    ide_select_drive(dev);

    if (lba + sectors > dev->size || sectors == 0 || sectors > dev->sector_count) {
        return -1;
    }

    // Send LBA and sector count (LBA 28)
    outb(dev->regs.base + ATA_REG_LBA0, (uint8_t)lba);
    outb(dev->regs.base + ATA_REG_LBA1, (uint8_t)(lba >> 8));
    outb(dev->regs.base + ATA_REG_LBA2, (uint8_t)(lba >> 16));
    outb(dev->regs.base + ATA_REG_SECCOUNT0, sectors);

    // Send write command
    outb(dev->regs.base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

    ide_wait_drq(&dev->regs);

    for (int i = 0; i < sectors; ++i) {
        // Write data from the buffer to the drive's data register
        outsw(dev->regs.base + ATA_REG_DATA, buf + (i * SECTOR_SIZE), SECTOR_SIZE / 2);
        ide_wait(&dev->regs);
    }

    ide_flush_cache(dev);
    ide_wait(&dev->regs);
    return (0);
}