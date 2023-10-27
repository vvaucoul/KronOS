/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 13:28:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VFS_H
#define VFS_H

/**
 * Virtual File System
 * VFS: Virtual File System
 *
 * Le VFS est une couche d'abstraction dans le noyau d'un système d'exploitation qui permet
 * au système d'exploitation de manipuler des fichiers sur différents types de systèmes de fichiers comme
 * s'ils étaient tous du même type.
 * Il fournit une interface uniforme pour les opérations de fichiers et de répertoires,
 * et délègue les opérations spécifiques au système de fichiers sous-jacent.
 *
 */

#include <kernel.h>

/**
 * WIP:
 * To continue, when multiple filesystems will be implemented
 * or ext2 will be implemented
 */

// typedef struct s_vfs_inode VfsInode;

// typedef uint32_t (*read_type_t)(VfsInode *inode, uint32_t offset, uint32_t size, uint8_t *buffer);
// typedef uint32_t (*write_type_t)(VfsInode *inode, uint32_t offset, uint32_t size, uint8_t *buffer);
// typedef void (*open_type_t)(VfsInode *inode);
// typedef void (*close_type_t)(VfsInode *inode);
// typedef struct dirent *(*readdir_type_t)(VfsInode *inode, uint32_t index);
// typedef VfsInode *(*finddir_type_t)(VfsInode *inode, char *name);

// typedef struct s_file_operations {
//     read_type_t read;
//     write_type_t write;
//     open_type_t open;
//     close_type_t close;
//     readdir_type_t readdir;
//     finddir_type_t finddir;
// } VfsFileOperations;

// typedef struct s_vfs_inode {
//     char *name;
//     FileOperations *fops;
// } VfsInode;

// typedef struct s_vfs_file {
//     VfsInode *inode;
// } VfsFile;

// extern int vfs_read(VfsFile *file, void *buffer, uint32_t len);
// extern int vfs_write(VfsFile *file, void *buffer, uint32_t len);

#endif /* !VFS_H */