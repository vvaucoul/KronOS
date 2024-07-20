/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 23:25:17 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/30 18:34:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <memory/memory.h>

VfsInfo tinyfs_infos = {
    .name = TINYFS_FILESYSTEM_NAME,
    .d_name = TINYFS_FILESYSTEM_D_NAME,
    .version = TINYFS_VERSION,
    .type = TINYFS_FILESYSTEM};

VfsFsOps tinyfs_fspos = {
    .mount = tinyfs_mount,
    .unmount = tinyfs_unmount,
};

VfsFileOps tinyfs_fops = {
    .read = tinyfs_read,
    .write = tinyfs_write,
    .unlink = NULL,
    .open = NULL,
    .close = NULL,
    .readdir = tinyfs_readdir,
    .finddir = tinyfs_finddir,
    .mkdir = tinyfs_mkdir,
    .rmdir = NULL,
    .move = NULL,
    .chmod = NULL,
    .chown = NULL,
    .create = tinyfs_create,
    .stat = tinyfs_stat,
};

VfsNodeOps tinyfs_nops = {
    .create_node = NULL,
    .remove_node = NULL,
    .get_name = tinyfs_get_name,
    .get_parent = tinyfs_get_parent,
    .get_links = tinyfs_get_links,
};

TinyFS *tinyfs_init(Device *device) {
    printk("TinyFS: Initializing file system\n");
    TinyFS *tinyfs = NULL;

    if (device == NULL) {
        __THROW("TinyFS: Failed to get device", NULL);
    } else {
        if ((tinyfs = kmalloc(sizeof(TinyFS))) == NULL) {
            __THROW("TinyFS: Failed to allocate memory for tinyfs", NULL);
        }

        /* Init SuperBlock */
        tinyfs->superblock = NULL;
        tinyfs->fs.device = device;

        /* Init Inodes */
        memset_s(tinyfs->inodes, sizeof(TinyFS_Inode) * TINYFS_MAX_FILES, 0, sizeof(TinyFS_Inode) * TINYFS_MAX_FILES);

        /* Init Data Blocks */
        for (uint32_t i = 0; i < TINYFS_MAX_BLOCKS; i++) {
            memset_s(tinyfs->data_blocks[i], TINYFS_BLOCK_SIZE, 0, TINYFS_BLOCK_SIZE);
        }

        /* Init VFS */
        if ((tinyfs->fs.vfs = vfs_create_fs(tinyfs, &tinyfs_infos, &tinyfs_fspos, &tinyfs_fops, &tinyfs_nops)) == NULL) {
            __THROW("TinyFS: Failed to create VFS", NULL);
        }
    }

    return (tinyfs);
}