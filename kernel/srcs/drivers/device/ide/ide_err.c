/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ide_err.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 15:44:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/23 10:32:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ide.h>
#include <system/io.h>

/**
 * @brief Generates an IDE error message.
 *
 * This function generates an IDE error message based on the specified status and display flag.
 *
 * @param status The IDE error status.
 * @param display The error message display flag.
 * @return The generated error code.
 */
uint8_t ide_error_msg(uint8_t status, uint8_t display) {
    if (status == 0) {
        return (0);
    } else {
        if (status & ATA_ER_BBK) {
            if (display == 1) {
                __WARND("Bad Block\n");
            }
            return (ATA_ER_BBK);
        }
        if (status & ATA_ER_UNC) {
            if (display == 1) {
                __WARND("Uncorrectable Data\n");
            }
            return (ATA_ER_UNC);
        }
        if (status & ATA_ER_MC) {
            if (display == 1) {
                __WARND("Media Change\n");
            }
            return (ATA_ER_MC);
        }
        if (status & ATA_ER_IDNF) {
            if (display == 1) {
                __WARND("ID mark Not Found\n");
            }
            return (ATA_ER_IDNF);
        }
        if (status & ATA_ER_MCR) {
            if (display == 1) {
                __WARND("Media Change Requested\n");
            }
            return (ATA_ER_MCR);
        }
        if (status & ATA_ER_ABRT) {
            if (display == 1) {
                __WARND("Command Aborted\n");
            }
            return (ATA_ER_ABRT);
        }
        if (status & ATA_ER_TK0NF) {
            if (display == 1) {
                __WARND("Track 0 Not Found\n");
            }
            return (ATA_ER_TK0NF);
        }
        if (status & ATA_ER_AMNF) {
            if (display == 1) {
                __WARND("No Address Mark\n");
            }
            return (ATA_ER_AMNF);
        }
    }
    return (0);
}

/**
 * Displays the state of the IDE disk.
 *
 * @param dev The IDE device to display the state for.
 */
void ide_display_disk_state(IDEDevice *dev) {
    uint8_t status = inb(dev->regs.base + ATA_REG_STATUS);
    uint8_t err = inb(dev->regs.base + ATA_REG_ERROR);

    if (status == 0) {
        __WARND("No device found on channel [%d] drive [%d]", dev->channel, dev->drive);
        return;
    }

    if ((status & ATA_SR_ERR) == 0) {
        printk("\t\t- IDE: Drive status: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t- IDE: Drive status: " _RED "[ERROR]" _END "\n");

        if (err & ATA_ER_BBK) {
            printk("\t\t- IDE: Bad block: " _RED "[ERROR]" _END "\n");
        }
        if (err & ATA_ER_UNC) {
            printk("\t\t- IDE: Uncorrectable data: " _RED "[ERROR]" _END "\n");
        }
        if (err & ATA_ER_MC) {
            printk("\t\t- IDE: Media change: " _RED "[ERROR]" _END "\n");
        }
        if (err & ATA_ER_IDNF) {
            printk("\t\t- IDE: ID mark not found: " _RED "[ERROR]" _END "\n");
        }
        if (err & ATA_ER_MCR) {
            printk("\t\t- IDE: Media change request: " _RED "[ERROR]" _END "\n");
        }
        if (err & ATA_ER_ABRT) {
            printk("\t\t- IDE: Command aborted: " _RED "[ERROR]" _END "\n");
        }
        if (err & ATA_ER_TK0NF) {
            printk("\t\t- IDE: Track 0 not found: " _RED "[ERROR]" _END "\n");
        }
        if (err & ATA_ER_AMNF) {
            printk("\t\t- IDE: No address mark: " _RED "[ERROR]" _END "\n");
        }
    }
    if ((status & ATA_SR_DF) == 0) {
        printk("\t\t- IDE: Drive fault: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t- IDE: Drive fault: " _RED "[ERROR]" _END "\n");
    }
    if (status & ATA_SR_DRDY) {
        printk("\t\t- IDE: Drive ready: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t- IDE: Drive ready: " _RED "[ERROR]" _END "\n");
    }
    if ((status & ATA_SR_BSY) == 0) {
        printk("\t\t- IDE: Drive busy: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t- IDE: Drive busy: " _RED "[ERROR]" _END "\n");
    }
    if ((status & ATA_SR_DRQ) == 0) {
        printk("\t\t- IDE: Drive DRQ: " _GREEN "[OK]" _END "\n");
    } else {
        printk("\t\t- IDE: Drive DRQ: " _RED "[ERROR]" _END "\n");
    }
}