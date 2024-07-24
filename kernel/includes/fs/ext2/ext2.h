/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 23:36:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 10:47:09 by vvaucoul         ###   ########.fr       */
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

#include <fs/vfs/vfs.h>
#include <fs/ext2/ext2_gd.h>
#include <kernel.h>

#include "ext2_bmap.h"

// Use ext2 file system
#define __EXT2__ 0

#define EXT2_MAGIC 0xEF53
#define EXT2_FILE_NAME_MAX_SIZE 128

#define EXT2_SUPERBLOCK_MAGIC 0xEF53

#define EXT2_SUPERBLOCK_TOTAL_INODE_OFFSET 0
#define EXT2_SUPERBLOCK_TOTAL_BLOCK_OFFSET 4
#define EXT2_SUPERBLOCK_RESERVED_BLOCKS_SUPERUSE_OFFSET 8
#define EXT2_SUPERBLOCK_UNALLOCATED_BLOCKS_OFFSET 12
#define EXT2_SUPERBLOCK_UNALLOCATED_INODES_OFFSET 16
#define EXT2_SUPERBLOCK_SUPERBLOCK_BLOCK_NUMBER_OFFSET 20
#define EXT2_SUPERBLOCK_LOG2_BLOCK_SIZE_OFFSET 24
#define EXT2_SUPERBLOCK_LOG2_FRAGMENT_SIZE_OFFSET 28
#define EXT2_SUPERBLOCK_BLOCKS_PER_GROUP_OFFSET 32
#define EXT2_SUPERBLOCK_FRAGMENTS_PER_GROUP_OFFSET 36
#define EXT2_SUPERBLOCK_INODES_PER_GROUP_OFFSET 40
#define EXT2_SUPERBLOCK_LAST_MOUNT_TIME_OFFSET 44
#define EXT2_SUPERBLOCK_LAST_WRITTEN_TIME_OFFSET 48
#define EXT2_SUPERBLOCK_MOUNT_COUNT_SINCE_CHECK_OFFSET 52
#define EXT2_SUPERBLOCK_MAX_MOUNT_COUNT_BEFORE_CHECK_OFFSET 54
#define EXT2_SUPERBLOCK_EXT2_SIGNATURE_OFFSET 56
#define EXT2_SUPERBLOCK_FILE_SYSTEM_STATE_OFFSET 58
#define EXT2_SUPERBLOCK_ERROR_HANDLING_BEHAVIOR_OFFSET 60
#define EXT2_SUPERBLOCK_VERSION_MINOR_PART_OFFSET 62
#define EXT2_SUPERBLOCK_LAST_CONSISTENCY_CHECK_TIME_OFFSET 64
#define EXT2_SUPERBLOCK_INTERVAL_BETWEEN_FSCKS_OFFSET 68
#define EXT2_SUPERBLOCK_OPERATING_SYSTEM_ID_OFFSET 72
#define EXT2_SUPERBLOCK_VERSION_MAJOR_PART_OFFSET 76
#define EXT2_SUPERBLOCK_USER_ID_RESERVED_BLKS_OFFSET 80
#define EXT2_SUPERBLOCK_GROUP_ID_RESERVED_BLKS_OFFSET 82

#define EXT_SUPERBLOCK_SIZE sizeof(Ext2SuperBlock)

// Reserved Inodes
#define EXT2_BAD_INO 1         /* Bad blocks inode */
#define EXT2_ROOT_INO 2        /* Root inode */
#define EXT2_ACL_IDX_INO 3     /* ACL inode */
#define EXT2_ACL_DATA_INO 4    /* ACL inode */
#define EXT2_BOOT_LOADER_INO 5 /* Boot loader inode */
#define EXT2_UNDEL_DIR_INO 6   /* Undelete directory inode */
#define EXT2_FIRST_INO 11      /* First non-reserved inode */

typedef struct fs_node Ext2Node;

typedef uint32_t mode_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;

typedef uint32_t (*read_type_t)(Ext2Node *inode, uint32_t offset, uint32_t size, uint8_t *buffer);
typedef uint32_t (*write_type_t)(Ext2Node *inode, uint32_t offset, uint32_t size, uint8_t *buffer);
typedef uint32_t (*unlink_type_t)(Ext2Node *inode, char *name);

typedef void (*open_type_t)(Ext2Node *inode);
typedef void (*close_type_t)(Ext2Node *inode);

typedef struct dirent *(*readdir_type_t)(Ext2Node *inode, uint32_t index);
typedef Ext2Node *(*finddir_type_t)(Ext2Node *inode, char *name);

typedef uint32_t (*mkdir_type_t)(Ext2Node *inode, char *name, mode_t permission);
typedef uint32_t (*rmdir_type_t)(Ext2Node *inode, char *name);

typedef uint32_t (*move_type_t)(Ext2Node *inode, char *name, char *new_name);
typedef uint32_t (*chmod_type_t)(Ext2Node *inode, mode_t mode);
typedef uint32_t (*chown_type_t)(Ext2Node *inode, uid_t uid, gid_t gid);

typedef struct s_file_operations {
    // File operations
    read_type_t read;     // Read from file
    write_type_t write;   // Write to file
    unlink_type_t unlink; // Delete file

    // Directory operations
    open_type_t open;       // Open directory
    close_type_t close;     // Close directory
    readdir_type_t readdir; // Read directory
    finddir_type_t finddir; // Find directory
    mkdir_type_t mkdir;     // Create directory
    rmdir_type_t rmdir;     // Delete directory

    // Other operations
    move_type_t move;   // Move file or directory
    chmod_type_t chmod; // Change file permissions
    chown_type_t chown; // Change file owner
} __attribute__((packed)) Ext2FileOperations;

/**
 * @brief Structure representing the ext2 superblock.
 *
 * The ext2 superblock contains information about the file system, such as the total number of inodes and blocks,
 * the block size, and the location of important data structures.
 */
typedef struct {
    uint32_t total_inodes;                 // Offset 0-3
    uint32_t total_blocks;                 // Offset 4-7
    uint32_t reserved_blocks_superuser;    // Offset 8-11
    uint32_t unallocated_blocks;           // Offset 12-15
    uint32_t unallocated_inodes;           // Offset 16-19
    uint32_t superblock_block_number;      // Offset 20-23
    uint32_t log2_block_size;              // Offset 24-27
    uint32_t log2_fragment_size;           // Offset 28-31
    uint32_t blocks_per_group;             // Offset 32-35
    uint32_t fragments_per_group;          // Offset 36-39
    uint32_t inodes_per_group;             // Offset 40-43
    uint32_t last_mount_time;              // Offset 44-47
    uint32_t last_written_time;            // Offset 48-51
    uint16_t mount_count_since_check;      // Offset 52-53
    uint16_t max_mount_count_before_check; // Offset 54-55
    uint16_t signature;                    // Offset 56-57
    uint16_t file_system_state;            // Offset 58-59
    uint16_t error_handling_behavior;      // Offset 60-61
    uint16_t version_minor_part;           // Offset 62-63
    uint32_t last_consistency_check_time;  // Offset 64-67
    uint32_t interval_between_fscks;       // Offset 68-71
    uint32_t operating_system_id;          // Offset 72-75
    uint32_t version_major_part;           // Offset 76-79
    uint16_t user_id_reserved_blks;        // Offset 80-81
    uint16_t group_id_reserved_blks;       // Offset 82-83
} __attribute__((packed)) Ext2SuperBlock;

/**
 * @brief Structure representing the ext2 inode
 *
 * The ext2 inode is used in the ext2 file system to represent files and directories.
 * It contains information about the file or directory, such as its size, permissions,
 * and pointers to the data blocks that store its contents.
 */

#define EXT2_NDIR_BLOCKS 12     /* Number of direct blocks */
#define EXT2_NDirect 15         /* Maximum number of direct blocks */
#define EXT2_NSymlinksPerDir 60 /* Maximum number of symlinks per dir entry*/
#define EXT2_NBlockGroups 256   /* Number of block groups in disk layout */

#pragma pack(push, 1) // Ensure proper alignment without padding
typedef struct ext2_inode {
    uint16_t mode;        /* File mode */
    uint16_t uid;         /* Owner UID */
    uint16_t gid;         /* Group ID */
    uint32_t size;        /* Size of the file in bytes */
    uint32_t atime;       /* Access time */
    uint32_t ctime;       /* Creation time */
    uint32_t mtime;       /* Modification time */
    uint32_t dtime;       /* Deletion time */
    uint16_t links_count; /* Link count */
    uint32_t blocks;      /* Total number of blocks used by this inode */
    uint32_t flags;       /* Flags associated with the inode */
    union {
        struct {
            uint32_t block[EXT2_NDIR_BLOCKS]; /* Pointers to direct blocks */
        } linux1;                             // Linux < 2.4.0
        struct {
            uint32_t block[EXT2_NDirect + EXT2_NSymlinksPerDir]; /* Direct & symlink pointers */
        } linux2;                                                // Linux >= 2.4.0
    };
    uint32_t generation;               /* Inode generation number */
    uint32_t file_acl;                 /* File access control list */
    uint32_t dir_acl;                  /* Directory access control list */
    uint32_t faddr;                    /* Fragment address */
    uint8_t frag;                      /* Fragment number */
    uint8_t fsize;                     /* Fragment size */
    uint16_t pad1;                     /* Padding */
    uint32_t osd1;                     /* Operating System dependent 1 */
    uint32_t block[EXT2_NBlockGroups]; /* Pointers to indirect blocks */
    uint32_t osd2;                     /* Operating System dependent 2 */
    uint32_t u;                        /* Used for htree extension */
    uint32_t padd[7];                  /* Padding */
} __attribute__((packed)) Ext2Inode;
#pragma pack(pop)

typedef struct fs_node {
    char name[EXT2_FILE_NAME_MAX_SIZE]; // The filename.

    uint32_t owner; // Owner of the file (used for task permissions).

    uint32_t flags;  // Includes the node type. See #defines above.
    uint32_t inode;  // This is device-specific - provides a way for a filesystem to identify files.
    uint32_t length; // Size of the file, in bytes.
    uint32_t impl;   // An implementation-defined number.

    // Device-specific
    uint32_t major; // The major device number.
    uint32_t minor; // The minor device number.

    Ext2FileOperations *fops; // File Operations

    // struct fs_node **childs; // linked list childs of the node
    // struct fs_node *parent;  // parent of the node

    uint32_t n_children; // number of childs
} __attribute__((packed)) Ext2Node;

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

extern Vfs *ext2_fs;
extern Ext2Node *fs_root;             // The root of the filesystem.
extern Ext2SuperBlock *fs_superblock; // The superblock of the filesystem.

extern int ext2_init(void);

/**
 * EXT2 File Operations
 */

// File operations
extern uint32_t ext2_read(Ext2Node *node, uint32_t offset, uint32_t size, uint8_t *buffer);
extern uint32_t ext2_write(Ext2Node *node, uint32_t offset, uint32_t size, uint8_t *buffer);
extern uint32_t ext2_unlink(Ext2Node *inode, char *name);

// Directory operations
extern void ext2_open(Ext2Node *node);
extern void ext2_close(Ext2Node *node);
extern struct dirent *ext2_readdir(Ext2Node *node, uint32_t index);
extern Ext2Node *ext2_finddir(Ext2Node *node, char *name);
extern uint32_t ext2_mkdir(Ext2Node *inode, char *name, mode_t permission);
extern uint32_t ext2_rmdir(Ext2Node *inode, char *name);

// Other operations
extern uint32_t ext2_move(Ext2Node *inode, char *name, char *new_name);
extern uint32_t ext2_chmod(Ext2Node *inode, mode_t mode);
extern uint32_t ext2_chown(Ext2Node *inode, uid_t uid, gid_t gid);

/**
 * EXT2 FileSystem Operations
 */

extern int ext2_mount(void *fs);
extern int ext2_unmount(void *fs);

/**
 * EXT2 Node Operations
 */

extern VfsNode *ext2_create_node(VfsNode *root_node, const char *node_name);
extern int ext2_remove_node(VfsNode *node);

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