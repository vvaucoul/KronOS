/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_formater.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:48:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/30 16:22:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>
#include <system/pit.h>

static int __tinyfs_setup(TinyFS *tfs);

/**
 * @brief Hard format disk to tinyfs (reset to 0x0)
 *
 * @param tfs
 * @return int
 *
 * @note This function will format the disk to tinyfs
 */
static int __tinyfs_hard_format(TinyFS *tfs) {
    // Todo: implement size in device struct
    uint32_t disk_size = ide_device_get_size(tfs->fs.device->device) * 1024;
    uint32_t percent = 0;
    const char buffer[512] = {0};

    for (uint32_t i = 0; i < disk_size; i += 512) {
        uint32_t size = 512;

        if (size + i > disk_size) {
            size = disk_size - i;
        }

        if ((tfs->fs.device->swrite(tfs->fs.device->device, i, size, buffer)) != 0) {
            printk("TinyFS: Error: writing [%d] octets to disk\n", size);
            return (-EIO);
        } else {
            printk(_LCYAN "TinyFS: Formating: "_LGREEN
                          "[%d%c]\n" _CURSOR_MOVE_UP _END,
                   percent, '%');
        }
        percent = (i * 100) / disk_size;
    }
    printk(_LCYAN "TinyFS: Formating: "_LGREEN
                  "[%d%c]\n" _END,
           100, '%');

    return (0);
}

/**
 * @brief Format disk for tinyfs file system
 *
 * @param tfs
 * @param hard_format
 * @return int
 */
int tinyfs_formater(TinyFS *tfs, bool hard_format) {
    printk("TinyFS: Formating file system\n");

    /* Hard format disk (reset to 0x0) */
    if (hard_format == true) {
        printk("TinyFS: Hard formating\n");
        if ((__tinyfs_hard_format(tfs)) != 0) {
            printk("TinyFS: Hard formating failed\n");
            return (-1);
        }
    }

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                SETUP SUPERBLOCK                                ||
    // ! ||--------------------------------------------------------------------------------||

    TinyFS_SuperBlock write_superblock = {
        .magic_number = TINYFS_MAGIC,
        .total_inodes = TINYFS_MAX_FILES,
        .free_inodes = TINYFS_MAX_FILES,
        .total_blocks = TINYFS_MAX_FILES,
        .free_blocks = TINYFS_MAX_FILES};

    if ((tfs->fs.device->swrite(tfs->fs.device->device, TINY_FS_SUPERBLOCK_OFFSET, sizeof(TinyFS_SuperBlock), &write_superblock)) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-EIO);
    }

    /* Check if superblock has been correctly written */
    uint8_t read_buffer[sizeof(TinyFS_SuperBlock)];
    TinyFS_SuperBlock read_superblock = {0};

    if (tfs->fs.device->sread(tfs->fs.device->device, TINY_FS_SUPERBLOCK_OFFSET, sizeof(TinyFS_SuperBlock), &read_buffer) != 0) {
        printk("TinyFS: Formatting (read) failed\n");
        return (-EIO);
    } else {
        memcpy_s(&read_superblock, sizeof(TinyFS_SuperBlock), read_buffer, sizeof(TinyFS_SuperBlock));

        if (read_superblock.magic_number != TINYFS_MAGIC) {
            printk("TinyFS: Formatting failed, magic number mismatch\n");
            return (-EIO);
        }
    }

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                  SETUP INODES                                  ||
    // ! ||--------------------------------------------------------------------------------||

    uint32_t inode_size = sizeof(TinyFS_Inode);

    for (uint32_t i = 0; i < TINYFS_MAX_FILES; i++) {
        uint32_t inode_offset = (i * inode_size) + TINY_FS_INODES_OFFSET;

        TinyFS_Inode inode = {
            .fs = NULL,
            .name = "",
            .mode = 0,
            .inode_number = i,
            .parent_inode_number = 0,
            .size = 0,
            .nlink = 0,
            .used = 0,
            .block_pointers = {0}};

        /* Write inode to disk */
        if (tfs->fs.device->swrite(tfs->fs.device->device, inode_offset, sizeof(TinyFS_Inode), &inode) != 0) {
            printk("TinyFS: Formatting (write) failed\n");
            return (-EIO);
        } else {
            printk(_LCYAN "TinyFS: writing Inodes: "_LGREEN
                          "[%d%c]\n" _CURSOR_MOVE_UP _END,
                   (i * 100) / TINYFS_MAX_FILES, '%');
        }
    }
    printk(_LCYAN "TinyFS: writing Inodes: "_LGREEN
                  "[%d%c]\n" _END,
           100, '%');

    return (__tinyfs_setup(tfs));
}

static int __tinyfs_setup(TinyFS *tfs) {

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                SETUP FILESYSTEM                                ||
    // ! ||--------------------------------------------------------------------------------||

    // * Create root directory *
    TinyFS_Inode root_directory = {
        .fs = NULL,
        .name = "/",
        .mode = VFS_DIRECTORY,
        .inode_number = 0,
        .parent_inode_number = 0,
        .size = 0,
        .nlink = 0,
        .used = 1,
        .block_pointers = {0}};

    // * Write root directory to disk (LBA 1) *
    if (tfs->fs.device->swrite(tfs->fs.device->device, TINY_FS_INODES_OFFSET, sizeof(TinyFS_Inode), &root_directory) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-EIO);
    }

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                SETUP BASIC FILES                               ||
    // ! ||--------------------------------------------------------------------------------||

    // * Create basic files *
    TinyFS_Inode file1 = {
        .fs = NULL,
        .name = "file1.txt",
        .mode = VFS_FILE,
        .inode_number = 1,
        .parent_inode_number = 0,
        .size = 0,
        .nlink = 0,
        .used = 1,
        .block_pointers = {0}};

    TinyFS_Inode file2 = {
        .fs = NULL,
        .name = "file2.txt",
        .mode = VFS_FILE,
        .inode_number = 2,
        .parent_inode_number = 0,
        .size = 0,
        .nlink = 0,
        .used = 1,
        .block_pointers = {0}};

    // * Write file1 to disk (LBA 2) *
    if (tfs->fs.device->swrite(tfs->fs.device->device, TINY_FS_INODES_OFFSET + (sizeof(TinyFS_Inode) * 1), sizeof(TinyFS_Inode), &file1) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-EIO);
    }

    // * Write file2 to disk (LBA 3) *
    if (tfs->fs.device->swrite(tfs->fs.device->device, TINY_FS_INODES_OFFSET + (sizeof(TinyFS_Inode) * 2), sizeof(TinyFS_Inode), &file2) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-EIO);
    }

// ! ||--------------------------------------------------------------------------------||
// ! ||                               SETUP FILE CONTENT                               ||
// ! ||--------------------------------------------------------------------------------||

// * Write file content to disk (LBA 64) *
#define FILE1_CONTENT "Hello, this is file1.txt\n"
#define FILE2_CONTENT "Hello, this is file2.txt\n"

    // memcpy_s(tfs->data_blocks[1], strlen(FILE1_CONTENT), FILE1_CONTENT, strlen(FILE1_CONTENT));
    // memcpy_s(tfs->data_blocks[2], strlen(FILE2_CONTENT), FILE2_CONTENT, strlen(FILE2_CONTENT));

    if (tfs->fs.device->swrite(tfs->fs.device->device, TINY_FS_DATA_OFFSET, strlen(FILE1_CONTENT), FILE1_CONTENT) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    }

    if (tfs->fs.device->swrite(tfs->fs.device->device, TINY_FS_DATA_OFFSET + strlen(FILE1_CONTENT), strlen(FILE2_CONTENT), FILE2_CONTENT) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-1);
    }

    // * Update Inodes information *
    root_directory.links[0] = file1.inode_number;
    root_directory.links[1] = file2.inode_number;
    root_directory.nlink = 2;

    file1.size = strlen(FILE1_CONTENT);
    file2.size = strlen(FILE2_CONTENT);

    file1.parent_inode_number = 0;
    file2.parent_inode_number = 0;

    file1.block_pointers[0] = TINYFS_MAX_FILES + 1;
    file2.block_pointers[0] = TINYFS_MAX_FILES + 2;

    // * Write root directory to disk (LBA 1) *
    if (tfs->fs.device->swrite(tfs->fs.device->device, TINY_FS_INODES_OFFSET, sizeof(TinyFS_Inode), &root_directory) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-EIO);
    }

    // * Write file1 to disk (LBA 2) *
    if (tfs->fs.device->swrite(tfs->fs.device->device, TINY_FS_INODES_OFFSET + (sizeof(TinyFS_Inode) * 1), sizeof(TinyFS_Inode), &file1) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-EIO);
    }

    // * Write file2 to disk (LBA 3) *
    if (tfs->fs.device->swrite(tfs->fs.device->device, TINY_FS_INODES_OFFSET + (sizeof(TinyFS_Inode) * 2), sizeof(TinyFS_Inode), &file2) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-EIO);
    }

    // * Update superblock information *
    TinyFS_SuperBlock superblock = {0};
    if (tfs->fs.device->sread(tfs->fs.device->device, TINY_FS_SUPERBLOCK_OFFSET, sizeof(TinyFS_SuperBlock), &superblock) != 0) {
        printk("TinyFS: Formatting (read) failed\n");
        return (-EIO);
    }

    // * Update superblock information *
    superblock.free_inodes -= 3; // root - file1 - file2
    superblock.free_blocks -= 3; // root - file1 - file2

    if (tfs->fs.device->swrite(tfs->fs.device->device, TINY_FS_SUPERBLOCK_OFFSET, sizeof(TinyFS_SuperBlock), &superblock) != 0) {
        printk("TinyFS: Formatting (write) failed\n");
        return (-EIO);
    }

    printk(_LGREEN "TinyFS: Formating done\n" _END);
    return (0);
}