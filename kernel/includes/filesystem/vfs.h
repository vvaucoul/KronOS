/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/16 12:10:33 by vvaucoul         ###   ########.fr       */
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

#include <kernel.h>

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

#define EXT2_FILESYSTEM_NAME "ext2"
#define VFS_NODE_FILE_LEN 256

#define VFS_NODE_CHILD_ALLOC 32

typedef struct vfs_node VfsNode;

typedef struct s_dirent {
    char name[VFS_NODE_FILE_LEN];
    uint32_t ino;
} Dirent;

typedef struct vfs_file_ops {
    int (*read)(void *node, void *buf, uint32_t size);
    int (*write)(void *node, void *buf, uint32_t size);

    int (*open)(void *node, const char *path, uint32_t flags);
    int (*close)(void *node);

    Dirent *(*readdir)(void *node);
    VfsNode *(*finddir)(void *node, const char *name);
} VfsFileOps;

typedef struct vfs_fs_ops {
    int (*mount)(void);
    int (*unmount)(void);
} VfsFsOps;

typedef struct vfs_parent {
    VfsNode *parent;
    uint32_t ref_cnt; // Number of references to this node
} VfsParent;

typedef struct vfs_childs {
    VfsNode **childs;
    uint32_t child_cnt; // Number of childs
    uint32_t alloced;   // Number of allocated childs
} VfsChilds;

typedef struct vfs_node {
    char name[VFS_NODE_FILE_LEN];
    uint32_t mask;
    uint32_t uid;
    uint32_t gid;
    uint32_t flags;
    uint32_t inode;
    uint32_t length;
    uint32_t impl;

    uint32_t dir_index;

    VfsFileOps fops;

    VfsParent parent;
    VfsChilds childs;
} VfsNode;

typedef struct s_vfs {
    char *fs_name; // Filesystem name

    VfsNode *fs_root;         // Filesystem root node
    VfsFsOps *(*fsops)(void); // Filesystem ops function
    int (*init)(void);        // Filesystem init function
} Vfs;

extern Vfs *vfs_init(const char *fs_name, VfsNode *fs_root, int (*init)(void));
extern VfsNode *vfs_create_node(const char *node_name);

extern int vfs_add_node(VfsNode *root_node, VfsNode *node);
extern int vfs_remove_node(VfsNode *root_node, VfsNode *node);

extern int vfs_open(VfsNode *node, uint32_t flags);
extern int vfs_close(VfsNode *node);

extern int vfs_opendir(VfsNode *node);
extern int vfs_closedir(VfsNode *node);

extern Dirent *vfs_readdir(VfsNode *node);
extern VfsNode *vfs_finddir(VfsNode *node, const char *name);

extern int vfs_mount(Vfs *vfs);
extern int vfs_unmount(Vfs *vfs);

#endif /* !VFS_H */