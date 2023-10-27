/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 23:41:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 12:57:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/ext2/ext2.h>

Ext2Inode *fs_root = 0; // The root of the filesystem.



uint32_t read_fs(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (node->read != 0) {
        printk("Reading from disk...\n");
        return node->read(node, offset, size, buffer);
    }
    else {
        __WARND("Cannot read from disk");
        return 0;
    }
}

uint32_t write_fs(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (node->write != 0) {
        printk("Writing to disk...\n");
        return node->write(node, offset, size, buffer);
    }
    else {
        __WARND("Cannot write to disk");
        return 0;
    }
}

void open_fs(Ext2Inode *node, uint8_t read, uint8_t write) {
    if (node->open != 0)
        return node->open(node);
}

void close_fs(Ext2Inode *node) {
    if (node->close != 0)
        return node->close(node);
}

struct dirent *readdir_fs(Ext2Inode *node, uint32_t index) {
    if ((node->flags & 0x07) == FS_DIRECTORY && node->readdir != 0)
        return node->readdir(node, index);
    else
        return 0;
}

Ext2Inode *finddir_fs(Ext2Inode *node, char *name) {
    if ((node->flags & 0x07) == FS_DIRECTORY && node->finddir != 0)
        return node->finddir(node, name);
    else
        return 0;
}