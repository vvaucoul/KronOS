/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_updater.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:00:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/09 22:56:31 by vvaucoul         ###   ########.fr       */
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
    return (tinyfs_device->swrite(tinyfs_device->device, 0, sizeof(TinyFS_SuperBlock), ((TinyFS *)fs->fs)->superblock));
}