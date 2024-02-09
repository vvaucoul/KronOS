/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2_gd.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 17:33:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/19 18:03:06 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXT2_GD_H
#define EXT2_GD_H

#include <kernel.h>

/**
 * @file ext2_gd.h
 * @brief Header file for the ext2 group descriptor.
 *
 * This file defines the structure representing the ext2 group descriptor.
 * The ext2 group descriptor contains information about the block and inode bitmaps,
 * the inode table, and the number of free blocks and inodes in the group.
 */

typedef uint32_t block_t;
typedef uint16_t block_group_t;
typedef uint32_t ino_t;

typedef struct s_block_bitmap {
    block_t block_bitmap_blk : 24;
} __attribute__((packed)) Ext2BlockBitmap;

typedef struct s_inode_bitmap {
    block_t inode_bitmap_blk : 24;
} __attribute__((packed)) Ext2InodeBitmap;

typedef struct s_inode_table {
    block_t inode_tab_start_blk : 24;
    ino_t num_inodes : 16;
    ino_t free_inodes : 16;
} __attribute__((packed)) Ext2InodeTable;

/**
 * @brief Structure representing the ext2 group descriptor.
 *
 * The ext2 group descriptor contains information about the block and inode bitmaps,
 * the inode table, and the number of free blocks and inodes in the group.
 * It is located in the block immediately following the superblock.
 * The number of group descriptors is equal to the number of block groups in the file system.
 */
typedef struct s_group_descriptor {
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    uint32_t bg_reserved[3];
} __attribute__((packed)) Ext2GroupDescriptor;

extern Ext2GroupDescriptor *ext2_gd;

#endif /* !EXT2_GD_H */