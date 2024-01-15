/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ata.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:22:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/13 19:14:05 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ata.h>

#include <drivers/device/devices.h>

#include <memory/memory.h>
#include <system/io.h>

ATADevice *ata_devices[MAX_ATA_DEVICES];

static int ata_identify(ATADevice *dev);
static void delay400ns(uint16_t io_base);

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
    for (uint8_t i = 0; i < 4; ++i) {
        inb(ATA_REG_STATUS(io_base));
    }
    while ((inb(ATA_REG_STATUS(io_base)) & ATA_STATUS_BSY) != 0)
        ;

    while ((inb(ATA_REG_STATUS(io_base)) & ATA_STATUS_DRQ) == 0) {
        if ((inb(ATA_REG_STATUS(io_base)) & ATA_STATUS_ERR) != 0) {
            __THROW_NO_RETURN("ATA: Error in __ata_wait_disk_data_ready");
        }
    }
}

static void __ata_wait_disk_busy_clear(uint16_t io_base) {
    while ((inb(ATA_REG_STATUS(io_base)) & ATA_STATUS_BSY) != 0)
        ;
}

static void __ata_polling(uint16_t io_base) {
    __ata_wait_disk_data_ready(io_base);
    while ((inb(ATA_REG_STATUS(io_base)) & ATA_STATUS_DRQ) == 0) {
        if ((inb(ATA_REG_STATUS(io_base)) & ATA_STATUS_ERR) != 0) {
            __THROW_NO_RETURN("ATA: Error in __ata_polling");
        }
    }
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
    outb(ATA_REG_SECCOUNT0(io_base), 0x00);
    outb(ATA_REG_LBA0(io_base), 0x00);
    outb(ATA_REG_LBA1(io_base), 0x00);
    outb(ATA_REG_LBA2(io_base), 0x00);
    // Send IDENTIFY command
    outb(ATA_REG_COMMAND(io_base), ATA_CMD_IDENTIFY);

    delay400ns(io_base);

    // Read the status register
    uint8_t status = inb(ATA_REG_STATUS(io_base));

    // If status is 0, no drive is connected
    if (status == 0) {
        return 0;
    }

    // Wait for BSY bit to clear, indicating the drive is no longer busy
    while ((status & ATA_STATUS_BSY) != 0) {
        status = inb(ATA_REG_STATUS(io_base));
    }

    // At this point, if ERR is set, the IDENTIFY command failed
    if (status & ATA_STATUS_ERR) {
        return 0;
    }

    // Check if the drive is present by reading LBA1 and LBA2 registers
    return (inb(ATA_REG_LBA1(io_base)) == 0x00 && inb(ATA_REG_LBA2(io_base)) == 0x00);
}

/**
 * @brief Get the LBA mode
 *
 * @param io_base
 * @param master
 * @return ATALBA
 *
 * @note: This function will return the LBA mode (28 or 48 if supported)
 */

#if __ALLOW_LBA48__ == 1
static ATALBA __ata_get_lba_mode(uint16_t io_base, int master) {
    __ata_select_drive(io_base, master);

    outb(ATA_REG_SECCOUNT0(io_base), 0x55);
    outb(ATA_REG_LBA0(io_base), 0xAA);
    outb(ATA_REG_COMMAND(io_base), ATA_CMD_IDENTIFY);

    uint8_t status = inb(ATA_REG_STATUS(io_base));

    if (status == 0) {
        return (LBA_28);
    } else {
        return (LBA_48);
    }
}
#endif

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
    /* Select drive */
    __ata_select_drive(ATA_PRIMARY_IO, 1);

    /* Print drive status */
    uint8_t status = inb(ATA_REG_STATUS(dev->io_base));
    if ((status & ATA_STATUS_ERR) == 0) {
        printk("\t\t\t   - ATA: Drive status: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t\t   - ATA: Drive status: " _RED "[ERROR]" _END "\n");
    }
    if ((status & ATA_STATUS_DF) == 0) {
        printk("\t\t\t   - ATA: Drive fault: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t\t   - ATA: Drive fault: " _RED "[ERROR]" _END "\n");
    }
    if (status & ATA_STATUS_RDY) {
        printk("\t\t\t   - ATA: Drive ready: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t\t   - ATA: Drive ready: " _RED "[ERROR]" _END "\n");
    }
    if ((status & ATA_STATUS_BSY) == 0) {
        printk("\t\t\t   - ATA: Drive busy: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t\t   - ATA: Drive busy: " _RED "[ERROR]" _END "\n");
    }
    if ((status & ATA_STATUS_DRQ) == 0) {
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

static uint8_t __ata_get_error_code(uint32_t io_base, int master) {
    __ata_select_drive(io_base, master);

    return (inb(ATA_REG_ERROR(io_base)));
}

static uint32_t __ata_get_capacity(ATADevice *dev) {

    // LBA Mode
    if (dev->identity->capabilities & 0x0200) {
        uint32_t total_sectors = dev->identity->MaxLBA;
        printk("ATA: Total sectors (LBA): %d\n", total_sectors);
        printk("ATA: Capacity: %d Octets (%d Mo)\n", total_sectors * 512, (total_sectors * 512) / (1024 * 1024));
        return (total_sectors * 512);
    }
    // CHS Mode
    else {
        uint32_t capacity = dev->identity->logical_cylinders * dev->identity->logical_heads * dev->identity->logical_sectors;
        printk("ATA: Capacity (CHS): %d Octets (%d Mo)\n", capacity * 512, (capacity * 512) / (1024 * 1024));
        return (capacity * 512);
    }
}

/**
 * @brief Display the error code
 *
 * @param error
 *
 * @note: This function will display the error code
 */
static void __ata_display_error_code(uint8_t error) {
    switch (error) {
    case 0x0:
        break;
    case ATA_ERR_AMNF:
        __WARND("ATA: Error code: " _RED "[0x%x:AMNF]" _END " (%s)\n", ATA_ERR_AMNF, "Address mark not found");
        break;
    case ATA_ERR_TK0NF:
        __WARND("ATA: Error code: " _RED "[0x%x:TKZNF]" _END " (%s)\n", ATA_ERR_TK0NF, "Track zero not found");
        break;
    case ATA_ERR_ABRT:
        __WARND("ATA: Error code: " _RED "[0x%x:ABRT]" _END " (%s)\n", ATA_ERR_ABRT, "Aborted Command");
        break;
    case ATA_ERR_MCR:
        __WARND("ATA: Error code: " _RED "[0x%x:MCR]" _END " (%s)\n", ATA_ERR_MCR, "Media Change Request");
        break;
    case ATA_ERR_IDNF:
        __WARND("ATA: Error code: " _RED "[0x%x:IDNF]" _END " (%s)\n", ATA_ERR_IDNF, "ID Not Found");
        break;
    case ATA_ERR_MC:
        __WARND("ATA: Error code: " _RED "[0x%x:MC]" _END " (%s)\n", ATA_ERR_MC, "Media Changed");
        break;
    case ATA_ERR_UNC:
        __WARND("ATA: Error code: " _RED "[0x%x:UNC]" _END " (%s)\n", ATA_ERR_UNC, "Uncorrectable Data Error");
        break;
    case ATA_ERR_BBK:
        __WARND("ATA: Error code: " _RED "[0x%x:BBK]" _END " (%s)\n", ATA_ERR_BBK, "Bad Block Detected");
        break;
    default:
        break;
    }
}

static void __ata_display_connections(void) {
    int master_present = __ata_drive_is_present(ATA_PRIMARY_IO, 1);
    int slave_present = __ata_drive_is_present(ATA_PRIMARY_IO, 0);
    int master_present2 = __ata_drive_is_present(ATA_SECONDARY_IO, 1);
    int slave_present2 = __ata_drive_is_present(ATA_SECONDARY_IO, 0);

    if (master_present) {
        printk("\t\t- ATA: Primary channel: Master: "_GREEN
               "[%s]"_END
               "\n",
               "PRESENT");
    } else {
        printk("\t\t- ATA: Primary channel: Master: "_RED
               "[%s]"_END
               "\n",
               "NOT PRESENT");
    }
    if (slave_present) {
        printk("\t\t- ATA: Primary channel: Slave: "_GREEN
               "[%s]"_END
               "\n",
               "PRESENT");
    } else {
        printk("\t\t- ATA: Primary channel: Slave: "_RED
               "[%s]"_END
               "\n",
               "NOT PRESENT");
    }
    if (master_present2) {
        printk("\t\t- ATA: Secondary channel: Master: "_GREEN
               "[%s]"_END
               "\n",
               "PRESENT");
    } else {
        printk("\t\t- ATA: Secondary channel: Master: "_RED
               "[%s]"_END
               "\n",
               "NOT PRESENT");
    }
    if (slave_present2) {
        printk("\t\t- ATA: Secondary channel: Slave: "_GREEN
               "[%s]"_END
               "\n",
               "PRESENT");
    } else {
        printk("\t\t- ATA: Secondary channel: Slave: "_RED
               "[%s]"_END
               "\n",
               "NOT PRESENT");
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
 * eg: outb(ATA_REG_COMMAND(io_base), ATA_CMD_IDENTIFY);
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
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_IDENTIFY);

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

#if __USE_ATA_PIO__ == 1
static int __ata_support_pio_mode(ATADevice *dev) {
    __ata_select_drive(dev->io_base, dev->master);

    // Send the IDENTIFY DEVICE command
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_IDENTIFY);

    // Wait for the drive to be ready
    delay400ns(dev->io_base);
    __ata_wait_ready(dev);

    // Read the data from the drive
    uint16_t data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = inw(dev->io_base);
    }

    // Check if the PIO mode is supported
    if (data[63] & 0xFF) {
        return (1);
    } else {
        return (0);
    }
}

static int __ata_is_pio_enabled(ATADevice *dev) {
    uint16_t identify_data[256 + 1];

    __ata_select_drive(dev->io_base, dev->master);

    // Envoyer la commande IDENTIFY
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_IDENTIFY);

    delay400ns(dev->io_base);
    __ata_wait_disk_data_ready(dev->io_base);

    // Lire les données IDENTIFY
    for (int i = 0; i < 256; i++) {
        identify_data[i] = inw(ATA_REG_DATA(dev->io_base));
    }

    // Vérifier si le mode PIO est actif
    return identify_data[ATA_IDENTIFY_WORD_PIO_MODES] & 1;
}

static void __ata_set_pio_mode(ATADevice *dev, int pio) {
    __ata_select_drive(dev->io_base, dev->master);

    outb(ATA_REG_FEATURES(dev->io_base), ATA_FEATURE_SET_TRANSFER_MODE);
    outb(ATA_REG_SECCOUNT0(dev->io_base), pio ? ATA_TRANSFER_MODE_PIO : ATA_TRANSFER_MODE_DMA);
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_SET_FEATURES);

    delay400ns(dev->io_base);
}
#endif

#if __USE_ATA_DMA__ == 1
static int __ata__support_dma_mode(ATADevice *dev) {
    __ata_select_drive(dev->io_base, dev->master);

    // Send the IDENTIFY DEVICE command
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_IDENTIFY);

    // Wait for the drive to be ready
    delay400ns(dev->io_base);
    __ata_wait_ready(dev);

    // Read the data from the drive
    uint16_t data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = inw(dev->io_base);
    }

    // Check if the DMA mode is supported
    if (data[63] & (1 << 8)) {
        return (1);
    } else {
        return (0);
    }
}

static int __ata_is_dma_enabled(ATADevice *dev) {
    uint16_t identify_data[256 + 1];

    __ata_select_drive(dev->io_base, dev->master);

    // Envoyer la commande IDENTIFY
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_IDENTIFY);

    delay400ns(dev->io_base);
    __ata_wait_disk_data_ready(dev->io_base);

    // Lire les données IDENTIFY
    for (int i = 0; i < 256; i++) {
        identify_data[i] = inw(ATA_REG_DATA(dev->io_base));
    }

    // Vérifier si le mode DMA est actif
    return identify_data[ATA_IDENTIFY_WORD_DMA_ACTIVE] & 1;
}

static void __ata_set_dma_mode(ATADevice *dev, int dma) {
    __ata_select_drive(dev->io_base, dev->master);

    outb(ATA_REG_FEATURES(dev->io_base), ATA_FEATURE_SET_TRANSFER_MODE);
    outb(ATA_REG_SECCOUNT0(dev->io_base), dma ? ATA_TRANSFER_MODE_DMA : ATA_TRANSFER_MODE_PIO);
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_SET_FEATURES);

    delay400ns(dev->io_base);
}
#endif

static int __ata_support_read_operation(ATADevice *dev) {
    // __ata_select_drive(dev->io_base, dev->master);

    // // Send the IDENTIFY DEVICE command
    // outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_IDENTIFY);

    // // Wait for the drive to be ready
    // __ata_wait_ready(dev);

    // // Read the data from the drive
    // uint16_t data[256];
    // for (int i = 0; i < 256; i++) {
    //     data[i] = inw(dev->io_base);
    // }

    // // Check if the READ SECTORS command is supported
    // if (data[83] & (1 << 9)) {
    //     return (1);
    // } else {
    //     return (0);
    // }

    __ata_select_drive(dev->io_base, dev->master);

    // Send the IDENTIFY DEVICE command
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_IDENTIFY);

    // Wait for the drive to be ready
    if (!__ata_wait_ready(dev)) {
        printk("ATA: Drive not ready\n");
        return 0;
    }
    return 1;
}

static int __ata_support_write_operation(ATADevice *dev) {
    // __ata_select_drive(dev->io_base, dev->master);

    // // Send the IDENTIFY DEVICE command
    // outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_IDENTIFY);

    // // Wait for the drive to be ready
    // __ata_wait_ready(dev);

    // // Read the data from the drive
    // uint16_t data[256];
    // for (int i = 0; i < 256; i++) {
    //     data[i] = inw(dev->io_base);
    // }

    // // Check if the WRITE SECTORS command is supported
    // if (data[83] & (1 << 10)) {
    //     return (1);
    // } else {
    //     return (0);
    // }

    __ata_select_drive(dev->io_base, dev->master);

    // Send the IDENTIFY DEVICE command
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_IDENTIFY);

    // Wait for the drive to be ready
    if (!__ata_wait_ready(dev)) {
        printk("ATA: Drive not ready for write operation\n");
        return 0;
    }
    return 1;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  ATA FUNCTIONS                                 ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief ATA Functions
 */

void ata_primary_irq_handler(__unused__ struct regs *regs) {
    pic8259_send_eoi(IRQ_ATA1);

    uint8_t status = inb(ATA_REG_STATUS(ATA_PRIMARY_IO));
    if (ATA_REG_ERROR(status)) {
        uint8_t error = __ata_get_error_code(ATA_PRIMARY_IO, 1);
        __ata_display_error_code(error);
    }
}

void ata_secondary_irq_handler(__unused__ struct regs *regs) {
    pic8259_send_eoi(IRQ_ATA2);

    uint8_t status = inb(ATA_REG_STATUS(ATA_SECONDARY_IO));
    if (ATA_REG_ERROR(status)) {
        uint8_t error = __ata_get_error_code(ATA_SECONDARY_IO, 1);
        __ata_display_error_code(error);
    }
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

    /**
     * @brief Set the LBA mode
     *
     * @note: This function will set the LBA mode (28 or 48 if supported)
     */
#if __ALLOW_LBA48__ == 1
    dev->lba_mode = __ata_get_lba_mode(channel, master);
#else
    dev->lba_mode = LBA_28; // Force LBA 28
#endif

    // Identify the device
    if (ata_identify(dev) != 0) {
        kfree(dev);
        __WARN("ATA: Failed to identify device on channel [%d]", NULL, channel);
        return NULL;
    } else {

        /**
         * @brief Use PIO mode if supported
         *
         * @note: This function will set the PIO mode if supported
         * @pio: Programmed Input/Output, it allows the CPU to transfer data to and from devices
         */

#if __USE_ATA_PIO__ == 1
        int support_pio = __ata_support_pio_mode(dev);
        printk("ATA: PIO mode supported: %d\n", support_pio);
        __ata_set_pio_mode(dev, support_pio);
        if (__ata_is_pio_enabled(dev)) {
            __INFOD("ATA: PIO mode enabled on channel [%d]", NULL, channel);
        } else {
            __INFOD("ATA: PIO mode not enabled on channel [%d]", NULL, channel);
        }
#endif

        /**
         * @brief Use DMA mode if supported
         *
         * @note: This function will set the DMA mode if supported
         * @dma: Direct Memory Access, it allows the device to access the memory directly without the CPU
         */

#if __USE_ATA_DMA__ == 1
        int support_dma = __ata__support_dma_mode(dev);
        printk("ATA: DMA mode supported: %d\n", support_dma);
        __ata_set_dma_mode(dev, support_dma);
        if (__ata_is_dma_enabled(dev)) {
            __INFOD("ATA: DMA mode enabled on channel [%d]", NULL, channel);
        } else {
            __INFOD("ATA: DMA mode not enabled on channel [%d]", NULL, channel);
        }
#endif

        Device *device = device_init_new_device("ATA", DEVICE_BLOCK, ata_device_read, ata_device_write, dev);

        if (device == NULL) {
            __WARN("ATA: Failed to register device on channel [%d]", NULL, channel);
            return NULL;
        } else {
            int ret = device_register(device);

            __INFOD("ATA: Device registered on index "_GREEN
                    "[%d]"_END
                    "",
                    ret);
            return (dev);
        }
    }
    return (dev);
}

int ata_init(void) {
    // Setup interrupts for ATA devices (IRQ 14 & 15)
    irq_install_handler(IRQ_ATA1, ata_primary_irq_handler);
    irq_install_handler(IRQ_ATA2, ata_secondary_irq_handler);

    // Init ATA devices
    ata_devices[0] = ata_device_init(ATA_PRIMARY_IO, ATA_PRIMARY_DEV_CTRL, 1); // Primary Master
    ata_devices[1] = ata_device_init(ATA_PRIMARY_IO, ATA_PRIMARY_DEV_CTRL, 0); // Primary Slave

    // Init ATA devices secondary channel
    ata_devices[2] = ata_device_init(ATA_SECONDARY_IO, ATA_SECONDARY_DEV_CTRL, 1); // Secondary Master
    ata_devices[3] = ata_device_init(ATA_SECONDARY_IO, ATA_SECONDARY_DEV_CTRL, 0); // Secondary Slave

    // Identifier les dispositifs
    for (int i = 0; i < MAX_ATA_DEVICES; ++i) {
        if (ata_devices[i] != NULL) {
            __ata_display_disk_state(ata_devices[i]);
            // print device infos:

            printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Serial number", ata_devices[i]->identity->serial_number);
            printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Firmware revision", ata_devices[i]->identity->firmware_revision);
            printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Model number", ata_devices[i]->identity->model_number);
            printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "Type",
                   ata_devices[i]->type == ATADEV_PATA ? "PATA" : ata_devices[i]->type == ATADEV_SATA ? "SATA"
                                                              : ata_devices[i]->type == ATADEV_PATAPI ? "PATAPI"
                                                              : ata_devices[i]->type == ATADEV_SATAPI ? "SATAPI"
                                                                                                      : "UNKNOWN");
            printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%d (Octets)" _END " / "_GREEN"%d (Mo)" _END "\n", "Capacity", __ata_get_capacity(ata_devices[i]),
                   (__ata_get_capacity(ata_devices[i]) / (1024 * 1024)));
            printk("\t\t\t   - ATA " _YELLOW "[%s]" _END " - " _GREEN "%s" _END "\n", "LBA mode",
                   ata_devices[i]->lba_mode == LBA_28 ? "LBA 28" : ata_devices[i]->lba_mode == LBA_48 ? "LBA 48"
                                                                                                      : "UNKNOWN");
        }
    }

    // Debug TMP
    __ata_display_connections();
    // kpause();
    return (0);
}

static int __ata_identify_read_data(ATADevice *dev) {
    uint16_t buffer[256];

    memset(buffer, 0, sizeof(buffer));

    __ata_wait_disk_data_ready(dev->io_base);

    for (uint32_t i = 0; i < 256; i++) {
        __ata_wait_disk_data_ready(dev->io_base);
        uint8_t status = inb(ATA_REG_STATUS(dev->io_base));
        if (status & ATA_STATUS_ERR) {
            printk("ATA: Error during data transfer\n");
            return (1);
        }

        if (status & ATA_STATUS_DRQ) {
            ((uint16_t *)buffer)[i] = inw(ATA_REG_DATA(dev->io_base));
        } else {
            printk("ATA: DRQ not set for data transfer\n");
            return (1);
        }
    }

    dev->identity->general_config = buffer[ATA_IDENT_DEVICETYPE];
    dev->identity->logical_cylinders = buffer[ATA_IDENT_CYLINDERS];
    dev->identity->logical_heads = buffer[ATA_IDENT_HEADS];
    dev->identity->logical_sectors = buffer[ATA_IDENT_SECTORS];
    dev->identity->reserved2 = buffer[ATA_IDENT_SERIAL];

    // Assign capabilities
    dev->identity->capabilities = buffer[ATA_IDENT_CAPABILITIES];

    // Assign commandsets
    dev->identity->commandsets = buffer[ATA_IDENT_COMMANDSETS];

    // Get the serial number
    // 20 bytes / 2 bytes per uint16_t = 10
    for (int i = 0; i < 10; ++i) {
        uint16_t word = buffer[ATA_IDENT_SERIAL / 2 + i];
        dev->identity->serial_number[i * 2] = word >> 8;
        dev->identity->serial_number[i * 2 + 1] = word & 0xFF;
    }
    dev->identity->serial_number[20] = '\0'; // Null-terminate the string

    // Get the firmware revision
    // 40 bytes / 2 bytes per uint16_t = 20
    for (int i = 0; i < 20; ++i) {
        uint16_t word = buffer[ATA_IDENT_MODEL / 2 + i];
        dev->identity->model_number[i * 2] = word >> 8;
        dev->identity->model_number[i * 2 + 1] = word & 0xFF;
    }
    dev->identity->model_number[40] = '\0'; // Null-terminate the string

    // Get the firmware revision
    // Firmware revision does not need swapping in this case
    memcpy(dev->identity->firmware_revision, &buffer[ATA_IDENT_FIRMWARE / 2], 8);

    // Check device type
    uint32_t cl = inb(ATA_REG_LBA1(dev->io_base));
    uint32_t ch = inb(ATA_REG_LBA2(dev->io_base));

    if (cl == 0x14 && ch == 0xEB) {
        dev->type = ATADEV_PATAPI;
    } else if (cl == 0x69 && ch == 0x96) {
        dev->type = ATADEV_SATAPI;
    } else if (cl == 0 && ch == 0) {
        dev->type = ATADEV_PATA;
    } else if (cl == 0x3c && ch == 0xc3) {
        dev->type = ATADEV_SATA;
    } else {
        dev->type = ATADEV_UNKNOWN;
    }

    if (!(dev->identity->general_config & 0x8000)) {
        dev->disk_mode = DISK_TYPE_CHS;
    } else if (dev->identity->capabilities & 0x0200) {
        if (dev->identity->commandsets & 0x0400) {
            dev->disk_mode = DISK_TYPE_LBA48;
        } else {
            dev->disk_mode = DISK_TYPE_LBA28;
        }
    } else {
        dev->disk_mode = DISK_TYPE_UNKNOWN;
    }

    if (dev->disk_mode == DISK_TYPE_LBA28 || dev->disk_mode == DISK_TYPE_LBA48) {

        if (dev->disk_mode == DISK_TYPE_LBA48) {
            dev->identity->MaxLBA = buffer[ATA_IDENT_MAX_LBA_EXT];
        } else if (dev->disk_mode == DISK_TYPE_LBA28) {
            dev->identity->MaxLBA = buffer[ATA_IDENT_MAX_LBA];
        }
    }
    delay400ns(dev->io_base);
    return (0);
}

static int ata_identify(ATADevice *dev) {
    __ata_select_drive(dev->io_base, dev->master);

    outb(ATA_REG_SECCOUNT0(dev->io_base), 0x00);
    outb(ATA_REG_LBA0(dev->io_base), 0x00);
    outb(ATA_REG_LBA1(dev->io_base), 0x00);
    outb(ATA_REG_LBA2(dev->io_base), 0x00);

    outb(ATA_REG_STATUS(dev->io_base), ATA_CMD_IDENTIFY);

    if (!__ata_wait_ready(dev)) {
        __THROW("ATA: Device not ready for IDENTIFY command", 1);
        return (1);
    }

    int drive_state = inb(ATA_REG_STATUS(dev->io_base));

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
    } else {
        __THROW("ATA: Device not present", 1);
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

/**
 * @brief Reads data from the ATA device using the LBA48 addressing mode.
 *
 * This function reads data from the ATA device using the LBA48 addressing mode.
 * It is used to read data from the device's sectors.
 *
 * @param lba The logical block address of the sector to read.
 * @param count The number of sectors to read.
 * @param buffer The buffer to store the read data.
 *
 * @return 0 if the read operation is successful, otherwise an error code.
 */
int ata_read_lba48(ATADevice *dev, uint32_t lba, uint8_t *buffer, uint32_t sectors) {
    __ata_select_drive(dev->io_base, dev->master);

    outb(ATA_REG_HDDEVSEL(dev->io_base), 0x40); // Select drive

    // Send LBA48 command
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_READ_PIO_EXT);

    // Send LBA and sector count
    outb(ATA_REG_SECCOUNT0(dev->io_base), (sectors >> 8) & 0xFF); // High byte of sector count
    outb(ATA_REG_LBA3(dev->io_base), (lba >> 24) & 0xFF);
    outb(ATA_REG_LBA4(dev->io_base), (lba >> 32) & 0xFF);
    outb(ATA_REG_LBA5(dev->io_base), (lba >> 40) & 0xFF);
    outb(ATA_REG_SECCOUNT0(dev->io_base), sectors & 0xFF); // Low byte of sector count
    outb(ATA_REG_LBA0(dev->io_base), lba & 0xFF);
    outb(ATA_REG_LBA1(dev->io_base), (lba >> 8) & 0xFF);
    outb(ATA_REG_LBA2(dev->io_base), (lba >> 16) & 0xFF);

    // Read data
    for (uint32_t i = 0; i < sectors; i++) {
        // Wait for drive to be ready
        while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_BSY) != 0)
            ;

        // Read 512 bytes of data
        insw(ATA_REG_DATA(dev->io_base), &buffer[i * 512], 512 / 2);
    }

    return 0; // Success
}

/**
 * @brief Writes data to the specified Logical Block Address (LBA) using the ATA48 command set.
 *
 * This function allows writing data to a specific LBA using the ATA48 command set. It is typically used
 * for low-level disk operations. The function takes the LBA address, data buffer, and the number of sectors
 * to write as parameters. It returns a status code indicating the success or failure of the operation.
 *
 * @param lba The Logical Block Address (LBA) to write the data to.
 * @param buffer The buffer containing the data to be written.
 * @param num_sectors The number of sectors to write.
 * @return int The status code indicating the success or failure of the operation.
 */
int ata_write_lba48(ATADevice *dev, uint32_t lba, const uint8_t *buffer, uint32_t sectors) {
    __ata_select_drive(dev->io_base, dev->master);

    outb(ATA_REG_HDDEVSEL(dev->io_base), 0x40); // Select drive

    // Send LBA48 command
    outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_WRITE_PIO_EXT);

    // Send LBA and sector count
    outb(ATA_REG_SECCOUNT0(dev->io_base), (sectors >> 8) & 0xFF); // High byte of sector count
    outb(ATA_REG_LBA3(dev->io_base), (lba >> 24) & 0xFF);
    outb(ATA_REG_LBA4(dev->io_base), (lba >> 32) & 0xFF);
    outb(ATA_REG_LBA5(dev->io_base), (lba >> 40) & 0xFF);
    outb(ATA_REG_SECCOUNT0(dev->io_base), sectors & 0xFF); // Low byte of sector count
    outb(ATA_REG_LBA0(dev->io_base), lba & 0xFF);
    outb(ATA_REG_LBA1(dev->io_base), (lba >> 8) & 0xFF);
    outb(ATA_REG_LBA2(dev->io_base), (lba >> 16) & 0xFF);

    // Write data
    for (uint32_t i = 0; i < sectors; i++) {
        // Wait for drive to be ready
        while ((inb(ATA_REG_STATUS(dev->io_base)) & ATA_STATUS_BSY) != 0)
            ;

        // Write 512 bytes of data
        outsw(ATA_REG_DATA(dev->io_base), &buffer[i * 512], 512 / 2);
    }
    return (0);
}

/**
 * @brief Reads data from the specified Logical Block Address (LBA) using the ATA-28 command set.
 *
 * This function reads data from the storage device at the specified LBA using the ATA-28 command set.
 * The data is stored in the provided buffer.
 *
 * @param lba The Logical Block Address (LBA) from which to read the data.
 * @param buffer The buffer to store the read data.
 * @param count The number of blocks to read.
 *
 * @return 0 if the read operation is successful, -1 otherwise.
 */
int ata_read_lba28(ATADevice *dev, uint32_t lba, uint8_t *buffer, uint32_t sectors) {
    __ata_select_drive(dev->io_base, dev->master);

    uint32_t cmd = dev->master ? 0xE0 : 0xF0;

    for (uint32_t sect = 0; sect < sectors; sect++) {
        uint32_t curr_lba = lba + sect;

        outb(ATA_REG_HDDEVSEL(dev->io_base), cmd | (((uint8_t)(curr_lba >> 24)) & 0x0F)); // Select the drive and set the high 4 bits of the LBA
        outb(ATA_REG_ERROR(dev->io_base), 0x00);                                          // Clear the error register
        outb(ATA_REG_SECCOUNT0((dev->io_base)), 1);                                       // Set the sector count
        outb(ATA_REG_LBA0((dev->io_base)), (uint8_t)(curr_lba));                          // Set the low 8 bits of the LBA
        outb(ATA_REG_LBA1((dev->io_base)), (uint8_t)(curr_lba >> 8));                     // Set the next 8 bits of the LBA
        outb(ATA_REG_LBA2((dev->io_base)), (uint8_t)(curr_lba >> 16));                    // Set the next 8 bits of the LBA

        outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_READ_PIO); // Send the WRITE SECTORS command

        __ata_polling(dev->io_base);

        for (uint32_t i = 0; i < 256; i++) {
            uint16_t data = inw(ATA_REG_DATA(dev->io_base));
            *(uint16_t *)(buffer + i * 2) = data;
        }

        delay400ns(dev->io_base);

        uint8_t status = __ata_get_error_code(dev->io_base, dev->master);
        __ata_display_error_code(status);

        // Error displayed in IRQ handler
        if (status != 0) {
            return (1);
        }

        buffer += 512;
    }

    delay400ns(dev->io_base);
    __ata_wait_disk_busy_clear(dev->io_base);

    printk("ATA: Read "_GREEN
           "[SUCCESS]"_END
           "\n");
    return (0);
}

/**
 * Writes data to the specified Logical Block Address (LBA) using the ATA-28 command set.
 *
 * @param lba The Logical Block Address to write the data to.
 * @param data The data to be written.
 * @param size The size of the data in bytes.
 * @return 0 if the write operation is successful, -1 otherwise.
 */
int ata_write_lba28(ATADevice *dev, uint32_t lba, const uint8_t *buffer, uint32_t sectors) {
    __ata_select_drive(dev->io_base, dev->master);

    uint32_t cmd = dev->master ? 0xE0 : 0xF0;

    for (uint32_t sect = 0; sect < sectors; sect++) {
        uint32_t curr_lba = lba + sect;

        outb(ATA_REG_HDDEVSEL(dev->io_base), cmd | (((uint8_t)(curr_lba >> 24)) & 0x0F)); // Select the drive and set the high 4 bits of the LBA
        outb(ATA_REG_ERROR(dev->io_base), 0x00);                                          // Clear the error register
        outb(ATA_REG_SECCOUNT0((dev->io_base)), 1);                                       // Set the sector count
        outb(ATA_REG_LBA0((dev->io_base)), (uint8_t)(curr_lba));                          // Set the low 8 bits of the LBA
        outb(ATA_REG_LBA1((dev->io_base)), (uint8_t)(curr_lba >> 8));                     // Set the next 8 bits of the LBA
        outb(ATA_REG_LBA2((dev->io_base)), (uint8_t)(curr_lba >> 16));                    // Set the next 8 bits of the LBA

        outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_WRITE_PIO); // Send the READ SECTORS command

        __ata_polling(dev->io_base);

        for (uint32_t i = 0; i < 256; i++) {
            uint16_t data = *(uint16_t *)(buffer + i * 2);
            outw(ATA_REG_DATA(dev->io_base), data);
        }

        delay400ns(dev->io_base);

        uint8_t status = __ata_get_error_code(dev->io_base, dev->master);
        __ata_display_error_code(status);

        // Error displayed in IRQ handler
        if (status != 0) {
            return (1);
        }

        // Flush the cache
        outb(ATA_REG_COMMAND(dev->io_base), ATA_CMD_FLUSH);
        __ata_polling(dev->io_base);

        buffer += 512;
    }

    delay400ns(dev->io_base);
    __ata_wait_disk_busy_clear(dev->io_base);

    printk("ATA: Write "_GREEN
           "[SUCCESS]"_END
           "\n");
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                              ATA BLOCKS FUNCTIONS                              ||
// ! ||--------------------------------------------------------------------------------||

uint32_t ata_device_read(void *device, uint32_t offset, uint32_t size, void *buffer) {
    ATADevice *ata_device = (ATADevice *)device;

    if (device == NULL || buffer == NULL) {
        __THROW("ATA: Cannot read from device", 1);
    } else {
    }

    if (__ata_support_read_operation(ata_device) == 0) {
        __THROW("ATA: Cannot read from device (Unsuported)", 1);
    }

    if (ata_device->lba_mode == LBA_28) {
        printk("ATA: Reading from device (LBA 28)\n");
        return (ata_read_lba28((ATADevice *)device, offset, buffer, size));
    } else if (ata_device->lba_mode == LBA_48) {
        printk("ATA: Reading from device (LBA 48)\n");
        return (ata_read_lba48((ATADevice *)device, offset, buffer, size));
    } else {
        __THROW("ATA: Cannot read from device", 1);
    }

    return (0);
}

uint32_t ata_device_write(void *device, uint32_t offset, uint32_t size, void *buffer) {
    ATADevice *ata_device = (ATADevice *)device;

    if (device == NULL || buffer == NULL) {
        __THROW("ATA: Cannot write to device", 1);
    } else {
    }

    if (__ata_support_write_operation(ata_device) == 0) {
        __THROW("ATA: Cannot write to device (Unsuported)", 1);
    }

    if (ata_device->lba_mode == LBA_28) {
        printk("ATA: Writing to device (LBA 28)\n");
        return (ata_write_lba28((ATADevice *)device, offset, buffer, size));
    } else if (ata_device->lba_mode == LBA_48) {
        printk("ATA: Writing to device (LBA 48)\n");
        return (ata_write_lba48((ATADevice *)device, offset, buffer, size));
    } else {
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