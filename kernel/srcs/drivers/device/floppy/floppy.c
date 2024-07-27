/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floppy.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 15:11:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 08:47:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/floppy.h>
#include <system/io.h>
#include <system/irq.h>

#include <macros.h>

static volatile bool floppy_irq_received = false;

void floppy_irq_handler(__unused__ struct regs *r) {
    // Handle the IRQ...

    // Signal that the IRQ has been received
    floppy_irq_received = true;
}

static void fdc_wait_irq() {
    // Install the IRQ handler
    irq_install_handler(IRQ_FLOPPY, floppy_irq_handler);

    // Wait for the IRQ
    while (!floppy_irq_received) {
        // This is a busy wait loop, you might want to put the process to sleep instead
    }

    // Reset the IRQ received flag
    floppy_irq_received = false;
}

static void fdc_send_command(uint8_t cmd) {
    for (int i = 0; i < 600; i++) {
        if (inb(FDC_MSR) & 0x80) {
            outb(FDC_DATA, cmd);
            return;
        }
    }
}

static uint8_t fdc_read_data() {
    for (int i = 0; i < 600; i++) {
        if (inb(FDC_MSR) & 0x80) {
            return inb(FDC_DATA);
        }
    }
    return 0;
}

static void fdc_check_interrupt(int *st0, int *cyl) {
    fdc_send_command(0x08);
    *st0 = fdc_read_data();
    *cyl = fdc_read_data();
}

void fdc_initialize() {
    outb(FDC_DOR, 0x00); // Désactiver le contrôleur
    outb(FDC_DOR, 0x0C); // Activer le contrôleur

    printk("FDC initialized\n");
    fdc_wait_irq(); // Attendre l'IRQ
    printk("FDC IRQ received\n");
    fdc_send_command(0x03); // Spécifiez la vitesse de transfert
    fdc_send_command(0xDF);
    fdc_send_command(0x02);
}

void fdc_motor_on() {
    outb(FDC_DOR, 0x1C);
    // Attendre que le moteur soit prêt
}

void fdc_motor_off() {
    outb(FDC_DOR, 0x0C);
}

void fdc_read_sector(FloppyDisk *fd, uint8_t head, uint8_t track, uint8_t sector) {
    fdc_motor_on();

    fdc_send_command(0x46); // Lire secteur avec MFM
    fdc_send_command(head << 2 | fd->drive);
    fdc_send_command(track);
    fdc_send_command(head);
    fdc_send_command(sector);
    fdc_send_command(2);    // Taille du secteur = 512 octets
    fdc_send_command(18);   // Dernier secteur
    fdc_send_command(0x1B); // Gap3 length
    fdc_send_command(0xFF); // Taille du secteur

    fdc_wait_irq(); // Attendre l'IRQ

    for (int i = 0; i < SECTOR_SIZE; i++) {
        fd->buffer[i] = fdc_read_data();
    }

    int st0, cyl;
    fdc_check_interrupt(&st0, &cyl);
    fdc_motor_off();
}

void fdc_write_sector(FloppyDisk *fd, uint8_t head, uint8_t track, uint8_t sector) {
    fdc_motor_on();

    fdc_send_command(0x45); // Écrire secteur avec MFM
    fdc_send_command(head << 2 | fd->drive);
    fdc_send_command(track);
    fdc_send_command(head);
    fdc_send_command(sector);
    fdc_send_command(2);    // Taille du secteur = 512 octets
    fdc_send_command(18);   // Dernier secteur
    fdc_send_command(0x1B); // Gap3 length
    fdc_send_command(0xFF); // Taille du secteur

    for (int i = 0; i < SECTOR_SIZE; i++) {
        outb(FDC_DATA, fd->buffer[i]);
    }

    fdc_wait_irq(); // Attendre l'IRQ

    int st0, cyl;
    fdc_check_interrupt(&st0, &cyl);
    fdc_motor_off();
}
