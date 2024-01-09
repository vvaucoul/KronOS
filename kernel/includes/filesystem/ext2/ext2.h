/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 23:36:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 10:46:21 by vvaucoul         ###   ########.fr       */
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

typedef struct fs_node Ext2Inode;

typedef uint32_t (*read_type_t)(Ext2Inode *inode, uint32_t offset, uint32_t size, uint8_t *buffer);
typedef uint32_t (*write_type_t)(Ext2Inode *inode, uint32_t offset, uint32_t size, uint8_t *buffer);
typedef void (*open_type_t)(Ext2Inode *inode);
typedef void (*close_type_t)(Ext2Inode *inode);
typedef struct dirent *(*readdir_type_t)(Ext2Inode *inode, uint32_t index);
typedef Ext2Inode *(*finddir_type_t)(Ext2Inode *inode, char *name);
typedef void (*flush_type_t)(Ext2Inode *inode);
typedef uint32_t (*mkdir_type_t)(Ext2Inode *inode, char *name, uint16_t permission);
typedef uint32_t (*unlink_type_t)(Ext2Inode *inode, char *name);
typedef uint32_t (*move_type_t)(Ext2Inode *inode, char *name, char *new_name);

typedef struct s_file_operations {
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    flush_type_t flush;
    mkdir_type_t mkdir;
    unlink_type_t unlink;
    move_type_t move;
} __attribute__((packed)) Ext2FileOperations;

typedef struct fs_node {
    char name[EXT2_FILE_NAME_MAX_SIZE]; // The filename.
    uint32_t mask;                      // The permissions mask.
    uint32_t uid;                       // The owning user.
    uint32_t gid;                       // The owning group.
    uint32_t flags;                     // Includes the node type. See #defines above.
    uint32_t inode;                     // This is device-specific - provides a way for a filesystem to identify files.
    uint32_t length;                    // Size of the file, in bytes.
    uint32_t impl;                      // An implementation-defined number.

    Ext2FileOperations fops; // File Operations

    struct fs_node **childs; // linked list childs of the node
    struct fs_node *parent; // parent of the node
    
    uint32_t n_children;    // number of childs
} __attribute__((packed)) Ext2Inode;

typedef struct dirent // One of these is returned by the readdir call, according to POSIX.
{
    uint32_t inode;                     // Inode number. Required by POSIX.
    uint16_t rec_len;                   // Directory entry length.
    uint8_t name_len;                   // Name length.
    uint8_t file_type;                  // Type of file.
    char name[EXT2_FILE_NAME_MAX_SIZE]; // Filename.
} __attribute__((packed)) Ext2Dirent;

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

extern uint32_t ext2_read_fs(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer);
extern uint32_t ext2_write_fs(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer);
extern uint32_t ext2_write_fs_full(Ext2Inode *node, uint32_t size, uint8_t *buffer);
extern void ext2_open_fs(Ext2Inode *node, uint8_t read, uint8_t write);
extern void ext2_close_fs(Ext2Inode *node);
extern struct dirent *ext2_readdir_fs(Ext2Inode *node, uint32_t index);
extern Ext2Inode *ext2_finddir_fs(Ext2Inode *node, char *name);

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