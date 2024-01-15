/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pata.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 14:32:41 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/12 11:55:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ata.h>
#include <drivers/device/pata.h>

#include <memory/memory.h>
#include <system/io.h>

PATADevice *pata_dev = NULL;

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   PATA UTILS                                   ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief Wait for the drive to be ready
 *
 * @param dev
 */
static int __pata_wait_ready(PATADevice *dev) {
    // Wait for the drive to be not busy
    while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_BSY) != 0)
        ;

    // Wait for the drive to be ready
    while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_RDY) == 0)
        ;

    // Check if there was an error
    return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR) == 0;
}

/**
 * @brief Wait for the drive to be ready
 *
 * @param dev
 */
static void __pata_wait_for_device(PATADevice *dev) {
    // Wait for drive exist
    while (inb(ATA_REG_STATUS(dev->io_base)) == 0xFF)
        ;
}

static void delay400ns(uint16_t io_base) {
    inb(io_base + 0x206);
    inb(io_base + 0x206);
    inb(io_base + 0x206);
    inb(io_base + 0x206);
}

static int __check_disk_state(uint16_t io_base) {
    uint8_t status = inb(ATA_REG_STATUS(io_base));
    if (status & ATA_STATUS_ERR) {
        printk("ATA ERROR: Error bit set\n");
        return (1);
    }
    if (status & ATA_STATUS_DF) {
        printk("ATA ERROR: Drive fault error\n");
        return (1);
    }
    if (!(status & ATA_STATUS_RDY)) {
        printk("ATA ERROR: Drive not ready\n");
        return (1);
    }
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 PATA FUNCTIONS                                 ||
// ! ||--------------------------------------------------------------------------------||

int pata_init(uint16_t io_base, uint16_t ctrl_base) {
    if (!(pata_dev = kmalloc(sizeof(PATADevice)))) {
        __THROW("PATA_INIT: Failed to allocate memory for PATA device", 1);
    }

    pata_dev->io_base = io_base;
    pata_dev->ctrl_base = ctrl_base;
    return (0);
}

void pata_identify_device(PATADevice *dev) {
    outb(ATA_REG_HDDEVSEL(dev->io_base), ATA_SELECT_MASTER);
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_IDENTIFY);

    __pata_wait_ready(dev);

    memset(dev->id, 0, sizeof(PATAIdentify));

    uint16_t buffer[256];
    for (int i = 0; i < 256; i++) {
        buffer[i] = inw(ATA_REG_DATA(dev->io_base));
    }

    // Parse the identification data
    char model[41];
    for (int i = 0; i < 40; i += 2) {
        model[i] = buffer[27 + i / 2] >> 8;
        model[i + 1] = buffer[27 + i / 2] & 0xff;
    }
    model[40] = '\0'; // Null-terminate the string

    uint32_t sectors = buffer[60] | (buffer[61] << 16);

    // Store the parsed data in the device structure
    strncpy(dev->id->model, model, sizeof(dev->id->model));
    memcpy(dev->id->serial, &buffer[10], sizeof(dev->id->serial));
    memcpy(dev->id->sectors, buffer, sizeof(dev->id->sectors));
    dev->id->capabilities = buffer[49];
    dev->id->config = buffer[53];
    dev->id->size = sectors;
    dev->id->major_version = buffer[80];
    dev->id->minor_version = buffer[81];

    printk("PATA: Device info:\n");

    printk("\t\t\t   - PATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Model", dev->id->model);
    printk("\t\t\t   - PATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Serial", dev->id->serial);
    printk("\t\t\t   - PATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Sectors", dev->id->size);
    printk("\t\t\t   - PATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Major version", dev->id->major_version);
    printk("\t\t\t   - PATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Minor version", dev->id->minor_version);
}

uint8_t pata_identify(PATADevice *dev) {
    outb(ATA_REG_HDDEVSEL(pata_dev->io_base), ATA_SELECT_MASTER);
    outb(ATA_REG_COMMAND(pata_dev->io_base), ATA_CMD_IDENTIFY);

    __pata_wait_for_device(dev);

    uint8_t status = inb(ATA_REG_STATUS(pata_dev->io_base));

    if (status != 0x00) {
        __pata_wait_for_device(dev);

        uint16_t type = inw(ATA_REG_STATUS(pata_dev->io_base));

        if ((type & (1 << 15)) == 0) {
            printk("\t\t\t   - PATA " _YELLOW "[%d]" _END " - " _GREEN "%s" _END "\n", type, "PATA (HDD, SSD...)");
        } else {
            printk("\t\t\t   - PATA " _YELLOW "[%d]" _END " - " _GREEN "%s" _END "\n", type, "PATAPI (CD-ROM...))");
        }
    } else {
        printk("\t\t\t   - PATA " _RED "[%d]" _END " - " _RED "%s" _END "\n", 0, "No device");
        return (ATADEV_UNKNOWN);
    }

    return (0);
}

int pata_read(PATADevice *dev, uint32_t lba, uint8_t *buffer, uint32_t sectors) {
    // Select the device and set the LBA and sector count
    outb(ATA_REG_HDDEVSEL(dev->io_base), ATA_SELECT_MASTER | ((lba >> 24) & 0x0f));
    outb(ATA_REG_SECCOUNT0(dev->io_base), sectors);
    outb(ATA_REG_LBA0(dev->io_base), lba & 0xff);
    outb(ATA_REG_LBA1(dev->io_base), (lba >> 8) & 0xff);
    outb(ATA_REG_LBA2(dev->io_base), (lba >> 16) & 0xff);

    // Send the read command
    outb(ATA_REG_COMMAND(dev->io_base), ATA_SECTOR_READ);

    // Read the data
    for (uint32_t i = 0; i < sectors; i++) {
        // Wait for the device to be ready
        if (!__pata_wait_ready(dev)) {
            return -1;
        }

        // Read one sector of data
        insw(ATA_REG_DATA(dev->io_base), buffer, 256);
        buffer += 256;
    }

    // Check if there was an error
    if (inb(ATA_REG_STATUS(dev->io_base)) & ATA_REQ_ERR_BIT) {
        __WARND("ATA: Read error");
    }

    delay400ns(dev->io_base);

    return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR && __check_disk_state(dev->io_base) != 0 ? 1 : 0);
}

int pata_write(PATADevice *dev, uint32_t lba, const uint8_t *buffer, uint32_t sectors) {
    // Select the device and set the LBA and sector count
    outb(ATA_REG_HDDEVSEL(dev->io_base), ATA_SELECT_MASTER | ((lba >> 24) & 0x0f));
    outb(ATA_REG_SECCOUNT0(dev->io_base), sectors);
    outb(ATA_REG_LBA0(dev->io_base), lba & 0xff);
    outb(ATA_REG_LBA1(dev->io_base), (lba >> 8) & 0xff);
    outb(ATA_REG_LBA2(dev->io_base), (lba >> 16) & 0xff);

    // Send the write command
    outb(ATA_REG_COMMAND(dev->io_base), ATA_SECTOR_WRITE);

    // Write the data
    for (uint32_t i = 0; i < sectors; i++) {
        // Wait for the device to be ready
        if (!__pata_wait_ready(dev)) {
            return -1;
        }

        // Write one sector of data
        outsw(ATA_REG_DATA(dev->io_base), buffer, 256);
        buffer += 256;
    }

    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_FLUSH); // Send the FLUSH CACHE command

    // Check if there was an error
    if (inb(ATA_REG_STATUS(dev->io_base)) & ATA_REQ_ERR_BIT) {
        __WARND("ATA: Write error");
    }

    delay400ns(dev->io_base);

    return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR && __check_disk_state(dev->io_base) != 0 ? 1 : 0);
}