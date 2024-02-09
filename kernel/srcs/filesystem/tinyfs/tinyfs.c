/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 23:25:17 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/09 12:02:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <memory/memory.h>

Vfs *tiny_vfs = NULL;
TinyFS *tinyfs = NULL;
TinyFS_Inode *tinyfs_root = NULL;
TinyFS_SuperBlock *tinyfs_sb = NULL;
Device *tinyfs_device = NULL;
VfsCache *tinyfs_cache = NULL;

VfsInfo tinyfs_infos = {
    .name = "TinyFS",
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
    .readdir = 0,
    .finddir = 0,
    .mkdir = 0,
    .rmdir = 0,
    .move = 0,
    .chmod = 0,
    .chown = 0,
};

VfsNodeOps tinyfs_nops = {
    .create_node = NULL,
    .remove_node = NULL,
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
    }

    if ((tiny_vfs = vfs_create_fs(tinyfs, &tinyfs_infos, &tinyfs_fspos, &tinyfs_fops, &tinyfs_nops)) == NULL) {
        __THROW("TinyFS: Failed to create VFS", 1);
    } else {
        if ((tinyfs_cache = vfs_create_cache(tiny_vfs, tinyfs_cache_fn)) == NULL) {
            __THROW("TinyFS: Failed to create cache", 1);
        }
    }

    printk("TinyFS: mounting filesystem\n");
    return (vfs_mount(tiny_vfs));
    // return (tiny_fs->fsops->mount(tiny_fs));
}