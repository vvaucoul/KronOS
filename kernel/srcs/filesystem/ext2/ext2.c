/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 23:41:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/18 22:53:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/ext2/ext2.h>
#include <filesystem/vfs/vfs.h>

#include <drivers/device/ide.h>
#include <memory/memory.h>

Vfs *ext2_fs = NULL;
Ext2Node *fs_root = NULL;
Ext2SuperBlock *fs_superblock = NULL;

VfsInfo ext2_fs_info = {
    .name = "ext2",
    .d_name = "Extended file system 2",
    .version = "0.1",
    .type = EXT2_FILESYSTEM};

VfsFsOps ext2_fsops = {
    .mount = ext2_mount,
    .unmount = ext2_unmount,
};

VfsFileOps ext2_fops = {
    .read = 0,
    .write = 0,
    .unlink = 0,
    .open = 0,
    .close = 0,
    .readdir = 0,
    .finddir = 0,
    .mkdir = 0,
    .rmdir = 0,
    .move = 0,
    .chmod = 0,
    .chown = 0,
};

VfsNodeOps ext2_nops = {
    .create_node = 0,
    .remove_node = 0,
};

// ! ||--------------------------------------------------------------------------------||
// ! ||                                      EXT2                                      ||
// ! ||--------------------------------------------------------------------------------||

int ext2_init(void) {
    // Todo: Replace ext2_init by ext2_mount (and add ext2_unmount)
    // Todo: VFS must call ext2_mount to mount the filesystem instead of calling ext2_init
    printk("Initializing EXT2 file system...\n");

    ext2_fs = vfs_create_fs(&ext2_fs_info, &ext2_fsops, &ext2_fops, &ext2_nops);
    if (ext2_fs == NULL) {
        __THROW("EXT2: Failed to create EXT2 filesystem", 1);
    }
    return (0);
}