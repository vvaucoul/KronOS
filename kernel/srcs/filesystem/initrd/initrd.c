/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 13:07:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/14 11:50:30 by vvaucoul         ###   ########.fr       */
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

static int __initrd_fs_read(void *buf, uint32_t size);

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

        VfsNode *node = __initrd_register_node((char *)initrd_file_headers[i].name);

        printk("Initrd: Registering file "_GREEN
               "%s"_END
               "\n",
               initrd_file_headers[i].name);
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

    printk("Initrd: Found %d files\n", initrd_header->nfiles);
    printk("Initrd File: %s\n", initrd_file_headers->name);

    // Create initrd filesystem
    initrd_fs = __initrd_register_fs(INITRD_FILESYSTEM_NAME);

    // Create initrd root node (initrd root directory)
    initrd_fs->fs_root = __initrd_register_node(INITRD_FILESYSTEM_NAME);
    initrd_fs->fs_root->parent = NULL;
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
        __INFOD("Initrd: Initrd node created");
        return (node);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 FILE OPERATIONS                                ||
// ! ||--------------------------------------------------------------------------------||

static Dirent *__initrd_fs_readdir(void *node) {
    VfsNode *vfs_node = (VfsNode *)node;

    printk("\t- dir_index %d > child_count %d\n", vfs_node->dir_index, vfs_node->child_count);
    if (vfs_node->dir_index > vfs_node->child_count) {
        return (NULL);
    }
    
    VfsNode *child_node = vfs_node->next;
    static Dirent dirent = {0};

    if (child_node == NULL) {
        return (NULL);
    } else {
        while (child_node != NULL) {
            printk("\t- child_node->inode %d | vfs_node->dir_index %d == %d\n", child_node->inode, vfs_node->dir_index, child_node->inode == vfs_node->dir_index);
            if (child_node->inode == vfs_node->dir_index) {
                vfs_node->dir_index++;
                
                memcpy(dirent.name, child_node->name, strlen(child_node->name));
                dirent.ino = child_node->inode;

                printk("\t- dirent.name %s\n", dirent.name);
                return (&dirent);
            }
            printk("\t- child_node->name %s | child_node->next %p\n", child_node->name, child_node->next);
            child_node = child_node->next;
        }
    }

    printk("no dirent\n");
    return (NULL);
}

static VfsNode *__initrd_fs_finddir(void *node, const char *name) {
    VfsNode *vfs_node = (VfsNode *)node;

    while (vfs_node != NULL) {
        if (strcmp(vfs_node->name, name) == 0) {
            return (vfs_node);
        }
        vfs_node = vfs_node->next;
    }
    return (NULL);
}

static int __initrd_fs_read(void *buf, uint32_t size) {
    printk("Initrd: Read\n");
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                              INITRD FS OPERATIONS                              ||
// ! ||--------------------------------------------------------------------------------||

static int __initrd_fs_mount(void) {
    return (0);
}

static int __initrd_fs_unmount(void) {
    return (0);
}