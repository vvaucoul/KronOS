/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 21:05:18 by vvaucoul         ###   ########.fr       */
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

// todo:
// tableau de pointeur de fonction pour avec les meme protos que le ext2 excepete les nodes etc...
// pointer a l'initialisation sur le bon filesystem et le bon pointeur de fonction

/**
 * WIP:
 * To continue, when multiple filesystems will be implemented
 * or ext2 will be implemented
 */

#define FILESYSTEM ext2

/*
**  VFS NODE
**
**  VFS Node EXT2
*/
#if FILESYSTEM == ext2
typedef Ext2Inode VfsInode;
#define FILESYSTEM_NAME "ext2"
#endif

/* Todo: Implement other filesystems */

typedef struct s_vfs {
    char *fs_name;           // Filesystem name
    VfsInode *fs_root;       // Filesystem root node
    VfsInode *(*init)(void); // Filesystem init function
} Vfs;

extern Vfs *vfs;

extern int vfs_init(void);

extern void vfs_delete_file(VfsInode *inode, char *name);

#endif /* !VFS_H */