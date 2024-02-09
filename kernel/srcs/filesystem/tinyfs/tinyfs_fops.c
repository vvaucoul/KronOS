/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_fops.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:49:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/09 14:43:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>

/**
 * @brief Read data from file
 *
 * @param node
 * @param offset
 * @param size
 * @param buffer
 *
 * @note This function will read data from file
 *
 * @return int
 */
int tinyfs_read(__unused__ void *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    return (tinyfs_device->read(tinyfs_device->device, offset, size, buffer));
}

/**
 * @brief Write data to file
 *
 * @param node
 * @param offset
 * @param size
 * @param buffer
 *
 * @note This function will write data to file
 *
 * @return int
 */
int tinyfs_write(__unused__ void *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    return (tinyfs_device->write(tinyfs_device->device, offset, size, buffer));
}

VfsNode *tinyfs_finddir(void *node, const char *name) {
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;
    // Check if the node is a directory
    if ((tfs_node->mode & VFS_DIRECTORY) == 0) {
        return NULL;
    }

    // Iterate over the entries in the directory
    for (uint32_t i = 0; i < tfs_node->nlink; i++) {
        TinyFS_Inode *child_node = tinyfs_get_inode(tiny_vfs, i); // tfs_node->block_pointers[i]

        // Check if the child node's name matches the requested name
        if (strcmp(child_node->name, name) == 0) {
            return (child_node);
        }
    }

    // If no matching entry was found, return NULL
    return NULL;
}