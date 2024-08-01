/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2_fsops.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 11:17:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 19:09:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/ext2/ext2.h>

#include <drivers/device/ide.h>
#include <mm/mm.h>
#include <system/pit.h>

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
    .chmod = NULL,
    .chown = NULL,
};

__unused__ static Ext2Node ext2_super_root = {
    .name = "/",
    .inode = EXT2_ROOT_INO,
    .owner = 0,
    .length = 0,
    .flags = FS_DIRECTORY,
    .major = 0,
    .minor = 0,
    .fops = &ext2_fops,
};

// ! ||--------------------------------------------------------------------------------||
// ! ||                                GROUP DESCRIPTOR                                ||
// ! ||--------------------------------------------------------------------------------||

// Todo: Fix this function
static int init_group_descriptor(const Ext2SuperBlock *fs_superblock) {
    // Calculate the size of the group descriptor table
    size_t desc_table_size = fs_superblock->blocks_per_group * sizeof(Ext2GroupDescriptor);

    // Allocate a buffer to hold the contents of the group descriptor table
    uint8_t *buf = kmalloc(desc_table_size);
    printk("Allocated buffer: %d\n", (desc_table_size));

    if (buf == NULL) {
        return -1;
    }

    // Calculate the starting LBA of the group descriptor table
    // uint32_t desc_table_lba = (sizeof(Ext2SuperBlock) + fs_superblock->blocks_per_group * fs_superblock->fragments_per_group * sizeof(char) + fs_superblock->blocks_per_group * sizeof(char) + fs_superblock->inodes_per_group * sizeof(Ext2Inode)) / fs_superblock->log2_block_size;
    // uint32_t desc_table_start_block = 1 + 1;
    // uint32_t desc_table_lba = desc_table_start_block * (1024 / 512);

    // Read the group descriptor table from the file system
    // ide_read(ide_devices[0], desc_table_lba, desc_table_size / fs_superblock->log2_block_size, buf);
    ide_read(ide_devices[0], 2, 2, buf);
    buf += sizeof(Ext2SuperBlock);

    for (uint32_t i = 0; i < sizeof(Ext2GroupDescriptor); i++) {
        printk("%d ", buf[i]);
        if (i % 16 == 0) {
            printk("\n");
        }
        kmsleep(25);
    }
    printk("\n");
    printk("\n");

    // Copy the first group descriptor from the file system to the 'ext2_gd' buffer
    Ext2GroupDescriptor *ext2_gd = kmalloc(sizeof(Ext2GroupDescriptor));

    if (ext2_gd == NULL) {
        kfree(buf);
        return -1;
    }

    memcpy((void *)ext2_gd, buf, sizeof(Ext2GroupDescriptor));

    printk("ext2_gd: %p\n", ext2_gd);

    printk("Block Bitmap: %d\n", ext2_gd->bg_block_bitmap);
    printk("Inode Bitmap: %d\n", ext2_gd->bg_inode_bitmap);
    printk("Inode Table: %d\n", ext2_gd->bg_inode_table);
    printk("Free Blocks Count: %d\n", ext2_gd->bg_free_blks_count);
    printk("Free Inodes Count: %d\n", ext2_gd->bg_free_inodes_count);
    printk("Used Directories Count: %d\n", ext2_gd->bg_used_dirs_count);
    printk("Padding: %u\n", ext2_gd->bg_pad);
    printk("Reserved: %d, %d, %d\n", ext2_gd->bg_reserved[0], ext2_gd->bg_reserved[1], ext2_gd->bg_reserved[2]);

    // Perform any additional operations with the populated 'ext2_gd' pointer before releasing its memory

    kfree(ext2_gd);

    return 0;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   SUPERBLOCK                                   ||
// ! ||--------------------------------------------------------------------------------||

static int ext2_read_superblock(Ext2SuperBlock *sb) {
    printk("Reading superblock...\n");

    uint8_t buf[EXT_SUPERBLOCK_SIZE];
    if (ide_get_device(0) == NULL) {
        printk("Failed to get device\n");
        return (1);
    } else {
        memset(buf, 0, EXT_SUPERBLOCK_SIZE);
        ide_read(ide_devices[0], 2, 2, buf);
        memcpy(sb, buf, sizeof(Ext2SuperBlock));
    }

    printk(_GREEN "[EXT2 - FS]"_END
                  " found on device "_GREEN
                  "[%d]"_END
                  "\n",
           0);

    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                           EXT2 FILESYSTEM OPERATIONS                           ||
// ! ||--------------------------------------------------------------------------------||

int ext2_mount(__unused__ void *fs) {
    printk("Mounting EXT2 file system...\n");

    // Init root node
    // fs_root = ext2_fs->fs_root = vfs_create_node(ext2_fs, NULL, "/");
    fs_superblock = kmalloc(sizeof(Ext2SuperBlock));

    if (fs_superblock == NULL) {
        return (1);
    }

    // if (fs_root == NULL || fs_superblock == NULL) {
    //     return (1);
    // }

    // Read superblock
    if (ext2_read_superblock(fs_superblock) != 0) {
        kfree(fs_superblock);
        return (1);
    } else {
        if (fs_superblock->signature != EXT2_MAGIC) {
            printk("Invalid EXT2 filesystem signature\n");
            return (1);
        }
    }

    if (init_group_descriptor(fs_superblock) != 0) {
        kfree(fs_superblock);
        return (1);
    }

    // ext2_mkdir(&ext2_super_root, "/", 0);
    kpause();
    return (0);
}

int ext2_unmount(__unused__ void *fs) {
    return (0);
}

// printk("Total Inodes: %u\n", fs_superblock->total_inodes);
// printk("Total Blocks: %u\n", fs_superblock->total_blocks);
// printk("Reserved Blocks for Superuser: %u\n", fs_superblock->reserved_blocks_superuser);
// printk("Unallocated Blocks: %u\n", fs_superblock->unallocated_blocks);
// printk("Unallocated Inodes: %u\n", fs_superblock->unallocated_inodes);
// printk("Superblock Block Number: %u\n", fs_superblock->superblock_block_number);
// printk("Log2 Block Size: %u\n", fs_superblock->log2_block_size);
// printk("Log2 Fragment Size: %u\n", fs_superblock->log2_fragment_size);
// printk("Blocks per Group: %u\n", fs_superblock->blocks_per_group);
// printk("Fragments per Group: %u\n", fs_superblock->fragments_per_group);
// printk("Inodes per Group: %u\n", fs_superblock->inodes_per_group);
// printk("Last Mount Time: %u\n", fs_superblock->last_mount_time);
// printk("Last Written Time: %u\n", fs_superblock->last_written_time);
// printk("Mount Count Since Check: %u\n", fs_superblock->mount_count_since_check);
// printk("Max Mount Count Before Check: %u\n", fs_superblock->max_mount_count_before_check);
// printk("Signature: %u\n", fs_superblock->signature);
// printk("File System State: %u\n", fs_superblock->file_system_state);
// printk("Error Handling Behavior: %u\n", fs_superblock->error_handling_behavior);
// printk("Version Minor Part: %u\n", fs_superblock->version_minor_part);
// printk("Last Consistency Check Time: %u\n", fs_superblock->last_consistency_check_time);
// printk("Interval Between FSCKs: %u\n", fs_superblock->interval_between_fscks);
// printk("Operating System ID: %u\n", fs_superblock->operating_system_id);
// printk("Version Major Part: %u\n", fs_superblock->version_major_part);
// printk("User ID Reserved Blocks: %u\n", fs_superblock->user_id_reserved_blks);
// printk("Group ID Reserved Blocks: %u", fs_superblock->group_id_reserved_blks);