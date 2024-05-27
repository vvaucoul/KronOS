/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_updater.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:00:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/13 11:39:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>

/**
 * @brief Write superblock to disk
 *
 * @param fs
 *
 * @return int
 */
int tinyfs_write_superblock(Vfs *fs) {
    TinyFS *tinyfs = (TinyFS *)fs->fs;

    return (tinyfs->fs.device->swrite(tinyfs->fs.device->device, 0, sizeof(TinyFS_SuperBlock), tinyfs->superblock));
}