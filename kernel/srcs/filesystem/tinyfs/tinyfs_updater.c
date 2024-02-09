/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_updater.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:00:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/09 13:18:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>

int tinyfs_write_superblock(Vfs *fs) {
    uint32_t superblock_size_sectors = sizeof(TinyFS_SuperBlock) / SECTOR_SIZE;
    if (sizeof(TinyFS_SuperBlock) % SECTOR_SIZE != 0) {
        superblock_size_sectors += 1;
    }

    return (tinyfs_device->write(tinyfs_device->device, 0, superblock_size_sectors, ((TinyFS *)fs->fs)->superblock));
}