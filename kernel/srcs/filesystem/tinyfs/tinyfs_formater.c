/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_formater.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:48:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/09 13:47:22 by vvaucoul         ###   ########.fr       */
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

    TinyFS_SuperBlock superblock = {
        .magic_number = TINYFS_MAGIC,
        .total_inodes = TINYFS_MAX_FILES,
        .free_inodes = TINYFS_MAX_FILES,
        .total_blocks = TINYFS_MAX_FILES,
        .free_blocks = TINYFS_MAX_FILES};

    // Calculating superblock size in sectors
    uint32_t superblock_size_sectors = sizeof(TinyFS_SuperBlock) / SECTOR_SIZE;
    if (sizeof(TinyFS_SuperBlock) % SECTOR_SIZE != 0) {
        superblock_size_sectors += 1;
    }
    printk("TinyFS: Superblock size in sectors: %d\n", superblock_size_sectors);

    // Write superblock to disk (LBA 0)
    if (device->write(device->device, 0, superblock_size_sectors, &superblock) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    }

    // Read superblock from disk
    uint8_t read_buffer[SECTOR_SIZE * superblock_size_sectors];
    TinyFS_SuperBlock read_superblock = {0};

    if (device->read(device->device, 0, superblock_size_sectors, &read_buffer) != 0) {
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

    for (uint32_t i = 0; i < TINYFS_MAX_FILES; i++) {
        TinyFS_Inode inode = {
            .used = 0,
            .inode_number = i,
            .name = {0},
            .size = 0,
            .block_pointers = {0},
            .indirect_pointer = 0};

        // Write inode to disk (LBA 1 + i)
        if (device->write(device->device, 1 + i, 1, &inode) != 0) {
            printk("TinyFS: Formatting (write) failed\n");
            return (-1);
        } else {
            printk("TinyFS: Inode %d written to disk\n", i);
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
        .type = VFS_DIRECTORY,
        .inode_number = 0,
        .parent_inode_number = 0};

    // * Write root directory to disk (LBA 1) *
    if (device->write(device->device, 1, 1, &root_directory) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    } else {
        printk("TinyFS: Root directory written to disk\n");
        kmsleep(50);
    }

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                SETUP BASIC FILES                                ||
    // ! ||--------------------------------------------------------------------------------||

    // Create basic files
    TinyFS_Inode file1 = {
        .name = "file1.txt",
        .type = VFS_FILE,
        .inode_number = 1,
        .parent_inode_number = 0,
        .size = 0};

    TinyFS_Inode file2 = {
        .name = "file2.txt",
        .type = VFS_FILE,
        .inode_number = 2,
        .parent_inode_number = 0,
        .size = 0};

    // * Write file1 to disk (LBA 2) *
    if (device->write(device->device, 2, 1, &file1) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    } else {
        printk("TinyFS: File1 written to disk\n");
        kmsleep(50);
    }

    // * Write file2 to disk (LBA 3) *
    if (device->write(device->device, 3, 1, &file2) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    } else {
        printk("TinyFS: File2 written to disk\n");
        kmsleep(50);
    }

    tinyfs->inodes[0] = root_directory;
    tinyfs->inodes[1] = file1;
    tinyfs->inodes[2] = file2;

// ! ||--------------------------------------------------------------------------------||
// ! ||                               SETUP FILE CONTENT                               ||
// ! ||--------------------------------------------------------------------------------||

// * Write file content to disk (LBA 4) *
#define FILE1_CONTENT "Hello, this is file1.txt\n"
#define FILE2_CONTENT "Hello, this is file2.txt\n"

    if (device->write(device->device, TINYFS_MAX_FILES + 1, 1, FILE1_CONTENT) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    } else {
        printk("TinyFS: File1 content written to disk\n");
        kmsleep(50);
    }

    if (device->write(device->device, TINYFS_MAX_FILES + 2, 1, FILE2_CONTENT) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    } else {
        printk("TinyFS: File2 content written to disk\n");
        kmsleep(50);
    }

    // Update inode information
    tinyfs->inodes[1].size = strlen(FILE1_CONTENT);
    tinyfs->inodes[2].size = strlen(FILE2_CONTENT);

    tinyfs->inodes[0].used = 1;
    tinyfs->inodes[1].used = 1;
    tinyfs->inodes[2].used = 1;

    // * Update Inodes on disk *
    // Warning: At this point we can't use tinyfs_write_inode, because tinyfs_device is not set yet
    if (device->write(device->device, 1, 1, &tinyfs->inodes[0]) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    }

    if (device->write(device->device, 2, 1, &tinyfs->inodes[1]) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    }

    if (device->write(device->device, 3, 1, &tinyfs->inodes[2]) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    }

    tiny_vfs->fs_root = (VfsNode *)(&(tinyfs->inodes[0]));

    printk("TinyFS: Setup success\n");

    return (0);
}