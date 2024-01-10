/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floppy.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 15:11:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 16:30:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/floppy.h>

#include <system/irq.h>

#include <memory/memory.h>
#include <system/io.h>

FloppyDrive *floppy_dev = NULL;

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  FLOPPY UTILS                                  ||
// ! ||--------------------------------------------------------------------------------||

static int __floppy_wait_ready(void) {
    // Wait for the drive to be not busy
    while ((inb(FLOPPY_STATUS_PORT) & 0x80) != 0) {
        printk("floppy_wait_ready: Floppy drive is busy (%d)\n", inb(FLOPPY_STATUS_PORT) & 0x80);
        kmsleep(500);
    }
        ;

    // Wait for the drive to be ready
    while ((inb(FLOPPY_STATUS_PORT) & 0x40) == 0)
        ;

    // Check if there was an error
    return (inb(FLOPPY_STATUS_PORT) & 0x20) == 0;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                FLOPPY FUNCTIONS                                ||
// ! ||--------------------------------------------------------------------------------||

int floppy_init(void) {
    // Create the drive
    if (!(floppy_dev = (FloppyDrive *)kmalloc(sizeof(FloppyDrive)))) {
        __THROW("floppy_init: Failed to allocate floppy drive", 1);
    }
    floppy_dev->id = 0;
    floppy_dev->status = inb(FLOPPY_STATUS_PORT);

    irq_install_handler(FLOPPY_IRQ, floppy_interrupt_handler);


    // Select the first drive
    outb(FLOPPY_CMD_PORT, 0x10);

    // Check if the drive exists
    if (inb(FLOPPY_STATUS_PORT) == 0xFF) {
        __THROW("floppy_init: No floppy drive found", 1);
    } else {
        printk("floppy_init: Floppy drive found\n");
    }

    // Wait for the drive to be ready
    __floppy_wait_ready();

    // Return the drive
    return 0;
}

void floppy_send_command(uint8_t drive, uint8_t cmd) {
    outb(FLOPPY_CMD_PORT, cmd);
    outb(FLOPPY_DATA_PORT, drive);
}

void floppy_read_sector(FloppyDrive *drive, uint8_t *buffer, uint8_t sector) {
    // Send the read command
    floppy_send_command(drive->id, FLOPPY_CMD_READ);

    // Send the sector number
    outb(FLOPPY_DATA_PORT, sector);

    // Wait for the drive to be ready
    __floppy_wait_ready();

    // Read the data
    for (int i = 0; i < 512; i++) {
        buffer[i] = inb(FLOPPY_DATA_PORT);
    }
}

void floppy_write_sector(FloppyDrive *drive, const uint8_t *buffer, uint8_t sector) {
    // Send the write command
    floppy_send_command(drive->id, FLOPPY_CMD_WRITE);

    // Send the sector number
    outb(FLOPPY_DATA_PORT, sector);

    // Wait for the drive to be ready
    __floppy_wait_ready();

    // Write the data
    for (int i = 0; i < 512; i++) {
        outb(FLOPPY_DATA_PORT, buffer[i]);
    }
}

void floppy_interrupt_handler(struct regs *r) {
    // Read the interrupt status
    uint8_t status = inb(FLOPPY_CMD_PORT);

    // Check if the interrupt was caused by the floppy drive
    if ((status & 0x80) == 0) {
        return;
    }

    // Check if the interrupt was caused by the floppy drive
    if ((status & 0x40) == 0) {
        return;
    }

    // Check if the interrupt was caused by the floppy drive
    if ((status & 0x20) == 0) {
        return;
    }

    // Check if the interrupt was caused by the floppy drive
    if ((status & 0x10) == 0) {
        return;
    }

    // Check if the interrupt was caused by the floppy drive
    if ((status & 0x08) == 0) {
        return;
    }

    // Check if the interrupt was caused by the floppy drive
    if ((status & 0x04) == 0) {
        return;
    }

    // Check if the interrupt was caused by the floppy drive
    if ((status & 0x02) == 0) {
        return;
    }

    // Check if the interrupt was caused by the floppy drive
    if ((status & 0x01) == 0) {
        return;
    }
}