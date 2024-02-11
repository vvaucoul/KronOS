/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_fsops.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:43:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 14:56:32 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <memory/memory.h>

static int __tinyfs_init_fs(TinyFS *fs) {
    printk("TinyFS: Initializing file system\n");
}

int tinyfs_mount(void *fs) {
    TinyFS *tfs = ((Vfs *)fs)->fs;
    printk("TinyFS: Mounting\n");

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                   SUPERBLOCK                                   ||
    // ! ||--------------------------------------------------------------------------------||

    if ((tfs->superblock = (TinyFS_SuperBlock *)kmalloc(sizeof(TinyFS_SuperBlock))) == NULL) {
        __THROW("TinyFS: Failed to allocate memory for superblock", 1);
    }

    uint8_t read_buffer[sizeof(TinyFS_SuperBlock)];
    printk("TinyFS: Reading superblock\n");

    // Todo: Use tiny_vfs instead of device
    if (tinyfs_device->sread(tinyfs_device->device, 0, sizeof(TinyFS_SuperBlock), read_buffer) != 0) {
        __THROW("TinyFS: Failed to read superblock", 1);
    }

    printk("TinyFS: read done\n");
    memcpy_s(tfs->superblock, sizeof(TinyFS_SuperBlock), read_buffer, sizeof(TinyFS_SuperBlock));

    if (tfs->superblock->magic_number != TINYFS_MAGIC) {
        __THROW("TinyFS: Invalid magic number", 1);
    } else {
        printk("TinyFS: Valid magic number 0x%x\n", tfs->superblock->magic_number);
    }

    // Update superblock information
    ((TinyFS *)(tiny_vfs->fs))->superblock->free_inodes -= 2;
    ((TinyFS *)(tiny_vfs->fs))->superblock->free_blocks -= 2;

    if ((tinyfs_write_superblock(tiny_vfs)) != 0) {
        printk("TinyFS: Formatting (write superblock) failed\n");
        return (-1);
    }

    printk("TinyFS: Total inodes: %d\n", tfs->superblock->total_inodes);
    printk("TinyFS: Free inodes: %d\n", tfs->superblock->free_inodes);
    printk("TinyFS: Total blocks: %d\n", tfs->superblock->total_blocks);
    printk("TinyFS: Free blocks: %d\n", tfs->superblock->free_blocks);

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                     INODES                                     ||
    // ! ||--------------------------------------------------------------------------------||

    uint32_t inodes_size_sectors = sizeof(TinyFS_Inode) / SECTOR_SIZE;
    if (sizeof(TinyFS_Inode) % SECTOR_SIZE != 0) {
        inodes_size_sectors += 1;
    }
    printk("TinyFS: Inodes size in sectors: %d\n", inodes_size_sectors);

    uint8_t read_buffer_inode[sizeof(TinyFS_Inode)];
    uint32_t inode_offset = sizeof(TinyFS_SuperBlock);

    printk("TinyFS: Reading inodes\n");
    for (uint32_t i = 0; i < tfs->superblock->total_inodes; i++) {
        memset(read_buffer_inode, 0, sizeof(read_buffer_inode));

        // Todo: Use tiny_vfs instead of device
        if (tinyfs_device->sread(tinyfs_device->device, inode_offset, sizeof(TinyFS_Inode), read_buffer_inode) != 0) {
            __THROW("TinyFS: Failed to read inodes", 1);
        } else {
            TinyFS_Inode *tmp_inode = kmalloc(sizeof(TinyFS_Inode));

            if (tmp_inode == NULL) {
                __WARND("TinyFS: Failed to allocate memory for inode [%d]", 1);
                continue;
            }
            memcpy_s(tmp_inode, sizeof(TinyFS_Inode), read_buffer_inode, sizeof(TinyFS_Inode));
            tfs->inodes[i] = tmp_inode;
            
            memcpy_s(tfs->inodes[i], sizeof(TinyFS_Inode), read_buffer_inode, sizeof(TinyFS_Inode));

            printk("TinyFS: Read Inode [%d]\n", i);
            printk("TinyFS: Inode [%d] name: [%s] | %d %d\n", tfs->inodes[i]->inode_number, tfs->inodes[i]->name, tfs->inodes[i]->used, tfs->inodes[i]->size);
            inode_offset += sizeof(TinyFS_Inode);
            kmsleep(50);
        }
    }

    tiny_vfs->fs_root = (VfsNode *)(tfs->inodes[0]);
    printk("TinyFS: Root: 0x%x | %s\n", tiny_vfs->fs_root, ((TinyFS_Inode *)tiny_vfs->fs_root)->name);

    printk("TinyFS: Mounted\n");
    return (0);
}

int tinyfs_unmount(void *fs) {
    TinyFS *tfs = ((Vfs *)fs)->fs;

    printk("TinyFS: Unmounting\n");
    kfree(tfs->superblock);
    printk("TinyFS: Unmounted\n");
    return (0);
}
