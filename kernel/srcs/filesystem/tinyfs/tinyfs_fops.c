/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_fops.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:49:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/11 12:30:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <memory/memory.h>
#include <multitasking/process.h>

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
    return (tinyfs_device->sread(tinyfs_device->device, offset, size, buffer));
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
    return (tinyfs_device->swrite(tinyfs_device->device, offset, size, buffer));
}

VfsNode *tinyfs_finddir(void *node, const char *name) {
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;
    // Check if the node is a directory
    if ((tfs_node->mode & VFS_DIRECTORY) == 0) {
        return (NULL);
    }

    // Iterate over the entries in the directory
    for (uint32_t i = 0; i <= tfs_node->nlink; i++) {
        TinyFS_Inode *child_node = tinyfs_get_inode(tiny_vfs, i); // tfs_node->block_pointers[i]

        // Check if the child node's name matches the requested name
        if (strcmp(child_node->name, name) == 0) {
            return (child_node);
        }
    }

    // If no matching entry was found, return NULL
    return (NULL);
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
    // Update the parent directory's inode on disk
    tinyfs_write_inode(tiny_vfs, tfs_node->inode_number, tfs_node);

    return 0;
}

Dirent *tinyfs_readdir(void *node, uint32_t index) {
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;
    // Check if the node is a directory
    if ((tfs_node->mode & VFS_DIRECTORY) == 0) {
        return (NULL);
    }

    // Check if the index is valid
    if (index >= tfs_node->nlink) {
        return (NULL);
    }

    // Create a new dirent for the entry
    static Dirent dirent;

    memset(&dirent, 0, sizeof(Dirent));

    // Get the child node
    TinyFS_Inode *child_node = tinyfs_get_inode(tiny_vfs, tfs_node->links[index]);
    if (child_node == NULL) {
        return (NULL);
    }

    // Fill the dirent with the child node's information
    memcpy(dirent.d_name, child_node->name, TINYFS_FILENAME_MAX + 1);
    dirent.ino = child_node->inode_number;
    dirent.parent_ino = tfs_node->inode_number;

    return (&dirent);
}

int tinyfs_create(void *node, const char *name, uint16_t permission) {
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;
    // Check if the node is a directory
    if ((tfs_node->mode & VFS_DIRECTORY) == 0) {
        return -1;
    }

    // Check if the file already exists
    if (tinyfs_finddir(node, name) != NULL) {
        return -1;
    }

    // Create a new inode for the file (parent automatically set to tfs_node)
    TinyFS_Inode *new_node = tinyfs_create_inode(tfs_node, name, VFS_FILE);
    if (new_node == NULL) {
        return -1;
    }

    // Write the new file to disk
    tinyfs_write_inode(tiny_vfs, new_node->inode_number, new_node);
    // Update the parent directory's inode on disk
    tinyfs_write_inode(tiny_vfs, tfs_node->inode_number, tfs_node);

    return 0;
}

int tinyfs_stat(void *node, struct stat *buf) {
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;

    memset(buf, 0, sizeof(struct stat));

    // Fill the stat buffer with the child node's information
    buf->st_dev = tinyfs_device->uid;
    buf->st_ino = tfs_node->inode_number;
    buf->st_mode = tfs_node->mode;
    buf->st_nlink = tfs_node->nlink;
    buf->st_size = tfs_node->size;
    
    task_t *task = get_current_task();

    if (task) {
        buf->st_uid = task->owner;
        // buf->st_gid = task->group; //Todo: Add group to task
    }

    // Todo: Add more information to the stat buffer (e.g. st_atime, st_mtime, st_ctime)

    return (0);
}

void tinyfs_display_hierarchy(TinyFS_Inode *node, uint32_t depth) {
    if (node == NULL) {
        return;
    }

    if (depth == 0) {
        printk("%c", 201);
    } else {
        printk("%c", 204);
    }
    for (uint32_t i = 0; i < depth; i++) {
        printk("%c", 205);
    }
    if (node->mode == VFS_DIRECTORY) {
        printk(_CYAN);
    }

    printk(" %s" _END, node->name);
    printk(" [%d] - %d Bytes ", node->inode_number, node->size);
    printk("\n");

    if (node->mode == VFS_DIRECTORY) {
        for (uint32_t i = 0; i < node->nlink; i++) {
            TinyFS_Inode *child_node = tinyfs_get_inode(tiny_vfs, node->links[i]);

            tinyfs_display_hierarchy(child_node, depth + 1);
        }
    }
}