/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ide_ops.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 15:04:08 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/23 15:43:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ide.h>
#include <memory/memory.h>
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

    if (lba >= dev->size || sectors == 0 || lba + sectors > dev->size) {
        return -1;
    }

    uint32_t remaining_sectors = sectors;
    uint32_t current_lba = lba;
    uint8_t *buffer_ptr = (uint8_t *)buf;
    const uint8_t max_sectors_per_read = 8;

    while (remaining_sectors > 0) {
        uint8_t sectors_to_read = (remaining_sectors > max_sectors_per_read) ? max_sectors_per_read : remaining_sectors;

        outb(dev->regs.base + ATA_REG_LBA0, (uint8_t)current_lba);
        outb(dev->regs.base + ATA_REG_LBA1, (uint8_t)(current_lba >> 8));
        outb(dev->regs.base + ATA_REG_LBA2, (uint8_t)(current_lba >> 16));
        outb(dev->regs.base + ATA_REG_SECCOUNT0, sectors_to_read);

        outb(dev->regs.base + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

        ide_wait_drq(&dev->regs);

        for (int i = 0; i < sectors_to_read; ++i) {
            insw(dev->regs.base + ATA_REG_DATA, buffer_ptr + (i * SECTOR_SIZE), SECTOR_SIZE / 2);
            ide_wait(&dev->regs);
        }

        buffer_ptr += sectors_to_read * SECTOR_SIZE;
        current_lba += sectors_to_read;
        remaining_sectors -= sectors_to_read;
    }

    ide_wait(&dev->regs);
    return 0;
}

int ide_write(IDEDevice *dev, uint32_t lba, uint8_t sectors, const void *buf) {
    ide_select_drive(dev);

    if (lba >= dev->size || sectors == 0 || lba + sectors > dev->size) {
        return -1;
    }

    uint32_t remaining_sectors = sectors;
    uint32_t current_lba = lba;
    const uint8_t *buffer_ptr = (const uint8_t *)buf;
    const uint8_t max_sectors_per_write = 8;

    while (remaining_sectors > 0) {
        uint8_t sectors_to_write = (remaining_sectors > max_sectors_per_write) ? max_sectors_per_write : remaining_sectors;

        outb(dev->regs.base + ATA_REG_LBA0, (uint8_t)current_lba);
        outb(dev->regs.base + ATA_REG_LBA1, (uint8_t)(current_lba >> 8));
        outb(dev->regs.base + ATA_REG_LBA2, (uint8_t)(current_lba >> 16));
        outb(dev->regs.base + ATA_REG_SECCOUNT0, sectors_to_write);

        outb(dev->regs.base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

        ide_wait_drq(&dev->regs);

        for (int i = 0; i < sectors_to_write; ++i) {
            outsw(dev->regs.base + ATA_REG_DATA, buffer_ptr + (i * SECTOR_SIZE), SECTOR_SIZE / 2);
            ide_wait(&dev->regs);
        }

        buffer_ptr += sectors_to_write * SECTOR_SIZE;
        current_lba += sectors_to_write;
        remaining_sectors -= sectors_to_write;
    }

    ide_flush_cache(dev);
    ide_wait(&dev->regs);
    return 0;
}

int ide_simple_read(IDEDevice *dev, uint32_t offset, uint32_t size, void *buffer) {
    if (!dev || !buffer || size == 0 || offset + size > dev->size * SECTOR_SIZE) {
        return -1; // Paramètres invalides
    }

    uint32_t first_sector = offset / SECTOR_SIZE;
    uint32_t last_sector = (offset + size - 1) / SECTOR_SIZE;
    uint32_t sectors_to_read = last_sector - first_sector + 1;

    uint8_t *temp_buffer = (uint8_t *)kmalloc(SECTOR_SIZE); // Pour les lectures partielles
    if (!temp_buffer) {
        return -1; // Erreur d'allocation de mémoire
    }

    uint8_t *buf_ptr = (uint8_t *)buffer;

    for (uint32_t sector = first_sector; sector <= last_sector; sector++) {
        int read_result = ide_read(dev, sector, 1, temp_buffer);
        if (read_result != 0) {
            kfree(temp_buffer);
            return -1; // Erreur de lecture
        }

        uint32_t buffer_offset = (sector == first_sector) ? offset % SECTOR_SIZE : 0;
        uint32_t bytes_to_copy = SECTOR_SIZE - buffer_offset;
        if (sector == last_sector) {
            bytes_to_copy = ((offset + size - 1) % SECTOR_SIZE) - buffer_offset + 1;
        }
        if (sector == first_sector && sectors_to_read == 1) {
            bytes_to_copy = size;
        }

        memcpy(buf_ptr, temp_buffer + buffer_offset, bytes_to_copy);
        buf_ptr += bytes_to_copy;
    }

    kfree(temp_buffer);
    return 0; // Succès
}

int ide_simple_write(IDEDevice *dev, uint32_t offset, uint32_t size, const void *buffer) {
    if (!dev || !buffer || size == 0 || offset + size > dev->size * SECTOR_SIZE) {
        return -1; // Paramètres invalides
    }

    uint32_t first_sector = offset / SECTOR_SIZE;
    uint32_t last_sector = (offset + size - 1) / SECTOR_SIZE;
    uint32_t sectors_to_write = last_sector - first_sector + 1;

    uint8_t *temp_buffer = (uint8_t *)kmalloc(SECTOR_SIZE);
    if (!temp_buffer) {
        return -1; // Erreur d'allocation de mémoire
    }

    const uint8_t *buf_ptr = (const uint8_t *)buffer;

    for (uint32_t sector = first_sector; sector <= last_sector; sector++) {
        if (sector != first_sector && sector != last_sector) {
            if (ide_write(dev, sector, 1, buf_ptr) != 0) {
                kfree(temp_buffer);
                return -1; // Erreur d'écriture
            }
            buf_ptr += SECTOR_SIZE;
            continue;
        }

        // Pour les premiers et derniers secteurs, ou si tout tient dans un seul secteur
        if (ide_read(dev, sector, 1, temp_buffer) != 0) {
            kfree(temp_buffer);
            return -1; // Erreur de lecture
        }

        uint32_t buffer_offset = (sector == first_sector) ? offset % SECTOR_SIZE : 0;
        uint32_t bytes_to_write = SECTOR_SIZE - buffer_offset;
        if (sector == last_sector) {
            bytes_to_write = ((offset + size - 1) % SECTOR_SIZE) - buffer_offset + 1;
        }
        if (sector == first_sector && sectors_to_write == 1) {
            bytes_to_write = size;
        }

        memcpy(temp_buffer + buffer_offset, buf_ptr, bytes_to_write);
        if (ide_write(dev, sector, 1, temp_buffer) != 0) {
            kfree(temp_buffer);
            return -1; // Erreur d'écriture
        }

        buf_ptr += bytes_to_write;
    }

    kfree(temp_buffer);
    return 0; // Succès
}