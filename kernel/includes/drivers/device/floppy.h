/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floppy.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 15:11:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 16:41:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FLOPPY_H
#define FLOPPY_H

#include <kernel.h>
#include <system/isr.h>

#define FLOPPY_DRIVER 1

/**
 * @file floppy_disk_driver.h
 * @brief Header file for the floppy disk driver module.
 *
 * This module provides functions for interacting with a floppy disk drive.
 * It includes functions for reading and writing data to the floppy disk,
 * as well as functions for controlling the motor and selecting the drive.
 */

#define FLOPPY_CMD_READ 0x06
#define FLOPPY_CMD_WRITE 0x05
#define FLOPPY_CMD_SPECIFY 0x03
#define FLOPPY_CMD_RECALIBRATE 0x07
#define FLOPPY_CMD_SENSE_INTERRUPT 0x08

#define FLOPPY_REG_STATUS_A 0x3F0
#define FLOPPY_REG_STATUS_B 0x3F1
#define FLOPPY_REG_DOR 0x3F2
#define FLOPPY_REG_TDR 0x3F3
#define FLOPPY_REG_MSR 0x3F4
#define FLOPPY_REG_DSR 0x3F5
#define FLOPPY_REG_FIFO 0x3F5
#define FLOPPY_REG_DIR 0x3F7
#define FLOPPY_REG_CCR 0x3F7
#define FLOPPY_REG_CTRL 0x3F7

#define FLOPPY_CMD_READ 0x06
#define FLOPPY_CMD_WRITE 0x05
#define FLOPPY_CMD_RECALIBRATE 0x07

#define FLOPPY_CMD_PORT 0x3F5
#define FLOPPY_DATA_PORT 0x3F5
#define FLOPPY_STATUS_PORT 0x3F4

#define FLOPPY_IRQ 0x06

typedef struct {
    uint8_t status;
    uint8_t cylinder;

    uint32_t id; // 0 = master, 1 = slave
} FloppyDrive;

extern FloppyDrive *floppy_dev;

extern int floppy_init(void);
extern void floppy_send_command(uint8_t drive, uint8_t cmd);
extern void floppy_read_sector(FloppyDrive *drive, uint8_t *buffer, uint8_t sector);
extern void floppy_write_sector(FloppyDrive *drive, const uint8_t *buffer, uint8_t sector);

extern void floppy_interrupt_handler(struct regs *r);

#endif /* !FLOPPY_H */