/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 18:45:52 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/ext2/ext2.h>
#include <filesystem/vfs.h>

#include <memory/memory.h>

/*
VfsFileOps ext2_file_ops = {
    .read = ext2_read,
    .write = ext2_write,
    .open = ext2_open,
    .close = ext2_close,
};

VfsFsOps ext2_fs_ops = {
    .mount = ext2_mount,
    .unmount = ext2_unmount,
};

Vfs ext2_fs = {
    .fs_name = EXT2_FILESYSTEM_NAME,
    .fs_root = NULL,
    .init = ext2_init,
};

Vfs *fs[FILESYSTEMS_COUNT] = {
    &ext2_fs,
};

Vfs *vfs_init(const char *fs_name) {
    for (uint8_t i = 0; i < FILESYSTEMS_COUNT; i++) {
        if (strcmp(fs[i]->fs_name, fs_name) == 0) {

            fs[i]->init();
            fs[i]->fs_root = NULL1;

            return (fs[i]);
        }
    }
    __THROW("vfs_init: filesystem not found", 1);
}
*/