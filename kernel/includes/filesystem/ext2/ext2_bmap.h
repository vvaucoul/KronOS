/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2_bmap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 11:34:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/18 11:44:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXT2_BMAP_H
#define EXT2_BMAP_H

/*
 * This header file contains function declarations for managing bitmaps in the ext2 filesystem.
 *
 * In the ext2 filesystem, bitmaps are used to manage the allocation and deallocation of blocks and inodes.
 * Each bit in a bitmap corresponds to a block or an inode in the filesystem. If the bit is set to 1,
 * the block or inode is in use. If the bit is set to 0, the block or inode is free.
 *
 * The functions declared in this file are used to manipulate these bitmaps, allowing for the efficient
 * allocation and deallocation of blocks and inodes.
 */


/* Bitmap management macros */
#define BMAP_GET(bitmap, idx) ((bitmap)[(idx) / 8] & (1 << ((idx) % 8)))
#define BMAP_SET(bitmap, idx) ((bitmap)[(idx) / 8] |= (1 << ((idx) % 8)))
#define BMAP_CLEAR(bitmap, idx) ((bitmap)[(idx) / 8] &= ~(1 << ((idx) % 8)))

#define BLOCKS_PER_GROUP 1024
#define INODES_PER_GROUP (8 * BLOCKS_PER_GROUP)
#define FIRST_DATA_BLOCK 1

extern void init_bitmaps();
extern int alloc_block();
extern int free_block(int block_num);
extern int alloc_inode();
extern int free_inode(int inode_num);

#endif /* !EXT2_BMAP_H */