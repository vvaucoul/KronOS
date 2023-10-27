/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 12:55:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VFS_H
# define VFS_H

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

typedef struct {
    int (*read)(void *buffer, uint32_t len);
    int (*write)(void *buffer, uint32_t len);
} FileOperations;

typedef struct {
    char *name;
    FileOperations *fops;
} VfsInode;

typedef struct {
    VfsInode *inode;
} VfsFile;

extern int vfs_read(VfsFile *file, void *buffer, uint32_t len);
extern int vfs_write(VfsFile *file, void *buffer, uint32_t len);

#endif /* !VFS_H */