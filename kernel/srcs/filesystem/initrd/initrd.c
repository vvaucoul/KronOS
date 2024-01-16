/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 13:07:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/16 16:36:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/initrd.h>

#include <drivers/device/devices.h>
#include <memory/memory.h>

InitrdHeader *initrd_header = NULL;
InitrdFileHeader *initrd_file_headers = NULL;

Vfs *initrd_fs = NULL;

uint32_t initrd_nroot_nodes = 0;

uint32_t initrd_start = 0x0;
uint32_t initrd_end = 0x0;

static Vfs *__initrd_register_fs(char *fs_name);
static VfsNode *__initrd_register_node(char *fs_name);

static int __initrd_fs_mount(void);
static int __initrd_fs_unmount(void);

static int __initrd_fs_read(void *node, void *buf, uint32_t size);

static Dirent *__initrd_fs_readdir(void *node);
static VfsNode *__initrd_fs_finddir(void *node, const char *name);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   INITRD INIT                                  ||
// ! ||--------------------------------------------------------------------------------||

static int __initrd_register_files(void) {
    uint32_t n_files = initrd_header->nfiles;
    printk("Initrd: Registering %d files\n", n_files);

    for (uint32_t i = 0; i < n_files; i++) {
        initrd_file_headers[i].offset += initrd_start;

        uint8_t magic = initrd_file_headers[i].magic;

        if (magic != INITD_MAGIC) {
            __WARN("Initrd: Invalid magic number", 1);
        }
        VfsNode *node = __initrd_register_node((char *)initrd_file_headers[i].name);

        node->mask = node->uid = node->gid = 0;
        node->flags = VFS_FILE;
        node->length = initrd_file_headers[i].size;
        node->inode = i;
        node->fops.read = __initrd_fs_read;

        if ((vfs_add_node(initrd_fs->fs_root, node)) != 0) {
            __WARN("Initrd: Failed to register initrd file", 1);
        }
    }
    return (0);
}

int initrd_init(uint32_t start, uint32_t end) {
    initrd_start = start;
    initrd_end = end;

    initrd_header = (InitrdHeader *)((uintptr_t)start);
    initrd_file_headers = (InitrdFileHeader *)((uintptr_t)(start + sizeof(InitrdHeader)));

    // Create initrd filesystem
    initrd_fs = __initrd_register_fs(INITRD_FILESYSTEM_NAME);

    // Create initrd root node (initrd root directory)
    initrd_fs->fs_root = __initrd_register_node(INITRD_FILESYSTEM_NAME);
    initrd_fs->fs_root->parent.parent = NULL;
    initrd_fs->fs_root->flags = VFS_DIRECTORY;
    initrd_fs->fs_root->fops.readdir = __initrd_fs_readdir;
    initrd_fs->fs_root->fops.finddir = __initrd_fs_finddir;

    // Create initrd dev node (initrd device directory)
    VfsNode *dev_node = __initrd_register_node(INITRD_DEV_NAME);

    dev_node->flags = VFS_DIRECTORY;
    dev_node->fops.readdir = __initrd_fs_readdir;
    dev_node->fops.finddir = __initrd_fs_finddir;
    vfs_add_node(initrd_fs->fs_root, dev_node);

    if (initrd_fs == NULL) {
        __WARN("Initrd: Failed to create initrd filesystem", 1);
    }

    if ((__initrd_register_files()) != 0) {
        __WARN("Initrd: Failed to register initrd files", 1);
    }

    // Register initrd filesystem
    Device *device = device_init_new_device("initrd_fs", DEVICE_BLOCK, NULL, NULL, initrd_fs);

    if ((device_register(device)) != 0) {
        kfree(device);
        __WARN("Initrd: Failed to register initrd device", 1);
    } else {
        __INFOD("Initrd: Initrd device registered");
    }
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  INITRD NODES                                  ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief Create initrd fs
 */
static Vfs *__initrd_register_fs(char *fs_name) {
    Vfs *vfs = vfs_init(fs_name, NULL, NULL);

    if (vfs == NULL) {
        __WARN("Initrd: Failed to create initrd filesystem", NULL);
    } else {
        __INFOD("Initrd: Initrd filesystem created");
        return (vfs);
    }
}

/**
 * @brief Create initrd node
 */
static VfsNode *__initrd_register_node(char *node_name) {
    VfsNode *node = vfs_create_node(node_name);

    if (node == NULL) {
        __WARN("Initrd: Failed to create initrd node", NULL);
    } else {
        return (node);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 FILE OPERATIONS                                ||
// ! ||--------------------------------------------------------------------------------||

static Dirent *__initrd_fs_readdir(void *node) {
    VfsNode *__node = (VfsNode *)node;
    static uint32_t __index = 0;
    static Dirent dirent = {0};

    memset(&dirent, 0, sizeof(Dirent));
    for (uint32_t i = 0; i < __node->childs.child_cnt; i++) {
        VfsNode *__child = __node->childs.childs[i];

        if (__index == i) {
            __index++;
            memcpy(dirent.name, __child->name, strlen(__child->name));
            dirent.ino = __child->inode;
            return (&dirent);
        }
    }

    __index = 0;
    return (NULL);
}

static VfsNode *__initrd_fs_finddir(void *node, const char *name) {
    VfsNode *vfs_node = (VfsNode *)node;

    for (uint32_t i = 0; i < vfs_node->childs.child_cnt; i++) {
        VfsNode *sub_node = vfs_node->childs.childs[i];

        if (strcmp(sub_node->name, name) == 0) {
            return (sub_node);
        }
    }
    return (NULL);
}

static int __initrd_fs_read(void *node, void *buf, uint32_t size) {
    VfsNode *vfs_node = (VfsNode *)node;
    InitrdFileHeader *file_header = &initrd_file_headers[vfs_node->inode];

    if (size > file_header->size) {
        size = file_header->size;
    }

    void *content_addr = (void *)(file_header->offset);
    memcpy(buf, content_addr, size);
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                              INITRD FS OPERATIONS                              ||
// ! ||--------------------------------------------------------------------------------||

static __unused__ int __initrd_fs_mount(void) {
    return (0);
}

static __unused__ int __initrd_fs_unmount(void) {
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  INITRD UTILS                                  ||
// ! ||--------------------------------------------------------------------------------||

void initrd_display_hierarchy(void) {
    printk("Initrd files:\n");
    Dirent *_d_node;

    VfsNode *node = initrd_fs->fs_root;

    vfs_opendir(node);
    while ((_d_node = vfs_readdir(node)) != NULL) {
        printk("Found node: %s\n", _d_node->name);

        VfsNode *_f_node = vfs_finddir(node, _d_node->name);

        if (_f_node == NULL) {
            printk("Error: vfs_finddir failed\n");
            continue;
        } else {
            printk("Found Node: %s Flags: [%d]\n", _f_node->name, _f_node->flags);
        }

        printk("Flags: %u\n", _f_node->flags);

        if ((_f_node->flags & VFS_DIRECTORY) != 0) {
            printk("Directory: %s\n", _d_node->name);
            printk("--------------------\n\n");

        } else if ((_f_node->flags & VFS_FILE) != 0) {
            printk("File: %s\n", _d_node->name);

            printk("Lenght: %u\n", _f_node->length);
            uint8_t *buffer = kmalloc(sizeof(char) * (_f_node->length + 1));

            memset(buffer, 0, _f_node->length + 1);

            _f_node->fops.read(_f_node, buffer, _f_node->length);

            printk("File content: %s\n", buffer);
            kfree(buffer);
            printk("--------------------\n\n");
        }
        ksleep(1);
    }
    vfs_closedir(node);

    printk("Done!\n");
}