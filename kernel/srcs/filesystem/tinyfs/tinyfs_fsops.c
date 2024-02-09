/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_fsops.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:43:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 00:10:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <memory/memory.h>

static int __tinyfs_init_fs(TinyFS *fs) {
    printk("TinyFS: Initializing file system\n");
}

int tinyfs_mount(void *fs) {
    Vfs *vfs = (Vfs *)fs;
    printk("TinyFS: Mounting\n");

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                   SUPERBLOCK                                   ||
    // ! ||--------------------------------------------------------------------------------||

    if ((((TinyFS *)(vfs->fs))->superblock = (TinyFS_SuperBlock *)kmalloc(sizeof(TinyFS_SuperBlock))) == NULL) {
        __THROW("TinyFS: Failed to allocate memory for superblock", 1);
    }

    uint32_t superblock_size_sectors = sizeof(TinyFS_SuperBlock) / SECTOR_SIZE;
    if (sizeof(TinyFS_SuperBlock) % SECTOR_SIZE != 0) {
        superblock_size_sectors += 1;
    }

    uint8_t read_buffer[sizeof(TinyFS_SuperBlock)];
    printk("TinyFS: Reading superblock\n");

    // Todo: Use tiny_vfs instead of device
    if (tinyfs_device->sread(tinyfs_device->device, 0, sizeof(TinyFS_SuperBlock), read_buffer) != 0) {
        __THROW("TinyFS: Failed to read superblock", 1);
    }

    printk("TinyFS: read done\n");
    memcpy_s(((TinyFS *)(vfs->fs))->superblock, sizeof(TinyFS_SuperBlock), read_buffer, sizeof(TinyFS_SuperBlock));

    if (((TinyFS *)(vfs->fs))->superblock->magic_number != TINYFS_MAGIC) {
        __THROW("TinyFS: Invalid magic number", 1);
    } else {
        printk("TinyFS: Valid magic number 0x%x\n", ((TinyFS *)(vfs->fs))->superblock->magic_number);
    }

    // Update superblock information
    ((TinyFS *)(tiny_vfs->fs))->superblock->free_inodes -= 2;
    ((TinyFS *)(tiny_vfs->fs))->superblock->free_blocks -= 2;

    if ((tinyfs_write_superblock(tiny_vfs)) != 0) {
        printk("TinyFS: Formatting (write superblock) failed\n");
        return (-1);
    }

    printk("TinyFS: Total inodes: %d\n", ((TinyFS *)(vfs->fs))->superblock->total_inodes);
    printk("TinyFS: Free inodes: %d\n", ((TinyFS *)(vfs->fs))->superblock->free_inodes);
    printk("TinyFS: Total blocks: %d\n", ((TinyFS *)(vfs->fs))->superblock->total_blocks);
    printk("TinyFS: Free blocks: %d\n", ((TinyFS *)(vfs->fs))->superblock->free_blocks);
    
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
    for (uint32_t i = 0; i < ((TinyFS *)(vfs->fs))->superblock->total_inodes; i++) {
        memset(read_buffer_inode, 0, sizeof(read_buffer_inode));

        // Todo: Use tiny_vfs instead of device
        if (tinyfs_device->sread(tinyfs_device->device, inode_offset, sizeof(TinyFS_Inode), read_buffer_inode) != 0) {
            __THROW("TinyFS: Failed to read inodes", 1);
        } else {
            TinyFS_Inode tmp_inode = {0};
            memcpy_s(&tmp_inode, sizeof(TinyFS_Inode), read_buffer_inode, sizeof(TinyFS_Inode));
            ((TinyFS *)(vfs->fs))->inodes[i] = &tmp_inode;
            // memcpy_s(((TinyFS *)(vfs->fs))->inodes[i], sizeof(TinyFS_Inode), read_buffer_inode, sizeof(TinyFS_Inode));

            printk("TinyFS: Read Inode [%d]\n", i);
            printk("TinyFS: Inode [%d] name: [%s] | %d %d\n", ((TinyFS *)(vfs->fs))->inodes[i]->inode_number, ((TinyFS *)(vfs->fs))->inodes[i]->name, ((TinyFS *)(vfs->fs))->inodes[i]->used, ((TinyFS *)(vfs->fs))->inodes[i]->size);
            inode_offset += sizeof(TinyFS_Inode);
            kmsleep(50);
        }
    }
    printk("TinyFS: Mounted\n");
    return (0);
}

int tinyfs_unmount(void *fs) {
    Vfs *vfs = (Vfs *)fs;

    printk("TinyFS: Unmounting\n");
    kfree(((TinyFS *)(vfs->fs))->superblock);
    printk("TinyFS: Unmounted\n");
    return (0);
}
