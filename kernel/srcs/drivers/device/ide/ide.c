/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ide.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 00:34:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/17 21:43:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ide.h>
#include <memory/memory.h>
#include <system/io.h>

IDEDevice *ide_devices[MAX_IDE_DEVICES];

IDEDevice *ide_get_device(uint8_t drive) {
    if (drive > 3) {
        return (NULL);
    }
    return (ide_devices[drive]);
}

void ide_primary_irq_handler(__unused__ struct regs *regs) {
    printk("IDE Primary IRQ\n");
    int status = inb(ATA_PRIMARY_IO + ATA_REG_STATUS);

    if (status & ATA_SR_ERR) {
        ide_error_msg(status, true);
    }
}

void ide_secondary_irq_handler(__unused__ struct regs *regs) {
    printk("IDE Secondary IRQ\n");

    int status = inb(ATA_SECONDARY_IO + ATA_REG_STATUS);

    if (status & ATA_SR_ERR) {
        ide_error_msg(status, true);
    }
}

int ide_device_init(IDEDevice *dev, IDEChannel channel, IDEDrive drive,
                    uint16_t base, uint16_t ctrl, uint16_t bmide, uint8_t nIEN) {
    dev->reserved = 1;

    dev->channel = channel;
    dev->drive = drive;

    dev->regs.base = base;
    dev->regs.ctrl = ctrl;
    dev->regs.bmide = bmide;
    dev->regs.nIEN = nIEN;

    if ((ide_identify(dev)) != 0) {
        __WARND("Failed to identify IDE Device\n");
        return (-1);
    }
    return (0);
}

int ide_init(void) {
    irq_install_handler(IRQ_ATA1, ide_primary_irq_handler);
    irq_install_handler(IRQ_ATA2, ide_secondary_irq_handler);

    uint16_t ide_io_ports[2] = {0x1F0, 0x170};
    uint16_t ide_ctrl_ports[2] = {0x3F0, 0x370};
    uint16_t ide_bus_master_ports[2] = {0x376, 0x276};
    IDEChannel ide_channels[2] = {IDE_PRIMARY, IDE_SECONDARY};
    IDEDrive ide_drives[2] = {IDE_MASTER, IDE_SLAVE};

    printk("Initializing IDE Devices\n");
    for (uint8_t bus = 0; bus < 2; bus++) {
        for (uint8_t drive = 0; drive < 2; drive++) {

            if ((ide_device_is_present(bus, drive)) == 0) {
                __INFOD("No device found on channel [%d] drive [%d]", bus, drive);
                continue;
            }

            IDEDevice *dev = ide_devices[bus * 2 + drive];

            if ((dev = (IDEDevice *)kmalloc(sizeof(IDEDevice))) == NULL) {
                __WARND("Failed to allocate memory for IDE Device [%d]", bus * 2 + drive);
                continue;
            }

            ide_device_init(dev, ide_channels[bus], ide_drives[drive],
                            ide_io_ports[bus], ide_ctrl_ports[bus], ide_bus_master_ports[bus], 0);
            ide_devices[bus * 2 + drive] = dev;
            kmsleep(4);
        }
    }
    return (0);
}