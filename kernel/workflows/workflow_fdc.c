/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_fdc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 14:27:53 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 19:10:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>
#include <drivers/device/floppy.h>
#include <mm/mm.h>

void workflow_fdc(void) {
    __WORKFLOW_HEADER();

    FloppyDisk fd;
    fd.drive = 0;
    fd.buffer = (uint8_t *)kmalloc(SECTOR_SIZE);

    fdc_initialize();

    // Lire le secteur 1 de la piste 0, tête 0
    fdc_read_sector(&fd, 0, 0, 1);
    printk("Data read: %s\n", fd.buffer);

    // Écrire des données dans le secteur 1 de la piste 0, tête 0
    memset(fd.buffer, 'A', SECTOR_SIZE);
    fdc_write_sector(&fd, 0, 0, 1);

    // Relire le secteur pour vérifier
    fdc_read_sector(&fd, 0, 0, 1);
    printk("Data read: %s\n", fd.buffer);

    kfree(fd.buffer);

    __WORKFLOW_FOOTER();
}