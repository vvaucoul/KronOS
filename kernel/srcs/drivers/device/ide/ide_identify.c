/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ide_identify.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 13:55:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/19 15:00:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ide.h>
#include <memory/memory.h>
#include <system/io.h>

// ! ||--------------------------------------------------------------------------------||
// ! ||                                       LBA                                      ||
// ! ||--------------------------------------------------------------------------------||

IDELBA __ide_get_lba(uint32_t lba) {
    uint8_t device;

    if (lba > 0x0FFFFFFF) {
        device = 0xE0;
    } else if (lba > 0x00FFFFFF) {
        device = 0xE0 | ((lba >> 24) & 0x0F);
    } else if (lba > 0x0000FFFF) {
        device = 0xF0 | ((lba >> 16) & 0x0F);
    } else {
        device = 0xF0 | ((lba >> 24) & 0x0F);
    }

    return (IDELBA){
        .lba = (uint8_t)(lba & LBA_MASK_8BIT),
        .lba_high = (uint8_t)((lba & LBA_MASK_16BIT) >> 8),
        .lba_mid = (uint8_t)((lba & LBA_MASK_24BIT) >> 16),
        .device = device,
    };
}

IDELBAMode __ide_detect_geometry(const uint16_t buffer[256]) {
    uint8_t majorVersion = buffer[1];

    if (majorVersion == 0 || majorVersion >= 8) {
        return (IDE_LBA28);
    } else if (majorVersion == 1 && buffer[9] & 0x8000) {
        return (IDE_LBA48);
    } else {
        return (IDE_CHS);
    }
}

static uint32_t __ide_get_sector_size(const uint16_t buffer[256]) {
    uint32_t sector_size = 0;

    if (buffer[106] & (1 << 12)) {
        sector_size = 4096;
    } else {
        sector_size = 512;
    }
    return (sector_size);
}

static uint32_t __ide_get_sector_count(const uint16_t buffer[256]) {
    uint32_t sector_count = 0;

    if (buffer[61] != 0) {
        sector_count = buffer[61];
    } else if (buffer[60] != 0) {
        sector_count = buffer[60];
    }
    return (sector_count);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   DEVICE TYPE                                  ||
// ! ||--------------------------------------------------------------------------------||

static IDEType __ide_get_devicetype(uint16_t base, uint16_t ctrl) {
    uint8_t cl = inb(base + ATA_REG_LBA1);
    uint8_t ch = inb(base + ATA_REG_LBA2);

    if (cl == 0x14 && ch == 0xEB) {
        return (IDE_ATAPI);
    }
    if (cl == 0x69 && ch == 0x96) {
        return (IDE_ATAPI);
    }
    return (IDE_ATA);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    IDENTIFY                                    ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief Identifies the IDE device connected to the specified bus and drive.
 *
 * This function takes the bus and drive parameters and identifies the IDE device
 * connected to that specific bus and drive. It returns an integer value representing
 * the identification result.
 *
 * @param bus The bus number to identify the device on.
 * @param drive The drive number to identify the device on.
 * @return An integer value representing the identification result.
 */

int ide_identify(IDEDevice *dev) {
    if (dev == NULL) {
        __THROW("IDE Device is NULL", -1);
    }

    uint16_t buffer[256];

    // Select the drive
    outb(dev->regs.base + ATA_REG_HDDEVSEL, ATA_CMD_PACKET | (dev->drive << 4));

    // Set values at 0 for the ATA registers
    outb(dev->regs.base + ATA_REG_SECCOUNT0, 0);
    outb(dev->regs.base + ATA_REG_LBA0, 0);
    outb(dev->regs.base + ATA_REG_LBA1, 0);
    outb(dev->regs.base + ATA_REG_LBA2, 0);

    // Send the IDENTIFY command
    outb(dev->regs.base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

    // Wait for the BSY flag to be cleared
    if (inb(dev->regs.base + ATA_REG_STATUS) == 0) {
        __THROW("No device found on channel [%d] drive [%d]", dev->channel, dev->drive);
    }

    // Wait for the DRQ flag to be set
    while ((inb(dev->regs.base + ATA_REG_STATUS) & ATA_SR_DRQ) == 0)
        ;

    // Read the data from the buffer
    for (int i = 0; i < 256; ++i) {
        buffer[i] = inw(dev->regs.base + ATA_REG_DATA);
    }

    // Get the sector size (512 or 4096) / LBA mode (CHS, LBA28, LBA48) / device type (ATA, ATAPI)
    dev->sector_size = __ide_get_sector_size(buffer);
    dev->sector_count = __ide_get_sector_count(buffer);
    dev->lba_mode = __ide_detect_geometry(buffer);
    dev->type = __ide_get_devicetype(dev->regs.base, dev->regs.ctrl);

    // Get signature / capabilities / command sets / disk size
    dev->signature = buffer[0];
    dev->capabilities = buffer[49];
    dev->commandsets = buffer[83];
    dev->size = buffer[60] | (buffer[61] << 16);

    // Get model
    for (int i = 0; i < 40; i += 2) {
        dev->model[i] = buffer[27 + i / 2] >> 8;
        dev->model[i + 1] = buffer[27 + i / 2] & 0xFF;
    }
    dev->model[40] = 0;

    // Get Firmware Revision

    for (int i = 0; i < 8; i += 2) {
        dev->firmware[i] = buffer[23 + i / 2] >> 8;
        dev->firmware[i + 1] = buffer[23 + i / 2] & 0xFF;
    }
    dev->firmware[8] = 0;

    return (0);
}

/**
 * @brief Checks if a device is present on the specified bus.
 *
 * This function takes the bus number and checks if a device is present on that bus.
 * It returns an integer value representing the presence of a device.
 *
 * @param bus The bus number to check for a device on.
 * @return An integer value representing the presence of a device.
 */
int ide_device_is_present(IDEChannel channel, IDEDrive drive) {
    uint32_t base = (channel == IDE_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

    // Select the drive
    outb(base + ATA_REG_HDDEVSEL, ATA_CMD_PACKET | (drive << 4));

    // Set values at 0 for the ATA registers
    outb(base + ATA_REG_SECCOUNT0, 0);
    outb(base + ATA_REG_LBA0, 0);
    outb(base + ATA_REG_LBA1, 0);
    outb(base + ATA_REG_LBA2, 0);

    // Send the IDENTIFY command
    outb(base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

    uint8_t status = inb(base + ATA_REG_STATUS);

    if (status == 0 || status & ATA_SR_ERR || status & ATA_SR_DF) {
        return (0);
    }
    return (1);
}