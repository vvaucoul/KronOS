/*
 * Copyright (c) 2007 Vreixo Formoso
 * Copyright (c) 2009 - 2016 Thomas Schmitt
 * 
 * This file is part of the libisofs project; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License version 2 
 * or later as published by the Free Software Foundation. 
 * See COPYING file for details.
 */
#ifndef LIBISO_IMAGE_H_
#define LIBISO_IMAGE_H_

#include "libisofs.h"
#include "node.h"
#include "fsource.h"
#include "builder.h"

/* Size of a inode recycling window. Each new window causes a tree traversal.
   Window memory consumption is ISO_USED_INODE_RANGE / 8.
   This must be a power of 2 smaller than 30 bit and larger than 8 bit.
   Here: 32 kB memory for 256k inodes.
*/
#define ISO_USED_INODE_RANGE (1 << 18)

/* How many warnings to issue about name collisions during iso_image_import()
*/
#define ISO_IMPORT_COLL_WARN_MAX 10

/*
 * Image is a context for image manipulation.
 * Global objects such as the message_queues must belogn to that
 * context. Thus we will have, for example, a msg queue per image,
 * so images are completelly independent and can be managed together.
 * (Useful, for example, in Multiple-Document-Interface GUI apps.
 * [The stuff we have in init belongs really to image!]
 */

struct Iso_Image
{

    int refcount;

    IsoDir *root;

    char *volset_id;

    char *volume_id; /**< Volume identifier. */
    char *publisher_id; /**< Volume publisher. */
    char *data_preparer_id; /**< Volume data preparer. */
    char *system_id; /**< Volume system identifier. */
    char *application_id; /**< Volume application id */
    char *copyright_file_id;
    char *abstract_file_id;
    char *biblio_file_id;
    char *creation_time;
    char *modification_time;
    char *expiration_time;
    char *effective_time;
    char application_use[512];
    
    /* el-torito boot catalog */
    struct el_torito_boot_catalog *bootcat;

    /* Eventually loaded system area data, or NULL */
    char *system_area_data;
    /* Prescribed/detected options, see iso_write_opts_set_system_area() */
    /* >>> Needs to be coordinated with .imported_sa_info->system_area_options
    */
    int system_area_options;

   /*
    * Up to 15 boot files can be referred by a MIPS Big Endian Volume Header.
      The mips_boot_file_paths are ISO 9660 Rock Ridge paths.
    */
    int num_mips_boot_files;
    char *mips_boot_file_paths[15]; /* ISO 9660 Rock Ridge Paths */

    /* A data file of which the position and size shall be written after
       a SUN Disk Label.
    */
    IsoFile *sparc_core_node;

    /*
     * Parameters for HP-PA PALO boot sector. cmdline is a string. The other
     * four are absolute paths to data files in the ISO image.
     */
    char *hppa_cmdline;
    char *hppa_bootloader;
    char *hppa_kernel_32;
    char *hppa_kernel_64;
    char *hppa_ramdisk;

    /* Absolute DEC Alpha boot image path in the ISO image */
    char *alpha_boot_image;

    /* image identifier, for message origin identifier */
    int id;

    /**
     * Default filesystem to use when adding files to the image tree.
     */
    IsoFilesystem *fs;

    /**
     * Block storage of imported ISO if demanded by IsoReadOpts.
     */
    IsoDataSource *import_src;

    /*
     * Default builder to use when adding files to the image tree.
     */
    IsoNodeBuilder *builder;

    /**
     * Whether to follow symlinks or just add them as symlinks
     */
    unsigned int follow_symlinks : 1;

    /**
     * Whether to skip hidden files
     */
    unsigned int ignore_hidden : 1;

    /**
     * Flags that determine what special files should be ignore. It is a
     * bitmask:
     * bit0: ignore FIFOs
     * bit1: ignore Sockets
     * bit2: ignore char devices
     * bit3: ignore block devices
     */
    int ignore_special;

    /**
     * Whether to ignore ACL when inserting nodes into the image.
     * Not in effect with loading a complete ISO image but only with image
     * manipulation.
     */
    unsigned int builder_ignore_acl : 1;

    /**
     * Whether to ignore EAs when inserting nodes into the image.
     * Not in effect with loading a complete ISO image but only with image
     * manipulation. ACL does not count as EA.
     */
    unsigned int builder_ignore_ea : 1;

    /**
     * Files to exclude. Wildcard support is included.
     */
    char** excludes;
    int nexcludes;

    /**
     * if the dir already contains a node with the same name, whether to
     * replace or not the old node with the new. 
     */
    enum iso_replace_mode replace;

    /* TODO
    enum iso_replace_mode (*confirm_replace)(IsoFileSource *src, IsoNode *node);
    */

    /**
     * What to do in case of name longer than truncate_length:
     *  0= throw FAILURE
     *  1= truncate to truncate_length with MD5 of whole name at end
     */
    int truncate_mode;
    int truncate_length;

    /**
     * This is a convenience buffer for name truncation during image
     * manipulation where libisofs is not thread-safe anyway.
     */
    char truncate_buffer[4096];
    
    /**
     * When this is not NULL, it is a pointer to a function that will
     * be called just before a file will be added. You can control where
     * the file will be in fact added or ignored.
     * 
     * @return
     *      1 add, 0 ignore, < 0 cancel
     */
    int (*report)(IsoImage *image, IsoFileSource *src);

    /**
     * User supplied data
     */
    void *user_data;
    void (*user_data_free)(void *ptr);

    /**
     * Inode number management. inode_counter is taken over from
     * IsoImageFilesystem._ImageFsData after image import.
     * It is to be used with img_give_ino_number()
     * This is a Rock Ridge file serial number. Thus 32 bit.
    */
    uint32_t inode_counter;
    /*
     * A bitmap of used inode numbers in an interval beginning at
     * used_inodes_start and holding ISO_USED_INODE_RANGE bits.
     * If a bit is set, then the corresponding inode number is occupied.
     * This interval is kept around inode_counter and eventually gets
     * advanced by ISO_USED_INODE_RANGE numbers in a tree traversal
     * done by img_collect_inos(). The value will stay in the 32 bit range,
     * although used_inodes_start is 64 bit to better handle rollovers.
     */
    uint8_t *used_inodes;
    uint64_t used_inodes_start;

    /**
     * Array of MD5 checksums as announced by xattr "isofs.ca" of the 
     * root node. Array element 0 contains an overall image checksum for the
     * block range checksum_start_lba,checksum_end_lba. Element size is
     * 16 bytes. IsoFile objects in the image may have xattr "isofs.cx"
     * which gives their index in checksum_array.
     */
    uint32_t checksum_start_lba;
    uint32_t checksum_end_lba;
    uint32_t checksum_idx_count;
    char *checksum_array;

    /**
     * Whether a write run has been started by iso_image_create_burn_source()
     * and has not yet been finished.
     */
    int generator_is_running;

    /* Pointers to directories or files which shall be get a HFS+ blessing.
     * libisofs/hfsplus.c et.al. will compare these pointers
     * with the ->node pointer of Ecma119Nodes.
     * See libisofs.h
     */
    IsoNode *hfsplus_blessed[ISO_HFSPLUS_BLESS_MAX];

    /* Counts the name collisions while iso_image_import() */
    size_t collision_warnings;

    /* Contains the assessment of boot aspects of the loaded image */
    struct iso_imported_sys_area *imported_sa_info;

};


/* Apply truncation mode to name, using image->truncate_buffer to perform
   truncation if needed.

   Warning: Not thread-safe !
*/
int iso_image_truncate_name(IsoImage *image, const char *name, char **namept,
                            int flag);

    
/* Collect the bitmap of used inode numbers in the range of
   _ImageFsData.used_inodes_start + ISO_USED_INODE_RANGE
   @param flag bit0= recursion is active
*/
int img_collect_inos(IsoImage *image, IsoDir *dir, int flag);

/**
 * A global counter for inode numbers for the ISO image filesystem.
 * On image import it gets maxed by the eventual inode numbers from PX
 * entries. Up to the first 32 bit rollover it simply increments the counter.
 * After the first rollover it uses a look ahead bitmap which gets filled
 * by a full tree traversal. It covers the next inode numbers to come
 * (somewhere between 1 and ISO_USED_INODE_RANGE which is quite many)
 * and advances when being exhausted.
 * @param image The image where the number shall be used
 * @param flag  bit0= reset count (Caution: image must get new inos then)
 * @return
 *     Since 0 is used as default and considered self-unique, 
 *     the value 0 should only be returned in case of error.
 */ 
uint32_t img_give_ino_number(IsoImage *image, int flag);

/* @param flag bit0= overwrite any ino, else only ino == 0
               bit1= install inode with non-data, non-directory files
               bit2= install inode with directories
               bit3= with bit2: install inode on parameter dir
*/
int img_make_inos(IsoImage *image, IsoDir *dir, int flag);


/* Free the checksum array of an image and reset its layout parameters
*/
int iso_image_free_checksums(IsoImage *image, int flag);


/* Equip an ISO image with a new checksum array buffer (after isofs.ca and
   isofs.cx have already been adjusted).
*/
int iso_image_set_checksums(IsoImage *image, char *checksum_array, 
                            uint32_t start_lba, uint32_t end_lba,
                            uint32_t idx_count, int flag);


int iso_image_set_pvd_times(IsoImage *image,
                            char *creation_time, char *modification_time,
                            char *expiration_time, char *effective_time);


/* Collects boot block information obtained from the system area of
   imported images
*/
struct iso_imported_sys_area {

    int refcount;

    /* Whether there was some System Area data at all */
    int is_not_zero;

    /* Giving the error number if the assessment ended by an error */
    int overall_return;

    /* Block address of loaded Primar Volume Descriptor */
    uint32_t pvd_block;

    /* Size of the imported ISO image */
    uint32_t image_size;

    /* see libisofs.h : iso_write_opts_set_system_area() */
    int system_area_options;

    /* The perceived MBR partitions */
    struct iso_mbr_partition_request **mbr_req;
    int mbr_req_count;

    /* see ecma119.h : struct ecma119_image , struct iso_write_opts */
    /* Effective partition table parameter: 1 to 63, 0= disabled/default */
    int partition_secs_per_head; 
    /* 1 to 255, 0= disabled/default */
    int partition_heads_per_cyl;

    /* see ecma119.h :  struct iso_write_opts */
    uint32_t partition_offset;

    /* 2048-byte start LBA and block count of PreP partition */
    uint32_t prep_part_start;
    uint32_t prep_part_size;

    /* see ecma119.h : struct ecma119_image */
    struct iso_apm_partition_request **apm_req;
    int apm_req_count;
    int apm_req_flags;
    /* Number of found "GapNN", "ISO9660_data" partitions in APM */
    int apm_gap_count;

    /* see ecma119.h : struct iso_write_opts */
    int apm_block_size;

    /* >>> see ecma119.h : struct iso_write_opts */
    int hfsp_block_size;

    /* see ecma119.h : struct ecma119_image */
    struct iso_gpt_partition_request **gpt_req;
    int gpt_req_count;
    int gpt_req_flags;

    /* see ecma119.h : struct ecma119_image */
    uint8_t gpt_disk_guid[16];
    /* Start of GPT entries in System Area, block size 512 */
    uint64_t gpt_part_start;
    uint32_t gpt_max_entries;
    uint64_t gpt_first_lba;
    uint64_t gpt_last_lba;
    uint64_t gpt_backup_lba;
    char *gpt_backup_comments;
    uint32_t gpt_head_crc_found;
    uint32_t gpt_head_crc_should;
    uint32_t gpt_array_crc_found;
    uint32_t gpt_array_crc_should;

    /* see image.h : struct Iso_Image */
    int num_mips_boot_files;
    char **mips_boot_file_paths; /* ISO 9660 Rock Ridge Paths */
    struct iso_mips_voldir_entry **mips_vd_entries;

    /* see ecma119.h : struct ecma119_image */
    /* Memorized ELF parameters from MIPS Little Endian boot file */
    uint32_t mipsel_e_entry;
    uint32_t mipsel_p_offset;
    uint32_t mipsel_p_vaddr; 
    uint32_t mipsel_p_filesz;
    uint32_t mipsel_seg_start;
    char *mipsel_boot_file_path;

    /* see image.h : struct Iso_Image */
    char *sparc_disc_label;
    int sparc_secs_per_head;
    int sparc_heads_per_cyl;
    struct iso_sun_disk_label_entry *sparc_entries;
    int sparc_entry_count;

    /* grub2-sparc-core : a node in the ISO image
                          published at bytes 0x228 to 0x233
    */
    uint64_t sparc_grub2_core_adr;
    uint32_t sparc_grub2_core_size;
    IsoFile *sparc_core_node;

    /* see image.h : struct Iso_Image */
    int hppa_hdrversion;
    char *hppa_cmdline;
    uint32_t hppa_kern32_adr;
    uint32_t hppa_kern32_len;
    uint32_t hppa_kern64_adr;
    uint32_t hppa_kern64_len;
    uint32_t hppa_ramdisk_adr;
    uint32_t hppa_ramdisk_len;
    uint32_t hppa_bootloader_adr;
    uint32_t hppa_bootloader_len;
    uint32_t hppa_ipl_entry;
    char *hppa_kernel_32;
    char *hppa_kernel_64;
    char *hppa_ramdisk;
    char *hppa_bootloader;

    uint64_t alpha_boot_image_size;
    uint64_t alpha_boot_image_adr;
    char *alpha_boot_image;

    /* Some block addresses of active and first session:
         PVD, L Pathtable, Opt L, M Pathtable, Opt M, root directory
    */
    uint32_t meta_struct_blocks[12];
    int num_meta_struct_blocks;
};

int iso_imported_sa_new(struct iso_imported_sys_area **sa_info, int flag);

int iso_imported_sa_unref(struct iso_imported_sys_area **sa_info, int flag);


#endif /*LIBISO_IMAGE_H_*/
