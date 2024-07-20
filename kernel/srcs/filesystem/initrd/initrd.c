/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 13:07:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/30 16:18:32 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/initrd.h>

#include <drivers/device/devices.h>
#include <memory/memory.h>

InitrdHeader *initrd_header = NULL;
InitrdFileHeader *initrd_file_headers = NULL;
Vfs *initrd_fs = NULL;

/* Initrd Addr Infos */
static uint32_t initrd_start = 0x0;
static uint32_t initrd_end = 0x0;
static uint32_t __inode = 0;

/* Mount - Unmount */
static int __initrd_fs_mount(void *fs);
static int __initrd_fs_unmount(void *fs);

/* File Operations */
static int __initrd_fs_read(void *node, __unused__ uint32_t offset, uint32_t size, uint8_t *buffer);

/* Directory Operations */
static Dirent *__initrd_fs_readdir(void *node, uint32_t index);
static VfsNode *__initrd_fs_finddir(void *node, const char *name);

/* Node Operations */
static VfsNode *__initrd_create_node(VfsNode *root_node, const char *node_name);

/* Cache Operations (Used by VFS Cache) */
static int __initrd_set_cache_link(VfsNode *node, VfsCacheLinks *links);
static VfsCacheLinks *__initrd_get_cache_link(VfsNode *node);

// ! ||--------------------------------------------------------------------------------||
// ! ||                             INITRD FILESYSTEM INFOS                            ||
// ! ||--------------------------------------------------------------------------------||

/* VFS Cache infos */
VfsCacheFn initrd_cache_fn = {
    .vfs_set_cache_links = __initrd_set_cache_link,
    .vfs_get_cache_links = __initrd_get_cache_link,
};

/* VFS Filesystem infos */
VfsInfo initrd_fs_info = {
    .name = INITRD_FILESYSTEM_NAME,
    .d_name = INITRD_FILESYSTEM_NAME_D_NAME,
    .version = INITRD_VERSION,
    .type = INITRD_FILESYSTEM,
};

/* VFS Filesystem Operations */
VfsFsOps initrd_fs_ops = {
    .mount = __initrd_fs_mount,
    .unmount = __initrd_fs_unmount,
};

/* VFS Filesystem File Operations */
VfsFileOps initrd_fs_fops = {
    .read = __initrd_fs_read,
    .readdir = __initrd_fs_readdir,
    .finddir = __initrd_fs_finddir,
};

/* VFS Filesystem Node Operations */
VfsNodeOps initrd_fs_nops = {
    .create_node = __initrd_create_node,
};

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   INITRD INIT                                  ||
// ! ||--------------------------------------------------------------------------------||

int initrd_init(uint32_t start, uint32_t end) {
    initrd_start = start;
    initrd_end = end;
    initrd_header = (InitrdHeader *)((uintptr_t)start);
    initrd_file_headers = (InitrdFileHeader *)((uintptr_t)(start + sizeof(InitrdHeader)));

    initrd_fs = vfs_create_fs(NULL, &initrd_fs_info, &initrd_fs_ops, &initrd_fs_fops, &initrd_fs_nops);
    if (!initrd_fs) {
        __WARN("Initrd: Failed to create initrd filesystem", 1);
        return -1;
    }

    return vfs_mount(initrd_fs);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                             INITRD FILE OPERATIONS                             ||
// ! ||--------------------------------------------------------------------------------||

static Dirent *__initrd_fs_readdir(void *node, uint32_t index) {
    InitrdNode *initrd_node = (InitrdNode *)node;
    static Dirent dirent = {0};

    if (index >= initrd_node->links->childrens_count) {
        return NULL;
    }

    InitrdNode *child = (InitrdNode *)initrd_node->links->childrens[index]->node;
    memset(&dirent, 0, sizeof(Dirent));
    strncpy(dirent.d_name, child->name, sizeof(dirent.d_name) - 1);
    dirent.ino = child->inode;
    return &dirent;
}

static VfsNode *__initrd_fs_finddir(void *node, const char *name) {
    InitrdNode *initrd_node = (InitrdNode *)node;

    if ((initrd_node->flags & VFS_DIRECTORY) == 0) {
        return NULL;
    }

    for (uint32_t i = 0; i < initrd_node->links->childrens_count; i++) {
        InitrdNode *data_node = (InitrdNode *)initrd_node->links->childrens[i]->node;
        if (strcmp(data_node->name, name) == 0) {
            return data_node;
        }
    }
    return NULL;
}

static int __initrd_fs_read(void *node, __unused__ uint32_t offset, uint32_t size, uint8_t *buffer) {
    InitrdNode *initrd_node = (InitrdNode *)node;
    InitrdFileHeader *file_header = &initrd_file_headers[initrd_node->inode];

    if (size > file_header->size) {
        size = file_header->size;
    }

    void *content_addr = (void *)(file_header->offset);
    memcpy(buffer, content_addr, size);
    return size;
}

static VfsNode *__initrd_create_node(__unused__ VfsNode *root_node, const char *node_name) {
    InitrdNode *node = kmalloc(sizeof(InitrdNode));
    if (!node) {
        __WARN("Initrd: Failed to create initrd node", NULL);
        return NULL;
    }

    memset(node, 0, sizeof(InitrdNode));
    strncpy(node->name, node_name, sizeof(node->name) - 1);
    node->magic = INITD_MAGIC;
    BIT_SET(node->flags, VFS_FILE);
    node->inode = __inode;
    node->size = initrd_file_headers[__inode].size;
    return node;
}

static int __initrd_set_cache_link(VfsNode *node, VfsCacheLinks *links) {
    InitrdNode *initrd_node = (InitrdNode *)node;
    initrd_node->links = links;
    return 0;
}

static VfsCacheLinks *__initrd_get_cache_link(VfsNode *node) {
    InitrdNode *initrd_node = (InitrdNode *)node;
    return initrd_node->links;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                              INITRD FS OPERATIONS                              ||
// ! ||--------------------------------------------------------------------------------||

static int __initrd_fs_mount(void *fs) {
    Vfs *initrd_fs = (Vfs *)fs;
    if (!initrd_fs) {
        __WARN("Initrd: Failed to create initrd filesystem", 1);
        return -1;
    }

    vfs_create_cache(initrd_fs, initrd_cache_fn);

    initrd_fs->fs_root = initrd_fs->fs_current_node = vfs_create_node(initrd_fs, NULL, INITRD_FILESYSTEM_NAME);
    if (!initrd_fs->fs_root) {
        __WARN("Initrd: Failed to create initrd root node", 1);
        return -1;
    }

    BIT_CLEAR(((InitrdNode *)initrd_fs->fs_root)->flags, VFS_FILE);
    BIT_SET(((InitrdNode *)initrd_fs->fs_root)->flags, VFS_DIRECTORY);

    InitrdNode *dev_node = vfs_create_node(initrd_fs, initrd_fs->fs_root, INITRD_DEV_NAME);
    if (!dev_node) {
        __WARN("Initrd: Failed to create initrd dev node", 1);
        return -1;
    }

    BIT_CLEAR(((InitrdNode *)dev_node)->flags, VFS_FILE);
    BIT_SET(dev_node->flags, VFS_DIRECTORY);

    for (uint32_t i = 0; i < initrd_header->nfiles; i++) {
        initrd_file_headers[i].offset += initrd_start;
        __inode = i;

        if (initrd_file_headers[i].magic != INITD_MAGIC) {
            __WARN("Initrd: Invalid magic number", 1);
            continue;
        }

        InitrdNode *node = (InitrdNode *)vfs_create_node(initrd_fs, initrd_fs->fs_root, initrd_file_headers[i].name);
        if (!node) {
            __WARN("Initrd: Failed to create initrd node", 1);
            continue;
        }
        BIT_SET(node->flags, VFS_FILE);
    }
    return 0;
}

static __unused__ int __initrd_fs_unmount(__unused__ void *fs) {
    if (!fs) {
        __WARN("Initrd: Invalid filesystem", 1);
        return -1;
    }

    initrd_fs = NULL;
    initrd_header = NULL;
    initrd_file_headers = NULL;
    initrd_start = 0x0;
    initrd_end = 0x0;
    __inode = 0;
    return 0;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  INITRD UTILS                                  ||
// ! ||--------------------------------------------------------------------------------||

/**
 * Displays the hierarchy of the initrd filesystem.
 *
 * This function is responsible for displaying the hierarchy of the initrd filesystem.
 * It provides a visual representation of the directory structure and file contents
 * within the initrd.
 */
void initrd_display_hierarchy(void) {
    if (initrd_fs == NULL) {
        __WARND("Initrd: initrd not initialized");
        return;
    }

    printk("Initrd files:\n");
    printk("-------------------------------------------------------------------------------\n");
    printk("| %-20s | %-10s | %-10s | %-20s |\n", "Name", "Type", "Size", "Content");
    printk("-------------------------------------------------------------------------------\n");

    Dirent *_d_node;
    InitrdNode *node = initrd_fs->fs_root;
    uint32_t index = 0;

    if (node == NULL) {
        printk("Error: initrd root node is NULL\n");
        return;
    }

    vfs_opendir(initrd_fs, node);
    while ((_d_node = vfs_readdir(initrd_fs, node, index)) != NULL) {
        InitrdNode *_f_node = vfs_finddir(initrd_fs, node, _d_node->d_name);
        if (_f_node == NULL) {
            printk("Error: vfs_finddir failed\n");
            ++index;
            continue;
        }

        const char *type_str = "Unknown";
        if ((_f_node->flags & VFS_DIRECTORY) != 0) {
            type_str = "Directory";
        } else if ((_f_node->flags & VFS_FILE) != 0) {
            type_str = "File";
        } else if ((_f_node->flags & VFS_CHARDEVICE) != 0) {
            type_str = "Char Device";
        } else if ((_f_node->flags & VFS_BLOCKDEVICE) != 0) {
            type_str = "Block Device";
        } else if ((_f_node->flags & VFS_PIPE) != 0) {
            type_str = "Pipe";
        } else if ((_f_node->flags & VFS_SYMLINK) != 0) {
            type_str = "Symlink";
        } else if ((_f_node->flags & VFS_MOUNTPOINT) != 0) {
            type_str = "Mount Point";
        }

        uint32_t size = _f_node->size;
        char content[21] = "";

        if ((_f_node->flags & VFS_FILE) != 0 && size > 0) {
            uint8_t *buffer = kmalloc(sizeof(char) * (size + 1));
            if (buffer) {
                memset(buffer, 0, size + 1);
                initrd_fs->fops->read(_f_node, 0, size, buffer);
                strncpy(content, buffer, 20);
                kfree(buffer);
            }
        }

        printk("| %-20s | %-10s | %-10u | %-20s |\n", _d_node->d_name, type_str, size, content);
        index++;
        kmsleep(100);
    }
    vfs_closedir(initrd_fs, node);

    printk("-------------------------------------------------------------------------------\n");
    printk("Done!\n");
}