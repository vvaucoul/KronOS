/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_allocator.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 15:50:15 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 10:47:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/tinyfs/tinyfs.h>

/**
 * Allocates a block for the given inode in the TinyFS filesystem.
 *
 * @param inode The pointer to the TinyFS_Inode structure representing the inode.
 * @param block_index A pointer to store the index of the allocated block.
 * @param data A pointer to the data to be written to the allocated block.
 * @param size The size of the data to be written.
 * @param write_disk A boolean value indicating whether to write the allocated block to disk.
 * @return An integer value indicating the success or failure of the block allocation.
 */
int tinyfs_allocate_block(TinyFS_Inode *inode, uint32_t *block_index, uint8_t *data, uint32_t size, bool write_disk) {
    TinyFS *tfs = (TinyFS *)inode->fs;
    uint32_t remaining_size = size;
    uint8_t *data_ptr = data;

    while (remaining_size > 0) {
        bool block_found = false;
        for (uint32_t i = 0; i < TINYFS_MAX_BLOCKS; ++i) {
            if (tfs->data_blocks[i][0] == 0) { // Block is free
                block_found = true;
                uint32_t current_block_size = (remaining_size > TINYFS_BLOCK_SIZE) ? TINYFS_BLOCK_SIZE : remaining_size;
                *block_index = (TINY_FS_DATA_OFFSET + (i * TINYFS_BLOCK_SIZE));
                tfs->superblock->free_blocks--;

                memset(tfs->data_blocks[i], 0, TINYFS_BLOCK_SIZE);
                memcpy(tfs->data_blocks[i], data_ptr, current_block_size);

                if (write_disk) {
                    if (tfs->fs.device->swrite(tfs->fs.device->device, *block_index, current_block_size, data_ptr) != 0) {
                        printk("TinyFS: Block write failed\n");
                        return (-1);
                    }
                }

                tinyfs_write_superblock(tfs->fs.vfs);
                remaining_size -= current_block_size;
                data_ptr += current_block_size;

                if (remaining_size == 0) {
                    return 0; // All data written
                }

                break; // Move to next block if needed
            }
        }
        if (!block_found) {
            printk("TinyFS: No free blocks available\n");
            return -1; // No free blocks
        }
    }
    return 0; // Success
}

/**
 * Frees a block in the TinyFS file system.
 *
 * @param inode The pointer to the TinyFS inode.
 * @param block_index The index of the block to be freed.
 * @param write_disk A boolean indicating whether to write the changes to disk.
 * @return An integer indicating the status of the operation.
 */
int tinyfs_free_block(TinyFS_Inode *inode, uint32_t block_index, bool write_disk) {
    TinyFS *tfs = (TinyFS *)inode->fs;

    uint32_t block = (block_index - TINY_FS_DATA_OFFSET) / TINYFS_BLOCK_SIZE;
    if (block >= TINYFS_MAX_BLOCKS) {
        printk("TinyFS: Invalid block index\n");
        return -1;
    }

    tfs->superblock->free_blocks++;
    memset(tfs->data_blocks[block], 0, TINYFS_BLOCK_SIZE); // Clear the block

    if (write_disk) {
        if (tfs->fs.device->swrite(tfs->fs.device->device, block_index, TINYFS_BLOCK_SIZE, tfs->data_blocks[block]) != 0) {
            printk("TinyFS: Formatting (write) failed\n");
            return -1;
        }
    }

    // Update TFS Superblock
    tinyfs_write_superblock(tfs->fs.vfs);
    return 0;
}