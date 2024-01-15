/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ata.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:22:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/13 02:18:54 by vvaucoul         ###   ########.fr       */
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
#define ATA_REG_DATA(base) (base)          // Data register
#define ATA_REG_ERROR(base) (base + 1)     // Error register
#define ATA_REG_SECCOUNT0(base) (base + 2) // Sector count register
#define ATA_REG_LBA0(base) (base + 3)      // LBA low register
#define ATA_REG_LBA1(base) (base + 4)      // LBA mid register
#define ATA_REG_LBA2(base) (base + 5)      // LBA high register
#define ATA_REG_HDDEVSEL(base) (base + 6)  // Drive select register
#define ATA_REG_COMMAND(base) (base + 7)   // Command register
#define ATA_REG_STATUS(base) (base + 7)    // Status register
#define ATA_REG_LBA3(base) (base + 8)      // LBA register
#define ATA_REG_LBA4(base) (base + 9)      // LBA register
#define ATA_REG_LBA5(base) (base + 10)     // LBA register
#define ATA_REG_FEATURES(base) (base + 1)  // Features register

#define ATA_FEATURE_SET_TRANSFER_MODE 0x03

#define ATA_TRANSFER_MODE_DMA 0x20
#define ATA_TRANSFER_MODE_PIO 0x08

#define ATA_IDENTIFY_WORD_DMA_ACTIVE 0x0100
#define ATA_IDENTIFY_WORD_PIO_MODES 0x0080

#define ATA_SECTOR_READ 0x20
#define ATA_SECTOR_WRITE 0x30

#define ATA_CMD_FLUSH 0xE7         // Flush cache
#define ATA_CMD_IDENTIFY 0xEC      // Identify command
#define ATA_CMD_READ_PIO 0x20      // Read PIO mode
#define ATA_CMD_READ_PIO_EXT 0x24  // Read PIO mode with extend
#define ATA_CMD_READ_DMA 0xC8      // Read DMA mode
#define ATA_CMD_READ_DMA_EXT 0x25  // Read DMA mode with extend
#define ATA_CMD_WRITE_PIO 0x30     // Write PIO mode
#define ATA_CMD_WRITE_PIO_EXT 0x34 // Write PIO mode with extend
#define ATA_CMD_SET_FEATURES 0xEF

#define ATA_ERR_AMNF 0x01  // Address Mark Not Found
#define ATA_ERR_TK0NF 0x02 // Track 0 Not Found
#define ATA_ERR_ABRT 0x04  // Aborted Command
#define ATA_ERR_MCR 0x08   // Media Change Request
#define ATA_ERR_IDNF 0x10  // ID Not Found
#define ATA_ERR_MC 0x20    // Media Changed
#define ATA_ERR_UNC 0x40   // Uncorrectable Data Error
#define ATA_ERR_BBK 0x80   // Bad Block Detected

// Identify types
#define ATA_IDENT_DEVICETYPE 0
#define ATA_IDENT_CYLINDERS 1
#define ATA_IDENT_RETIRED4 4
#define ATA_IDENT_HEADS 6
#define ATA_IDENT_VENDOR7 7
#define ATA_IDENT_SECTORS 12
#define ATA_IDENT_SERIAL 20
#define ATA_IDENT_RETIRED20 20
#define ATA_IDENT_OBSOLETE23 23
#define ATA_IDENT_FIRMWARE 46
#define ATA_IDENT_CAPABILITIES 49
#define ATA_IDENT_MODEL 54
#define ATA_IDENT_MAX_LBA 60
#define ATA_IDENT_COMMANDSETS 82
#define ATA_IDENT_MAX_LBA_EXT 100
#define ATA_IDENT_FIELDVALID 106

// #define ATA_IDENT_DEVICETYPE 0
// #define ATA_IDENT_CYLINDERS 1
// #define ATA_IDENT_HEADS 3
// #define ATA_IDENT_SECTORS 6
// #define ATA_IDENT_SERIAL 10
// #define ATA_IDENT_FIRMWARE 23
// #define ATA_IDENT_MODEL 27
// #define ATA_IDENT_CAPABILITIES 49
// #define ATA_IDENT_COMMANDSETS 82
// #define ATA_IDENT_MAX_LBA 60      // LBA2
// #define ATA_IDENT_MAX_LBA_EXT 100 // LBA48
// #define ATA_IDENT_FIELDVALID 53

#define __USE_ATA_DMA__ 0
#define __USE_ATA_PIO__ 0
#define __ALLOW_LBA48__ 0

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
    ATADEV_PATAPI,
    ATADEV_SATAPI,
    ATADEV_PATA,
    ATADEV_SATA,
    ATADEV_UNKNOWN
} ATADeviceType;

#define ATA_DEVICE_STRINGIFY(x) \
    (x == ATADEV_PATAPI ? "PATAPI" : (x == ATADEV_SATAPI ? "SATAPI" : (x == ATADEV_PATA ? "PATA" : (x == ATADEV_SATA ? "SATA" : (x == ATADEV_UNKNOWN ? "UNKNOWN" : "UNKNOWN")))))

/**
 * ATA Addressing mode.
 *
 * This is the addressing mode used by the ATA device. This is used to
 * determine the maximum number of sectors that can be addressed by the
 * device.
 */
typedef enum {
    DISK_TYPE_CHS,
    DISK_TYPE_LBA28,
    DISK_TYPE_LBA48,
    DISK_TYPE_UNKNOWN
} DISKAddressMode;

#define DISKAddressMode_STRINGIFY(x) \
    (x == DISK_TYPE_CHS ? "CHS" : (x == DISK_TYPE_LBA28 ? "LBA28" : (x == DISK_TYPE_LBA48 ? "LBA48" : (x == DISK_TYPE_UNKNOWN ? "UNKNOWN" : "UNKNOWN"))))

typedef enum {
    LBA_28,
    LBA_48
} ATALBA;

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
    uint16_t general_config;
    uint16_t logical_cylinders;
    uint16_t reserved2;
    uint16_t logical_heads;
    uint16_t retired4[2];
    uint16_t logical_sectors;
    uint16_t vendor7[3];
    char serial_number[21];
    uint16_t retired20[3];
    uint16_t obsolete23;
    uint16_t capabilities;
    uint16_t commandsets;
    uint16_t MaxLBA;
    char firmware_revision[8];
    char model_number[41];
} __attribute__((packed)) ATAIdentity;

typedef struct {
    uint16_t io_base;
    uint16_t ctrl_base;
    uint8_t master;
    ATALBA lba_mode;
    ATAIdentity *identity;
    ATADeviceType type;
    DISKAddressMode disk_mode;
} ATADevice;

extern ATADevice *ata_devices[MAX_ATA_DEVICES];

extern int ata_init(void);

extern ATADevice *ata_get_device(uint32_t i);

extern void ata_disk_details(uint32_t i);
extern void ata_disk_size(uint32_t i);
extern int ata_disk_count(void);

extern int ata_read_lba28(ATADevice *dev, uint32_t lba, uint8_t *buffer, uint32_t sectors);
extern int ata_write_lba28(ATADevice *dev, uint32_t lba, const uint8_t *buffer, uint32_t sectors);

// extern int ata_block_read(BlockDevice *device, uint64_t block, uint64_t count, void *buf);
// extern int ata_block_write(BlockDevice *device, uint64_t block, uint64_t count, void *buf);

extern uint32_t ata_device_read(void *device, uint32_t lba, uint32_t count, void *buffer);
extern uint32_t ata_device_write(void *device, uint32_t lba, uint32_t count, void *buffer);

extern int ata_read_lba48(ATADevice *dev, uint32_t lba, uint8_t *buffer, uint32_t sectors);
extern int ata_write_lba48(ATADevice *dev, uint32_t lba, const uint8_t *buffer, uint32_t sectors);

/**
 * Debug functions
 */

extern void __ata_display_disk_state(ATADevice *dev);

#endif /* !ATA_H */