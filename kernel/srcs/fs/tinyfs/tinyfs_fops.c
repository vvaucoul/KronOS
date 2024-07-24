/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_fops.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:49:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 00:57:17 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/tinyfs/tinyfs.h>
#include <memory/memory.h>
#include <multitasking/process.h>
#include <system/fs/stat.h>

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

    int ret = tinyfs->fs.device->swrite(tinyfs->fs.device->device, offset, size, buffer);

    if (ret == 0) {
        TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;
        if (offset + size > tfs_node->size) {
            tfs_node->size = offset + size;
            // Todo: Update inode and write it to disk
            // Avoid following line for now, it will cause a loop
            // (tinyfs_write_inode -> tinyfs_write -> tinyfs_write_inode -> ...)
            // tinyfs_write_inode(tinyfs->fs.vfs, tfs_node->inode_number, tfs_node);
        }
    } else {
        return (ret);
    }

    return (0);
}

/**
 * @brief Finds a directory node in the TinyFS filesystem.
 *
 * This function searches for a directory node with the specified name in the given node.
 *
 * @param node A pointer to the node in which to search for the directory.
 * @param name The name of the directory to find.
 * @return A pointer to the found directory node, or NULL if not found.
 */
VfsNode *tinyfs_finddir(void *node, const char *name) {
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;

    // Check if the node is a directory
    if ((tfs_node->mode & VFS_DIRECTORY) == 0) {
        return (NULL);
    }

    // Iterate over the entries in the directory
    VfsNode **links = tinyfs_get_links(tinyfs->fs.vfs, tfs_node);

    for (uint32_t i = 0; i < tfs_node->nlink; i++) {
        VfsNode *child_node = links[i];
        TinyFS_Inode *tmp_tfs_node = (TinyFS_Inode *)child_node;

        // Check if the child node's name matches the requested name
        if (strcmp(tmp_tfs_node->name, name) == 0) {
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

    tinyfs_display_inode(new_node);

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

void tinyfs_display_inode(TinyFS_Inode *inode) {
    TinyFS *tinyfs = (TinyFS *)inode->fs;

    if (inode == NULL || tinyfs == NULL) {
        return;
    }

    // Display Inode infos
    printk("Inode %d: %s\n", inode->inode_number, inode->name);
    printk("  Type: %s\n", (inode->mode & VFS_DIRECTORY) ? "Directory" : "File");
    printk("  Size: %d bytes\n", inode->size);
    printk("  Links: %d\n", inode->nlink);
    printk("  Block Pointers: ");
    for (uint32_t i = 0; i < TINYFS_MAX_BLOCKS_PER_FILE; i++) {
        if (inode->block_pointers[i] != 0) {
            printk("%d ", inode->block_pointers[i]);
        }
    }
    printk("\n");

    // Display parent node
    TinyFS_Inode *parent = (TinyFS_Inode *)tinyfs_get_parent(inode);
    printk("  Parent: %d %s\n", inode->parent_inode_number, vfs_get_fs(TINYFS_FILESYSTEM_NAME)->nops->get_name(parent));

    // Display links node
    printk("  Links: ");
    for (uint32_t i = 0; i < inode->nlink; i++) {
        TinyFS_Inode *link_node = tinyfs_get_inode(tinyfs->fs.vfs, inode->links[i]);
        if (link_node != NULL) {
            printk("%s ", link_node->name);
        }
    }
    printk("\n");
}

int tinyfs_open(void *node, uint32_t flags) {
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;

    if (tinyfs == NULL || tfs_node == NULL) {
        return -1;
    }

    // Increment the node's open count
    tfs_node->open_count++;
    return 0;
}

int tinyfs_close(void *node) {
    TinyFS *tinyfs = (TinyFS *)((TinyFS_Inode *)node)->fs;
    TinyFS_Inode *tfs_node = (TinyFS_Inode *)node;

    if (tinyfs == NULL || tfs_node == NULL) {
        return -1;
    }

    // Decrement the node's open count
    tfs_node->open_count--;
    return 0;
}