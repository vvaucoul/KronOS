/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 23:41:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 18:42:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/ext2/ext2.h>

Ext2Inode *fs_root = NULL;

void ext2_delete(Ext2Inode *inode, char *name) {
    if (inode->fops.unlink != 0) {
        inode->fops.unlink(inode, name);
    } else {
        __WARND("Cannot delete file");
    }
}

void ext2_move(Ext2Inode *inode, char *name, char *new_name) {
    if (inode->fops.move != 0) {
        inode->fops.move(inode, name, new_name);
    } else {
        __WARND("Cannot move file or directory");
    }
}

void ext2_create_directory(Ext2Inode *inode, char *name, uint16_t permission) {
    if (inode->fops.mkdir != 0) {
        inode->fops.mkdir(inode, name, permission);
    } else {
        __WARND("Cannot create directory");
    }
}

void ext2_flush_disk_cache(Ext2Inode *inode) {
    if (inode->fops.flush != 0) {
        inode->fops.flush(inode);
    } else {
        __WARND("Cannot flush the disk cache");
    }
}

uint32_t ext2_read(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (node == NULL || buffer == NULL) {
        __THROW("EXT2: Cannot read from disk", 1);
    }

    if (node->fops.read != 0) {
        printk("Reading from disk...\n");
        return node->fops.read(node, offset, size, buffer);
    } else {
        __WARND("Cannot read from disk");
        return 0;
    }
}

uint32_t ext2_write_full(Ext2Inode *node, uint32_t size, uint8_t *buffer) {
    if (node->fops.write != 0) {
        printk("Writing to disk...\n");
        // Écrire les données sur tout le fichier
        uint32_t result = node->fops.write(node, 0, size, buffer);
        // Remplir le reste du fichier avec des zéros
        uint8_t zero = 0;
        for (uint32_t i = size; i < node->length; i++) {
            node->fops.write(node, i, 1, &zero);
        }
        // Tronquer le fichier à la taille des données écrites
        node->length = size;

        node->fops.flush(node); // Flush the disk cache

        return result;
    } else {
        __WARND("Cannot write to disk");
        return 0;
    }
}

uint32_t ext2_write(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (node->fops.write != 0) {
        printk("Writing to disk...\n");
        return node->fops.write(node, offset, size, buffer);
    } else {
        __WARND("Cannot write to disk");
        return 0;
    }
}

void ext2_open(Ext2Inode *node, uint8_t read, uint8_t write) {
    if (node->fops.open != 0)
        return node->fops.open(node);
}

void ext2_close(Ext2Inode *node) {
    if (node->fops.close != 0)
        return node->fops.close(node);
}

struct dirent *ext2_readdir(Ext2Inode *node, uint32_t index) {
    if ((node->flags & 0x07) == FS_DIRECTORY && node->fops.readdir != 0)
        return node->fops.readdir(node, index);
    else
        return 0;
}

Ext2Inode *ext2_finddir(Ext2Inode *node, char *name) {
    if ((node->flags & 0x07) == FS_DIRECTORY && node->fops.finddir != 0)
        return node->fops.finddir(node, name);
    else
        return 0;
}