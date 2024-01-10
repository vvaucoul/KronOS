/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ata.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:22:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 16:25:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ATA_H
#define ATA_H

#include <kernel.h>

#define ATA_DRIVER 0

/**
 * @file ata.h
 * @brief Header file for ATA driver.
 *
 * This file contains the declarations and documentation for the ATA driver.
 * The ATA driver provides functions for initializing ATA devices, identifying
 * devices, and reading/writing data from/to ATA devices.
 */

#define ATA_PRIMARY_IO 0x1F0   // Primary IO port
#define ATA_SECONDARY_IO 0x170 // Secondary IO port

#define ATA_PRIMARY_DEV_CTRL 0x3F6   // Primary device control port
#define ATA_SECONDARY_DEV_CTRL 0x376 // Secondary device control port

#define ATA_SELECT_MASTER 0xA0 // Select master drive
#define ATA_IDENTIFY_CMD 0xEC  // Identify command

#define ATA_STATUS_ERR 0x01 // Error
#define ATA_STATUS_DRQ 0x08 // Data request ready
#define ATA_STATUS_SRV 0x10 // Overlapped mode service request
#define ATA_STATUS_DF 0x20  // Drive fault error
#define ATA_STATUS_RDY 0x40 // Drive ready
#define ATA_STATUS_BSY 0x80 // Drive busy

#define ATA_DEV_CTRL_DISABLE_IRQ 0x02 // Disable IRQ
#define ATA_DEV_CTRL_SOFT_RESET 0x04  // Soft reset
#define ATA_DEV_CTRL_HOB 0x80         // High order byte

#define ATA_REQ_MASTER_DRIVE 0x80 // Request master drive
#define ATA_REQ_SLAVE_DRIVE 0x10  // Request slave drive
#define ATA_REQ_DRQ_BIT 0x08      // Request DRQ bit
#define ATA_REQ_ERR_BIT 0x01      // Request ERR bit

// ATA Registers
#define ATA_REG_DATA(base) (base)             // Data register
#define ATA_REG_ERROR(base) (base + 1)        // Error register
#define ATA_REG_SECTOR_COUNT(base) (base + 2) // Sector count register
#define ATA_REG_LBA_LOW(base) (base + 3)       // LBA low register
#define ATA_REG_LBA_MID(base) (base + 4)      // LBA mid register
#define ATA_REG_LBA_HIGH(base) (base + 5)       // LBA high register
#define ATA_REG_DRIVE_SELECT(base) (base + 6) // Drive select register
#define ATA_REG_COMMAND(base) (base + 7)      // Command register
#define ATA_REG_STATUS(base) (base + 7)       // Status register

#define ATA_SECTOR_READ 0x20
#define ATA_SECTOR_WRITE 0x30

#define ATA_CMD_FLUSH 0xE7

typedef enum {
    ATA_PRIMARY_MASTER = 0x00,
    ATA_PRIMARY_SLAVE = 0x01,
    ATA_SECONDARY_MASTER = 0x10,
    ATA_SECONDARY_SLAVE = 0x11
} ATAChannelType;

typedef enum {
    ATA_HARD_DRIVE,
    ATA_CDROM,
    ATA_UNKNOWN
} ATADeviceType;

typedef struct {
    uint8_t error;
    uint8_t feature;
    uint8_t sector_count;
    uint8_t lba_low;
    uint8_t lba_mid;
    uint8_t lba_high;
    uint8_t device;
    uint8_t command;
} __attribute__((packed)) ata_regs_t;

typedef struct {
    uint16_t general_config;    // Word 0
    uint16_t logical_cylinders; // Word 1
    uint16_t reserved2;         // Word 2
    uint16_t logical_heads;     // Word 3
    uint16_t retired4[2];       // Word 4-5
    uint16_t logical_sectors;   // Word 6
    uint16_t vendor7[3];        // Word 7-9
    char serial_number[20];     // Word 10-19
    uint16_t retired20[3];      // Word 20-22
    uint16_t obsolete23;        // Word 23
    char firmware_revision[8];  // Word 23-26
    char model_number[40];      // Word 27-46
} __attribute__((packed)) ata_identify_t;

typedef struct {
    uint16_t io_base;
    uint16_t ctrl_base;
    ata_identify_t *id;
} ATADevice;

extern ATADevice *dev;

extern int ata_init(uint16_t io_base, uint16_t ctrl_base);

extern uint8_t ata_identify(ATADevice *dev);
extern void ata_identify_devide(ATADevice *dev);

extern int ata_read(ATADevice *dev, uint32_t lba, uint8_t *buffer, uint32_t sectors);
extern int ata_write(ATADevice *dev, uint32_t lba, const uint8_t *buffer, uint32_t sectors);

#endif /* !ATA_H */