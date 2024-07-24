/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 23:25:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 23:22:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TINYFS_H
#define TINYFS_H

#include <drivers/device/devices.h>
#include <drivers/device/ide.h>
#include <fs/vfs/vfs.h>
#include <kernel.h>

#define __TINYFS__ 1

#define TINYFS_MAGIC 0x00546E79       // : "TnY" with padding
#define TINYFS_FILENAME_MAX 32        // 32 characters
#define TINYFS_MAX_FILES 64           // 64 files
#define TINYFS_MAX_BLOCKS 1024        // 1024 blocks
#define TINYFS_BLOCK_SIZE 1024        // 1KB
#define TINYFS_MAX_BLOCKS_PER_FILE 12 // 12 direct blocks
#define TINYFS_FILESYSTEM_NAME "TinyFS"
#define TINYFS_FILESYSTEM_D_NAME "TinyFS Filesystem"
#define TINYFS_VERSION "0.1"

#define TINY_FS_SUPERBLOCK_OFFSET 0
#define TINY_FS_INODES_OFFSET sizeof(TinyFS_SuperBlock)
#define TINY_FS_DATA_OFFSET sizeof(TinyFS_SuperBlock) + (sizeof(TinyFS_Inode) * TINYFS_MAX_FILES)

/* File system structure */
typedef struct {
    void *fs; // File system reference, must be casted to TinyFS

    char name[TINYFS_FILENAME_MAX + 1]; // File name
    uint8_t used;                       // 0: Free, 1: Used
    uint8_t mode;                       // 1: File, 2: Directory (Ref vfs.h)

    uint16_t nlink; // Number of links

    uint32_t size; // File size

    uint8_t inode_number;            // Inode number (0 - 255 > TINYFS_MAX_FILES)
    uint8_t parent_inode_number;     // Parent inode number
    uint8_t links[TINYFS_MAX_FILES]; // Links to other files

    uint32_t open_count; // Number of open files

    uint32_t block_pointers[TINYFS_MAX_BLOCKS_PER_FILE]; // Direct block pointers
} __packed__ TinyFS_Inode;

/* SuperBlock structure */
typedef struct {
    uint32_t magic_number;
    uint32_t total_inodes;
    uint32_t free_inodes;
    uint32_t total_blocks;
    uint32_t free_blocks;
} __packed__ TinyFS_SuperBlock;

typedef struct {
    TinyFS_SuperBlock *superblock;
    TinyFS_Inode *inodes[TINYFS_MAX_FILES];
    uint8_t data_blocks[TINYFS_MAX_BLOCKS][TINYFS_BLOCK_SIZE]; // 1024 * 1024 = 1MB of data

    /* File system */
    struct {
        Vfs *vfs;       // Virtual file system
        Device *device; // Device (IDE, etc...)
    } fs;
} TinyFS;

extern TinyFS *tinyfs_init(Device *device);

extern int tinyfs_mount(void *fs);
extern int tinyfs_unmount(void *fs);

extern int tinyfs_read(void *node, uint32_t offset, uint32_t size, uint8_t *buffer);
extern int tinyfs_write(void *node, uint32_t offset, uint32_t size, uint8_t *buffer);

extern int tinyfs_formater(TinyFS *tfs, bool hard_format);

extern int tinyfs_write_superblock(Vfs *fs);

extern TinyFS_Inode tinyfs_read_inode(Vfs *fs, uint32_t inode);
extern int tinyfs_write_inode(Vfs *fs, uint32_t inode, TinyFS_Inode *tinyfs_inode);
extern TinyFS_Inode *tinyfs_get_inode(Vfs *fs, uint32_t inode);
extern int tinyfs_get_inode_idx(VfsNode *node);

// Todo: merge both create_inode functions
extern TinyFS_Inode *tinyfs_create_inode(TinyFS *tfs, TinyFS_Inode *parent_inode, const char name[TINYFS_FILENAME_MAX + 1], uint8_t mode);
extern TinyFS_Inode *tinyfs_create_file_inode(TinyFS_Inode *root_node, const char *node_name);

extern int tinyfs_delete_inode(TinyFS_Inode *tinyfs_inode);

extern int tinyfs_set_cache_links(VfsNode *node, VfsCacheLinks *links);
extern VfsCacheLinks *tinyfs_get_cache_links(VfsNode *node);

extern int tinyfs_mkdir(void *node, const char *name, uint16_t permission);
extern VfsNode *tinyfs_finddir(void *node, const char *name);
extern Dirent *tinyfs_readdir(void *node, uint32_t index);

extern int tinyfs_create(void *node, const char *name, uint16_t permission);

extern char *tinyfs_get_name(VfsNode *node);
extern VfsNode *tinyfs_get_parent(VfsNode *node);
extern VfsNode **tinyfs_get_links(Vfs *vfs, VfsNode *node);
extern int tinyfs_stat(void *node, struct stat *buf);

extern int tinyfs_open(void *node, uint32_t flags);
extern int tinyfs_close(void *node);

extern void tinyfs_display_hierarchy(TinyFS_Inode *node, uint32_t depth);
extern void tinyfs_display_inode(TinyFS_Inode *inode);

extern int tinyfs_allocate_block(TinyFS_Inode *inode, uint32_t *block_index, uint8_t *data, uint32_t size, bool write_disk);
extern int tinyfs_free_block(TinyFS_Inode *inode, uint32_t block_index, bool write_disk);

#endif /* !TINYFS_H */