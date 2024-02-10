/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_formater.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:48:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 10:23:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <system/pit.h>

static int __tinyfs_setup(Device *device);

/**
 * @brief Format disk to tinyfs
 *
 * @param device
 *
 * @note This function will format the disk to tinyfs
 *
 * @return int
 */
int tinyfs_formater(Device *device) {
    printk("TinyFS: Formating file system\n");

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                SETUP SUPERBLOCK                                ||
    // ! ||--------------------------------------------------------------------------------||

    ((TinyFS *)tiny_vfs->fs)->superblock->magic_number = TINYFS_MAGIC;
    ((TinyFS *)tiny_vfs->fs)->superblock->total_inodes = TINYFS_MAX_FILES;
    ((TinyFS *)tiny_vfs->fs)->superblock->free_inodes = TINYFS_MAX_FILES;
    ((TinyFS *)tiny_vfs->fs)->superblock->total_blocks = TINYFS_MAX_FILES;
    ((TinyFS *)tiny_vfs->fs)->superblock->free_blocks = TINYFS_MAX_FILES;

    // Calculating superblock size in sectors
    uint32_t superblock_size_sectors = sizeof(TinyFS_SuperBlock) / SECTOR_SIZE;
    if (sizeof(TinyFS_SuperBlock) % SECTOR_SIZE != 0) {
        superblock_size_sectors += 1;
    }
    printk("TinyFS: Superblock size in sectors: %d\n", superblock_size_sectors);

    if ((device->swrite(device->device, 0, sizeof(TinyFS_SuperBlock), ((TinyFS *)tiny_vfs->fs)->superblock)) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    }

    // Read superblock from disk
    uint8_t read_buffer[SECTOR_SIZE * superblock_size_sectors];
    TinyFS_SuperBlock read_superblock = {0};

    if (device->sread(device->device, 0, sizeof(TinyFS_SuperBlock), &read_buffer) != 0) {
        printk("TinyFS: Formatting (read) failed\n");
        return (-1);
    } else {
        memcpy_s(&read_superblock, sizeof(TinyFS_SuperBlock), read_buffer, sizeof(TinyFS_SuperBlock));
    }

    // Check if superblock is valid (magic number)
    if (read_superblock.magic_number != TINYFS_MAGIC) {
        printk("TinyFS: Formatting failed, magic number mismatch\n");
        return (-1);
    } else {
        printk("TinyFS: Valid magic number\n");
    }

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                  SETUP INODES                                  ||
    // ! ||--------------------------------------------------------------------------------||

    uint32_t inode_size = sizeof(TinyFS_Inode);
    printk("TinyFS: Inode size: %d\n", inode_size);
    printk("TinyFS: Toal inode size in disk: %d\n", inode_size * TINYFS_MAX_FILES);

    for (uint32_t i = 0; i < TINYFS_MAX_FILES; i++) {
        uint32_t inode_offset = (i * inode_size) + sizeof(TinyFS_SuperBlock);

        TinyFS_Inode inode = {
            .name = "",
            .mode = 0,
            .inode_number = i,
            .parent_inode_number = 0,
            .size = 0,
            .nlink = 0,
            .used = 0,
            .block_pointers = {0}};

        // Write inode to disk at offset [inode_offset]

        if (device->swrite(device->device, inode_offset, sizeof(TinyFS_Inode), &inode) != 0) {
            printk("TinyFS: Formatting (write) failed\n");
            return (-1);
        } else {
            // printk("TinyFS: Inode %d written to disk at offset [%d]\n", i, inode_offset);
            kmsleep(50);
        }
    }
    printk("TinyFS: Formatting success\n");
    return (__tinyfs_setup(device));
}

static int __tinyfs_setup(Device *device) {
    printk("TinyFS: Setting up file system\n");

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                SETUP FILESYSTEM                                ||
    // ! ||--------------------------------------------------------------------------------||

    // * Create root directory *
    TinyFS_Inode root_directory = {
        .name = "/",
        .mode = VFS_DIRECTORY,
        .inode_number = 0,
        .parent_inode_number = 0,
        .size = 0,
        .nlink = 0,
        .used = 1,
        .block_pointers = {0}};

    // * Write root directory to disk (LBA 1) *
    printk("Write root directory to disk at offset [%d] with size [%d]\n", sizeof(TinyFS_SuperBlock), sizeof(TinyFS_Inode));
    if (device->swrite(device->device, sizeof(TinyFS_SuperBlock), sizeof(TinyFS_Inode), &root_directory) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    } else {
        printk("TinyFS: Root directory written to disk\n");
        kmsleep(50);
    }

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                SETUP BASIC FILES                               ||
    // ! ||--------------------------------------------------------------------------------||

    // * Create basic files *
    TinyFS_Inode file1 = {
        .name = "file1.txt",
        .mode = VFS_FILE,
        .inode_number = 1,
        .parent_inode_number = 0,
        .size = 0,
        .nlink = 0,
        .used = 1,
        .block_pointers = {0}};

    TinyFS_Inode file2 = {
        .name = "file2.txt",
        .mode = VFS_FILE,
        .inode_number = 2,
        .parent_inode_number = 0,
        .size = 0,
        .nlink = 0,
        .used = 1,
        .block_pointers = {0}};

    // * Write file1 to disk (LBA 2) *
    if (device->swrite(device->device, sizeof(TinyFS_SuperBlock) + (sizeof(TinyFS_Inode) * 1), sizeof(TinyFS_Inode), &file1) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    } else {
        printk("TinyFS: File1 written to disk\n");
        kmsleep(50);
    }

    // * Write file2 to disk (LBA 3) *
    if (device->swrite(device->device, sizeof(TinyFS_SuperBlock) * (sizeof(TinyFS_Inode) * 2), sizeof(TinyFS_Inode), &file2) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    } else {
        printk("TinyFS: File2 written to disk\n");
        kmsleep(50);
    }

    ((TinyFS *)(tiny_vfs->fs))->inodes[0] = &root_directory;    
    ((TinyFS *)(tiny_vfs->fs))->inodes[1] = &file1;
    ((TinyFS *)(tiny_vfs->fs))->inodes[2] = &file2;

// ! ||--------------------------------------------------------------------------------||
// ! ||                               SETUP FILE CONTENT                               ||
// ! ||--------------------------------------------------------------------------------||

// * Write file content to disk (LBA 64) *
#define FILE1_CONTENT "Hello, this is file1.txt\n"
#define FILE2_CONTENT "Hello, this is file2.txt\n"

    memcpy_s(((TinyFS *)(tiny_vfs->fs))->data_blocks[1], strlen(FILE1_CONTENT), FILE1_CONTENT, strlen(FILE1_CONTENT));
    memcpy_s(((TinyFS *)(tiny_vfs->fs))->data_blocks[2], strlen(FILE2_CONTENT), FILE2_CONTENT, strlen(FILE2_CONTENT));

    if (device->swrite(device->device, TINY_FS_DATA_OFFSET, strlen(FILE1_CONTENT), FILE1_CONTENT) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    } else {
        printk("TinyFS: File1 content written to disk\n");
        kmsleep(50);
    }

    if (device->swrite(device->device, TINY_FS_DATA_OFFSET + strlen(FILE1_CONTENT), strlen(FILE2_CONTENT), FILE2_CONTENT) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    } else {
        printk("TinyFS: File2 content written to disk\n");
        kmsleep(50);
    }

    // Update inode information
    ((TinyFS *)(tiny_vfs->fs))->inodes[1]->size = strlen(FILE1_CONTENT);
    ((TinyFS *)(tiny_vfs->fs))->inodes[2]->size = strlen(FILE2_CONTENT);

    ((TinyFS *)(tiny_vfs->fs))->inodes[1]->block_pointers[0] = TINYFS_MAX_FILES + 1;
    ((TinyFS *)(tiny_vfs->fs))->inodes[2]->block_pointers[0] = TINYFS_MAX_FILES + 2;

    ((TinyFS *)(tiny_vfs->fs))->inodes[0]->nlink = 2;
    ((TinyFS *)(tiny_vfs->fs))->inodes[0]->links[0] = 1;
    ((TinyFS *)(tiny_vfs->fs))->inodes[0]->links[1] = 2;
    

    // * Update Inodes on disk *
    if (device->swrite(device->device, sizeof(TinyFS_SuperBlock), sizeof(TinyFS_Inode), ((TinyFS *)(tiny_vfs->fs))->inodes[0]) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    }

    if (device->swrite(device->device, sizeof(TinyFS_SuperBlock) + (sizeof(TinyFS_Inode) * 1), sizeof(TinyFS_Inode), ((TinyFS *)(tiny_vfs->fs))->inodes[1]) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    }

    if (device->swrite(device->device, sizeof(TinyFS_SuperBlock) + (sizeof(TinyFS_Inode) * 2), sizeof(TinyFS_Inode), ((TinyFS *)(tiny_vfs->fs))->inodes[2]) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    }

    // tiny_vfs->fs_root = (VfsNode *)(((TinyFS *)(tiny_vfs->fs))->inodes[0]);

    // printk("TinyFS: Root: 0x%x | %s\n", &tiny_vfs->fs_root, ((TinyFS_Inode *)tiny_vfs->fs_root)->name);
    printk("TinyFS: Setup success\n");
    return (0);
}