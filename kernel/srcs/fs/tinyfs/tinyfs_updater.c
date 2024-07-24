/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_updater.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:00:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 10:47:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/tinyfs/tinyfs.h>

/**
 * Writes the superblock of the TinyFS filesystem.
 *
 * This function is responsible for writing the superblock of the TinyFS filesystem.
 * It takes a pointer to the Vfs structure representing the filesystem as a parameter.
 *
 * @param fs A pointer to the Vfs structure representing the filesystem.
 * @return Returns an integer indicating the success or failure of the operation.
 *         0 indicates success, while a negative value indicates failure.
 */
int tinyfs_write_superblock(Vfs *fs) {
    TinyFS *tinyfs = (TinyFS *)fs->fs;

    return (tinyfs->fs.device->swrite(tinyfs->fs.device->device, 0, sizeof(TinyFS_SuperBlock), tinyfs->superblock));
}