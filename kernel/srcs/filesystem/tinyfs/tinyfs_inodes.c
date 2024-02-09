/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_inodes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:20:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/09 13:39:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>

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

TinyFS_Inode tinyfs_get_inode(Vfs *fs, uint32_t inode) {
    return (((TinyFS *)fs->fs)->inodes[inode]);
}