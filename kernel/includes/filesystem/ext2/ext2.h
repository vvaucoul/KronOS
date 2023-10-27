/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 23:36:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 12:57:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXT2_H
#define EXT2_H

/**
 * @brief      EXT2 Filesystem
 * 
 * EXT2: Second Extended Filesystem
 * 
 * ext2 est un système de fichiers spécifique utilisé principalement sur les systèmes Linux.
 * Il stocke les fichiers, les métadonnées des fichiers et les répertoires sur le disque dur.
 */

#include <kernel.h>

#define EXT2_MAGIC 0xEF53
#define EXT2_FILE_NAME_MAX_SIZE 128

struct fs_node;

typedef uint32_t (*read_type_t)(struct fs_node *, uint32_t, uint32_t, uint8_t *);
typedef uint32_t (*write_type_t)(struct fs_node *, uint32_t, uint32_t, uint8_t *);
typedef void (*open_type_t)(struct fs_node *);
typedef void (*close_type_t)(struct fs_node *);
typedef struct dirent *(*readdir_type_t)(struct fs_node *, uint32_t);
typedef struct fs_node *(*finddir_type_t)(struct fs_node *, char *name);

typedef struct fs_node {
    char name[EXT2_FILE_NAME_MAX_SIZE]; // The filename.
    uint32_t mask;                      // The permissions mask.
    uint32_t uid;                       // The owning user.
    uint32_t gid;                       // The owning group.
    uint32_t flags;                     // Includes the node type. See #defines above.
    uint32_t inode;                     // This is device-specific - provides a way for a filesystem to identify files.
    uint32_t length;                    // Size of the file, in bytes.
    uint32_t impl;                      // An implementation-defined number.
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct fs_node *ptr; // Used by mountpoints and symlinks.
} Ext2Inode;

typedef struct dirent // One of these is returned by the readdir call, according to POSIX.
{
    char name[EXT2_FILE_NAME_MAX_SIZE]; // Filename.
    uint32_t ino;                       // Inode number. Required by POSIX.
} Ext2Dirent;

#define FS_FILE 0x01        // File
#define FS_DIRECTORY 0x02   // Directory
#define FS_CHARDEVICE 0x03  // Character device
#define FS_BLOCKDEVICE 0x04 // Block device
#define FS_PIPE 0x05        // FIFO
#define FS_SYMLINK 0x06     // Symlink
#define FS_MOUNTPOINT 0x08  // Mountpoint

extern Ext2Inode *fs_root; // The root of the filesystem.

// Standard read/write/open/close functions. Note that these are all suffixed with
// _fs to distinguish them from the read/write/open/close which deal with file descriptors
// not file nodes.
extern uint32_t read_fs(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer);
extern uint32_t write_fs(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer);
extern void open_fs(Ext2Inode *node, uint8_t read, uint8_t write);
extern void close_fs(Ext2Inode *node);
extern struct dirent *readdir_fs(Ext2Inode *node, uint32_t index);
extern Ext2Inode *finddir_fs(Ext2Inode *node, char *name);

/*
◦ Name
◦ Size
◦ Type
◦ Inode
◦ Links
◦ Master
◦ Father
◦ Children
◦ Rights
◦ Next of kin
*/

#endif /* EXT2_H */