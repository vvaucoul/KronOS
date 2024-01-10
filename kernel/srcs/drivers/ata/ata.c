/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ata.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:22:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 13:19:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/ata/ata.h>
#include <memory/memory.h>
#include <system/io.h>

ATADevice *dev = NULL;

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    ATA utils                                   ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief Wait for the drive to be ready
 *
 * @param dev
 */
static int __ata_wait_ready(ATADevice *dev) {
    // Wait for the drive to be not busy
    while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_BSY) != 0)
        ;

    // Wait for the drive to be ready
    while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_DRQ) == 0)
        ;

    return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR) == 0;
}

/**
 * @brief Wait for the drive to be ready
 *
 * @param dev
 */
static void __ata_wait_for_drive(ATADevice *dev) {
    // Wait for the drive to exist
    while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_REQ_MASTER_DRIVE) != 0)
        ;
}

static void delay400ns(uint16_t io_base) {
    inb(io_base + 0x206);
    inb(io_base + 0x206);
    inb(io_base + 0x206);
    inb(io_base + 0x206);
}

static void __display_disk_state(void) {
    /* Print drive status */
    uint8_t status = inb(ATA_REG_STATUS(dev->io_base));
    if (status & ATA_STATUS_ERR) {
        printk("\t\t\t   - ATA: Drive status: " _RED "[ERROR]" _END "\n");
    } else {
        printk("\t\t\t   - ATA: Drive status: " _GREEN "[OK]" _END "\n");
    }
    if (status & ATA_STATUS_DF) {
        printk("\t\t\t   - ATA: Drive fault: " _RED "[ERROR]" _END "\n");
    } else {
        printk("\t\t\t   - ATA: Drive fault: " _GREEN "[OK]" _END "\n");
    }
    if (!(status & ATA_STATUS_RDY)) {
        printk("\t\t\t   - ATA: Drive ready: " _RED "[ERROR]" _END "\n");
    } else {
        printk("\t\t\t   - ATA: Drive ready: " _GREEN "[OK]" _END "\n");
    }
    if (status & ATA_STATUS_BSY) {
        printk("\t\t\t   - ATA: Drive busy: " _RED "[ERROR]" _END "\n");
    } else {
        printk("\t\t\t   - ATA: Drive busy: " _GREEN "[OK]" _END "\n");
    }
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
// ! ||                                  ATA Functions                                 ||
// ! ||--------------------------------------------------------------------------------||

int ata_init(uint16_t io_base, uint16_t ctrl_base) {
    if (!(dev = (ATADevice *)kmalloc(sizeof(ATADevice)))) {
        __THROW("Failed to allocate memory for ATA device", 1);
    }

    dev->io_base = io_base;
    dev->ctrl_base = ctrl_base;
    return 0;
}

void ata_identify_devide(ATADevice *dev) {
    outb(ATA_REG_DRIVE_SELECT(dev->io_base), ATA_SELECT_MASTER);
    outb(ATA_REG_COMMAND(dev->io_base), ATA_IDENTIFY_CMD);

    __ata_wait_ready(dev);

    memset(dev->id, 0, sizeof(ata_identify_t));

    for (uint32_t i = 0; i < 256; i++) {
        ((uint16_t *)dev->id)[i] = inw(ATA_REG_DATA(dev->io_base));
    }

    // The strings are in Big Endian, convert them to Little Endian
    for (int i = 0; i < 20; i += 2) {
        char temp = dev->id->serial_number[i];
        dev->id->serial_number[i] = dev->id->serial_number[i + 1];
        dev->id->serial_number[i + 1] = temp;
    }
    for (int i = 0; i < 8; i += 2) {
        char temp = dev->id->firmware_revision[i];
        dev->id->firmware_revision[i] = dev->id->firmware_revision[i + 1];
        dev->id->firmware_revision[i + 1] = temp;
    }
    for (int i = 0; i < 40; i += 2) {
        char temp = dev->id->model_number[i];
        dev->id->model_number[i] = dev->id->model_number[i + 1];
        dev->id->model_number[i + 1] = temp;
    }

    printk("ATA: Device info:\n");

    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "General config", dev->id->general_config);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Logical cylinders", dev->id->logical_cylinders);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Logical heads", dev->id->logical_heads);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Logical sectors", dev->id->logical_sectors);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Serial number", dev->id->serial_number);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Firmware revision", dev->id->firmware_revision);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Model number", dev->id->model_number);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Reserved 2", dev->id->reserved2);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Retired 4", dev->id->retired4);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Retired 20", dev->id->retired20);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Retired 23", dev->id->obsolete23);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%d" _END "\n", "Vendor 7", dev->id->vendor7);

    delay400ns(dev->io_base);
}

uint8_t ata_identify(ATADevice *dev) {
    outb(ATA_REG_DRIVE_SELECT(dev->io_base), ATA_SELECT_MASTER); // Select the master drive
    outb(ATA_REG_STATUS(dev->io_base), ATA_IDENTIFY_CMD);        // Send the IDENTIFY command

    if (inb(ATA_REG_STATUS(dev->io_base)) != 0x00) { // If the value is not 0, the drive exists
        // Wait for the drive to be ready
        __ata_wait_for_drive(dev);

        // Read the first word from the IDENTIFY response
        uint16_t type = inw(dev->io_base);

        // Check bit 15 to determine the drive type
        if ((type & (1 << 15)) == 0) {
            printk("\t\t\t   - ATA " _YELLOW "[%d]" _END " - " _GREEN "%s" _END "\n", type, "ATA (HDD, SSD...)");
        } else {
            printk("\t\t\t   - ATA " _YELLOW "[%d]" _END " - " _GREEN "%s" _END "\n", type, "ATAPI (CD-ROM...))");
        }
    } else {
        printk("\t\t\t   - ATA " _RED "[%d]" _END " - " _RED "%s" _END "\n", 0, "No device");
        return (1);
    }

    delay400ns(dev->io_base);

    __display_disk_state();

    return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR && __check_disk_state(dev->io_base) != 0 ? 1 : 0);
}

int ata_read(ATADevice *dev, uint32_t lba, uint8_t *buffer, uint32_t sectors) {
    // This is a very simplified implementation that only works with LBA28
    // and doesn't handle errors. A full implementation would need to use LBA48
    // and handle errors properly.

    outb(ATA_REG_DRIVE_SELECT(dev->io_base), 0xE0 | ((lba >> 24) & 0x0F)); // Select the drive and set the high 4 bits of the LBA
    outb(ATA_REG_SECTOR_COUNT((dev->io_base)), sectors);                   // Set the sector count
    outb(ATA_REG_LBA_LO((dev->io_base)) + 3, lba);                         // Set the low 8 bits of the LBA
    outb(ATA_REG_LBA_MID((dev->io_base)), lba >> 8);                       // Set the next 8 bits of the LBA
    outb(ATA_REG_LBA_HI((dev->io_base)), lba >> 16);                       // Set the next 8 bits of the LBA
    outb(ATA_REG_STATUS(dev->io_base), ATA_STATUS_DF);                     // Send the READ SECTORS command

    outb(ATA_REG_COMMAND(dev->io_base), ATA_SECTOR_READ); // Send the READ SECTORS command

    for (uint32_t i = 0; i < sectors; i++) {

        __ata_wait_ready(dev);

        // Wait for the drive to be ready
        while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_REQ_DRQ_BIT) == 0)
            ;

        // Read the sector into the buffer
        for (uint32_t j = 0; j < 256; j++) {
            ((uint16_t *)buffer)[i * 256 + j] = inw(dev->io_base);
        }

        if (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR) {
            printk("ATA: Error bit set\n");
            break;
        }
    }

    // Check if there was an error
    if (inb(ATA_REG_STATUS(dev->io_base)) & ATA_REQ_ERR_BIT) {
        __WARND("ATA: Read error");
    }

    delay400ns(dev->io_base);

    return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR && __check_disk_state(dev->io_base) != 0 ? 1 : 0);
}

int ata_write(ATADevice *dev, uint32_t lba, const uint8_t *buffer, uint32_t sectors) {
    // This is a very simplified implementation that only works with LBA28
    // and doesn't handle errors. A full implementation would need to use LBA48
    // and handle errors properly.

    outb(ATA_REG_DRIVE_SELECT(dev->io_base), 0xE0 | ((lba >> 24) & 0x0F)); // Select the drive and set the high 4 bits of the LBA
    outb(ATA_REG_SECTOR_COUNT((dev->io_base)), sectors);                   // Set the sector count
    outb(ATA_REG_LBA_LO((dev->io_base)) + 3, lba);                         // Set the low 8 bits of the LBA
    outb(ATA_REG_LBA_MID((dev->io_base)), lba >> 8);                       // Set the next 8 bits of the LBA
    outb(ATA_REG_LBA_HI((dev->io_base)), lba >> 16);                       // Set the next 8 bits of the LBA
    outb(ATA_REG_STATUS(dev->io_base), ATA_STATUS_DF);                     // Send the READ SECTORS command

    outb(ATA_REG_COMMAND(dev->io_base), ATA_SECTOR_WRITE); // Send the WRITE SECTORS command

    for (uint32_t i = 0; i < sectors; i++) {

        __ata_wait_ready(dev);

        // Wait for the drive to be ready
        while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_REQ_DRQ_BIT) == 0)
            ;

        // Write the sector from the buffer
        for (uint32_t j = 0; j < 256; j++) {
            outw(dev->io_base, ((uint16_t *)buffer)[i * 256 + j]);
        }

        if (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR) {
            printk("ATA: Error bit set\n");
            break;
        }
    }

    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_FLUSH); // Send the FLUSH CACHE command

    // Check if there was an error
    if (inb(ATA_REG_STATUS(dev->io_base)) & ATA_REQ_ERR_BIT) {
        __WARND("ATA: Write error");
    }

    delay400ns(dev->io_base);

    return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR && __check_disk_state(dev->io_base) != 0 ? 1 : 0);
}