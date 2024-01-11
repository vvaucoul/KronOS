/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 23:36:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 18:55:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXT2_H
#define EXT2_H

/**
* @file ext2.h
* @brief Ext2 Filesystem
*
* This file contains the definition and structures related to the Ext2 filesystem.
* The Ext2 filesystem is a widely used filesystem in Linux-based operating systems.
* It provides support for file and directory operations, including reading, writing, opening, closing, and directory listing.
*
* The Ext2 filesystem implementation includes structures such as Ext2Inode and Ext2Dirent,
* which represent an inode and a directory entry, respectively. It also defines various
* file operations functions, such as read, write, open, close, readdir, finddir, flush,
* mkdir, unlink, and move.
*
* The Ext2 filesystem module is part of the Virtual File System (VFS) implementation,
* which provides a unified interface for interacting with different file systems.
* It is used by the kernel to manage file systems and perform file system operations.
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
typedef uint32_t (*rmdir_type_t)(Ext2Inode *inode, char *name);

typedef uint32_t mode_t;
typedef uint32_t uid_t;

typedef uint32_t (*chmod_type_t)(Ext2Inode *inode, mode_t mode);
typedef uint32_t (*chown_type_t)(Ext2Inode *inode, uint32_t uid, uint32_t gid);

typedef struct s_file_operations {
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    flush_type_t flush;
    mkdir_type_t mkdir;
    rmdir_type_t rmdir;
    unlink_type_t unlink;
    move_type_t move;
    chmod_type_t chmod;
    chown_type_t chown;
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

extern void ext2_init(void);

extern uint32_t ext2_read(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer);
extern uint32_t ext2_write(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer);
extern uint32_t ext2_write_full(Ext2Inode *node, uint32_t size, uint8_t *buffer);
extern void ext2_open(Ext2Inode *node, uint8_t read, uint8_t write);
extern void ext2_close(Ext2Inode *node);
extern struct dirent *ext2_readdir(Ext2Inode *node, uint32_t index);
extern Ext2Inode *ext2_finddir(Ext2Inode *node, char *name);

extern void ext2_mount(void);
extern void ext2_unmount(void);

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