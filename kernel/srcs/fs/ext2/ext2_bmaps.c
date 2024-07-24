/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2_bmaps.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 11:01:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 10:47:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/ext2/ext2.h>

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

/**
 * @brief Initializes the bitmaps for the ext2 filesystem.
 */
void init_bitmaps() {
    memset(bmap_blocks, 0, sizeof(bmap_blocks));
    memset(bmap_inodes, 0, sizeof(bmap_inodes));
}

/**
 * @brief Allocates a block in the ext2 filesystem.
 * @return The allocated block number.
 */
int alloc_block() {
    for (uint32_t i = 0; i < BLOCKS_PER_GROUP; ++i) {
        if (!BMAP_GET(bmap_blocks, i)) {
            BMAP_SET(bmap_blocks, i);
            return i + FIRST_DATA_BLOCK;
        }
    }
    return -1;
}

/**
 * @brief Frees a block in the ext2 filesystem.
 * @param block_num The block number to free.
 * @return 0 if successful, -1 otherwise.
 */
int free_block(int block_num) {
    int block_idx = block_num - FIRST_DATA_BLOCK;
    if (block_idx >= 0 && block_idx < BLOCKS_PER_GROUP) {
        BMAP_CLEAR(bmap_blocks, block_idx);
        return 0;
    }
    return -1;
}

/**
 * @brief Allocates an inode in the ext2 filesystem.
 * @return The allocated inode number.
 */
int alloc_inode() {
    for (uint32_t i = 0; i < INODES_PER_GROUP; ++i) {
        if (!BMAP_GET(bmap_inodes, i)) {
            BMAP_SET(bmap_inodes, i);
            return i + 1;
        }
    }
    return -1;
}

/**
 * @brief Frees an inode in the ext2 filesystem.
 * @param inode_num The inode number to free.
 * @return 0 if successful, -1 otherwise.
 */
int free_inode(int inode_num) {
    if (inode_num > 0 && inode_num <= INODES_PER_GROUP) {
        BMAP_CLEAR(bmap_inodes, inode_num - 1);
        return 0;
    }
    return -1;
}