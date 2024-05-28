/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floppy.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 15:11:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/28 14:40:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FLOPPY_H
#define FLOPPY_H

#include <kernel.h>
#include <system/isr.h>

#define FLOPPY_DRIVER 0

/**
 * @file floppy_disk_driver.h
 * @brief Header file for the floppy disk driver module.
 *
 * This module provides functions for interacting with a floppy disk drive.
 * It includes functions for reading and writing data to the floppy disk,
 * as well as functions for controlling the motor and selecting the drive.
 */

#define FDC_DOR 0x3F2  // Digital Output Register
#define FDC_MSR 0x3F4  // Main Status Register (Input)
#define FDC_DATA 0x3F5 // Data Register
#define FDC_CTRL 0x3F7 // Control Register

#define FDC_IRQ 6

#define SECTOR_SIZE 512

/**
 * @brief Structure representing the Cylinder-Head-Sector (CHS) information of a floppy device.
 */
typedef struct {
    uint8_t cyl;    /**< The cylinder number. */
    uint8_t head;   /**< The head number. */
    uint8_t sector; /**< The sector number. */
} CHS;

typedef struct {
    uint8_t drive;
    CHS chs;
    uint8_t *buffer;
} FloppyDisk;

extern void fdc_initialize();
extern void fdc_read_sector(FloppyDisk *fd, uint8_t head, uint8_t track, uint8_t sector);
extern void fdc_write_sector(FloppyDisk *fd, uint8_t head, uint8_t track, uint8_t sector);

#endif /* FLOPPY_H */