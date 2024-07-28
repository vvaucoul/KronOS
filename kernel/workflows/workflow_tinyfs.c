/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_tinyfs.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 10:35:17 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 22:13:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/devices.h>
#include <drivers/device/ide.h>
#include <fs/tinyfs/tinyfs.h>
#include <workflows/workflows.h>

#include <assert.h>

Device *g_hdb = NULL;
IDEDevice *ide = NULL;
TinyFS *tfs = NULL;

static int __format_disk(void) {
    uint32_t disk_size = ide_device_get_size(ide) * 1024;
    const char buffer[512] = {0};

    printk(_LYELLOW "- Formating hdb [%u] octets\n" _END, disk_size);

    for (uint32_t i = 0; i < disk_size; i += 512) {
        uint32_t size = 512;

        if (size + i > disk_size) {
            size = disk_size - i;
        }

        if ((ide_device_simple_write(ide, i, size, buffer)) != 0) {
            // Todo: get perror / strerror etc.. with errno kernel side
            printk("Error: writing [%d] octets to disk\n", size);
            // perror("Error: ide_device_simple_write");
            return (1);
        }
    }

    printk(_LGREEN "- Disk formated successfully !\n" _END);
    return (0);
}

void workflow_tinyfs(void) {
    __WORKFLOW_HEADER();

    Device *hdb = device_get(1);

    if (hdb == NULL) {
        __THROW_NO_RETURN("HDB is requiered for [%s]", __FUNCTION__);
    } else {
        g_hdb = hdb;
        ide = (IDEDevice *)hdb->device;
    }

    /* Format (reset to 0x0) disk content */
    if ((__format_disk()) != 0) {
        return;
    }

    assert((tfs = tinyfs_init(hdb)) != NULL);
    assert(tinyfs_formater(tfs, true) == 0);
    assert(vfs_mount(tfs->fs.vfs) == 0);

    __WORKFLOW_FOOTER();
}