/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/12 10:33:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VFS_H
#define VFS_H

/**
 * @file vfs.h
 * @brief Virtual File System implementation
 *
 * This file contains the implementation of the Virtual File System (VFS) module.
 * The VFS module provides an abstraction layer for file systems, allowing the kernel
 * to interact with different file systems using a unified interface.
 */

#include <filesystem/vfs/vfs_cache.h>
#include <syscall/stat.h>
#include <kernel.h>

// Use virtual file system
#define __VFS__ 1

#define DIR_PROCESS "proc"
#define DIR_CONFIG_FILES "etc"
#define DIR_HOME "home"
#define DIR_BIN "bin"
#define DIR_ETC "etc"
#define DIR_HOME "home"
#define DIR_OPT "opt"
#define DIR_TMP "tmp"
#define DIR_USR "usr"
#define DIR_VAR "var"
#define DIR_BOOT "boot"
#define DIR_DEV "dev"
#define DIR_LIB "lib"
#define DIR_LOST_FOUND "lost+found"
#define DIR_MEDIA "media"
#define DIR_MNT "mnt"
#define DIR_PROC "proc"
#define DIR_RUN "run"
#define DIR_SBIN "sbin"
#define DIR_SRV "srv"
#define DIR_SYS "sys"

#define VFS_FILE 0x01
#define VFS_DIRECTORY 0x02
#define VFS_CHARDEVICE 0x03
#define VFS_BLOCKDEVICE 0x04
#define VFS_PIPE 0x05
#define VFS_SYMLINK 0x06
#define VFS_MOUNTPOINT 0x08

#define VFS_NODE_FILE_LEN 255

#define EXT2_FILESYSTEM_NAME "ext2"
#define EXT2_FILESYSTEM 0x1

#define INITRD_FILESYSTEM_NAME "initrd"
#define INITRD_FILESYSTEM 0x2

#define TINYFS_FILESYSTEM_NAME "TinyFS"
#define TINYFS_FILESYSTEM 0x3

typedef struct s_dirent {
    char d_name[VFS_NODE_FILE_LEN];
    uint32_t ino;
    uint32_t parent_ino;
} Dirent;

typedef void VfsNode; // Filesystem node
typedef void VfsFS;   // Filesystem

#define vfs_custom_ops(name, ...) \
    int (*name)(void *node, ##__VA_ARGS__)
#define CUSTOM_OPS_SIZE 32

typedef struct s_vfs_fops {
    // File operations
    int (*open)(void *node, uint32_t flags);
    int (*close)(void *node);
    int (*read)(void *node, uint32_t offset, uint32_t size, uint8_t *buffer);
    int (*write)(void *node, uint32_t offset, uint32_t size, uint8_t *buffer);

    // Directory operations
    int (*opendir)(void *node);
    int (*closedir)(void *node);
    Dirent *(*readdir)(void *node, uint32_t index);
    VfsNode *(*finddir)(void *node, const char *name);

    // Create and delete files and directories
    int (*create)(void *node, const char *name, uint16_t permission);
    int (*unlink)(void *node, const char *name);
    int (*mkdir)(void *node, const char *name, uint16_t permission);
    int (*rmdir)(void *node, const char *name);

    // Other operations
    int (*move)(void *node, const char *name, const char *new_name);
    int (*chmod)(void *node, uint16_t permission);
    int (*chown)(void *node, uint16_t owner);

    // Link operations
    int (*link)(void *node, const char *name, const char *new_name);
    int (*symlink)(void *node, const char *name, const char *new_name);
    int (*readlink)(void *node, const char *name, const char *new_name);

    // Utils operations
    int (*chdir)(void *node, const char *name);
    int (*stat)(void *node, struct stat *buf);

    // Custom operations
    // Array of custom operations (implement custom operations in your filesystem)
    void *(*custom_ops[CUSTOM_OPS_SIZE])(void *node, ...);
} VfsFileOps;

/**
 * @brief VFS node operations
 *
 * VFS node operations are used to create and delete nodes in a filesystem.
 * FS must implement algorithms to add and remove nodes in the filesystem.
 * However, the VFS module provides a default implementation of these operations.
 */
typedef struct s_vfs_nops {
    // Node operations
    VfsNode *(*create_node)(VfsNode *root_node, const char *node_name);
    int (*remove_node)(VfsNode *node);

    // Todo: Remove get_name, replace by stat function
    char *(*get_name)(VfsNode *node);

    VfsNode *(*get_parent)(VfsNode *node);

    // Todo: Check if we need to keep get_links
    VfsNode **(*get_links)(Vfs *vfs, VfsNode *node);

    // Custom operations
    // Array of custom operations (implement custom operations in your filesystem)
    void *(*custom_ops[CUSTOM_OPS_SIZE])(void *node, ...);
} VfsNodeOps;

typedef uint16_t fs_type;

typedef struct s_vfs_fs_ops {
    int (*mount)(void *fs);
    int (*unmount)(void *fs);
} VfsFsOps;

typedef struct s_vfs_info {
    char name[32];
    char d_name[VFS_NODE_FILE_LEN];
    char version[32];
    fs_type type; // Filesystem type (EXT2, INITRD, ...)
} VfsInfo;

typedef struct s_vfs {
    // Filesystem info
    VfsInfo *fs_info; // Filesystem info
    VfsNode *fs_root; // Filesystem root node

    // Filesystem specific data
    VfsFS *fs; // Filesystem specific data structure (EXT2, INITRD, ...)

    // VFS Cache (Use custom Cache or not)
    // ! 0x1 = Use default VFS Cache
    uint8_t use_vfs_cache; // Use Default VFS Cache (1) or not (0)
    VfsCache *vfs_cache;   // VFS Cache

    // Operations
    VfsFsOps *fsops;  // Filesystem operations
    VfsFileOps *fops; // File operations
    VfsNodeOps *nops; // Filesystem node operations
} Vfs;

// VFS mount points (Use hashtable)
extern Hashtable *vfs_mounts;

extern int vfs_init(void);

extern Vfs *vfs_create_fs(VfsFS *fs, VfsInfo *fs_info, VfsFsOps *fsops, VfsFileOps *fops, VfsNodeOps *nops);
extern VfsNode *vfs_create_node(Vfs *vfs, VfsNode *root_node, const char *node_name);
extern int vfs_delete_node(Vfs *vfs, VfsNode *node);

extern int vfs_open(Vfs *vfs, VfsNode *node, uint32_t flags);
extern int vfs_close(Vfs *vfs, VfsNode *node);
extern int vfs_opendir(Vfs *vfs, VfsNode *node);
extern int vfs_closedir(Vfs *vfs, VfsNode *node);
extern Dirent *vfs_readdir(Vfs *vfs, VfsNode *node, uint32_t index);
extern VfsNode *vfs_finddir(Vfs *vfs, VfsNode *node, const char *name);

extern int vfs_mount(Vfs *vfs);
extern int vfs_unmount(Vfs *vfs);

extern Vfs *vfs_get_current_fs(void);
extern Vfs *vfs_get_fs(const char *fs_name);

extern int vfs_chdir(Vfs *vfs, const char *name);

// VFS Utils
extern char *vfs_get_node_path(Vfs *vfs, VfsNode *node);
extern int vfs_get_node_stat(Vfs *vfs, VfsNode *node, struct stat *buf);

#endif /* !VFS_H */