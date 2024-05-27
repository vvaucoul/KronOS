/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_fops.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:49:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/13 17:47:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <memory/memory.h>
#include <multitasking/process.h>
#include <syscall/stat.h>

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
int tinyfs_read(void *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;

    return (tinyfs->fs.device->sread(tinyfs->fs.device->device, offset, size, buffer));
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
int tinyfs_write(void *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;

    return (tinyfs->fs.device->swrite(tinyfs->fs.device->device, offset, size, buffer));
}

VfsNode *tinyfs_finddir(void *node, const char *name) {
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;
    
    // Check if the node is a directory
    if ((tfs_node->mode & VFS_DIRECTORY) == 0) {
        return (NULL);
    }

    // Iterate over the entries in the directory
    for (uint32_t i = 0; i <= tfs_node->nlink; i++) {
        TinyFS_Inode *child_node = tinyfs_get_inode(tinyfs->fs.vfs, i);

        // Check if the child node's name matches the requested name
        if (strcmp(child_node->name, name) == 0) {
            return (child_node);
        }
    }

    // If no matching entry was found, return NULL
    return (NULL);
}

int tinyfs_mkdir(void *node, const char *name, uint16_t permission) {
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;

    if (tinyfs == NULL || tfs_node == NULL) {
        return -1;
    }

    // Check if the node is a directory
    if ((tfs_node->mode & VFS_DIRECTORY) == 0) {
        return -1;
    }

    // Check if the directory already exists
    if (tinyfs_finddir(node, name) != NULL) {
        return -1;
    }

    // Create a new inode for the directory (parent automatically set to tfs_node)
    TinyFS_Inode *new_node = tinyfs_create_inode(tinyfs, tfs_node, name, VFS_DIRECTORY);
    if (new_node == NULL) {
        return -1;
    }

    // Write the new directory to disk
    tinyfs_write_inode(tinyfs->fs.vfs, new_node->inode_number, new_node);
    // Update the parent directory's inode on disk
    tinyfs_write_inode(tinyfs->fs.vfs, tfs_node->inode_number, tfs_node);

    return 0;
}

Dirent *tinyfs_readdir(void *node, uint32_t index) {
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;
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
    TinyFS_Inode *child_node = tinyfs_get_inode(tinyfs->fs.vfs, tfs_node->links[index]);
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
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;
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
    TinyFS_Inode *new_node = tinyfs_create_inode(tinyfs, tfs_node, name, VFS_FILE);
    if (new_node == NULL) {
        return -1;
    }

    // Write the new file to disk
    tinyfs_write_inode(tinyfs->fs.vfs, new_node->inode_number, new_node);
    // Update the parent directory's inode on disk
    tinyfs_write_inode(tinyfs->fs.vfs, tfs_node->inode_number, tfs_node);

    return 0;
}

int tinyfs_stat(void *node, struct stat *buf) {
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;

    memset(buf, 0, sizeof(struct stat));

    buf->st_dev = tinyfs->fs.device->uid;
    buf->st_ino = tfs_node->inode_number;

    // Todo: Add support for other file types
    if (tfs_node->mode == VFS_DIRECTORY) {
        buf->st_mode = S_IFDIR | (0777 & 0777); // Todo: Add support for permissions
    } else if (tfs_node->mode == VFS_FILE) {
        buf->st_mode = S_IFREG | (0777 & 0777); // Todo: Add support for permissions
    }

    buf->st_nlink = tfs_node->nlink;
    buf->st_size = tfs_node->size;

    task_t *task = get_current_task();
    if (task) {
        buf->st_uid = task->owner;
        // buf->st_gid = task->group; // Add support for groups
    }

    buf->st_blocks = (tfs_node->size + TINYFS_BLOCK_SIZE - 1) / TINYFS_BLOCK_SIZE;
    // Todo: support for timestamps and other fields

    return (0);
}

void tinyfs_display_hierarchy(TinyFS_Inode *node, uint32_t depth) {
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;

    if (node == NULL || tinyfs == NULL) {
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
            TinyFS_Inode *child_node = tinyfs_get_inode(tinyfs->fs.vfs, node->links[i]);

            tinyfs_display_hierarchy(child_node, depth + 1);
        }
    }
}