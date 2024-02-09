/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_fops.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:49:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 00:13:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <memory/memory.h>

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

int tinyfs_mkdir(void *node, const char *name, uint16_t permission) {
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;
    // Check if the node is a directory
    if ((tfs_node->mode & VFS_DIRECTORY) == 0) {
        return -1;
    }

    // Check if the directory already exists
    if (tinyfs_finddir(node, name) != NULL) {
        return -1;
    }

    // Create a new inode for the directory (parent automatically set to tfs_node)
    TinyFS_Inode *new_node = tinyfs_create_inode(tfs_node, name, VFS_DIRECTORY);
    if (new_node == NULL) {
        return -1;
    }

    // Write the new directory to disk
    tinyfs_write_inode(tiny_vfs, new_node->inode_number, new_node);
    tinyfs_write_inode(tiny_vfs, tfs_node->inode_number, tfs_node);

    return 0;
}

Dirent *tinyfs_readdir(void *node, uint32_t index) {
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;
    // Check if the node is a directory
    if ((tfs_node->mode & VFS_DIRECTORY) == 0) {
        return NULL;
    }

    // Check if the index is valid
    if (index >= tfs_node->nlink) {
        return NULL;
    }

    // Create a new dirent for the entry
    Dirent *dirent = (Dirent *)kmalloc(sizeof(Dirent));
    if (dirent == NULL) {
        return NULL;
    }

    // Get the child node
    TinyFS_Inode *child_node = tinyfs_get_inode(tiny_vfs, tfs_node->links[index]);
    if (child_node == NULL) {
        kfree(dirent);
        return NULL;
    }

    // Fill the dirent with the child node's information
    memcpy(dirent->name, child_node->name, TINYFS_FILENAME_MAX + 1);
    dirent->ino = child_node->inode_number;

    return dirent;
}

void tinyfs_display_hierarchy(TinyFS_Inode *node, uint32_t depth) {
    if (node == NULL || node->used == 0 || node->name[0] == 0) {
        return;
    }

    for (uint32_t i = 0; i < depth; i++) {
        printk("  ");
    }
    printk("%s\n", node->name);

    if ((node->mode & VFS_DIRECTORY) != 0) {
        for (uint32_t i = 0; i < node->nlink; i++) {
            TinyFS_Inode *child_node = tinyfs_get_inode(tiny_vfs, node->links[i]);

            kmsleep(150);
            tinyfs_display_hierarchy(child_node, depth + 1);
        }
    }
}