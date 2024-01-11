/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 18:15:16 by vvaucoul         ###   ########.fr       */
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

#include <filesystem/ext2/ext2.h>
#include <filesystem/initrd.h>

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

#define FILESYSTEMS_COUNT 1

#define EXT2_FILESYSTEM_NAME "ext2"
#define VFS_NODE_FILE_LEN 256

typedef struct vfs_file_ops {
    int (*read)(void *buf, uint32_t size);
    int (*write)(void *buf, uint32_t size);
    int (*open)(const char *path, uint32_t flags);
    int (*close)(void);
} VfsFileOps;

typedef struct vfs_fs_ops {
    int (*mount)(void);
    int (*unmount)(void);
} VfsFsOps;

typedef struct vfs_node {
    char name[VFS_NODE_FILE_LEN];

    VfsFsOps *(*fsops)(void);
    VfsFileOps *(*fops)(void);
    struct vfs_node *parent;
} VfsNode;

typedef struct s_vfs {
    char *fs_name;           // Filesystem name

    VfsNode *fs_root;       // Filesystem root node
    VfsNode *(*init)(void); // Filesystem init function
} Vfs;

extern int vfs_init(const char *fs_name);

#endif /* !VFS_H */