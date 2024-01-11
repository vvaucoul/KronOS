/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ata.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:22:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/11 18:52:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ata.h>

#include <drivers/device/devices.h>

#include <memory/memory.h>
#include <system/io.h>

ATADevice *ata_devices[MAX_ATA_DEVICES];

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    ATA UTILS                                   ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief Select the ATA drive
 *
 * @param io_base
 * @param master
 *
 * @note: This function will select the master or slave drive on the channel
 */
static void __ata_select_drive(uint16_t io_base, int master) {
    outb(ATA_REG_HDDEVSEL(io_base), master ? ATA_SELECT_MASTER : ATA_SELECT_SLAVE);
}

static void __ata_wait_disk_data_ready(uint16_t io_base) {
    while ((inb(ATA_REG_STATUS(io_base)) & ATA_STATUS_DRQ) == 0)
        ;
}

static void __ata_wait_disk_busy_clear(uint16_t io_base) {
    while ((inb(ATA_REG_STATUS(io_base)) & ATA_STATUS_BSY) != 0)
        ;
}

/**
 * @brief Wait for the drive to be ready
 *
 * @param dev
 */
static int __ata_wait_ready(ATADevice *dev) {
    __ata_select_drive(dev->io_base, dev->master);
    // Wait for the drive to be not busy
    while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_BSY) != 0) {
        if ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR) != 0) {
            return (1);
        }
    }

    // Wait for the drive to be ready
    while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_DRQ) == 0) {

        if ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR) != 0) {
            return (1);
        }
    }

    // Check if there was an error
    return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR) == 0;
}

/**
 * @brief Wait for the drive to be ready
 *
 * @param io_base
 * @param master
 *
 * @return int
 *
 * @note: This function will return 1 if the drive is present, 0 otherwise
 */
static int __ata_drive_is_present(uint16_t io_base, int master) {
    __ata_select_drive(io_base, master);

    // Reset the drive
    outb(ATA_REG_SECTOR_COUNT(io_base), 0x00);
    outb(ATA_REG_LBA_LOW(io_base), 0x00);
    outb(ATA_REG_LBA_MID(io_base), 0x00);
    outb(ATA_REG_LBA_HIGH(io_base), 0x00);

    outb(ATA_REG_COMMAND(io_base), ATA_IDENTIFY_CMD);

    // Lisez le statut
    uint8_t status = inb(ATA_REG_STATUS(io_base));

    // Wait busy clear
    while ((status & ATA_STATUS_BSY) != 0) {
        status = inb(ATA_REG_STATUS(io_base));
    }

    return (status != 0);
    // return ((inb(ATA_REG_STATUS(io_base)) & (master ? ATA_REQ_MASTER_DRIVE : ATA_REQ_SLAVE_DRIVE)) != 0);
}

/**
 * @brief Delay 400ns
 *
 * @note: Delay 400ns (Handle ATA delay)
 */
static void delay400ns(uint16_t io_base) {
    for (int i = 0; i < 4; ++i) {
        inb(io_base + 0x0C);
    }
}

void __ata_display_disk_state(ATADevice *dev) {
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
    if (status & ATA_STATUS_DRQ) {
        printk("\t\t\t   - ATA: Drive DRQ: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t\t   - ATA: Drive DRQ: " _RED "[ERROR]" _END "\n");
    }
    if (status & ATA_STATUS_SRV) {
        printk("\t\t\t   - ATA: Drive SRV: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t\t   - ATA: Drive SRV: " _RED "[ERROR]" _END "\n");
    }
}

/**
 * @brief Check the disk state
 *
 * @param io_base
 * @return int
 *
 * @note: This function will return 0 if the disk is ready, otherwise it will return the error code
 * The function must be called after a 'ATA_REG_COMMAND' has been sent to the disk
 *
 * eg: outb(ATA_REG_COMMAND(io_base), ATA_IDENTIFY_CMD);
 * then: __ata_check_disk_state(io_base);
 */
static int __ata_check_disk_state(uint16_t io_base) {
    uint8_t status = inb(ATA_REG_STATUS(io_base));
    if (status & ATA_STATUS_ERR) {
        return (ATA_STATUS_ERR);
    }
    if (status & ATA_STATUS_DF) {
        return (ATA_STATUS_DF);
    }
    if ((status & ATA_STATUS_RDY) == 0) {
        return (ATA_STATUS_RDY);
    }
    if (status & ATA_STATUS_BSY) {
        return (ATA_STATUS_BSY);
    }
    if ((status & ATA_STATUS_DRQ) == 0) {
        return (ATA_STATUS_DRQ);
    }
    if ((status & ATA_STATUS_SRV) == 0) {
        return (ATA_STATUS_SRV);
    }
    return (0);
}

int __ata_check_device(ATADevice *dev, int master) {
    outb(ATA_REG_HDDEVSEL(dev->io_base), master ? ATA_SELECT_MASTER : ATA_SELECT_SLAVE);
    outb(ATA_REG_COMMAND(dev->io_base), ATA_IDENTIFY_CMD);

    // Attendez que le périphérique soit prêt
    __ata_wait_ready(dev);

    // Lisez le registre de statut
    uint8_t status = inb(ATA_REG_STATUS(dev->io_base));

    // Si le bit BSY est défini, le périphérique n'est pas prêt
    if (status & ATA_STATUS_BSY) {
        return 0;
    }

    // Si le bit DRQ est défini, le périphérique a des données à lire
    if (status & ATA_STATUS_DRQ) {
        return 1;
    }

    // Sinon, le périphérique n'est pas présent
    return 0;
}

int __ata_check_devices(ATADevice *dev) {
    int master_present = __ata_check_device(dev, 1);
    int slave_present = __ata_check_device(dev, 0);

    return master_present + slave_present;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  ATA FUNCTIONS                                 ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief ATA Functions
 */

void ata_primary_irq_handler(__attribute__((unused)) struct regs *regs) {
    pic8259_send_eoi(IRQ_ATA1);
}

void ata_secondary_irq_handler(__attribute__((unused)) struct regs *regs) {
    pic8259_send_eoi(IRQ_ATA2);
}

/**
 * @brief Initialize an ATA device
 *
 * @param channel
 * @param ctrl_base
 * @param master
 * @return ATADevice*
 *
 * @note: This function will return NULL if the device is not present on the channel
 */
static ATADevice *ata_device_init(uint16_t channel, uint16_t ctrl_base, int master) {
    ATADevice *dev = NULL;

    // Check if the device is present on the channel
    // Check if the device is ready
    if ((__ata_drive_is_present(channel, master)) == 0) {
        __INFO("ATA: No device found on channel [%d][%d][%d]", NULL, channel, ctrl_base, master);
    } else if ((__ata_check_disk_state(channel)) != 0) {
        __INFO("ATA: Device not available on channel [%d][%d][%d]", NULL, channel, ctrl_base, master);
    }

    // Allocate memory for the device
    if (!(dev = (ATADevice *)kmalloc(sizeof(ATADevice)))) {
        __THROW("Failed to allocate memory for ATA device", NULL);
    }

    // Set the IO and control base
    dev->io_base = channel;
    dev->ctrl_base = ctrl_base;
    dev->master = master;

    // Identify the device
    if (ata_identify(dev) != 0) {
        kfree(dev);
        __WARN("ATA: Failed to identify device on channel [%d]", NULL, channel);
        return NULL;
    } else {
        Device *device = device_init_new_device("ATA", DEVICE_BLOCK, ata_device_read, ata_device_write, dev);

        if (device == NULL) {
            __WARN("ATA: Failed to register device on channel [%d]", NULL, channel);
            return NULL;
        } else {
            int ret = device_register(device);

            __INFOD("ATA: Device registered on index "_GREEN"[%d]"_END"", ret);
            return (dev);
        }
    }
    return (dev);
}

int ata_init(void) {
    // Init ATA devices
    ata_devices[0] = ata_device_init(ATA_PRIMARY_IO, ATA_PRIMARY_DEV_CTRL, 1); // Primary Master
    ata_devices[1] = ata_device_init(ATA_PRIMARY_IO, ATA_PRIMARY_DEV_CTRL, 0); // Primary Slave

    // Init ATA devices secondary channel
    ata_devices[2] = ata_device_init(ATA_SECONDARY_IO, ATA_SECONDARY_DEV_CTRL, 1); // Secondary Master
    ata_devices[3] = ata_device_init(ATA_SECONDARY_IO, ATA_SECONDARY_DEV_CTRL, 0); // Secondary Slave

    // Setup interrupts for ATA devices (IRQ 14 & 15)
    irq_install_handler(IRQ_ATA1, ata_primary_irq_handler);
    irq_install_handler(IRQ_ATA2, ata_secondary_irq_handler);

    // Identifier les dispositifs
    for (int i = 0; i < MAX_ATA_DEVICES; ++i) {
        if (ata_devices[i] != NULL) {
            __ata_display_disk_state(ata_devices[i]);
            // print device infos:

            printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Serial number", ata_devices[i]->identity->serial_number);
            printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Firmware revision", ata_devices[i]->identity->firmware_revision);
            printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Model number", ata_devices[i]->identity->model_number);
        }
    }

    return (0);
}

static int __ata_identify_read_data(ATADevice *dev) {
    uint16_t buffer[256];

    memset(buffer, 0, sizeof(buffer));

    __ata_wait_disk_data_ready(dev->io_base);

    uint32_t i = 0;
    while ((inb((ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_DRQ) != 0) && i < 256) {
        ((uint16_t *)buffer)[i] = inw(ATA_REG_DATA(dev->io_base));
        i++;
    }

    dev->identity = (ATAIdentity *)buffer;

    // The strings are in Big Endian, convert them to Little Endian
    for (int i = 0; i < 20; i += 2) {
        char temp = dev->identity->serial_number[i];
        dev->identity->serial_number[i] = dev->identity->serial_number[i + 1];
        dev->identity->serial_number[i + 1] = temp;
    }
    for (int i = 0; i < 8; i += 2) {
        char temp = dev->identity->firmware_revision[i];
        dev->identity->firmware_revision[i] = dev->identity->firmware_revision[i + 1];
        dev->identity->firmware_revision[i + 1] = temp;
    }
    for (int i = 0; i < 40; i += 2) {
        char temp = dev->identity->model_number[i];
        dev->identity->model_number[i] = dev->identity->model_number[i + 1];
        dev->identity->model_number[i + 1] = temp;
    }

    delay400ns(dev->io_base);
    __ata_wait_disk_busy_clear(dev->io_base);

    return (0);
}

int ata_identify(ATADevice *dev) {
    __ata_select_drive(dev->io_base, dev->master);

    outb(ATA_REG_SECTOR_COUNT(dev->io_base), 0x00);
    outb(ATA_REG_LBA_LOW(dev->io_base), 0x00);
    outb(ATA_REG_LBA_MID(dev->io_base), 0x00);
    outb(ATA_REG_LBA_HIGH(dev->io_base), 0x00);

    outb(ATA_REG_STATUS(dev->io_base), ATA_IDENTIFY_CMD);

    int drive_state = inb(ATA_REG_STATUS(dev->io_base));
    printk("Drive [%d][%d] state: %d\n", dev->io_base, dev->master, drive_state);

    // If the value is not 0, the drive is present
    if (drive_state) {

        // Wait for the drive to be ready
        __ata_wait_disk_busy_clear(dev->io_base);

        do {
            drive_state = inb(ATA_REG_STATUS(dev->io_base));

            if (drive_state & ATA_STATUS_ERR) {
                __THROW("ATA: Error bit set", 1);
            }

        } while ((drive_state & ATA_STATUS_DRQ) == 0);

        __ata_identify_read_data(dev);

        return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR && __ata_check_disk_state(dev->io_base) != 0 ? 1 : 0);
    }
    return (1);
}

ATADevice *ata_get_device(uint32_t i) {
    if (i > MAX_ATA_DEVICES) {
        return (NULL);
    } else {
        return (ata_devices[i]);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                              ATA DISPLAY FUNCTIONS                             ||
// ! ||--------------------------------------------------------------------------------||

void ata_disk_details(uint32_t i) {
    ATADevice *device = ata_get_device(i);

    if (device == NULL) {
        __INFO_NO_RETURN("ATA: Cannot get device [%d]", i);
    }

    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Serial number", device->identity->serial_number);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Firmware revision", device->identity->firmware_revision);
    printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Model number", device->identity->model_number);
}

void ata_disk_size(uint32_t i) {
    ATADevice *device = ata_get_device(i);

    if (device == NULL) {
        __INFO_NO_RETURN("ATA: Cannot get device [%d]", i);
    }

    uint64_t disk_capacity = (uint64_t)device->identity->logical_sectors * 512;
    printk("\t\t- ATA " _YELLOW "[Disk capacity ("_GREEN
           " %d "_END
           " Octets) ("_GREEN
           " %d "_END
           " Mo) ("_GREEN
           " %d "_END
           " Go)]" _END "\n",
           disk_capacity, disk_capacity / (1024 * 1024), disk_capacity / (1024 * 1024 * 1024));
}

int ata_disk_count(void) {
    int i = 0;
    int count = 0;

    while (i < 4) {
        ATADevice *device = ata_get_device(i);
        if (ata_identify(device) != 0) {
            ++count;
        }
        ++i;
    }
    return (count);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                           ATA READ / WRITE FUNCTIONS                           ||
// ! ||--------------------------------------------------------------------------------||

int ata_read(ATADevice *dev, uint32_t lba, uint8_t *buffer, uint32_t sectors) {
    // This is a very simplified implementation that only works with LBA28
    // and doesn't handle errors. A full implementation would need to use LBA48
    // and handle errors properly.

    outb(ATA_REG_HDDEVSEL(dev->io_base), 0xE0 | ((lba >> 24) & 0x0F)); // Select the drive and set the high 4 bits of the LBA
    outb(ATA_REG_SECTOR_COUNT((dev->io_base)), sectors);               // Set the sector count
    outb(ATA_REG_LBA_LOW((dev->io_base)) + 3, lba);                    // Set the low 8 bits of the LBA
    outb(ATA_REG_LBA_MID((dev->io_base)), lba >> 8);                   // Set the next 8 bits of the LBA
    outb(ATA_REG_LBA_HIGH((dev->io_base)), lba >> 16);                 // Set the next 8 bits of the LBA
    outb(ATA_REG_STATUS(dev->io_base), ATA_STATUS_DF);                 // Send the READ SECTORS command

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

    return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR && __ata_check_disk_state(dev->io_base) != 0 ? 1 : 0);
}

int ata_write(ATADevice *dev, uint32_t lba, const uint8_t *buffer, uint32_t sectors) {
    // This is a very simplified implementation that only works with LBA28
    // and doesn't handle errors. A full implementation would need to use LBA48
    // and handle errors properly.

    outb(ATA_REG_HDDEVSEL(dev->io_base), 0xE0 | ((lba >> 24) & 0x0F)); // Select the drive and set the high 4 bits of the LBA
    outb(ATA_REG_SECTOR_COUNT((dev->io_base)), sectors);               // Set the sector count
    outb(ATA_REG_LBA_LOW((dev->io_base)) + 3, lba);                    // Set the low 8 bits of the LBA
    outb(ATA_REG_LBA_MID((dev->io_base)), lba >> 8);                   // Set the next 8 bits of the LBA
    outb(ATA_REG_LBA_HIGH((dev->io_base)), lba >> 16);                 // Set the next 8 bits of the LBA
    outb(ATA_REG_STATUS(dev->io_base), ATA_STATUS_DF);                 // Send the READ SECTORS command

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

    return (inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_ERR && __ata_check_disk_state(dev->io_base) != 0 ? 1 : 0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                              ATA BLOCKS FUNCTIONS                              ||
// ! ||--------------------------------------------------------------------------------||

uint32_t ata_device_read(void *device, uint32_t lba, uint32_t count, void *buffer) {
    if (device == NULL || buffer == NULL) {
        __THROW("ATA: Cannot read from device", 1);
    }

    if (ata_read((ATADevice *)device, lba, buffer, count) != 0) {
        __THROW("ATA: Cannot read from device", 1);
    }

    return (0);
}

uint32_t ata_device_write(void *device, uint32_t lba, uint32_t count, void *buffer) {
    if (device == NULL || buffer == NULL) {
        __THROW("ATA: Cannot write to device", 1);
    }

    if (ata_write((ATADevice *)device, lba, buffer, count) != 0) {
        __THROW("ATA: Cannot write to device", 1);
    }

    return (0);
}

// int ata_block_read(BlockDevice *device, uint64_t block, uint64_t count, void *buf) {
//     if (device == NULL || buf == NULL) {
//         __THROW("ATA: Cannot read from block device", 1);
//     }

//     if (block_read(device, block, count, buf) != 0) {
//         __THROW("ATA: Cannot read from block device", 1);
//     }

//     return (0);
// }

// int ata_block_write(BlockDevice *device, uint64_t block, uint64_t count, void *buf) {
//     if (device == NULL || buf == NULL) {
//         __THROW("ATA: Cannot write to block device", 1);
//     }

//     if (block_write(device, block, count, buf) != 0) {
//         __THROW("ATA: Cannot write to block device", 1);
//     }

//     return (0);
// }