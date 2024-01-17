/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 23:41:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/16 17:08:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/ext2/ext2.h>
#include <memory/memory.h>

Ext2Inode *fs_root = NULL;

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  EXT2 BITMAPS                                  ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief Structure representing the bitmap in the ext2 file system.
 *
 * The bitmap is used to keep track of the allocation status of blocks or inodes in the file system.
 * Each bit in the bitmap represents the allocation status of a specific block or inode.
 * If the bit is set to 1, it means the block or inode is allocated.
 * If the bit is set to 0, it means the block or inode is free and available for allocation.
 */

static uint8_t bmap_blocks[BLOCKS_PER_GROUP / 8];
static uint8_t bmap_inodes[INODES_PER_GROUP / 8];

void init_bitmaps() {
    memset(bmap_blocks, 0, sizeof(bmap_blocks));
    memset(bmap_inodes, 0, sizeof(bmap_inodes));
}

int alloc_block() {
    int i;
    for (i = 0; i < BLOCKS_PER_GROUP; ++i) {
        if (!BMAP_GET(bmap_blocks, i)) {
            BMAP_SET(bmap_blocks, i);
            return i + FIRST_DATA_BLOCK;
        }
    }
    return -1;
}

int free_block(int block_num) {
    int block_idx = block_num - FIRST_DATA_BLOCK;
    if (block_idx >= 0 && block_idx < BLOCKS_PER_GROUP) {
        BMAP_CLEAR(bmap_blocks, block_idx);
        return 0;
    }
    return -1;
}

int alloc_inode() {
    int i;
    for (i = 0; i < INODES_PER_GROUP; ++i) {
        if (!BMAP_GET(bmap_inodes, i)) {
            BMAP_SET(bmap_inodes, i);
            return i + 1;
        }
    }
    return -1;
}

int free_inode(int inode_num) {
    if (inode_num > 0 && inode_num <= INODES_PER_GROUP) {
        BMAP_CLEAR(bmap_inodes, inode_num - 1);
        return 0;
    }
    return -1;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 EXT2 OPERATIONS                                ||
// ! ||--------------------------------------------------------------------------------||

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

// ! ||--------------------------------------------------------------------------------||
// ! ||                                      EXT2                                      ||
// ! ||--------------------------------------------------------------------------------||

int ext2_init(void) {
    printk("Initializing EXT2 file system...\n");

    // Init bitmaps
    init_bitmaps();

    // Init root node
    fs_root = kmalloc(sizeof(Ext2Inode));
    
    return (0);
}