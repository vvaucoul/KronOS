/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ata.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:22:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/11 20:02:57 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ATA_H
#define ATA_H

#include <kernel.h>

#include <drivers/device/blocks.h>

#define ATA_DRIVER 1

/**
 * @file ata.h
 * @brief Header file for ATA driver.
 *
 * This file contains the declarations and documentation for the ATA driver.
 * The ATA driver provides functions for initializing ATA devices, identifying
 * devices, and reading/writing data from/to ATA devices.
 */

/**
 * |-----|-----------------|-----------------------------------------------------------------------------|
 * | Bit | Nom             | Description                                                                 |
 * |-----|-----------------|-----------------------------------------------------------------------------|
 * | 0   | ERR             | Une erreur s'est produite lors de la dernière commande.                     |
 * | 1   | INDEX           | Index. Non utilisé dans les disques modernes.                               |
 * | 2   | CORR            | Erreur corrigée. Non utilisé dans les disques modernes.                     |
 * | 3   | DRQ             | Le contrôleur est prêt à accepter des données (Data Request).               |
 * | 4   | SRV             | Overlapped Mode Service request. Non utilisé dans les disques modernes.     |
 * | 5   | DF              | Drive Fault. Une erreur non récupérable s'est produite.                     |
 * | 6   | RDY             | Le disque est prêt à accepter des commandes.                                |
 * | 7   | BSY             | Le disque est occupé. Aucune autre commande ne doit être envoyée.           |
 * |-----|-----------------|-----------------------------------------------------------------------------|
 */

#define ATA_PRIMARY_IO 0x1F0   // Primary IO port
#define ATA_SECONDARY_IO 0x170 // Secondary IO port

#define ATA_PRIMARY_DEV_CTRL 0x3F6   // Primary device control port
#define ATA_SECONDARY_DEV_CTRL 0x376 // Secondary device control port

#define ATA_SELECT_MASTER 0xA0 // Select master drive
#define ATA_SELECT_SLAVE 0xB0  // Select slave drive
#define ATA_IDENTIFY_CMD 0xEC  // Identify command

#define ATA_STATUS_ERR 0b00000001 // Error
#define ATA_STATUS_DRQ 0b00001000 // Data request
#define ATA_STATUS_SRV 0b00010000 // Overlapped mode service request
#define ATA_STATUS_DF 0b00100000  // Drive fault
#define ATA_STATUS_RDY 0b01000000 // Drive ready
#define ATA_STATUS_BSY 0b10000000 // Drive busy

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
#define ATA_REG_LBA_LOW(base) (base + 3)      // LBA low register
#define ATA_REG_LBA_MID(base) (base + 4)      // LBA mid register
#define ATA_REG_LBA_HIGH(base) (base + 5)     // LBA high register
#define ATA_REG_HDDEVSEL(base) (base + 6)     // Drive select register
#define ATA_REG_COMMAND(base) (base + 7)      // Command register
#define ATA_REG_STATUS(base) (base + 7)       // Status register

#define ATA_SECTOR_READ 0x20
#define ATA_SECTOR_WRITE 0x30

#define ATA_CMD_FLUSH 0xE7

/**
 * Maximum number of ATA devices.
 *
 * This is the maximum number of ATA devices that can be connected to the
 * system. This is used to allocate memory for the ATA device list.
 */
#define MAX_ATA_DEVICES 4

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
} __attribute__((packed)) ATAIdentity;

typedef struct {
    uint16_t io_base;
    uint16_t ctrl_base;
    uint8_t master;
    ATAIdentity *identity;
} ATADevice;

extern ATADevice *ata_devices[MAX_ATA_DEVICES];

extern int ata_init(void);

extern ATADevice *ata_get_device(uint32_t i);

extern void ata_disk_details(uint32_t i);
extern void ata_disk_size(uint32_t i);
extern int ata_disk_count(void);

extern int ata_read(ATADevice *dev, uint32_t lba, uint8_t *buffer, uint32_t sectors);
extern int ata_write(ATADevice *dev, uint32_t lba, const uint8_t *buffer, uint32_t sectors);

// extern int ata_block_read(BlockDevice *device, uint64_t block, uint64_t count, void *buf);
// extern int ata_block_write(BlockDevice *device, uint64_t block, uint64_t count, void *buf);

extern uint32_t ata_device_read(void *device, uint32_t lba, uint32_t count, void *buffer);
extern uint32_t ata_device_write(void *device, uint32_t lba, uint32_t count, void *buffer);

/**
 * Debug functions
 */

extern void __ata_display_disk_state(ATADevice *dev);

#endif /* !ATA_H */