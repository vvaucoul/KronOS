/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_inodes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:20:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/09 21:38:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <memory/memory.h>

uint32_t tinyfs_find_first_free_inode(TinyFS *fs) {
    for (uint32_t i = 0; i < TINYFS_MAX_FILES; i++) {
        if (fs->inodes[i]->used == 0) {
            return (i);
        }
    }
    return (0);
}

TinyFS_Inode *tinyfs_create_inode(TinyFS_Inode *parent_inode, const char name[TINYFS_FILENAME_MAX + 1], uint8_t mode) {
    TinyFS_Inode *tinyfs_inode = (TinyFS_Inode *)kmalloc(sizeof(TinyFS_Inode));

    if (tinyfs_inode == NULL) {
        __THROW("TinyFS: Failed to allocate memory for inode", NULL);
    }

    // Fill inode with default values
    memset(tinyfs_inode, 0, sizeof(TinyFS_Inode));

    // Set inode values
    memcpy(tinyfs_inode->name, name, TINYFS_FILENAME_MAX + 1);
    tinyfs_inode->used = 1;
    tinyfs_inode->mode = mode;
    tinyfs_inode->size = mode == VFS_FILE ? 0 : mode == VFS_DIRECTORY ? TINYFS_BLOCK_SIZE
                                                                      : 0;

    tinyfs_inode->inode_number = tinyfs_find_first_free_inode((TinyFS *)tiny_vfs->fs);
    tinyfs_inode->parent_inode_number = parent_inode ? parent_inode->inode_number : 0;
    memset(tinyfs_inode->links, 0, TINYFS_MAX_FILES);
    tinyfs_inode->nlink = 0;
    memset(tinyfs_inode->block_pointers, 0, TINYFS_MAX_BLOCKS_PER_FILE);

    // Update parent inode
    if (parent_inode) {
        parent_inode->links[parent_inode->nlink] = tinyfs_inode->inode_number;
        parent_inode->nlink += 1;
    }
    return (tinyfs_inode);
}

int tinyfs_delete_inode(TinyFS_Inode *tinyfs_inode) {
    tinyfs_inode->used = 0;
    kfree(tinyfs_inode);
    return (0);
}

TinyFS_Inode tinyfs_read_inode(Vfs *fs, uint32_t inode) {
    TinyFS_Inode tinyfs_inode = {0};
    uint32_t inode_size_sectors = sizeof(TinyFS_Inode) / SECTOR_SIZE;
    if (sizeof(TinyFS_Inode) % SECTOR_SIZE != 0) {
        inode_size_sectors += 1;
    }

    uint8_t read_buffer[SECTOR_SIZE * inode_size_sectors];
    if (fs->fops->read(fs, inode, inode_size_sectors, read_buffer) != 0) {
        __THROW("TinyFS: Failed to read inode", tinyfs_inode);
    }

    memcpy_s(&tinyfs_inode, sizeof(TinyFS_Inode), read_buffer, sizeof(TinyFS_Inode));
    return (tinyfs_inode);
}

int tinyfs_write_inode(Vfs *fs, uint32_t inode, TinyFS_Inode *tinyfs_inode) {
    uint32_t inode_size_sectors = sizeof(TinyFS_Inode) / SECTOR_SIZE;
    if (sizeof(TinyFS_Inode) % SECTOR_SIZE != 0) {
        inode_size_sectors += 1;
    }

    uint8_t write_buffer[SECTOR_SIZE * inode_size_sectors];
    memcpy_s(write_buffer, sizeof(TinyFS_Inode), tinyfs_inode, sizeof(TinyFS_Inode));
    printk("TinyFS: Writing inode %d on sector %d\n", inode, inode_size_sectors);

    if (fs->fops->write(fs, inode, inode_size_sectors, write_buffer) != 0) {
        __THROW("TinyFS: Failed to write inode", -1);
    }

    return (0);
}

TinyFS_Inode *tinyfs_get_inode(Vfs *fs, uint32_t inode) {
    return (((TinyFS *)fs->fs)->inodes[inode]);
}