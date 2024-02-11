/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 13:07:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 13:21:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/initrd.h>

#include <drivers/device/devices.h>
#include <memory/memory.h>

InitrdHeader *initrd_header = NULL;
InitrdFileHeader *initrd_file_headers = NULL;

Vfs *initrd_fs = NULL;

uint32_t initrd_start = 0x0;
uint32_t initrd_end = 0x0;

static uint32_t __inode = 0;

static int __initrd_fs_mount(void *fs);
static int __initrd_fs_unmount(void *fs);

static int __initrd_fs_read(void *node, __unused__ uint32_t offset, uint32_t size, uint8_t *buffer);

static Dirent *__initrd_fs_readdir(void *node, uint32_t index);
static VfsNode *__initrd_fs_finddir(void *node, const char *name);

static VfsNode *__initrd_create_node(VfsNode *root_node, const char *node_name);

static int __initrd_set_cache_link(VfsNode *node, VfsCacheLinks *links);
static VfsCacheLinks *__initrd_get_cache_link(VfsNode *node);

// ! ||--------------------------------------------------------------------------------||
// ! ||                             INITRD FILESYSTEM INFOS                            ||
// ! ||--------------------------------------------------------------------------------||

/**
 * VFS Cache infos
 */

VfsCacheFn initrd_cache_fn = {
    .vfs_set_cache_links = __initrd_set_cache_link,
    .vfs_get_cache_links = __initrd_get_cache_link,
};

/**
 * VFS Filesystem infos
 */
VfsInfo initrd_fs_info = {
    .name = INITRD_FILESYSTEM_NAME,
    .d_name = "initrd filesystem",
    .version = "0.1",
    .type = INITRD_FILESYSTEM,
};

VfsFsOps initrd_fs_ops = {
    .mount = __initrd_fs_mount,
    .unmount = __initrd_fs_unmount,
};

VfsFileOps initrd_fs_fops = {
    .read = __initrd_fs_read,
    .readdir = __initrd_fs_readdir,
    .finddir = __initrd_fs_finddir,
};

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

    // Create initrd filesystem
    initrd_fs = vfs_create_fs(NULL, &initrd_fs_info, &initrd_fs_ops, &initrd_fs_fops, &initrd_fs_nops);

    // Mount initrd filesystem
    return (vfs_mount(initrd_fs));
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                             INITRD FILE OPERATIONS                             ||
// ! ||--------------------------------------------------------------------------------||

static Dirent *__initrd_fs_readdir(void *node, uint32_t index) {
    InitrdNode *initrd_node = (InitrdNode *)node;
    static Dirent dirent = {0};

    memset(&dirent, 0, sizeof(Dirent));
    for (uint32_t i = 0; i < initrd_node->links->childrens_count; i++) {
        if (i == index) {
            InitrdNode *__child = (InitrdNode *)initrd_node->links->childrens[i]->node;

            memcpy(dirent.d_name, __child->name, strlen(__child->name));
            dirent.ino = __child->inode;
            return (&dirent);
        }
    }
    return (NULL);
}

static VfsNode *__initrd_fs_finddir(void *node, const char *name) {
    InitrdNode *initrd_node = (InitrdNode *)node;

    // Check if root node is a directory
    if ((initrd_node->flags & VFS_DIRECTORY) == 0) {
        return (NULL);
    }

    for (uint32_t i = 0; i < initrd_node->links->childrens_count; i++) {
        InitrdNode *data_node = (InitrdNode *)initrd_node->links->childrens[i]->node;

        if (strcmp(data_node->name, name) == 0) {
            return (data_node);
        }
    }
    return (NULL);
}

static int __initrd_fs_read(void *node, __unused__ uint32_t offset, uint32_t size, uint8_t *buffer) {
    InitrdNode *initrd_node = (InitrdNode *)node;
    InitrdFileHeader *file_header = &initrd_file_headers[initrd_node->inode];

    if (size > file_header->size) {
        size = file_header->size;
    }

    void *content_addr = (void *)(file_header->offset);
    memcpy(buffer, content_addr, size);
    return (0);
}

static VfsNode *__initrd_create_node(__unused__ VfsNode *root_node, const char *node_name) {
    InitrdNode *node = kmalloc(sizeof(InitrdNode));

    if (node == NULL) {
        __WARN("Initrd: Failed to create initrd node", NULL);
    } else {
        memset(node, 0, sizeof(InitrdNode));
        memcpy(node->name, node_name, strlen(node_name));

        node->magic = INITD_MAGIC;
        BIT_SET(node->flags, VFS_FILE);
        node->inode = __inode;
        node->size = initrd_file_headers[__inode].size;
    }
    return (node);
}

static int __initrd_set_cache_link(VfsNode *node, VfsCacheLinks *links) {
    InitrdNode *initrd_node = (InitrdNode *)node;

    initrd_node->links = links;
    return (0);
}

static VfsCacheLinks *__initrd_get_cache_link(VfsNode *node) {
    InitrdNode *initrd_node = (InitrdNode *)node;

    return (initrd_node->links);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                              INITRD FS OPERATIONS                              ||
// ! ||--------------------------------------------------------------------------------||

static int __initrd_fs_mount(void *fs) {
    Vfs *initrd_fs = (Vfs *)fs;

    if (initrd_fs == NULL) {
        __WARN("Initrd: Failed to create initrd filesystem", 1);
    }

    // Create VFS Cache nodes (Algorithm to cache nodes)
    vfs_create_cache(initrd_fs, initrd_cache_fn);

    // Create initrd root node (initrd root directory)
    initrd_fs->fs_root = vfs_create_node(initrd_fs, NULL, INITRD_FILESYSTEM_NAME);
    BIT_CLEAR(((InitrdNode *)initrd_fs->fs_root)->flags, VFS_FILE);
    BIT_SET(((InitrdNode *)initrd_fs->fs_root)->flags, VFS_DIRECTORY);

    // Create initrd dev node (initrd device directory)
    InitrdNode *dev_node = vfs_create_node(initrd_fs, initrd_fs->fs_root, INITRD_DEV_NAME);
    BIT_CLEAR(((InitrdNode *)dev_node)->flags, VFS_FILE);
    BIT_SET(dev_node->flags, VFS_DIRECTORY);

    // Register initrd files
    uint32_t n_files = initrd_header->nfiles;

    for (uint32_t i = 0; i < n_files; i++) {
        initrd_file_headers[i].offset += initrd_start;
        __inode = i;

        uint8_t magic = initrd_file_headers[i].magic;

        if (magic != INITD_MAGIC) {
            __WARN("Initrd: Invalid magic number", 1);
        }

        InitrdNode *node = (InitrdNode *)vfs_create_node(initrd_fs, initrd_fs->fs_root, initrd_file_headers[i].name);
        BIT_SET(node->flags, VFS_FILE);
    }
    return (0);
}

static __unused__ int __initrd_fs_unmount(__unused__ void *fs) {
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  INITRD UTILS                                  ||
// ! ||--------------------------------------------------------------------------------||

void initrd_display_hierarchy(void) {

    if (initrd_fs == NULL) {
        __WARND("Initrd: initrd not initialized");
        return;
    } else {
        printk("Initrd files:\n");
        printk("--------------------\n");
    }

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

        if ((_f_node->flags & VFS_DIRECTORY) != 0) {
            printk("Directory: %s\n", _d_node->d_name);
            printk("--------------------\n");

        } else if ((_f_node->flags & VFS_FILE) != 0) {
            printk("File: %s\n", _d_node->d_name);

            printk("Lenght: %u\n", _f_node->size);
            uint8_t *buffer = kmalloc(sizeof(char) * (_f_node->size + 1));

            memset(buffer, 0, _f_node->size + 1);

            initrd_fs->fops->read(_f_node, 0, _f_node->size, buffer);

            printk("File content: %s\n", buffer);
            kfree(buffer);
            printk("--------------------\n");
        }
        index++;
        ksleep(1);
    }
    vfs_closedir(initrd_fs, node);

    printk("Done!\n");
}