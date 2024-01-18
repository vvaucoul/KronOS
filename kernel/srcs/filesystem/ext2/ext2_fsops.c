/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2_fsops.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 11:17:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/18 15:23:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/ext2/ext2.h>

#include <drivers/device/ide.h>
#include <memory/memory.h>

static Ext2FileOperations ext2_fops = {
    .read = ext2_read,
    .write = ext2_write,
    .unlink = ext2_unlink,
    .open = ext2_open,
    .close = ext2_close,
    .readdir = ext2_readdir,
    .finddir = ext2_finddir,
    .mkdir = ext2_mkdir,
    .rmdir = ext2_rmdir,
    .move = ext2_move,
    .chmod = 0,
    .chown = 0,
};

static Ext2Node ext2_super_root = {.name = "/",
                                   .inode = EXT2_ROOT_INO,
                                   .owner = 0,
                                   .length = 0,
                                   .flags = FS_DIRECTORY,
                                   .major = 0,
                                   .minor = 0,
                                   .fops = &ext2_fops};

static int ext2_read_superblock(Ext2SuperBlock *sb) {
    uint8_t *buf = kmalloc(sizeof(Ext2SuperBlock));

    printk("Reading superblock...\n");
    if (buf == NULL) {
        printk("Failed to allocate memory for superblock\n");
        return (1);
    }

    if (ide_get_device(0) == NULL) {
        printk("Failed to get device\n");
        return (1);
    }
    // Read superblock from disk 0, sector 2
    ide_read(ide_devices[0], 2, 2, buf, sizeof(Ext2SuperBlock));
    memcpy(sb, buf, sizeof(Ext2SuperBlock));
    printk("Superblock magic: %x\n", sb->magic);
    kfree(buf);

    if (sb == NULL) {
        printk("Failed to read superblock\n");
        return (1);
    }

    if (sb->magic != EXT2_MAGIC) {
        printk("Invalid EXT2 magic number\n");
        return (1);
    }
    return (0);
}

static int ext2_create_superblock(Ext2SuperBlock *sb) {
    if (sb == NULL) {
        return (1);
    }

    memset(sb, 0, sizeof(Ext2SuperBlock));
    sb->magic = EXT2_MAGIC;

    if (ide_get_device(0) == NULL) {
        printk("Failed to get device\n");
        return (1);
    }

    ide_write(ide_devices[0], 2, 2, sb, sizeof(Ext2SuperBlock));

    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                           EXT2 FILESYSTEM OPERATIONS                           ||
// ! ||--------------------------------------------------------------------------------||

int ext2_mount(void *fs) {

    // Init bitmaps
    init_bitmaps();

    // Init root node
    fs_root = kmalloc(sizeof(Ext2Node));
    fs_superblock = kmalloc(sizeof(Ext2SuperBlock));

    if (fs_root == NULL || fs_superblock == NULL) {
        return (1);
    }

    // Read superblock
    if (ext2_read_superblock(fs_superblock) != 0) {
        if ((ext2_create_superblock(fs_superblock)) != 0) {
            return (1);
        }
    }

    ext2_mkdir(&ext2_super_root, "/", 0);
}

int ext2_unmount(void *fs) {
}
