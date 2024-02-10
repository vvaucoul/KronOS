/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 23:25:17 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 12:26:46 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <memory/memory.h>

Vfs *tiny_vfs = NULL;
static TinyFS *tinyfs = NULL;
TinyFS_SuperBlock *tinyfs_sb = NULL;
Device *tinyfs_device = NULL;
VfsCache *tinyfs_cache = NULL;

VfsInfo tinyfs_infos = {
    .name = TINYFS_FILESYSTEM_NAME,
    .d_name = "tinyfs",
    .version = "0.1",
    .type = TINYFS_FILESYSTEM};

VfsFsOps tinyfs_fspos = {
    .mount = tinyfs_mount,
    .unmount = tinyfs_unmount,
};

VfsFileOps tinyfs_fops = {
    .read = tinyfs_read,
    .write = tinyfs_write,
    .unlink = 0,
    .open = 0,
    .close = 0,
    .readdir = tinyfs_readdir,
    .finddir = tinyfs_finddir,
    .mkdir = tinyfs_mkdir,
    .rmdir = 0,
    .move = 0,
    .chmod = 0,
    .chown = 0,
    .create = tinyfs_create,
};

VfsNodeOps tinyfs_nops = {
    .create_node = NULL,
    .remove_node = NULL,
    .get_name = tinyfs_get_name,
    .get_parent = tinyfs_get_parent,
};

VfsCacheFn tinyfs_cache_fn = {
    .vfs_set_cache_links = tinyfs_set_cache_links,
    .vfs_get_cache_links = tinyfs_get_cache_links,
};

int tinyfs_init(void) {
    printk("TinyFS: Initializing file system\n");

    // Todo: Update device index
    tinyfs_device = device_get(0);
    if (tinyfs_device == NULL) {
        __THROW("TinyFS: Failed to get device", 1);
    } else {
        if ((tinyfs = kmalloc(sizeof(TinyFS))) == NULL) {
            __THROW("TinyFS: Failed to allocate memory for tinyfs", 1);
        }

        tinyfs->superblock = NULL;
        memset(tinyfs->inodes, 0, sizeof(TinyFS_Inode) * TINYFS_MAX_FILES);

        for (uint32_t i = 0; i < TINYFS_MAX_BLOCKS; i++) {
            memset(tinyfs->data_blocks[i], 0, TINYFS_BLOCK_SIZE);
        }
    }

    if ((tiny_vfs = vfs_create_fs(tinyfs, &tinyfs_infos, &tinyfs_fspos, &tinyfs_fops, &tinyfs_nops)) == NULL) {
        __THROW("TinyFS: Failed to create VFS", 1);
    }
    if ((tinyfs_cache = vfs_create_cache(tiny_vfs, tinyfs_cache_fn)) == NULL) {
        __THROW("TinyFS: Failed to create cache", 1);
    }

    return (0);
}