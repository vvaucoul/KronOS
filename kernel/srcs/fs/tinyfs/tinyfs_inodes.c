/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_inodes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:20:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 23:23:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/tinyfs/tinyfs.h>
#include <memory/memory.h>

static char *__tiny_fs_update_inode_name(const char name[TINYFS_FILENAME_MAX + 1]);

uint32_t tinyfs_find_first_free_inode(TinyFS *fs) {
    for (uint32_t i = 0; i < TINYFS_MAX_FILES; i++) {
        if (fs->inodes[i]->used == 0) {
            return (i);
        }
    }
    return (0);
}

TinyFS_Inode *tinyfs_create_inode(TinyFS *tfs, TinyFS_Inode *parent_inode, const char name[TINYFS_FILENAME_MAX + 1], uint8_t mode) {
    if (strlen(name) > TINYFS_FILENAME_MAX) {
        __THROW("TinyFS: File name too long", NULL);
    } else if (((TinyFS *)(tfs->fs.vfs->fs))->superblock->free_inodes == 0) {
        __THROW("TinyFS: No free inodes", NULL);
    } else if (mode != VFS_FILE && mode != VFS_DIRECTORY) {
        __THROW("TinyFS: Invalid mode", NULL);
    } else if (parent_inode && parent_inode->mode != VFS_DIRECTORY) {
        __THROW("TinyFS: Parent inode is not a directory", NULL);
    } else if (parent_inode && parent_inode->nlink == TINYFS_MAX_FILES) {
        __THROW("TinyFS: Parent inode is full", NULL);
    } else if (parent_inode && tinyfs_find_first_free_inode((TinyFS *)tfs->fs.vfs->fs) == 0) {
        __THROW("TinyFS: No free inodes", NULL);
    }

    TinyFS_Inode *tinyfs_inode = (TinyFS_Inode *)kmalloc(sizeof(TinyFS_Inode));

    if (tinyfs_inode == NULL) {
        __THROW("TinyFS: Failed to allocate memory for inode", NULL);
    }

    // Fill inode with default values
    memset(tinyfs_inode, 0, sizeof(TinyFS_Inode));

    // Set inode values
    memcpy(tinyfs_inode->name, __tiny_fs_update_inode_name(name), TINYFS_FILENAME_MAX + 1);
    tinyfs_inode->used = 1;
    tinyfs_inode->mode = mode;
    tinyfs_inode->size = mode == VFS_FILE ? 0 : mode == VFS_DIRECTORY ? TINYFS_BLOCK_SIZE
                                                                      : 0;
    tinyfs_inode->inode_number = tinyfs_find_first_free_inode((TinyFS *)tfs->fs.vfs->fs);
    tinyfs_inode->parent_inode_number = parent_inode ? parent_inode->inode_number : 0;
    memset(tinyfs_inode->links, 0, TINYFS_MAX_FILES);
    tinyfs_inode->nlink = 0;
    tinyfs_inode->fs = tfs;
    memset(tinyfs_inode->block_pointers, 0, TINYFS_MAX_BLOCKS_PER_FILE);

    // Update parent inode
    if (parent_inode) {
        parent_inode->links[parent_inode->nlink] = tinyfs_inode->inode_number;
        parent_inode->nlink += 1;
    }

    ((TinyFS *)(tfs->fs.vfs->fs))->inodes[tinyfs_inode->inode_number] = tinyfs_inode;

    return (tinyfs_inode);
}

TinyFS_Inode *tinyfs_create_file_inode(TinyFS_Inode *root_node, const char *node_name) {
    if (root_node == NULL) {
        __THROW("TinyFS: Root node is NULL", NULL);
    }

    if (root_node->mode != VFS_DIRECTORY) {
        __THROW("TinyFS: Root node is not a directory", NULL);
    }

    if (root_node->nlink == TINYFS_MAX_FILES) {
        __THROW("TinyFS: Root node is full", NULL);
    }

    TinyFS *tfs = (TinyFS *)root_node->fs;
    TinyFS_Inode *new_inode = tinyfs_create_inode(tfs, root_node, node_name, VFS_FILE);

    if (new_inode == NULL) {
        __THROW("TinyFS: Failed to create inode", NULL);
    }

    return (new_inode);
}

int tinyfs_delete_inode(TinyFS_Inode *tinyfs_inode) {
    tinyfs_inode->used = 0;
    kfree(tinyfs_inode);
    return (0);
}

TinyFS_Inode tinyfs_read_inode(Vfs *fs, uint32_t inode) {
    TinyFS_Inode tinyfs_inode = {0};
    uint32_t inode_size_sectors = sizeof(TinyFS_Inode) / SECTOR_SIZE;
    if (sizeof(TinyFS_Inode) % SECTOR_SIZE != 0) {
        inode_size_sectors += 1;
    }

    uint8_t read_buffer[SECTOR_SIZE * inode_size_sectors];
    if (fs->fops->read(fs, inode, inode_size_sectors, read_buffer) != 0) {
        __THROW("TinyFS: Failed to read inode", tinyfs_inode);
    }

    memcpy_s(&tinyfs_inode, sizeof(TinyFS_Inode), read_buffer, sizeof(TinyFS_Inode));
    return (tinyfs_inode);
}

int tinyfs_write_inode(Vfs *fs, uint32_t inode, TinyFS_Inode *tinyfs_inode) {
    uint8_t write_buffer[sizeof(TinyFS_Inode)];
    uint32_t inode_offset = TINY_FS_INODES_OFFSET + (inode * sizeof(TinyFS_Inode));

    memcpy_s(write_buffer, sizeof(TinyFS_Inode), tinyfs_inode, sizeof(TinyFS_Inode));

    if (fs->fops->write(tinyfs_inode, inode_offset, sizeof(TinyFS_Inode), write_buffer) != 0) {
        __THROW("TinyFS: Failed to write inode", -1);
    }

    return (0);
}

TinyFS_Inode *tinyfs_get_inode(Vfs *fs, uint32_t inode) {
    return (((TinyFS *)fs->fs)->inodes[inode]);
}

char *tinyfs_get_name(VfsNode *node) {
    return (((TinyFS_Inode *)node)->name);
}

VfsNode *tinyfs_get_parent(VfsNode *node) {
    TinyFS_Inode *inode = (TinyFS_Inode *)node;
    TinyFS *tfs = (TinyFS *)inode->fs;

    if (inode->inode_number == ((TinyFS_Inode *)tfs->fs.vfs->fs_root)->inode_number) {
        return (NULL);
    }

    return ((VfsNode *)(tfs->inodes[inode->parent_inode_number]));
}

VfsNode **tinyfs_get_links(Vfs *vfs, VfsNode *node) {
    TinyFS_Inode *inode = (TinyFS_Inode *)node;

    if (inode == NULL) {
        return (NULL);
    } else {
        VfsNode **links = kmalloc(sizeof(TinyFS_Inode) * (inode->nlink + 1));

        if (links == NULL) {
            return (NULL);
        } else {
            uint8_t i = 0;

            for (; i < inode->nlink && i < TINYFS_MAX_FILES; i++) {
                links[i] = tinyfs_get_inode(vfs, inode->links[i]);
            }
            links[i] = NULL;
        }
        return (links);
    }
    return (NULL);
}

int tinyfs_get_inode_idx(VfsNode *node) {
    return (((TinyFS_Inode *)node)->inode_number);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  INODES UTILS                                  ||
// ! ||--------------------------------------------------------------------------------||

static char *__tiny_fs_update_inode_name(const char name[TINYFS_FILENAME_MAX + 1]) {
    // Remove first / if present, remove multiple / if present and remove last / if present
    char *new_name = (char *)kmalloc(strlen(name) + 1);
    if (new_name == NULL) {
        __THROW("TinyFS: Failed to allocate memory for inode name", NULL);
    }
    memset(new_name, 0, strlen(name) + 1);
    uint32_t i = 0;
    uint32_t j = 0;
    while (name[i] != '\0') {
        if (name[i] == '/') {
            if (i == 0) {
                i++;
                continue;
            }
            if (name[i + 1] == '/') {
                i++;
                continue;
            }
            if (name[i + 1] == '\0') {
                i++;
                continue;
            }
        }
        new_name[j] = name[i];
        i++;
        j++;
    }
    return (new_name);
}