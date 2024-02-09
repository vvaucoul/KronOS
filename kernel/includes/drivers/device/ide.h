/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ide.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 00:35:11 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/09 22:33:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IDE_H
#define IDE_H

#include <kernel.h>

/**
 * @file ide.h
 * @brief Header file for IDE driver.
 *
 * This code represents the driver for the IDE (Integrated Development Environment).
 * It provides functionality for managing and controlling the IDE, including
 * initializing the IDE, identifying devices, and reading/writing data from/to the IDE.
 */

/**
 * +-------------------------+-----------------------------------------------------------+
 * |         Step            |                          Action                           |
 * +-------------------------+-----------------------------------------------------------+
 * | 1. Initialization       | - Initialize IDE channels (primary and secondary).        |
 * |                         | - Detect devices connected to each channel.               |
 * +-------------------------+-----------------------------------------------------------+
 * | 2. Identification       | - Select the device (master or slave).                    |
 * |                         | - Send the IDENTIFY command.                              |
 * |                         | - Read and process identification data.                   |
 * +-------------------------+-----------------------------------------------------------+
 * | 3. Read / Write         | - Prepare the device for reading or writing.              |
 * |                         |   (select the disk, configure LBA, etc.).                 |
 * |                         | - Send the read or write command.                         |
 * |                         | - Transfer data to/from the buffer.                       |
 * +-------------------------+-----------------------------------------------------------+
 * | 4. Error Handling       | - Check status and errors after each command.             |
 * |                         | - Handle errors appropriately.                            |
 * +-------------------------+-----------------------------------------------------------+
 * | 5. Advanced Functions   | - Implement DMA support (if necessary).                   |
 * |                         | - Handle interrupts (if used).                            |
 * +-------------------------+-----------------------------------------------------------+
 * | 6. Integration          | - Integrate the driver with the operating system.         |
 * |                         | - Ensure compatibility with the rest of the system.       |
 * +-------------------------+-----------------------------------------------------------+
 */

#define ATA_SR_BSY 0x80  // Busy
#define ATA_SR_DRDY 0x40 // Drive ready
#define ATA_SR_DF 0x20   // Drive write fault
#define ATA_SR_DSC 0x10  // Drive seek complete
#define ATA_SR_DRQ 0x08  // Data request ready
#define ATA_SR_CORR 0x04 // Corrected data
#define ATA_SR_IDX 0x02  // Index
#define ATA_SR_ERR 0x01  // Error

#define ATA_ER_BBK 0x80   // Bad block
#define ATA_ER_UNC 0x40   // Uncorrectable data
#define ATA_ER_MC 0x20    // Media changed
#define ATA_ER_IDNF 0x10  // ID mark not found
#define ATA_ER_MCR 0x08   // Media change request
#define ATA_ER_ABRT 0x04  // Command aborted
#define ATA_ER_TK0NF 0x02 // Track 0 not found
#define ATA_ER_AMNF 0x01  // No address mark

#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_WRITE_PIO_EXT 0x34
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC

#define ATAPI_CMD_READ 0xA8
#define ATAPI_CMD_EJECT 0x1B

#define ATA_IDENT_DEVICETYPE 0
#define ATA_IDENT_CYLINDERS 2
#define ATA_IDENT_HEADS 6
#define ATA_IDENT_SECTORS 12
#define ATA_IDENT_SERIAL 20
#define ATA_IDENT_MODEL 54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID 106
#define ATA_IDENT_MAX_LBA 120
#define ATA_IDENT_COMMANDSETS 164
#define ATA_IDENT_MAX_LBA_EXT 200

#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALTSTATUS 0x0C
#define ATA_REG_DEVADDRESS 0x0D

// Channels:
#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01

// Directions:
#define ATA_READ 0x00
#define ATA_WRITE 0x01

#define SECTOR_SIZE 512
#define MAX_IDE_DEVICES 4

// I/O Ports
#define ATA_PRIMARY_IO 0x1F0   // Primary IO port
#define ATA_SECONDARY_IO 0x170 // Secondary IO port

// Control Registers
#define ATA_PRIMARY_DEV_CTRL 0x3F6   // Primary device control port
#define ATA_SECONDARY_DEV_CTRL 0x376 // Secondary device control port

// Drive selection
#define ATA_SELECT_MASTER 0xA0 // Select master drive
#define ATA_SELECT_SLAVE 0xB0  // Select slave drive

#define INITIALIZE_DEVICE 0x01

// LBA Masks
#define LBA_MASK_8BIT 0x000000FF
#define LBA_MASK_16BIT 0x0000FF00
#define LBA_MASK_24BIT 0x00FF0000
#define LBA_MASK_28BIT 0x0F000000
#define LBA_MASK_32BIT 0x000F0000

typedef enum e_ide_type {
    IDE_ATA = 0x0,
    IDE_ATAPI = 0x1,
    IDE_NONE = 0x2,
} IDEType;

typedef enum e_ide_channel {
    IDE_PRIMARY = 0x0,
    IDE_SECONDARY = 0x1,
} IDEChannel;

typedef enum e_ide_drive {
    IDE_MASTER = 0x0,
    IDE_SLAVE = 0x1,
} IDEDrive;

typedef enum e_ide_lba_mode {
    IDE_LBA28 = 0x0, // Logical Block Addressing (28-bit)
    IDE_LBA48 = 0x1, // Logical Block Addressing (48-bit)
    IDE_CHS = 0x2,   // Cylinder-Head-Sector
} IDELBAMode;

typedef struct s_lba {
    uint8_t lba;
    uint8_t lba_high;
    uint8_t lba_mid;
    uint8_t device;
} IDELBA;

typedef struct s_ide_channel_registers {
    uint16_t base;  // I/O Base.
    uint16_t ctrl;  // Control Base
    uint16_t bmide; // Bus Master IDE
    uint8_t nIEN;   // nIEN (No Interrupt);
} IDEChannelRegisters;

typedef struct s_ide_device {
    uint8_t reserved; // 0 (Empty) or 1 (This Drive really exists).

    IDEChannel channel; // 0 (Primary Channel) or 1 (Secondary Channel).
    IDEDrive drive;     // 0 (Master Drive) or 1 (Slave Drive).

    uint16_t signature;    // Drive Signature
    uint16_t capabilities; // Features.
    uint32_t commandsets;  // Command Sets Supported.
    uint32_t size;         // Size in Sectors.
    uint32_t sector_size;  // Sector Size in Bytes. (512 default or 4096 for drives with 4k sectors)
    uint32_t sector_count; // Sector Count.
    uint8_t model[41];     // Model in string.
    uint8_t firmware[9];   // Firmware in string.

    IDEType type;             // 0: ATA, 1:ATAPI.
    IDEChannelRegisters regs; // I/O Ports.
    IDELBAMode lba_mode;      // LBA mode: 0: CHS, 1:LBA28, 2: LBA48.
} IDEDevice;

extern IDEDevice *ide_devices[MAX_IDE_DEVICES];

// Initialization
extern int ide_init(void);

// Identification
extern int ide_identify(IDEDevice *dev);
extern int ide_device_is_present(IDEChannel channel, IDEDrive drive);

// lba
extern IDELBA __ide_get_lba(uint32_t lba);

// Ops
extern void ide_select_drive(IDEDevice *dev);

extern int ide_read(IDEDevice *dev, uint32_t lba, uint8_t sectors, void *buf);
extern int ide_write(IDEDevice *dev, uint32_t lba, uint8_t sectors, const void *buf);

extern int ide_simple_read(IDEDevice *dev, uint32_t offset, uint32_t size, void *buffer);
extern int ide_simple_write(IDEDevice *dev, uint32_t offset, uint32_t size, const void *buffer);

extern int ide_device_read(void *device, uint32_t lba, uint32_t sectors, void *buffer);
extern int ide_device_write(void *device, uint32_t lba, uint32_t sectors, const void *buffer);
extern int ide_device_simple_read(void *device, uint32_t offset, uint32_t size, void *buffer);
extern int ide_device_simple_write(void *device, uint32_t offset, uint32_t size, const void *buffer);

extern uint32_t ide_device_get_size(void *device);
extern uint32_t ide_device_get_sector_size(void *device);
extern uint32_t ide_device_get_sector_count(void *device);

// Error
extern uint8_t ide_error_msg(uint8_t status, uint8_t display);
extern void ide_display_disk_state(IDEDevice *dev);

// Utils
extern IDEDevice *ide_get_device(uint8_t drive);

#endif /* !IDE_H */