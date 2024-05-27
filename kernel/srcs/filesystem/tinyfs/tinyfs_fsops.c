/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_fsops.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:43:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/13 17:36:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <memory/memory.h>

static int __tinyfs_init_fs(TinyFS *fs) {
    printk("TinyFS: Initializing file system\n");
}

int tinyfs_mount(void *fs) {
    TinyFS *tfs = ((Vfs *)fs)->fs;

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                   SUPERBLOCK                                   ||
    // ! ||--------------------------------------------------------------------------------||

    if ((tfs->superblock = (TinyFS_SuperBlock *)kmalloc(sizeof(TinyFS_SuperBlock))) == NULL) {
        __THROW("TinyFS: Failed to allocate memory for superblock", 1);
    }

    uint8_t read_buffer[sizeof(TinyFS_SuperBlock)];

    if (tfs->fs.device->sread(tfs->fs.device->device, 0, sizeof(TinyFS_SuperBlock), read_buffer) != 0) {
        __THROW("TinyFS: Failed to read superblock", 1);
    } else {
        memcpy_s(tfs->superblock, sizeof(TinyFS_SuperBlock), read_buffer, sizeof(TinyFS_SuperBlock));
    }

    if (tfs->superblock->magic_number != TINYFS_MAGIC) {
        __THROW("TinyFS: Invalid magic number", 1);
    }

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                     INODES                                     ||
    // ! ||--------------------------------------------------------------------------------||

    uint8_t read_buffer_inode[sizeof(TinyFS_Inode)];
    uint32_t inode_offset = sizeof(TinyFS_SuperBlock);

    for (uint32_t i = 0; i < tfs->superblock->total_inodes; i++) {
        memset(read_buffer_inode, 0, sizeof(read_buffer_inode));

        // Todo: Use tiny_vfs instead of device
        if (tfs->fs.device->sread(tfs->fs.device->device, inode_offset, sizeof(TinyFS_Inode), read_buffer_inode) != 0) {
            __THROW("TinyFS: Failed to read inodes", 1);
        } else {
            TinyFS_Inode *tmp_inode = kmalloc(sizeof(TinyFS_Inode));

            if (tmp_inode == NULL) {
                __WARND("TinyFS: Failed to allocate memory for inode [%d]", 1);
                continue;
            }
            memcpy_s(tmp_inode, sizeof(TinyFS_Inode), read_buffer_inode, sizeof(TinyFS_Inode));
            memcpy_s(tfs->inodes[i], sizeof(TinyFS_Inode), read_buffer_inode, sizeof(TinyFS_Inode));
            tfs->inodes[i] = tmp_inode;
            tfs->inodes[i]->fs = tfs;

            if (tmp_inode->used == 1) {
                tfs->superblock->free_inodes -= 1;
                if (tmp_inode->block_pointers[0] != 0) {
                    tfs->superblock->free_blocks -= 1;
                }
            }
            inode_offset += sizeof(TinyFS_Inode);
        }
    }

    if ((tinyfs_write_superblock(tfs->fs.vfs)) != 0) {
        printk("TinyFS: Formatting (write superblock) failed\n");
        return (-1);
    }

    tfs->fs.vfs->fs_root = tfs->fs.vfs->fs_current_node = (VfsNode *)(tfs->inodes[0]);

    printk(_LGREEN "TinyFS: Mounted\n" _END);
    return (0);
}

int tinyfs_unmount(void *fs) {
    TinyFS *tfs = ((Vfs *)fs)->fs;

    printk("TinyFS: Unmounting\n");
    kfree(tfs->superblock);

    for (uint32_t i = 0; i < tfs->superblock->total_inodes; i++) {
        kfree(tfs->inodes[i]);
    }

    kfree(tfs->fs.vfs);
    kfree(tfs);
    printk("TinyFS: Unmounted\n");
    return (0);
}
