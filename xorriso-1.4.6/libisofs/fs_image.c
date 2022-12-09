/*
 * Copyright (c) 2007 Vreixo Formoso
 * Copyright (c) 2009 - 2016 Thomas Schmitt
 *
 * This file is part of the libisofs project; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2 
 * or later as published by the Free Software Foundation. 
 * See COPYING file for details.
 */

/*
 * Filesystem/FileSource implementation to access an ISO image, using an
 * IsoDataSource to read image data.
 */

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include "libisofs.h"
#include "ecma119.h"
#include "messages.h"
#include "rockridge.h"
#include "image.h"
#include "tree.h"
#include "eltorito.h"
#include "node.h"
#include "aaip_0_2.h"
#include "system_area.h"

#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <langinfo.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>


/* Enable this and write the correct absolute path into the include statement
   below in order to test the pending contribution to syslinux:
     http://www.syslinux.org/archives/2013-March/019755.html

 # def ine Libisofs_syslinux_tesT 1

*/
#ifdef Libisofs_syslinux_tesT
#define Isolinux_rockridge_in_libisofS 1
#include "/reiser/syslinux/core/fs/iso9660/susp_rr.c"
/*
 # inc lude "/home/thomas/projekte/cdrskin_dir/libisoburn-develop/test/susp_rr.c"
*/
#endif /* Libisofs_syslinux_tesT */


/**
 * Options for image reading.
 * There are four kind of options:
 * - Related to multisession support.
 *   In most cases, an image begins at LBA 0 of the data source. However,
 *   in multisession discs, the later image begins in the last session on
 *   disc. The block option can be used to specify the start of that last
 *   session.
 * - Related to the tree that will be read.
 *   As default, when Rock Ridge extensions are present in the image, that
 *   will be used to get the tree. If RR extensions are not present, libisofs
 *   will use the Joliet extensions if available. Finally, the plain ISO-9660
 *   tree is used if neither RR nor Joliet extensions are available. With
 *   norock, nojoliet, and preferjoliet options, you can change this
 *   default behavior.
 * - Related to default POSIX attributes.
 *   When Rock Ridege extensions are not used, libisofs can't figure out what
 *   are the the permissions, uid or gid for the files. You should supply
 *   default values for that.
 */
struct iso_read_opts
{
    /**
     * Block where the image begins, usually 0, can be different on a
     * multisession disc.
     */
    uint32_t block;

    unsigned int norock : 1; /*< Do not read Rock Ridge extensions */
    unsigned int nojoliet : 1; /*< Do not read Joliet extensions */
    unsigned int noiso1999 : 1; /*< Do not read ISO 9660:1999 enhanced tree */
    unsigned int noaaip : 1; /* Do not read AAIP extension for xattr and ACL */
    unsigned int nomd5 : 2;  /* Do not read MD5 array */

    /**
     * Hand out new inode numbers and overwrite eventually read PX inode
     * numbers. This will split apart any hardlinks.
     */
    unsigned int make_new_ino : 1 ;

    /**
     * When both Joliet and RR extensions are present, the RR tree is used.
     * If you prefer using Joliet, set this to 1.
     */
    unsigned int preferjoliet : 1;

    /**
     * If neither Rock Ridge nor Joliet is used, the ECMA-119 names are mapped
     * according to one of these rules
     *  0 = unmapped:  show name as recorded in ECMA-119 directory record
     *                 (not suitable for writing them to a new ISO filesystem)
     *  1 = stripped:  like unmapped, but strip off trailing ";1" or ".;1"
     *  2 = uppercase: like stripped, but {a-z} mapped to {A-Z} 
     *  3 = lowercase: like stripped, but {A-Z} mapped to {a-z} 
     */
    unsigned int ecma119_map : 2;

    uid_t uid; /**< Default uid when no RR */
    gid_t gid; /**< Default uid when no RR */
    mode_t dir_mode; /**< Default mode when no RR (only permissions) */
    mode_t file_mode;
    /* TODO #00024 : option to convert names to lower case for iso reading */

    /**
     * Input charset for RR file names. NULL to use default locale charset.
     */
    char *input_charset;

    /**
     * Enable or disable methods to automatically choose an input charset.
     * This eventually overrides input_charset.
     *
     * bit0= allow to set the input character set automatically from
     *       attribute "isofs.cs" of root directory
     */
    int auto_input_charset;


    /** 
     * Enable or disable loading of the first 32768 bytes of the session and
     * submission by iso_write_opts_set_system_area(data, 0).
     */
    int load_system_area;

    /**
     * Keep data source of imported ISO filesystem in IsoImage.import_src
     */
    int keep_import_src;

    /**
     * What to do in case of name longer than truncate_length:
     *  0= throw FAILURE
     *  1= truncate to truncate_length with MD5 of whole name at end
     */
    int truncate_mode;
    int truncate_length;

};

/**
 * Return information for image.
 * Both size, hasRR and hasJoliet will be filled by libisofs with suitable
 * values.
 */
struct iso_read_image_features
{
    /**
     * Will be filled with the size (in 2048 byte block) of the image, as
     * reported in the PVM.
     */
    uint32_t size;

    /** It will be set to 1 if RR extensions are present, to 0 if not. */
    unsigned int hasRR :1;

    /** It will be set to 1 if Joliet extensions are present, to 0 if not. */
    unsigned int hasJoliet :1;

    /**
     * It will be set to 1 if the image is an ISO 9660:1999, i.e. it has
     * a version 2 Enhanced Volume Descriptor.
     */
    unsigned int hasIso1999 :1;

    /** It will be set to 1 if El-Torito boot record is present, to 0 if not.*/
    unsigned int hasElTorito :1;
};

static int ifs_fs_open(IsoImageFilesystem *fs);
static int ifs_fs_close(IsoImageFilesystem *fs);
static int iso_file_source_new_ifs(IsoImageFilesystem *fs,
           IsoFileSource *parent, struct ecma119_dir_record *record,
           IsoFileSource **src, int flag);

/** unique identifier for each image */
unsigned int fs_dev_id = 0;

/**
 * Should the RR extensions be read?
 */
enum read_rr_ext {
    RR_EXT_NO = 0, /*< Do not use RR extensions */
    RR_EXT_110 = 1, /*< RR extensions conforming version 1.10 */
    RR_EXT_112 = 2 /*< RR extensions conforming version 1.12 */
};


/**
 * Private data for the image IsoFilesystem
 */
typedef struct
{
    /** DataSource from where data will be read */
    IsoDataSource *src;

    /** unique id for the each image (filesystem instance) */
    unsigned int id;

    /**
     * Counter of the times the filesystem has been openned still pending of
     * close. It is used to keep track of when we need to actually open or
     * close the IsoDataSource.
     */
    unsigned int open_count;

    uid_t uid; /**< Default uid when no RR */
    gid_t gid; /**< Default uid when no RR */
    mode_t dir_mode; /**< Default mode when no RR (only permissions) */
    mode_t file_mode;

    int msgid;

    char *input_charset; /**< Input charset for RR names */
    char *local_charset; /**< For RR names, will be set to the locale one */

    /**
     * Enable or disable methods to automatically choose an input charset.
     * This eventually overrides input_charset.
     *
     * bit0= allow to set the input character set automatically from
     *       attribute "isofs.cs" of root directory
     */
    int auto_input_charset;

    /**
     * Will be filled with the block lba of the extend for the root directory
     * of the hierarchy that will be read, either from the PVD (ISO, RR) or
     * from the SVD (Joliet)
     */
    uint32_t iso_root_block;

    /**
     * Will be filled with the block lba of the extend for the root directory,
     * as read from the PVM
     */
    uint32_t pvd_root_block;

    /**
     * Will be filled with the block lba of the extend for the root directory,
     * as read from the SVD
     */
    uint32_t svd_root_block;

    /**
     * Will be filled with the block lba of the extend for the root directory,
     * as read from the enhanced volume descriptor (ISO 9660:1999)
     */
    uint32_t evd_root_block;

    /**
     * If we need to read RR extensions. i.e., if the image contains RR
     * extensions, and the user wants to read them.
     */
    enum read_rr_ext rr;

    /**
     * Bytes skipped within the System Use field of a directory record, before
     * the beginning of the SUSP system user entries. See IEEE 1281, SUSP. 5.3.
     */
    uint8_t len_skp;

    /* Volume attributes */
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

    /* extension information */

    /**
     * RR version being used in image.
     * 0 no RR extension, 1 RRIP 1.10, 2 RRIP 1.12
     */
    enum read_rr_ext rr_version;

    /** If Joliet extensions are available on image */
    unsigned int joliet : 1;

    /** If ISO 9660:1999 is available on image */
    unsigned int iso1999 : 1;

    /**
     * See struct iso_read_opts.
     */
    int truncate_mode;
    int truncate_length;
    unsigned int ecma119_map : 2;

    /** Whether AAIP info shall be loaded if it is present.
     *  1 = yes , 0 = no
     */
    int aaip_load;

    /** Whether the MD5 array shall be read if available.
     *  2 = yes, but do not check tags , 1 = yes , 0 = no
     */
    int md5_load;

    /** Whether AAIP is present. Version major.minor = major * 100 + minor
     *  Value -1 means that no AAIP ER was detected yet.
     */
    int aaip_version;

    /**
     * Number of blocks of the volume, as reported in the PVM.
     */
    uint32_t nblocks;

    /* el-torito information */
    unsigned int eltorito : 1; /* is el-torito available */
    int num_bootimgs;
    unsigned char platform_ids[Libisofs_max_boot_imageS];
    unsigned char id_strings[Libisofs_max_boot_imageS][28];
    unsigned char selection_crits[Libisofs_max_boot_imageS][20];
    unsigned char boot_flags[Libisofs_max_boot_imageS]; /* bit0= bootable */
    unsigned char media_types[Libisofs_max_boot_imageS];
    unsigned char partition_types[Libisofs_max_boot_imageS];
    short load_segs[Libisofs_max_boot_imageS];
    short load_sizes[Libisofs_max_boot_imageS];
    /** Block addresses of for El-Torito boot images.
        Needed to recognize them when the get read from the directory tree.
     */
    uint32_t bootblocks[Libisofs_max_boot_imageS];

    uint32_t catblock; /**< Block for El-Torito catalog */
    off_t catsize; /* Size of boot catalog in bytes */
    char *catcontent;

    /* Whether inode numbers from PX entries shall be discarded */
    unsigned int make_new_ino : 1 ;

    /* Inode number generator counter. 32 bit because for Rock Ridge PX. */
    uint32_t inode_counter;

    /* PX inode number status
       bit0= there were nodes with PX inode numbers
       bit1= there were nodes with PX but without inode numbers
       bit2= there were nodes without PX
       bit3= there were nodes with faulty PX
     */
    int px_ino_status;

    /* Which Rock Ridge error messages already have occurred
       bit0= Invalid PX entry
       bit1= Invalid TF entry
       bit2= New NM entry found without previous CONTINUE flag
       bit3= Invalid NM entry
       bit4= New SL entry found without previous CONTINUE flag
       bit5= Invalid SL entry
       bit6= Invalid SL entry, no child location
       bit7= Invalid PN entry
       bit8= Sparse files not supported
       bit9= SP entry found in a directory entry other than '.' entry of root
      bit10= ER entry found in a directory entry other than '.' entry of root
      bit11= Invalid AA entry
      bit12= Invalid AL entry
      bit13= Invalid ZF entry
      bit14= Rock Ridge PX entry is not present or invalid
      bit15= Incomplete NM
      bit16= Incomplete SL
      bit17= Charset conversion error
      bit18= Link without destination
    */
    int rr_err_reported;
    int rr_err_repeated;

    size_t joliet_ucs2_failures;

} _ImageFsData;

typedef struct image_fs_data ImageFileSourceData;

/* IMPORTANT: Any change must be reflected by ifs_clone_src */
struct image_fs_data
{
    IsoImageFilesystem *fs; /**< reference to the image it belongs to */
    IsoFileSource *parent; /**< reference to the parent (NULL if root) */

    struct stat info; /**< filled struct stat */
    char *name; /**< name of this file */

    /**
     * Location of file extents.
     */
    struct iso_file_section *sections;
    int nsections;

    unsigned int opened : 2; /**< 0 not opened, 1 opened file, 2 opened dir */

#ifdef Libisofs_with_zliB
    uint8_t header_size_div4;
    uint8_t block_size_log2;
    uint32_t uncompressed_size;
#endif

    /* info for content reading */
    struct
    {
        /**
         * - For regular files, once opened it points to a temporary data
         *   buffer of 2048 bytes.
         * - For dirs, once opened it points to a IsoFileSource* array with
         *   its children
         * - For symlinks, it points to link destination
         */
        void *content;

        /**
         * - For regular files, number of bytes already read.
         */
        off_t offset;
    } data;

    /**
     * malloc() storage for the string of AAIP fields which represent
     * ACLs and XFS-style Extended Attributes. (Not to be confused with
     * ECMA-119 Extended Attributes.)
     */
    unsigned char *aa_string;

};

struct child_list
{
    IsoFileSource *file;
    struct child_list *next;
};

void child_list_free(struct child_list *list)
{
    struct child_list *temp;
    struct child_list *next = list;
    while (next != NULL) {
        temp = next->next;
        iso_file_source_unref(next->file);
        free(next);
        next = temp;
    }
}

static
char* ifs_get_path(IsoFileSource *src)
{
    ImageFileSourceData *data;
    data = src->data;

    if (data->parent == NULL) {
        return strdup("");
    } else {
        char *path, *new_path;
        int pathlen;

        if (data->name == NULL)
            return NULL;
        path = ifs_get_path(data->parent);
        if (path == NULL)
            return NULL;
        pathlen = strlen(path);
        new_path = realloc(path, pathlen + strlen(data->name) + 2);
        if (new_path == NULL) {
            free(path);
            return NULL;
        }
        path= new_path;
        path[pathlen] = '/';
        path[pathlen + 1] = '\0';
        return strcat(path, data->name);
    }
}

static
char* ifs_get_name(IsoFileSource *src)
{
    ImageFileSourceData *data;
    data = src->data;
    return data->name == NULL ? NULL : strdup(data->name);
}

static
int ifs_lstat(IsoFileSource *src, struct stat *info)
{
    ImageFileSourceData *data;

    if (src == NULL || info == NULL) {
        return ISO_NULL_POINTER;
    }

    data = src->data;
    if (data == NULL)
        return ISO_NULL_POINTER;
    *info = data->info;
    return ISO_SUCCESS;
}

static
int ifs_stat(IsoFileSource *src, struct stat *info)
{
    ImageFileSourceData *data;

    if (src == NULL || info == NULL || src->data == NULL) {
        return ISO_NULL_POINTER;
    }

    data = (ImageFileSourceData*)src->data;

    if (S_ISLNK(data->info.st_mode)) {
        /* TODO #00012 : support follow symlinks on image filesystem */
        return ISO_FILE_BAD_PATH;
    }
    *info = data->info;
    return ISO_SUCCESS;
}

static
int ifs_access(IsoFileSource *src)
{
    /* we always have access, it is controlled by DataSource */
    return ISO_SUCCESS;
}

/**
 * Read all directory records in a directory, and creates an IsoFileSource for
 * each of them, storing them in the data field of the IsoFileSource for the
 * given dir.
 */
static
int read_dir(ImageFileSourceData *data)
{
    int ret;
    uint32_t size;
    uint32_t block;
    IsoImageFilesystem *fs;
    _ImageFsData *fsdata;
    struct ecma119_dir_record *record;
    uint8_t *buffer = NULL;
    IsoFileSource *child = NULL;
    uint32_t pos = 0;
    uint32_t tlen = 0;

    if (data == NULL) {
        ret = ISO_NULL_POINTER; goto ex;
    }

    LIBISO_ALLOC_MEM(buffer, uint8_t, BLOCK_SIZE);
    fs = data->fs;
    fsdata = fs->data;

    /* a dir has always a single extent */
    block = data->sections[0].block;
    ret = fsdata->src->read_block(fsdata->src, block, buffer);
    if (ret < 0) {
        goto ex;
    }

    /* "." entry, get size of the dir and skip */
    record = (struct ecma119_dir_record *)(buffer + pos);
    size = iso_read_bb(record->length, 4, NULL);
    tlen += record->len_dr[0];
    pos += record->len_dr[0];

    /* skip ".." */
    record = (struct ecma119_dir_record *)(buffer + pos);
    tlen += record->len_dr[0];
    pos += record->len_dr[0];

    while (tlen < size) {

        record = (struct ecma119_dir_record *)(buffer + pos);
        if (pos == 2048 || record->len_dr[0] == 0) {
            /*
             * The directory entries are splitted in several blocks
             * read next block
             */
            ret = fsdata->src->read_block(fsdata->src, ++block, buffer);
            if (ret < 0) {
                goto ex;
            }
            tlen += 2048 - pos;
            pos = 0;
            continue;
        }

        /* (Vreixo:)
         * What about ignoring files with existence flag?
         * if (record->flags[0] & 0x01)
         *	continue;
         * ts B20306 : >>> One should rather record that flag and write it
         *             >>> to the new image.
         */

#ifdef Libisofs_wrongly_skip_rr_moveD
        /* ts B20306 :
           This skipping by name is wrong resp. redundant:
           If no rr reading is enabled, then it is the only access point for
           the content of relocated directories. So one should not ignore it.
           If rr reading is enabled, then the RE entry of mkisofs' RR_MOVED
           will cause it to be skipped.
	*/

        /* (Vreixo:)
         * For a extrange reason, mkisofs relocates directories under
         * a RR_MOVED dir. It seems that it is only used for that purposes,
         * and thus it should be removed from the iso tree before
         * generating a new image with libisofs, that don't uses it.
         */

        if (data->parent == NULL && record->len_fi[0] == 8
            && !strncmp((char*)record->file_id, "RR_MOVED", 8)) {

            iso_msg_debug(fsdata->msgid, "Skipping RR_MOVE entry.");

            tlen += record->len_dr[0];
            pos += record->len_dr[0];
            continue;
        }

#endif /* Libisofs_wrongly_skip_rr_moveD */

        /*
         * We pass a NULL parent instead of dir, to prevent the circular
         * reference from child to parent.
         */
        ret = iso_file_source_new_ifs(fs, NULL, record, &child, 0);
        if (ret < 0) {
            if (child) {
                /*
                 * This can only happen with multi-extent files.
                 */
                ImageFileSourceData *ifsdata = child->data;
                free(ifsdata->sections);
                free(ifsdata->name);
                free(ifsdata);
                free(child);
            }
            goto ex;
        }

        /* add to the child list */
        if (ret == 1) {
            struct child_list *node;
            node = malloc(sizeof(struct child_list));
            if (node == NULL) {
                iso_file_source_unref(child);
                {ret = ISO_OUT_OF_MEM; goto ex;}
            }
            /*
             * Note that we insert in reverse order. This leads to faster
             * addition here, but also when adding to the tree, as insertion
             * will be done, sorted, in the first position of the list.
             */
            node->next = data->data.content;
            node->file = child;
            data->data.content = node;
            child = NULL;
        }

        tlen += record->len_dr[0];
        pos += record->len_dr[0];
    }

    ret = ISO_SUCCESS;
ex:;
    LIBISO_FREE_MEM(buffer);
    return ret;
}

static
int ifs_open(IsoFileSource *src)
{
    int ret;
    ImageFileSourceData *data;

    if (src == NULL || src->data == NULL) {
        return ISO_NULL_POINTER;
    }
    data = (ImageFileSourceData*)src->data;

    if (data->opened) {
        return ISO_FILE_ALREADY_OPENED;
    }

    if (S_ISDIR(data->info.st_mode)) {
        /* ensure fs is openned */
        ret = data->fs->open(data->fs);
        if (ret < 0) {
            return ret;
        }

        /*
         * Cache all directory entries.
         * This can waste more memory, but improves as disc is read in much more
         * sequencially way, thus reducing jump between tracks on disc
         */
        ret = read_dir(data);
        data->fs->close(data->fs);

        if (ret < 0) {
            /* free probably allocated children */
            child_list_free((struct child_list*)data->data.content);
        } else {
            data->opened = 2;
        }

        return ret;
    } else if (S_ISREG(data->info.st_mode)) {
        /* ensure fs is openned */
        ret = data->fs->open(data->fs);
        if (ret < 0) {
            return ret;
        }
        data->data.content = malloc(BLOCK_SIZE);
        if (data->data.content == NULL) {
            return ISO_OUT_OF_MEM;
        }
        data->data.offset = 0;
        data->opened = 1;
    } else {
        /* symlinks and special files inside image can't be openned */
        return ISO_FILE_ERROR;
    }
    return ISO_SUCCESS;
}

static
int ifs_close(IsoFileSource *src)
{
    ImageFileSourceData *data;

    if (src == NULL || src->data == NULL) {
        return ISO_NULL_POINTER;
    }
    data = (ImageFileSourceData*)src->data;

    if (!data->opened) {
        return ISO_FILE_NOT_OPENED;
    }

    if (data->opened == 2) {
        /*
         * close a dir, free all pending pre-allocated children.
         * not that we don't need to close the filesystem, it was already
         * closed
         */
        child_list_free((struct child_list*) data->data.content);
        data->data.content = NULL;
        data->opened = 0;
    } else if (data->opened == 1) {
        /* close regular file */
        free(data->data.content);
        data->fs->close(data->fs);
        data->data.content = NULL;
        data->opened = 0;
    } else {
        /* TODO only dirs and files supported for now */
        return ISO_ERROR;
    }

    return ISO_SUCCESS;
}

/**
 * Computes the block where the given offset should start.
 */
static
uint32_t block_from_offset(int nsections, struct iso_file_section *sections,
                           off_t offset)
{
    int section = 0;
    off_t bytes = 0;

    do {
        if ( (offset - bytes) < (off_t) sections[section].size ) {
            return sections[section].block + (offset - bytes) / BLOCK_SIZE;
        } else {
            bytes += (off_t) sections[section].size;
            section++;
        }

    } while(section < nsections);
    return 0; /* should never happen */
}

/**
 * Get the size available for reading on the corresponding block
 */
static
uint32_t size_available(int nsections, struct iso_file_section *sections,
                           off_t offset)
{
    int section = 0;
    off_t bytes = 0;

    do {
        if ( (offset - bytes) < (off_t) sections[section].size ) {
            uint32_t curr_section_offset = (uint32_t)(offset - bytes);
            uint32_t curr_section_left = sections[section].size - curr_section_offset;
            uint32_t available = BLOCK_SIZE - curr_section_offset % BLOCK_SIZE;
            return MIN(curr_section_left, available);
        } else {
            bytes += (off_t) sections[section].size;
            section++;
        }

    } while(section < nsections);
    return 0; /* should never happen */
}

/**
 * Get the block offset for reading the given file offset
 */
static
uint32_t block_offset(int nsections, struct iso_file_section *sections,
                      off_t offset)
{
    int section = 0;
    off_t bytes = 0;


    do {
        if ( (offset - bytes) < (off_t) sections[section].size ) {
            return (uint32_t)(offset - bytes) % BLOCK_SIZE;
        } else {
            bytes += (off_t) sections[section].size;
            section++;
        }

    } while(section < nsections);
    return 0; /* should never happen */
}

/**
 * Attempts to read up to count bytes from the given source into
 * the buffer starting at buf.
 *
 * The file src must be open() before calling this, and close() when no
 * more needed. Not valid for dirs. On symlinks it reads the destination
 * file.
 *
 * @return
 *     number of bytes read, 0 if EOF, < 0 on error
 *      Error codes:
 *         ISO_FILE_ERROR
 *         ISO_NULL_POINTER
 *         ISO_FILE_NOT_OPENED
 *         ISO_FILE_IS_DIR
 *         ISO_OUT_OF_MEM
 *         ISO_INTERRUPTED
 */
static
int ifs_read(IsoFileSource *src, void *buf, size_t count)
{
    int ret;
    ImageFileSourceData *data;
    uint32_t read = 0;

    if (src == NULL || src->data == NULL || buf == NULL) {
        return ISO_NULL_POINTER;
    }
    if (count == 0) {
        return ISO_WRONG_ARG_VALUE;
    }
    data = (ImageFileSourceData*)src->data;

    if (!data->opened) {
        return ISO_FILE_NOT_OPENED;
    } else if (data->opened != 1) {
        return ISO_FILE_IS_DIR;
    }

    while (read < count && data->data.offset < data->info.st_size) {
        size_t bytes;
        uint8_t *orig;

        if (block_offset(data->nsections, data->sections, data->data.offset) == 0) {
            /* we need to buffer next block */
            uint32_t block;
            _ImageFsData *fsdata;

            if (data->data.offset >= data->info.st_size) {
                /* EOF */
                break;
            }
            fsdata = data->fs->data;
            block = block_from_offset(data->nsections, data->sections,
                                      data->data.offset);
            ret = fsdata->src->read_block(fsdata->src, block,
                                          data->data.content);
            if (ret < 0) {
                return ret;
            }
        }

        /* how much can I read */
        bytes = MIN(size_available(data->nsections, data->sections, data->data.offset),
                    count - read);
        if (data->data.offset + (off_t)bytes > data->info.st_size) {
             bytes = data->info.st_size - data->data.offset;
        }
        orig = data->data.content;
        orig += block_offset(data->nsections, data->sections, data->data.offset);
        memcpy((uint8_t*)buf + read, orig, bytes);
        read += bytes;
        data->data.offset += (off_t)bytes;
    }
    return read;
}

static
off_t ifs_lseek(IsoFileSource *src, off_t offset, int flag)
{
    ImageFileSourceData *data;

    if (src == NULL) {
        return (off_t)ISO_NULL_POINTER;
    }
    if (offset < (off_t)0) {
        return (off_t)ISO_WRONG_ARG_VALUE;
    }

    data = src->data;

    if (!data->opened) {
        return (off_t)ISO_FILE_NOT_OPENED;
    } else if (data->opened != 1) {
        return (off_t)ISO_FILE_IS_DIR;
    }

    switch (flag) {
    case 0: /* SEEK_SET */
        data->data.offset = offset;
        break;
    case 1: /* SEEK_CUR */
        data->data.offset += offset;
        break;
    case 2: /* SEEK_END */
        /* do this make sense? */
        data->data.offset = data->info.st_size + offset;
        break;
    default:
        return (off_t)ISO_WRONG_ARG_VALUE;
    }

    /*
     * We check for block_offset != 0 because if it is already 0, the block
     * will be read from image in the read function
     */
    if (block_offset(data->nsections, data->sections, data->data.offset) != 0) {
        /* we need to buffer the block */
        uint32_t block;
        _ImageFsData *fsdata;

        if (data->data.offset < data->info.st_size) {
            int ret;
            fsdata = data->fs->data;
            block = block_from_offset(data->nsections, data->sections,
                                      data->data.offset);
            ret = fsdata->src->read_block(fsdata->src, block,
                                          data->data.content);
            if (ret < 0) {
                return (off_t)ret;
            }
        }
    }
    return data->data.offset;
}

static
int ifs_readdir(IsoFileSource *src, IsoFileSource **child)
{
    ImageFileSourceData *data, *cdata;
    struct child_list *children;

    if (src == NULL || src->data == NULL || child == NULL) {
        return ISO_NULL_POINTER;
    }
    data = (ImageFileSourceData*)src->data;

    if (!data->opened) {
        return ISO_FILE_NOT_OPENED;
    } else if (data->opened != 2) {
        return ISO_FILE_IS_NOT_DIR;
    }

    /* return the first child and free it */
    if (data->data.content == NULL) {
        return 0; /* EOF */
    }

    children = (struct child_list*)data->data.content;
    *child = children->file;
    cdata = (ImageFileSourceData*)(*child)->data;

    /* set the ref to the parent */
    cdata->parent = src;
    iso_file_source_ref(src);

    /* free the first element of the list */
    data->data.content = children->next;
    free(children);

    return ISO_SUCCESS;
}

/**
 * Read the destination of a symlink. You don't need to open the file
 * to call this.
 *
 * @param buf
 *     allocated buffer of at least bufsiz bytes.
 *     The dest. will be copied there, and it will be NULL-terminated
 * @param bufsiz
 *     characters to be copied. Destination link will be truncated if
 *     it is larger than given size. This include the \0 character.
 * @return
 *     1 on success, < 0 on error
 *      Error codes:
 *         ISO_FILE_ERROR
 *         ISO_NULL_POINTER
 *         ISO_WRONG_ARG_VALUE -> if bufsiz <= 0
 *         ISO_FILE_IS_NOT_SYMLINK
 *         ISO_OUT_OF_MEM
 *         ISO_FILE_BAD_PATH
 *         ISO_FILE_DOESNT_EXIST
 *
 */
static
int ifs_readlink(IsoFileSource *src, char *buf, size_t bufsiz)
{
    char *dest;
    size_t len;
    int ret;
    ImageFileSourceData *data;

    if (src == NULL || buf == NULL || src->data == NULL) {
        return ISO_NULL_POINTER;
    }

    if (bufsiz <= 0) {
        return ISO_WRONG_ARG_VALUE;
    }

    data = (ImageFileSourceData*)src->data;

    if (!S_ISLNK(data->info.st_mode)) {
        return ISO_FILE_IS_NOT_SYMLINK;
    }

    dest = (char*)data->data.content;
    len = strlen(dest);

    ret = ISO_SUCCESS;
    if (len >= bufsiz) {
        ret = ISO_RR_PATH_TOO_LONG;
        len = bufsiz - 1;
    }
    strncpy(buf, dest, len);
    buf[len] = '\0';
    return ret;
}

static
IsoFilesystem* ifs_get_filesystem(IsoFileSource *src)
{
    ImageFileSourceData *data;

    if (src == NULL) {
        return NULL;
    }

    data = src->data;
    return data->fs;
}

static
void ifs_free(IsoFileSource *src)
{
    ImageFileSourceData *data;

    data = src->data;

    /* close the file if it is already openned */
    if (data->opened) {
        src->class->close(src);
    }

    /* free destination if it is a link */
    if (S_ISLNK(data->info.st_mode)) {
        free(data->data.content);
    }
    iso_filesystem_unref(data->fs);
    if (data->parent != NULL) {
        iso_file_source_unref(data->parent);
    }

    free(data->sections);
    free(data->name);
    if (data->aa_string != NULL)
        free(data->aa_string);
    free(data);
}


static
int ifs_get_aa_string(IsoFileSource *src, unsigned char **aa_string, int flag)
{
    size_t len;
    ImageFileSourceData *data;

    data = src->data;

    if ((flag & 1) || data->aa_string == NULL) {
        *aa_string = data->aa_string;
        data->aa_string = NULL;
    } else {
        len = aaip_count_bytes(data->aa_string, 0);
        *aa_string = calloc(len, 1);
        if (*aa_string == NULL)
            return ISO_OUT_OF_MEM;
        memcpy(*aa_string, data->aa_string, len);
    }
    return 1;
}

static
int ifs_clone_src(IsoFileSource *old_source,
                  IsoFileSource **new_source, int flag)
{
    IsoFileSource *src = NULL;
    ImageFileSourceData *old_data, *new_data = NULL;
    char *new_name = NULL;
    struct iso_file_section *new_sections = NULL;
    void *new_aa_string = NULL;
    int i, ret;

    if (flag)
        return ISO_STREAM_NO_CLONE; /* unknown option required */

    old_data = (ImageFileSourceData *) old_source->data;
    *new_source = NULL; 
    src = calloc(1, sizeof(IsoFileSource));
    if (src == NULL)
        goto no_mem;
    new_name = strdup(old_data->name);
    if (new_name == NULL)
        goto no_mem;
    new_data = calloc(1, sizeof(ImageFileSourceData));

    if (new_data == NULL)
        goto no_mem;
    if (old_data->nsections > 0) {
        new_sections = calloc(old_data->nsections,
                              sizeof(struct iso_file_section));
        if (new_sections == NULL)
            goto no_mem;
    }
    ret = aaip_xinfo_cloner(old_data->aa_string, &new_aa_string, 0);
    if (ret < 0)
        goto no_mem;

    new_data->fs = old_data->fs;

    new_data->parent = old_data->parent;

    memcpy(&(new_data->info), &(old_data->info), sizeof(struct stat));
    new_data->name = new_name;
    new_data->sections = new_sections;
    new_data->nsections = old_data->nsections;
    for (i = 0; i < new_data->nsections; i++) 
        memcpy(new_data->sections + i, old_data->sections + i,
               sizeof(struct iso_file_section));
    new_data->opened = old_data->opened;
#ifdef Libisofs_with_zliB
    new_data->header_size_div4 = old_data->header_size_div4;
    new_data->block_size_log2 = old_data->block_size_log2;
    new_data->uncompressed_size = old_data->uncompressed_size;
#endif
    new_data->data.content = NULL;
    new_data->aa_string = (unsigned char *) new_aa_string;
    
    src->class = old_source->class;
    src->refcount = 1;
    src->data = new_data;
    *new_source = src;
    iso_file_source_ref(new_data->parent);
    iso_filesystem_ref(new_data->fs);
    return ISO_SUCCESS;
no_mem:;
    if (src != NULL)
        free((char *) src);
    if (new_data != NULL)
        free((char *) new_data);
    if (new_name != NULL)
        free(new_name);
    if (new_sections != NULL)
        free((char *) new_sections);
    if (new_aa_string != NULL)
        aaip_xinfo_func(new_aa_string, 1);
    return ISO_OUT_OF_MEM;
}


IsoFileSourceIface ifs_class = {

    2, /* version */
    ifs_get_path,
    ifs_get_name,
    ifs_lstat,
    ifs_stat,
    ifs_access,
    ifs_open,
    ifs_close,
    ifs_read,
    ifs_readdir,
    ifs_readlink,
    ifs_get_filesystem,
    ifs_free,
    ifs_lseek,
    ifs_get_aa_string,
    ifs_clone_src

};


/* Used from libisofs/stream.c : iso_stream_get_src_zf() */
int iso_ifs_source_get_zf(IsoFileSource *src, int *header_size_div4,
                          int *block_size_log2, uint32_t *uncompressed_size,
                          int flag)
{

#ifdef Libisofs_with_zliB

    ImageFileSourceData *data;

    if (src->class != &ifs_class)
        return 0;
    data = src->data;
    *header_size_div4 = data->header_size_div4;
    *block_size_log2 = data->block_size_log2;
    *uncompressed_size = data->uncompressed_size;
    return 1;

#else

    return 0;

#endif /* ! Libisofs_with_zliB */
}     


static
int make_hopefully_unique_name(_ImageFsData *fsdata,
                               char *str, size_t len, char **name)
{
    int ret, name_len, i;
    char c, *smashed = NULL, md5[16];
    void *md5_context = NULL;

    /* Shorten so that 32 characters of MD5 fit.
       If shorter than 8, pad up to 8 by '_'.
       Smash characters to [0-9A-Za-z_.].
       Append MD5 of original str as hex digits.
    */
    name_len = len > 223 ? 223 : len;
    LIBISO_ALLOC_MEM(smashed, char, (name_len >= 8 ? name_len : 8) + 32 + 1);
    memcpy(smashed, str, name_len);
    for (; name_len < 8; name_len++)
        smashed[name_len] = '_';
    smashed[name_len] = 0;
    for (i = 0; i < name_len; i++) {
        c = smashed[i];
        if (c == '.' || (c >= '0' && c <= '9') ||
            c == '_' || (c >= 'a' && c <= 'z'))
    continue;
        smashed[i] = '_';
    }
    ret = iso_md5_start(&md5_context);
    if (ret != 1)
        goto ex;
    ret = iso_md5_compute(md5_context, str, len);
    if (ret != 1)
        goto ex;
    ret = iso_md5_end(&md5_context, md5);
    if (ret != 1)
        goto ex;
    for (i = 0; i < 16; i++)
        sprintf(smashed + i * 2 + name_len, "%2.2x",
                                            ((unsigned char *) md5)[i]);
    name_len += 32;
    smashed[name_len] = 0;
    *name = smashed; smashed = NULL;
 
    ret = ISO_SUCCESS;
ex:
    LIBISO_FREE_MEM(smashed);
    if (md5_context != NULL)
        iso_md5_end(&md5_context, md5);
    return ret;
}


/**
 * Read a file name from a directory record, doing the needed charset
 * conversion
 */
static
char *get_name(_ImageFsData *fsdata, char *str, size_t len)
{
    int ret;
    char *name = NULL, *from_ucs = NULL;

    if (strcmp(fsdata->local_charset, fsdata->input_charset)) {
        /* charset conversion needed */
        ret = strnconv(str, fsdata->input_charset, fsdata->local_charset, len,
                       &name);
        if (ret == 1) {
            if (fsdata->iso_root_block == fsdata->svd_root_block) {
                /* Reading from Joliet : Check whether UTF-16 was needed */
                ret = strnconv(str, "UCS-2BE", fsdata->local_charset,
                               len, &from_ucs);
                if (ret == 1)
                    ret = (strcmp(name, from_ucs) == 0);
                if (ret != 1) {
                    fsdata->joliet_ucs2_failures++;
                    if (fsdata->joliet_ucs2_failures <=
                                                     ISO_JOLIET_UCS2_WARN_MAX)
                        iso_msg_submit(-1, ISO_NAME_NOT_UCS2, 0,
               "Joliet filename valid only with character set UTF-16 : \"%s\"",
                                       name);

                }
                if (from_ucs != NULL)
                    free(from_ucs);
            }
            return name;
        } else {
            ret = iso_msg_submit(fsdata->msgid, ISO_FILENAME_WRONG_CHARSET, ret,
                "Cannot convert from charset %s to %s",
                fsdata->input_charset, fsdata->local_charset);
            if (ret < 0) {
                return NULL; /* aborted */
            }
            /* fallback */
            ret = make_hopefully_unique_name(fsdata, str, len, &name);
            if (ret == ISO_SUCCESS)
                return name;
            return NULL;
        }
    }

    /* we reach here when the charset conversion is not needed */

    name = malloc(len + 1);
    if (name == NULL) {
        return NULL;
    }
    memcpy(name, str, len);
    name[len] = '\0';
    return name;
}


static
int iso_rr_msg_submit(_ImageFsData *fsdata, int rr_err_bit,
                      int errcode, int causedby, const char *msg)
{
    int ret;

    if ((fsdata->rr_err_reported & (1 << rr_err_bit)) &&
        (fsdata->rr_err_repeated & (1 << rr_err_bit))) {
        if (iso_msg_is_abort(errcode))
            return ISO_CANCELED;
        return 0;
    }
    if (fsdata->rr_err_reported & (1 << rr_err_bit)) {
        ret = iso_msg_submit(fsdata->msgid, errcode, causedby,
                             "MORE THAN ONCE : %s", msg);
        fsdata->rr_err_repeated |= (1 << rr_err_bit);
    } else {
        ret = iso_msg_submit(fsdata->msgid, errcode, causedby, "%s", msg);
        fsdata->rr_err_reported |= (1 << rr_err_bit);
    }
    return ret;
}


/**
 *
 * @param src
 *      if not-NULL, it points to a multi-extent file returned by a previous
 *      call to this function.
 * @param flag
 *      bit0= this is the root node attribute load call
 *            (parameter parent is not reliable for this)
 * @return
 *      2 node is still incomplete (multi-extent)
 *      1 success, 0 record ignored (not an error, can be a relocated dir),
 *      < 0 error
 */
static
int iso_file_source_new_ifs(IsoImageFilesystem *fs, IsoFileSource *parent,
                            struct ecma119_dir_record *record,
                            IsoFileSource **src, int flag)
{
    int ret, ecma119_map, skip_nm = 0;
    struct stat atts;
    time_t recorded;
    _ImageFsData *fsdata;
    IsoFileSource *ifsrc = NULL;
    ImageFileSourceData *ifsdata = NULL;

    int namecont = 0; /* 1 if found a NM with CONTINUE flag */
    char *name = NULL;

    /* 1 if found a SL with CONTINUE flag,
     * 2 if found a component with continue flag */
    int linkdestcont = 0;
    char *linkdest = NULL;

    uint32_t relocated_dir = 0;

    unsigned char *aa_string = NULL;
    size_t aa_size = 0, aa_len = 0, prev_field = 0;
    int aa_done = 0;
    char *msg = NULL;
    uint8_t *buffer = NULL;
    char *cpt;

    int has_px = 0;

#ifdef Libisofs_with_zliB
    uint8_t zisofs_alg[2], zisofs_hs4 = 0, zisofs_bsl2 = 0;
    uint32_t zisofs_usize = 0;
#endif

    if (fs == NULL || fs->data == NULL || record == NULL || src == NULL) {
        ret = ISO_NULL_POINTER; goto ex;
    }

    fsdata = (_ImageFsData*)fs->data;

    memset(&atts, 0, sizeof(struct stat));
    atts.st_nlink = 1;

    /* Set preliminary file type */
    if (record->flags[0] & 0x02) {
        atts.st_mode = S_IFDIR;
    } else {
        atts.st_mode = S_IFREG;
    }

    /*
     * First of all, check for unsupported ECMA-119 features
     */

    /* check for unsupported interleaved mode */
    if (record->file_unit_size[0] || record->interleave_gap_size[0]) {
        iso_msg_submit(fsdata->msgid, ISO_UNSUPPORTED_ECMA119, 0,
              "Unsupported image. This image has at least one file recorded "
              "in interleaved mode. We do not support this mode, as we think "
              "it is not used. If you are reading this, then we are wrong :) "
              "Please contact libisofs developers, so we can fix this.");
        {ret = ISO_UNSUPPORTED_ECMA119; goto ex;}
    }

    /* TODO #00013 : check for unsupported flags when reading a dir record */

    /*
     * If src is not-NULL, it refers to more extents of this file. We ensure
     * name matches, otherwise it means we are dealing with wrong image
     */
    if (*src != NULL) {
        ImageFileSourceData* data = (*src)->data;
        char* new_name = get_name(fsdata, (char*)record->file_id, record->len_fi[0]);
        if (new_name == NULL) {
            iso_msg_submit(fsdata->msgid, ISO_WRONG_ECMA119, 0,
                          "Cannot retrieve file name");
            {ret = ISO_WRONG_ECMA119; goto ex;}
        }
        if (strcmp(new_name, data->name)) {
            iso_msg_submit(fsdata->msgid, ISO_WRONG_ECMA119, 0,
                          "Multi-extent file lacks last entry.");
            free(new_name);
            {ret = ISO_WRONG_ECMA119; goto ex;}
        }
        free(new_name);
    }

    /* check for multi-extent */
    if (record->flags[0] & 0x80) {
        iso_msg_debug(fsdata->msgid, "Found multi-extent file");

        /*
         * Directory entries can only have one section (ECMA-119, 6.8.1)
         */
        if ((record->flags[0] & 0x02) || (flag & 1)) {
            iso_msg_submit(fsdata->msgid, ISO_WRONG_ECMA119, 0,
                          "Directories with more than one section are not allowed.");
            {ret = ISO_WRONG_ECMA119; goto ex;}
        }

        if (*src == NULL) {
            ifsdata = calloc(1, sizeof(ImageFileSourceData));
            if (ifsdata == NULL) {
                ret = ISO_OUT_OF_MEM;
                goto ifs_cleanup;
            }
            ifsrc = calloc(1, sizeof(IsoFileSource));
            if (ifsrc == NULL) {
                ret = ISO_OUT_OF_MEM;
                goto ifs_cleanup;
            }
            ifsrc->data = ifsdata;
            ifsdata->name = get_name(fsdata, (char*)record->file_id, record->len_fi[0]);
            if (ifsdata->name == NULL) {
                iso_msg_submit(fsdata->msgid, ISO_WRONG_ECMA119, 0,
                              "Cannot retrieve file name");
                ret = ISO_WRONG_ECMA119;
                goto ifs_cleanup;
            }

            *src = ifsrc;
        } else {
            ifsdata = (*src)->data;
        }

        /* store current extent */
        ifsdata->sections = realloc(ifsdata->sections,
                    (1 + ifsdata->nsections) * sizeof(struct iso_file_section));
        if (ifsdata->sections == NULL) {
            free(ifsdata->name);
            ret = ISO_OUT_OF_MEM;
            goto ifs_cleanup;
        }
        ifsdata->sections[ifsdata->nsections].block =
                       iso_read_bb(record->block, 4, NULL) + record->len_xa[0];
        ifsdata->sections[ifsdata->nsections].size = iso_read_bb(record->length, 4, NULL);

        ifsdata->info.st_size += (off_t) ifsdata->sections[ifsdata->nsections].size;
        ifsdata->nsections++;
        {ret = 2; goto ex;}
    }

    /*
     * The idea is to read all the RR entries (if we want to do that and RR
     * extensions exist on image), storing the info we want from that.
     * Then, we need some sanity checks.
     * Finally, we select what kind of node it is, and set values properly.
     */

    if (fsdata->rr) {
        struct susp_sys_user_entry *sue;
        SuspIterator *iter;


        iter = susp_iter_new(fsdata->src, record, fsdata->len_skp,
                             fsdata->msgid);
        if (iter == NULL) {
            {ret = ISO_OUT_OF_MEM; goto ex;}
        }

        while ((ret = susp_iter_next(iter, &sue)) > 0) {

            /* ignore entries from different version */
            if (sue->version[0] != 1)
                continue;

            if (SUSP_SIG(sue, 'P', 'X')) {
                has_px = 1;
                ret = read_rr_PX(sue, &atts);
                if (ret < 0) {
                    /* notify and continue */
                    ret = iso_rr_msg_submit(fsdata, 0, ISO_WRONG_RR_WARN, ret,
                                            "Invalid PX entry");
                    fsdata->px_ino_status |= 8;
                } if (ret == 2) {
                    if (fsdata->inode_counter < atts.st_ino) 
                        fsdata->inode_counter = atts.st_ino;
                    fsdata->px_ino_status |= 1;

                } else {
                    fsdata->px_ino_status |= 2;
                }

            } else if (SUSP_SIG(sue, 'T', 'F')) {
                ret = read_rr_TF(sue, &atts);
                if (ret < 0) {
                    /* notify and continue */
                    ret = iso_rr_msg_submit(fsdata, 1, ISO_WRONG_RR_WARN, ret,
                                            "Invalid TF entry");
                }
            } else if (SUSP_SIG(sue, 'N', 'M')) {
                if (skip_nm)
                    continue; /* in NM error bailout mode */

                if (name != NULL && namecont == 0) {
                    /* ups, RR standard violation */
                    ret = iso_rr_msg_submit(fsdata, 2, ISO_WRONG_RR_WARN, 0,
                                 "New NM entry found without previous"
                                 "CONTINUE flag. Ignored");
                    skip_nm = 1;
                    continue;
                }
                ret = read_rr_NM(sue, &name, &namecont);
                if (ret < 0) {
                    /* notify and continue */
                    ret = iso_rr_msg_submit(fsdata, 3, ISO_WRONG_RR_WARN, ret,
                                            "Invalid NM entry");
                    continue;
                }
                if (name != NULL) if (strlen(name) > 4095) {
                    /* Preliminarily truncate totally oversized name */
                    ret = iso_rr_msg_submit(fsdata, 3, ISO_WRONG_RR_WARN, ret,
                                            "Totally oversized NM list");
                    skip_nm = 1;
                    continue;
                }

#ifdef Libisofs_syslinux_tesT

if (name != NULL && !namecont) {
    struct device syslinux_dev;
    struct iso_sb_info syslinux_sbi;
    struct fs_info syslinux_fsi;
    char *syslinux_name = NULL;
    int syslinux_name_len;

    syslinux_dev.src = fsdata->src;
    memset(&(syslinux_sbi.root), 0, 256);
    syslinux_sbi.do_rr = 1;
    syslinux_sbi.susp_skip = 0;
    syslinux_fsi.fs_dev = &syslinux_dev;
    syslinux_fsi.fs_info = &syslinux_sbi;
    ret = susp_rr_get_nm(&syslinux_fsi, (char *) record,
                         &syslinux_name, &syslinux_name_len);
    if (ret == 1) {
        if (name == NULL || syslinux_name == NULL)
          fprintf(stderr, "################ Hoppla. NULL\n");
        else if(strcmp(syslinux_name, name) != 0)
          fprintf(stderr,
                  "################ libisofs '%s' != '%s' susp_rr_get_nm()\n",
                  name, syslinux_name);
    } else if (ret == 0) {
        fprintf(stderr,
                "################ '%s' not found by susp_rr_get_nm()\n", name);
    } else {
        fprintf(stderr, "################ 'susp_rr_get_nm() returned error\n");
    }
    if (syslinux_name != NULL)
        free(syslinux_name);

}

#endif /* Libisofs_syslinux_tesT */


            } else if (SUSP_SIG(sue, 'S', 'L')) {
                if (linkdest != NULL && linkdestcont == 0) {
                    /* ups, RR standard violation */
                    ret = iso_rr_msg_submit(fsdata, 4, ISO_WRONG_RR_WARN, 0,
                                 "New SL entry found without previous"
                                 "CONTINUE flag. Ignored");
                    continue;
                }
                ret = read_rr_SL(sue, &linkdest, &linkdestcont);
                if (ret < 0) {
                    /* notify and continue */
                    ret = iso_rr_msg_submit(fsdata, 5, ISO_WRONG_RR_WARN, ret,
                                            "Invalid SL entry");
                }
            } else if (SUSP_SIG(sue, 'R', 'E')) {
                /*
                 * this directory entry refers to a relocated directory.
                 * We simply ignore it, as it will be correctly handled
                 * when found the CL
                 */
                susp_iter_free(iter);
                free(name);
                {ret = 0; goto ex;} /* it's not an error */
            } else if (SUSP_SIG(sue, 'C', 'L')) {
                /*
                 * This entry is a placeholder for a relocated dir.
                 * We need to ignore other entries, with the exception of NM.
                 * Then we create a directory node that represents the
                 * relocated dir, and iterate over its children.
                 */
                relocated_dir = iso_read_bb(sue->data.CL.child_loc, 4, NULL);
                if (relocated_dir == 0) {
                    ret = iso_rr_msg_submit(fsdata, 6, ISO_WRONG_RR, 0,
                                  "Invalid SL entry, no child location");
                    break;
                }
            } else if (SUSP_SIG(sue, 'P', 'N')) {
                ret = read_rr_PN(sue, &atts);
                if (ret < 0) {
                    /* notify and continue */
                    ret = iso_rr_msg_submit(fsdata, 7, ISO_WRONG_RR_WARN, ret,
                                            "Invalid PN entry");
                }
            } else if (SUSP_SIG(sue, 'S', 'F')) {
                ret = iso_rr_msg_submit(fsdata, 8, ISO_UNSUPPORTED_RR, 0,
                                        "Sparse files not supported.");
                break;
            } else if (SUSP_SIG(sue, 'R', 'R')) {
                /* This was an optional flag byte in RRIP 1.09 which told the
                   reader what other RRIP fields to expect.
                   mkisofs emits it. We don't.
                */
                continue;
            } else if (SUSP_SIG(sue, 'S', 'P')) {
                /*
                 * Ignore this, to prevent the hint message, if we are dealing
                 * with root node (SP is only valid in "." of root node)
                 */
                if (!(flag & 1)) {
                    /* notify and continue */
                    ret = iso_rr_msg_submit(fsdata, 9, ISO_WRONG_RR, 0,
                                  "SP entry found in a directory entry other "
                                  "than '.' entry of root node");
                }
                continue;
            } else if (SUSP_SIG(sue, 'E', 'R')) {
                /*
                 * Ignore this, to prevent the hint message, if we are dealing
                 * with root node (ER is only valid in "." of root node)
                 */
                if (!(flag & 1)) {
                    /* notify and continue */
                    ret = iso_rr_msg_submit(fsdata, 10, ISO_WRONG_RR, 0,
                                  "ER entry found in a directory entry other "
                                  "than '.' entry of root node");
                }
                continue;

            /* Need to read AA resp. AL in any case so it is available for
               S_IRWXG mapping in case that fsdata->aaip_load != 1
             */
            } else if (SUSP_SIG(sue, 'A', 'A')) {

                ret = read_aaip_AA(sue, &aa_string, &aa_size, &aa_len,
                                   &prev_field, &aa_done, 0);
                if (ret < 0) {
                    /* notify and continue */
                    ret = iso_rr_msg_submit(fsdata, 11, ISO_WRONG_RR_WARN, ret,
                                            "Invalid AA entry");
                    continue;
                }

            } else if (SUSP_SIG(sue, 'A', 'L')) {

                ret = read_aaip_AL(sue, &aa_string, &aa_size, &aa_len,
                                   &prev_field, &aa_done, 0);
                if (ret < 0) {
                    /* notify and continue */
                    ret = iso_rr_msg_submit(fsdata, 12, ISO_WRONG_RR_WARN, ret,
                                            "Invalid AL entry");
                    continue;
                }

#ifdef Libisofs_with_zliB

            } else if (SUSP_SIG(sue, 'Z', 'F')) {

                ret = read_zisofs_ZF(sue, zisofs_alg, &zisofs_hs4,
                                     &zisofs_bsl2, &zisofs_usize, 0);
                if (ret < 0 || zisofs_alg[0] != 'p' || zisofs_alg[1] != 'z') {
                    /* notify and continue */
                    ret = iso_rr_msg_submit(fsdata, 13, ISO_WRONG_RR_WARN, ret,
                                 "Invalid ZF entry");
                    zisofs_hs4 = 0;
                    continue;
                }

#endif /* Libisofs_with_zliB */

/* This message is inflationary */
/*
            } else {
                ret = iso_msg_submit(fsdata->msgid, ISO_SUSP_UNHANDLED, 0,
                    "Unhandled SUSP entry %c%c.", sue->sig[0], sue->sig[1]);
*/

            }
        }

        susp_iter_free(iter);

        /* check for RR problems */

        if (ret < 0) {
            /* error was already submitted above */
            iso_msg_debug(fsdata->msgid, "Error parsing RR entries");
        } else if (!relocated_dir && atts.st_mode == (mode_t) 0 ) {
            ret = iso_rr_msg_submit(fsdata, 14, ISO_WRONG_RR, 0, "Mandatory "
                                 "Rock Ridge PX entry is not present or it "
                                 "contains invalid values.");
        } else {
            /* ensure both name and link dest are finished */
            if (namecont != 0) {
                ret = iso_rr_msg_submit(fsdata, 15, ISO_WRONG_RR, 0,
                        "Incomplete Rock Ridge name, last NM entry continues");
            }
            if (linkdestcont != 0) {
                ret = iso_rr_msg_submit(fsdata, 16, ISO_WRONG_RR, 0,
                    "Incomplete link destination, last SL entry continues");
            }
        }

        if (ret < 0) {
            free(name);
            goto ex;
        }

        /* convert name to needed charset */
        if (strcmp(fsdata->input_charset, fsdata->local_charset) && name) {
            /* we need to convert name charset */
            char *newname = NULL;
            ret = strconv(name, fsdata->input_charset, fsdata->local_charset,
                          &newname);
            if (ret < 0) {
                /* its just a hint message */
                LIBISO_FREE_MEM(msg);
                LIBISO_ALLOC_MEM(msg, char, 160);
                sprintf(msg,
                "Cannot convert from charset %.40s to %.40s",
                                 fsdata->input_charset, fsdata->local_charset);
                ret = iso_rr_msg_submit(fsdata, 17, ISO_FILENAME_WRONG_CHARSET,
                                        ret, msg);
                free(newname);
                if (ret < 0) {
                    free(name);
                    goto ex;
                }
            } else {
                free(name);
                name = newname;
            }
        }

        /* convert link destination to needed charset */
        if (strcmp(fsdata->input_charset, fsdata->local_charset) && linkdest) {
            /* we need to convert name charset */
            char *newlinkdest = NULL;
            ret = strconv(linkdest, fsdata->input_charset,
                          fsdata->local_charset, &newlinkdest);
            if (ret < 0) {
                LIBISO_FREE_MEM(msg);
                LIBISO_ALLOC_MEM(msg, char, 160);
                sprintf(msg,
                "Charset conversion error. Cannot convert from %.40s to %.40s",
                                 fsdata->input_charset, fsdata->local_charset);
                ret = iso_rr_msg_submit(fsdata, 17, ISO_FILENAME_WRONG_CHARSET,
                                     ret, msg);
                free(newlinkdest);
                if (ret < 0) {
                    free(name);
                    goto ex;
                }
            } else {
                free(linkdest);
                linkdest = newlinkdest;
            }
        }

    } else {
        /* RR extensions are not read / used */
        atts.st_gid = fsdata->gid;
        atts.st_uid = fsdata->uid;
        if (record->flags[0] & 0x02) {
            atts.st_mode = S_IFDIR | fsdata->dir_mode;
        } else {
            atts.st_mode = S_IFREG | fsdata->file_mode;
        }
    }

    if (!has_px) {
        fsdata->px_ino_status |= 4;
    }

    /*
     * if we haven't RR extensions, or no NM entry is present,
     * we use the name in directory record
     */
    if (!name) {
        size_t len;

        if (record->len_fi[0] == 1 && record->file_id[0] == 0) {
            /* "." entry, we can call this for root node, so... */
            if (!(atts.st_mode & S_IFDIR)) {
                ret = iso_msg_submit(fsdata->msgid, ISO_WRONG_ECMA119, 0,
                              "Wrong ISO file name. \".\" not dir");
                goto ex;
            }
        } else {

            name = get_name(fsdata, (char*)record->file_id, record->len_fi[0]);
            if (name == NULL) {
                ret = iso_msg_submit(fsdata->msgid, ISO_WRONG_ECMA119, 0,
                              "Cannot retrieve file name");
                goto ex;
            }

            /* remove trailing version number */
            len = strlen(name);
            ecma119_map = fsdata->ecma119_map;
            if (fsdata->iso_root_block == fsdata->svd_root_block)
                ecma119_map = 0;
            if (ecma119_map >= 1 && ecma119_map <= 3 &&
                len > 2 && name[len-2] == ';' && name[len-1] == '1') {
                if (len > 3 && name[len-3] == '.') {
                    /*
                     * the "." is mandatory, so in most cases is included only
                     * for standard compliance
                     */
                    name[len-3] = '\0';
                } else {
                    name[len-2] = '\0';
                }
            }

            if (ecma119_map == 2 || ecma119_map == 3) {
                for (cpt = name; *cpt != 0; cpt++) {
                    if (ecma119_map == 2) {
                        if (islower(*cpt))
                            *cpt = toupper(*cpt);
                    } else {
                        if (isupper(*cpt))
                            *cpt = tolower(*cpt);
                    }
                }
            }

        }
    }

    if (name != NULL) {
        if ((int) strlen(name) > fsdata->truncate_length) {
            ret = iso_truncate_rr_name(fsdata->truncate_mode,
                                       fsdata->truncate_length, name, 0);
            if (ret < 0)
                goto ex;
        }
    }

    if (relocated_dir) {

        /*
         * We are dealing with a placeholder for a relocated dir.
         * Thus, we need to read attributes for this directory from the "."
         * entry of the relocated dir.
         */

        LIBISO_ALLOC_MEM(buffer, uint8_t, BLOCK_SIZE);
        ret = fsdata->src->read_block(fsdata->src, relocated_dir, buffer);
        if (ret < 0) {
            goto ex;
        }

        ret = iso_file_source_new_ifs(fs, parent, (struct ecma119_dir_record*)
                                      buffer, src, 0);
        if (ret <= 0) {
            goto ex;
        }

        /* but the real name is the name of the placeholder */
        ifsdata = (ImageFileSourceData*) (*src)->data;
        ifsdata->name = name;

        {ret = ISO_SUCCESS; goto ex;}
    }

    /* Production of missing inode numbers is delayed until the image is
       complete. Then all nodes which shall get a new inode number will
       be served.
    */

    /*
     * if we haven't RR extensions, or a needed TF time stamp is not present,
     * we use plain iso recording time
     */
    recorded = iso_datetime_read_7(record->recording_time);
    if (atts.st_atime == (time_t) 0) {
        atts.st_atime = recorded;
    }
    if (atts.st_ctime == (time_t) 0) {
        atts.st_ctime = recorded;
    }
    if (atts.st_mtime == (time_t) 0) {
        atts.st_mtime = recorded;
    }

    /* the size is read from iso directory record */
    atts.st_size = iso_read_bb(record->length, 4, NULL);

    /* Fill last entries */
    atts.st_dev = fsdata->id;
    atts.st_blksize = BLOCK_SIZE;
    atts.st_blocks = DIV_UP(atts.st_size, BLOCK_SIZE);

    /* TODO #00014 : more sanity checks to ensure dir record info is valid */
    if (S_ISLNK(atts.st_mode) && (linkdest == NULL)) {
        ret = iso_rr_msg_submit(fsdata, 18, ISO_WRONG_RR, 0,
                                "Link without destination.");
        free(name);
        goto ex;
    }

    /* ok, we can now create the file source */
    if (*src == NULL) {
        ifsdata = calloc(1, sizeof(ImageFileSourceData));
        if (ifsdata == NULL) {
            ret = ISO_OUT_OF_MEM;
            goto ifs_cleanup;
        }
        ifsrc = calloc(1, sizeof(IsoFileSource));
        if (ifsrc == NULL) {
            ret = ISO_OUT_OF_MEM;
            goto ifs_cleanup;
        }
    } else {
        ifsdata = (*src)->data;
        ifsrc = (*src);
        free(ifsdata->name); /* we will assign a new one */
        ifsdata->name = NULL;
        atts.st_size += (off_t)ifsdata->info.st_size;
        if (ifsdata->aa_string != NULL)
            free(ifsdata->aa_string);
        ifsdata->aa_string = NULL;
    }

    /* fill data */
    ifsdata->fs = fs;
    iso_filesystem_ref(fs);
    if (parent != NULL) {
        ifsdata->parent = parent;
        iso_file_source_ref(parent);
    }
    ifsdata->info = atts;
    ifsdata->name = name;
    ifsdata->aa_string = aa_string;

#ifdef Libisofs_with_zliB
    if (zisofs_hs4 > 0) {
        ifsdata->header_size_div4 = zisofs_hs4;
        ifsdata->block_size_log2 = zisofs_bsl2;
        ifsdata->uncompressed_size = zisofs_usize;
    } else {
        ifsdata->header_size_div4 = 0;
    }
#endif

    /* save extents */
    ifsdata->sections = realloc(ifsdata->sections,
                (1 + ifsdata->nsections) * sizeof(struct iso_file_section));
    if (ifsdata->sections == NULL) {
        free(ifsdata->name);
        ret = ISO_OUT_OF_MEM;
        goto ifs_cleanup;
    }
    ifsdata->sections[ifsdata->nsections].block =
                       iso_read_bb(record->block, 4, NULL) + record->len_xa[0];
    ifsdata->sections[ifsdata->nsections].size = iso_read_bb(record->length, 4, NULL);
    ifsdata->nsections++;

    if (S_ISLNK(atts.st_mode)) {
        ifsdata->data.content = linkdest;
    }

    ifsrc->class = &ifs_class;
    ifsrc->data = ifsdata;
    ifsrc->refcount = 1;

    *src = ifsrc;
    {ret = ISO_SUCCESS; goto ex;}

ifs_cleanup: ;
    free(name);
    free(linkdest);
    free(ifsdata);
    free(ifsrc);

ex:;
    LIBISO_FREE_MEM(msg);
    LIBISO_FREE_MEM(buffer);
    return ret;
}

static
int ifs_get_root(IsoFilesystem *fs, IsoFileSource **root)
{
    int ret;
    _ImageFsData *data;
    uint8_t *buffer = NULL;

    if (fs == NULL || fs->data == NULL || root == NULL) {
        ret = ISO_NULL_POINTER; goto ex;
    }

    LIBISO_ALLOC_MEM(buffer, uint8_t, BLOCK_SIZE);
    data = (_ImageFsData*)fs->data;

    /* open the filesystem */
    ret = ifs_fs_open((IsoImageFilesystem*)fs);
    if (ret < 0) {
        goto ex;
    }

    /* read extend for root record */
    ret = data->src->read_block(data->src, data->iso_root_block, buffer);
    if (ret < 0) {
        ifs_fs_close((IsoImageFilesystem*)fs);
        goto ex;
    }

    /* get root attributes from "." entry */
    *root = NULL;
    ret = iso_file_source_new_ifs((IsoImageFilesystem*)fs, NULL,
                                 (struct ecma119_dir_record*) buffer, root, 1);

    ifs_fs_close((IsoImageFilesystem*)fs);
ex:;
    LIBISO_FREE_MEM(buffer);
    return ret;
}

/**
 * Find a file inside a node.
 *
 * @param file
 *     it is not modified if requested file is not found
 * @return
 *     1 success, 0 not found, < 0 error
 */
static
int ifs_get_file(IsoFileSource *dir, const char *name, IsoFileSource **file)
{
    int ret;
    IsoFileSource *src;

    ret = iso_file_source_open(dir);
    if (ret < 0) {
        return ret;
    }
    while ((ret = iso_file_source_readdir(dir, &src)) == 1) {
        char *fname = iso_file_source_get_name(src);
        if (!strcmp(name, fname)) {
            free(fname);
            *file = src;
            ret = ISO_SUCCESS;
            break;
        }
        free(fname);
        iso_file_source_unref(src);
    }
    iso_file_source_close(dir);
    return ret;
}

static
int ifs_get_by_path(IsoFilesystem *fs, const char *path, IsoFileSource **file)
{
    int ret;
    IsoFileSource *src = NULL;
    char *ptr, *brk_info, *component;

    if (fs == NULL || fs->data == NULL || path == NULL || file == NULL) {
        return ISO_NULL_POINTER;
    }

    if (path[0] != '/') {
        /* only absolute paths supported */
        return ISO_FILE_BAD_PATH;
    }

    /* open the filesystem */
    ret = ifs_fs_open((IsoImageFilesystem*)fs);
    if (ret < 0) {
        return ret;
    }

    ret = ifs_get_root(fs, &src);
    if (ret < 0) {
        return ret;
    }
    if (!strcmp(path, "/")) {
        /* we are looking for root */
        *file = src;
        ret = ISO_SUCCESS;
        goto get_path_exit;
    }

    ptr = strdup(path);
    if (ptr == NULL) {
        iso_file_source_unref(src);
        ret = ISO_OUT_OF_MEM;
        goto get_path_exit;
    }

    component = strtok_r(ptr, "/", &brk_info);
    while (component) {
        IsoFileSource *child = NULL;

        ImageFileSourceData *fdata;
        fdata = src->data;
        if (!S_ISDIR(fdata->info.st_mode)) {
            ret = ISO_FILE_BAD_PATH;
            break;
        }

        ret = ifs_get_file(src, component, &child);
        iso_file_source_unref(src);
        src = NULL;
        if (ret <= 0) {
            break;
        }

        src = child;
        component = strtok_r(NULL, "/", &brk_info);
    }

    free(ptr);
    if (ret < 0) {
        if (src != NULL)
            iso_file_source_unref(src);
    } else if (ret == 0) {
        ret = ISO_FILE_DOESNT_EXIST;
    } else {
        *file = src;
    }

    get_path_exit:;
    ifs_fs_close((IsoImageFilesystem*)fs);
    return ret;
}

unsigned int ifs_get_id(IsoFilesystem *fs)
{
    return ISO_IMAGE_FS_ID;
}

static
int ifs_fs_open(IsoImageFilesystem *fs)
{
    _ImageFsData *data;

    if (fs == NULL || fs->data == NULL) {
        return ISO_NULL_POINTER;
    }

    data = (_ImageFsData*)fs->data;

    if (data->open_count == 0) {
        /* we need to actually open the data source */
        int res = data->src->open(data->src);
        if (res < 0) {
            return res;
        }
    }
    ++data->open_count;
    return ISO_SUCCESS;
}

static
int ifs_fs_close(IsoImageFilesystem *fs)
{
    _ImageFsData *data;

    if (fs == NULL || fs->data == NULL) {
        return ISO_NULL_POINTER;
    }

    data = (_ImageFsData*)fs->data;

    if (--data->open_count == 0) {
        /* we need to actually close the data source */
        return data->src->close(data->src);
    }
    return ISO_SUCCESS;
}

static
void ifs_fs_free(IsoFilesystem *fs)
{
    _ImageFsData *data;

    data = (_ImageFsData*) fs->data;

    /* close data source if already openned */
    if (data->open_count > 0) {
        data->src->close(data->src);
    }

    /* free our ref to datasource */
    iso_data_source_unref(data->src);

    /* free volume atts */
    free(data->volset_id);
    free(data->volume_id);
    free(data->publisher_id);
    free(data->data_preparer_id);
    free(data->system_id);
    free(data->application_id);
    free(data->copyright_file_id);
    free(data->abstract_file_id);
    free(data->biblio_file_id);
    free(data->creation_time);
    free(data->modification_time);
    free(data->expiration_time);
    free(data->effective_time);
    free(data->input_charset);
    free(data->local_charset);

    if(data->catcontent != NULL)
        free(data->catcontent);

    free(data);
}

/**
 * Read the SUSP system user entries of the "." entry of the root directory,
 * indentifying when Rock Ridge extensions are being used.
 *
 * @return
 *      1 success, 0 ignored, < 0 error
 */
static
int read_root_susp_entries(_ImageFsData *data, uint32_t block)
{
    int ret;
    unsigned char *buffer = NULL;
    struct ecma119_dir_record *record;
    struct susp_sys_user_entry *sue;
    SuspIterator *iter;

    LIBISO_ALLOC_MEM(buffer, unsigned char, 2048);
    ret = data->src->read_block(data->src, block, buffer);
    if (ret < 0) {
        goto ex;
    }

    /* record will be the "." directory entry for the root record */
    record = (struct ecma119_dir_record *)buffer;

#ifdef Libisofs_syslinux_tesT

{
    struct device syslinux_dev;
    struct iso_sb_info syslinux_sbi;
    struct fs_info syslinux_fsi;

    syslinux_dev.src = data->src;
    memcpy(&(syslinux_sbi.root), (char *) record, 256);
    syslinux_sbi.do_rr = 1;
    syslinux_sbi.susp_skip = 0;
    syslinux_fsi.fs_dev = &syslinux_dev;
    syslinux_fsi.fs_info = &syslinux_sbi;
    
    ret = susp_rr_check_signatures(&syslinux_fsi, 1);
    fprintf(stderr, "--------- susp_rr_check_signatures == %d , syslinux_sbi.do_rr == %d\n", ret, syslinux_sbi.do_rr);
}
    
#endif /* Libisofs_syslinux_tesT */
    

    /*
     * TODO #00015 : take care of CD-ROM XA discs when reading SP entry
     * SUSP specification claims that for CD-ROM XA the SP entry
     * is not at position BP 1, but at BP 15. Is that used?
     * In that case, we need to set info->len_skp to 15!!
     */

    iter = susp_iter_new(data->src, record, data->len_skp, data->msgid);
    if (iter == NULL) {
        ret = ISO_OUT_OF_MEM; goto ex;
    }

    /* first entry must be an SP system use entry */
    ret = susp_iter_next(iter, &sue);
    if (ret < 0) {
        /* error */
        susp_iter_free(iter);
        goto ex;
    } else if (ret == 0 || !SUSP_SIG(sue, 'S', 'P') ) {
        iso_msg_debug(data->msgid, "SUSP/RR is not being used.");
        susp_iter_free(iter);
        {ret = ISO_SUCCESS; goto ex;}
    }

    /* it is a SP system use entry */
    if (sue->version[0] != 1 || sue->data.SP.be[0] != 0xBE
        || sue->data.SP.ef[0] != 0xEF) {

        susp_iter_free(iter);
        ret = iso_msg_submit(data->msgid, ISO_UNSUPPORTED_SUSP, 0,
                              "SUSP SP system use entry seems to be wrong. "
                              "Ignoring Rock Ridge Extensions.");
        goto ex;
    }

    iso_msg_debug(data->msgid, "SUSP/RR is being used.");

    /*
     * The LEN_SKP field, defined in IEEE 1281, SUSP. 5.3, specifies the
     * number of bytes to be skipped within each System Use field.
     * I think this will be always 0, but given that support this standard
     * feature is easy...
     */
    data->len_skp = sue->data.SP.len_skp[0];

    /*
     * Ok, now search for ER entry.
     * Just notice that the attributes for root dir are read elsewhere.
     *
     * TODO #00016 : handle non RR ER entries
     *
     * if several ER are present, we need to identify the position of
     * what refers to RR, and then look for corresponding ES entry in
     * each directory record. I have not implemented this (it's not used,
     * no?), but if we finally need it, it can be easily implemented in
     * the iterator, transparently for the rest of the code.
     */
    while ((ret = susp_iter_next(iter, &sue)) > 0) {

        /* ignore entries from different version */
        if (sue->version[0] != 1)
            continue;

        if (SUSP_SIG(sue, 'E', 'R')) {
            /*
             * it seems that Rock Ridge can be identified with any
             * of the following
             */
            if ( sue->data.ER.len_id[0] == 10 &&
                 !strncmp((char*)sue->data.ER.ext_id, "RRIP_1991A", 10) ) {

                iso_msg_debug(data->msgid,
                              "Suitable Rock Ridge ER found. Version 1.10.");
                data->rr_version = RR_EXT_110;

            } else if ( (sue->data.ER.len_id[0] == 10 &&
                    !strncmp((char*)sue->data.ER.ext_id, "IEEE_P1282", 10))
                 || (sue->data.ER.len_id[0] == 9 &&
                    !strncmp((char*)sue->data.ER.ext_id, "IEEE_1282", 9)) ) {

                iso_msg_debug(data->msgid,
                              "Suitable Rock Ridge ER found. Version 1.12.");
                data->rr_version = RR_EXT_112;

            } else if (sue->data.ER.len_id[0] == 9 &&
                  (strncmp((char*)sue->data.ER.ext_id, "AAIP_0002", 9) == 0 ||
                   strncmp((char*)sue->data.ER.ext_id, "AAIP_0100", 9) == 0 ||
                   strncmp((char*)sue->data.ER.ext_id, "AAIP_0200", 9) == 0)) {

                /* Tolerate AAIP ER even if not supported */
                iso_msg_debug(data->msgid, "Suitable AAIP ER found.");

                if (strncmp((char*)sue->data.ER.ext_id, "AAIP_0200", 9) == 0)
                    data->aaip_version = 200;
                else if (((char*)sue->data.ER.ext_id)[6] == '1')
                    data->aaip_version = 100;
                else
                    data->aaip_version = 2;
                if (!data->aaip_load)
                    iso_msg_submit(data->msgid, ISO_AAIP_IGNORED, 0,
           "Identifier for extension AAIP found, but loading is not enabled.");

            } else {
                ret = iso_msg_submit(data->msgid, ISO_SUSP_MULTIPLE_ER, 0,
                    "Unknown Extension Signature found in ER.\n"
                    "It will be ignored, but can cause problems in "
                    "image reading. Please notify us about this.");
                if (ret < 0) {
                    break;
                }
            }
        }
    }

    susp_iter_free(iter);

    if (ret < 0) {
        goto ex;
    }

    ret = ISO_SUCCESS;
ex:
    LIBISO_FREE_MEM(buffer);
    return ret;
}

static
int read_pvd_block(IsoDataSource *src, uint32_t block, uint8_t *buffer,
                   uint32_t *image_size)
{
    int ret;
    struct ecma119_pri_vol_desc *pvm;

    ret = src->read_block(src, block, buffer);
    if (ret < 0)
        return ret;
    pvm = (struct ecma119_pri_vol_desc *)buffer;

    /* sanity checks */
    if (pvm->vol_desc_type[0] != 1 || pvm->vol_desc_version[0] != 1
            || strncmp((char*)pvm->std_identifier, "CD001", 5)
            || pvm->file_structure_version[0] != 1) {

        return ISO_WRONG_PVD;
    }
    if (image_size != NULL)
        *image_size = iso_read_bb(pvm->vol_space_size, 4, NULL);
    return ISO_SUCCESS;
}

static
int read_pvm(_ImageFsData *data, uint32_t block)
{
    int ret;
    struct ecma119_pri_vol_desc *pvm;
    struct ecma119_dir_record *rootdr;
    uint8_t *buffer = NULL;

    LIBISO_ALLOC_MEM(buffer, uint8_t, BLOCK_SIZE);
    ret = read_pvd_block(data->src, block, buffer, NULL);
    if (ret < 0)
        goto ex;
    /* ok, it is a valid PVD */
    pvm = (struct ecma119_pri_vol_desc *)buffer;

    /* fill volume attributes  */
    /* TODO take care of input charset */
    data->volset_id = iso_util_strcopy_untail((char*)pvm->vol_set_id, 128);
    data->volume_id = iso_util_strcopy_untail((char*)pvm->volume_id, 32);
    data->publisher_id =
               iso_util_strcopy_untail((char*)pvm->publisher_id, 128);
    data->data_preparer_id =
               iso_util_strcopy_untail((char*)pvm->data_prep_id, 128);
    data->system_id = iso_util_strcopy_untail((char*)pvm->system_id, 32);
    data->application_id =
               iso_util_strcopy_untail((char*)pvm->application_id, 128);
    data->copyright_file_id =
               iso_util_strcopy_untail((char*) pvm->copyright_file_id, 37);
    data->abstract_file_id =
               iso_util_strcopy_untail((char*) pvm->abstract_file_id, 37);
    data->biblio_file_id =
               iso_util_strcopy_untail((char*) pvm->bibliographic_file_id, 37);
    if (data->copyright_file_id[0] == '_' && data->copyright_file_id[1] == 0 &&
        data->abstract_file_id[0] == '_' && data->abstract_file_id[1] == 0 &&
        data->biblio_file_id[0] == '_' && data->biblio_file_id[1] == 0) {
        /* This is bug output from libisofs <= 0.6.23 . The texts mean file
           names and should have been empty to indicate that there are no such
           files. It is obvious that not all three roles can be fulfilled by
           one file "_" so that one cannot spoil anything by assuming them
           empty now.
        */
        data->copyright_file_id[0] = 0;
        data->abstract_file_id[0] = 0;
        data->biblio_file_id[0] = 0;
    }
    data->creation_time =
            iso_util_strcopy_untail((char*) pvm->vol_creation_time, 17);
    data->modification_time =
            iso_util_strcopy_untail((char*) pvm->vol_modification_time, 17);
    data->expiration_time =
            iso_util_strcopy_untail((char*) pvm->vol_expiration_time, 17);
    data->effective_time =
            iso_util_strcopy_untail((char*) pvm->vol_effective_time, 17);

    data->nblocks = iso_read_bb(pvm->vol_space_size, 4, NULL);

    rootdr = (struct ecma119_dir_record*) pvm->root_dir_record;
    data->pvd_root_block = iso_read_bb(rootdr->block, 4, NULL) +
                           rootdr->len_xa[0];

    /*
     * TODO #00017 : take advantage of other atts of PVD
     * PVD has other things that could be interesting, but that don't have a
     * member in IsoImage, such as creation date. In a multisession disc, we
     * could keep the creation date and update the modification date, for
     * example.
     */

    ret = ISO_SUCCESS;
ex:;
    LIBISO_FREE_MEM(buffer);
    return ret;
}

/**
 * @return
 *      1 success, 0 ignored, < 0 error
 */
static
int read_el_torito_boot_catalog(_ImageFsData *data, uint32_t block)
{
    int ret, i, rx, last_done, idx, bufsize;
    struct el_torito_validation_entry *ve;
    struct el_torito_section_header *sh;
    struct el_torito_section_entry *entry; /* also usable as default_entry */
    unsigned char *buffer = NULL, *rpt;

    LIBISO_ALLOC_MEM(buffer, unsigned char, BLOCK_SIZE);
    data->num_bootimgs = 0;
    data->catsize = 0;
    ret = data->src->read_block(data->src, block, buffer);
    if (ret < 0) {
        goto ex;
    }

    ve = (struct el_torito_validation_entry*)buffer;

    /* check if it is a valid catalog (TODO: check also the checksum)*/
    if ( (ve->header_id[0] != 1) || (ve->key_byte1[0] != 0x55)
         || (ve->key_byte2[0] != 0xAA) ) {
        iso_msg_submit(data->msgid, ISO_WRONG_EL_TORITO, 0,
                      "Wrong or damaged El-Torito Catalog. El-Torito info "
                      "will be ignored.");
        {ret = ISO_WRONG_EL_TORITO; goto ex;}
    }

    /* check for a valid platform */
    if (ve->platform_id[0] != 0 && ve->platform_id[0] != 0xef) {
        iso_msg_submit(data->msgid, ISO_UNSUPPORTED_EL_TORITO, 0,
                     "Unsupported El-Torito platform. Only 80x86 and EFI are "
                     "supported. El-Torito info will be ignored.");
        {ret = ISO_UNSUPPORTED_EL_TORITO; goto ex;}
    }

    /* ok, once we are here we assume it is a valid catalog */

    /* parse the default entry */
    entry = (struct el_torito_section_entry *)(buffer + 32);

    data->eltorito = 1;
    /* The Default Entry is declared mandatory */
    data->catsize = 64;
    data->num_bootimgs = 1;
    data->platform_ids[0] = ve->platform_id[0];
    memcpy(data->id_strings[0], ve->id_string, 24);
    memset(data->id_strings[0] + 24, 0, 4);
    data->boot_flags[0] = entry->boot_indicator[0] ? 1 : 0;
    data->media_types[0] = entry->boot_media_type[0];
    data->partition_types[0] = entry->system_type[0];
    data->load_segs[0] = iso_read_lsb(entry->load_seg, 2);
    data->load_sizes[0] = iso_read_lsb(entry->sec_count, 2);
    data->bootblocks[0] = iso_read_lsb(entry->block, 4);
    /* The Default Entry has no selection criterion */
    memset(data->selection_crits[0], 0, 20);

    /* Read eventual more entries from the boot catalog */
    last_done = 0;
    for (rx = 64; (buffer[rx] & 0xfe) == 0x90 && !last_done; rx += 32) {
        last_done = buffer[rx] & 1;
        /* Read Section Header */

        /* >>> ts B10703 : load a new buffer if needed */;

        sh = (struct el_torito_section_header *) (buffer + rx);
        data->catsize += 32;
        for (i = 0; i < sh->num_entries[0]; i++) {
            rx += 32;
            data->catsize += 32;

            /* >>> ts B10703 : load a new buffer if needed */;

            if (data->num_bootimgs >= Libisofs_max_boot_imageS) {

                /* >>> ts B10703 : need to continue rather than abort */;

                iso_msg_submit(data->msgid, ISO_EL_TORITO_WARN, 0,
                               "Too many boot images found. List truncated.");
                goto after_bootblocks;
            }
            /* Read bootblock from section entry */
            entry = (struct el_torito_section_entry *)(buffer + rx);
            idx = data->num_bootimgs;
            data->platform_ids[idx] = sh->platform_id[0];
            memcpy(data->id_strings[idx], sh->id_string, 28);
            data->boot_flags[idx] = entry->boot_indicator[0] ? 1 : 0;
            data->media_types[idx] = entry->boot_media_type[0];
            data->partition_types[idx] = entry->system_type[0];
            data->load_segs[idx] = iso_read_lsb(entry->load_seg, 2);
            data->load_sizes[idx] = iso_read_lsb(entry->sec_count, 2);
            data->bootblocks[idx] = iso_read_lsb(entry->block, 4);
            data->selection_crits[idx][0] = entry->selec_criteria[0];
            memcpy(data->selection_crits[idx] + 1, entry->vendor_sc, 19);
            data->num_bootimgs++;
        }
    }
after_bootblocks:;
    if(data->catsize > 0) {
      if(data->catcontent != NULL)
          free(data->catcontent);
      if(data->catsize > 10 * BLOCK_SIZE)
          data->catsize = 10 * BLOCK_SIZE;
      bufsize = data->catsize;
      if (bufsize % BLOCK_SIZE)
          bufsize += BLOCK_SIZE - (bufsize % BLOCK_SIZE);
      data->catcontent = calloc(bufsize , 1);
      if(data->catcontent == NULL) {
         data->catsize = 0;
         ret = ISO_OUT_OF_MEM;
         goto ex; 
      }
      for(rx = 0; rx < bufsize; rx += BLOCK_SIZE) {
        rpt = (unsigned char *) (data->catcontent + rx);
        ret = data->src->read_block(data->src, block + rx / BLOCK_SIZE, rpt);
        if (ret < 0)
           goto ex;
      }
    }
    ret = ISO_SUCCESS;
ex:;
    LIBISO_FREE_MEM(buffer);
    return ret;
}


/*
 @return 1= ok, checked, go on with loading
         2= no checksum tags found, go on with loading
        <0= libisofs error
            especially ISO_SB_TREE_CORRUPTED
*/
static
int iso_src_check_sb_tree(IsoDataSource *src, uint32_t start_lba, int flag)
{
    int tag_type, ret;
    char *block = NULL, md5[16];
    int desired = (1 << 2);
    void *ctx = NULL;
    uint32_t next_tag = 0, i;

    LIBISO_ALLOC_MEM(block, char, 2048);    
    ret = iso_md5_start(&ctx);
    if (ret < 0)
        goto ex;
    if (start_lba == 0)
         desired |= (1 << 4);
    for (i = 0; i < 32; i++) {
        ret = src->read_block(src, start_lba + i, (uint8_t *) block);
        if (ret < 0)
            goto ex;
        ret = 0;
        if (i >= 16)
            ret = iso_util_eval_md5_tag(block, desired, start_lba + i,
                                      ctx, start_lba, &tag_type, &next_tag, 0);
        iso_md5_compute(ctx, block, 2048);
        if (ret == (int) ISO_MD5_TAG_COPIED) {/* growing without emulated TOC */
            ret = 2;
            goto ex;
        }
        if (ret == (int) ISO_MD5_AREA_CORRUPTED ||
            ret == (int) ISO_MD5_TAG_MISMATCH)
            ret = ISO_SB_TREE_CORRUPTED;
        if (ret < 0)
            goto ex;
        if (ret == 1)
    break;
    }
    if (i >= 32) {
        ret = 2;
        goto ex;
    }
    if (tag_type == 4) {
        /* Relocated Superblock: restart checking at real session start */
        if (next_tag < 32) {
            /* Non plausible session_start address */
            ret = ISO_SB_TREE_CORRUPTED;
            iso_msg_submit(-1, ret, 0, NULL);
            goto ex;
        }
        /* Check real session */
        ret = iso_src_check_sb_tree(src, next_tag, 0);
        goto ex;
    }

    /* Go on with tree */
    for (i++; start_lba + i <= next_tag; i++) {
        ret = src->read_block(src, start_lba + i, (uint8_t *) block);
        if (ret < 0)
            goto ex;
        if (start_lba + i < next_tag)
            iso_md5_compute(ctx, block, 2048);
    }
    ret = iso_util_eval_md5_tag(block, (1 << 3), start_lba + i - 1,
                                ctx, start_lba, &tag_type, &next_tag, 0);
    if (ret == (int) ISO_MD5_AREA_CORRUPTED ||
        ret == (int) ISO_MD5_TAG_MISMATCH)
        ret = ISO_SB_TREE_CORRUPTED;
    if (ret < 0)
        goto ex;

    ret = 1;
ex:
    if (ctx != NULL)
        iso_md5_end(&ctx, md5);
    LIBISO_FREE_MEM(block);
    return ret;
}


int iso_image_filesystem_new(IsoDataSource *src, struct iso_read_opts *opts,
                             int msgid, IsoImageFilesystem **fs)
{
    int ret, i;
    uint32_t block;
    IsoImageFilesystem *ifs;
    _ImageFsData *data;
    uint8_t *buffer = NULL;

    if (src == NULL || opts == NULL || fs == NULL) {
        ret = ISO_NULL_POINTER; goto ex;
    }

    LIBISO_ALLOC_MEM(buffer, uint8_t, BLOCK_SIZE);
    data = calloc(1, sizeof(_ImageFsData));
    if (data == NULL) {
        ret = ISO_OUT_OF_MEM; goto ex;
    }

    ifs = calloc(1, sizeof(IsoImageFilesystem));
    if (ifs == NULL) {
        free(data);
        {ret = ISO_OUT_OF_MEM; goto ex;}
    }

    /* get our ref to IsoDataSource */
    data->src = src;
    iso_data_source_ref(src);
    data->open_count = 0;

    data->catcontent = NULL;

    /* get an id for the filesystem */
    data->id = ++fs_dev_id;

    /* fill data from opts */
    data->gid = opts->gid;
    data->uid = opts->uid;
    data->file_mode = opts->file_mode & ~S_IFMT;
    data->dir_mode = opts->dir_mode & ~S_IFMT;
    data->msgid = msgid;
    data->aaip_load = !opts->noaaip;
    if (opts->nomd5 == 0)
        data->md5_load = 1;
    else if (opts->nomd5 == 2)
        data->md5_load = 2;
    else
        data->md5_load = 0;
    data->aaip_version = -1;
    data->make_new_ino = opts->make_new_ino;
    data->num_bootimgs = 0;
    for (i = 0; i < Libisofs_max_boot_imageS; i++)
        data->bootblocks[i] = 0;
    data->inode_counter = 0;
    data->px_ino_status = 0;
    data->rr_err_reported = 0;
    data->rr_err_repeated = 0;
    data->joliet_ucs2_failures = 0;


    data->local_charset = strdup(iso_get_local_charset(0));
    if (data->local_charset == NULL) {
        ret = ISO_OUT_OF_MEM;
        LIBISO_FREE_MEM(data);
        data = NULL;
        goto fs_cleanup;
    }

    strncpy(ifs->type, "iso ", 4);
    ifs->data = data;
    ifs->refcount = 1;
    ifs->version = 0;
    ifs->get_root = ifs_get_root;
    ifs->get_by_path = ifs_get_by_path;
    ifs->get_id = ifs_get_id;
    ifs->open = ifs_fs_open;
    ifs->close = ifs_fs_close;
    ifs->free = ifs_fs_free;

    /* read Volume Descriptors and ensure it is a valid image */
    if (data->md5_load == 1) {
        /* From opts->block on : check for superblock and tree tags */;
        ret = iso_src_check_sb_tree(src, opts->block, 0);
        if (ret < 0) {
            iso_msgs_submit(0,
                "Image loading aborted due to MD5 mismatch of image tree data",
                            0, "FAILURE", 0);
            iso_msgs_submit(0,
                     "You may override this refusal by disabling MD5 checking",
                            0, "HINT", 0);
            goto fs_cleanup;
        }
    }

    /* 1. first, open the filesystem */
    ifs_fs_open(ifs);

    /* 2. read primary volume description */
    ret = read_pvm(data, opts->block + 16);
    if (ret < 0) {
        goto fs_cleanup;
    }

    /* 3. read next volume descriptors */
    block = opts->block + 17;
    do {
        ret = src->read_block(src, block, buffer);
        if (ret < 0) {
            /* cleanup and exit */
            goto fs_cleanup;
        }
        switch (buffer[0]) {
        case 0:
            /* boot record */
            {
                struct ecma119_boot_rec_vol_desc *vol;
                vol = (struct ecma119_boot_rec_vol_desc*)buffer;

                /* some sanity checks */
                if (strncmp((char*)vol->std_identifier, "CD001", 5)
                    || vol->vol_desc_version[0] != 1
                    || strncmp((char*)vol->boot_sys_id,
                               "EL TORITO SPECIFICATION", 23)) {
                    iso_msg_submit(data->msgid,
                          ISO_UNSUPPORTED_EL_TORITO, 0,
                          "Unsupported Boot Vol. Desc. Only El-Torito "
                          "Specification, Version 1.0 Volume "
                          "Descriptors are supported. Ignoring boot info");
                } else {
                    data->catblock = iso_read_lsb(vol->boot_catalog, 4);
                    ret = read_el_torito_boot_catalog(data, data->catblock);
                    if (ret < 0 && ret != (int) ISO_UNSUPPORTED_EL_TORITO &&
                        ret != (int) ISO_WRONG_EL_TORITO) {
                        goto fs_cleanup;
                    }
                }
            }
            break;
        case 2:
            /* suplementary volume descritor */
            {
                struct ecma119_sup_vol_desc *sup;
                struct ecma119_dir_record *root;

                sup = (struct ecma119_sup_vol_desc*)buffer;
                if (sup->esc_sequences[0] == 0x25 &&
                    sup->esc_sequences[1] == 0x2F &&
                    (sup->esc_sequences[2] == 0x40 ||
                     sup->esc_sequences[2] == 0x43 ||
                     sup->esc_sequences[2] == 0x45) ) {

                    /* it's a Joliet Sup. Vol. Desc. */
                    iso_msg_debug(data->msgid, "Found Joliet extensions");
                    data->joliet = 1;
                    root = (struct ecma119_dir_record*)sup->root_dir_record;
                    data->svd_root_block = iso_read_bb(root->block, 4, NULL) +
                                           root->len_xa[0];
                    /* TODO #00019 : set IsoImage attribs from Joliet SVD? */
                    /* TODO #00020 : handle RR info in Joliet tree */
                } else if (sup->vol_desc_version[0] == 2) {
                    /*
                     * It is an Enhanced Volume Descriptor, image is an
                     * ISO 9660:1999
                     */
                    iso_msg_debug(data->msgid, "Found ISO 9660:1999");
                    data->iso1999 = 1;
                    root = (struct ecma119_dir_record*)sup->root_dir_record;
                    data->evd_root_block = iso_read_bb(root->block, 4, NULL) + 
                                           root->len_xa[0];
                    /* TODO #00021 : handle RR info in ISO 9660:1999 tree */
                } else {
                    ret = iso_msg_submit(data->msgid, ISO_UNSUPPORTED_VD, 0,
                        "Unsupported Sup. Vol. Desc found.");
                    if (ret < 0) {
                        goto fs_cleanup;
                    }
                }
            }
            break;
        case 255:
            /*
             * volume set terminator
             * ignore, as it's checked in loop end condition
             */
            break;
        default:
            iso_msg_submit(data->msgid, ISO_UNSUPPORTED_VD, 0,
                           "Ignoring Volume descriptor %x.", buffer[0]);
            break;
        }
        block++;
    } while (buffer[0] != 255);

    /* 4. check if RR extensions are being used */
    ret = read_root_susp_entries(data, data->pvd_root_block);
    if (ret < 0) {
        goto fs_cleanup;
    }

    /* user doesn't want to read RR extensions */
    if (opts->norock) {
        data->rr = RR_EXT_NO;
    } else {
        data->rr = data->rr_version;
    }

    /* select what tree to read */
    if (data->rr) {
        /* RR extensions are available */
        if (!opts->nojoliet && opts->preferjoliet && data->joliet) {
            /* if user prefers joliet, that is used */
            iso_msg_debug(data->msgid, "Reading Joliet extensions.");
            /* Although Joliet prescribes UCS-2BE, interpret names by its
               superset UTF-16BE in order to avoid conversion failures.
            */
            data->input_charset = strdup("UTF-16BE");
            data->rr = RR_EXT_NO;
            data->iso_root_block = data->svd_root_block;
        } else {
            /* RR will be used */
            iso_msg_debug(data->msgid, "Reading Rock Ridge extensions.");
            data->iso_root_block = data->pvd_root_block;
        }
    } else {
        /* RR extensions are not available */
        if (!opts->nojoliet && data->joliet) {
            /* joliet will be used */
            iso_msg_debug(data->msgid, "Reading Joliet extensions.");
            data->input_charset = strdup("UTF-16BE");
            data->iso_root_block = data->svd_root_block;
        } else if (!opts->noiso1999 && data->iso1999) {
            /* we will read ISO 9660:1999 */
            iso_msg_debug(data->msgid, "Reading ISO-9660:1999 tree.");
            data->iso_root_block = data->evd_root_block;
        } else {
            /* default to plain iso */
            iso_msg_debug(data->msgid, "Reading plain ISO-9660 tree.");
            data->iso_root_block = data->pvd_root_block;
            data->input_charset = strdup("ASCII");
        }
    }
    data->truncate_mode = opts->truncate_mode;
    data->truncate_length = opts->truncate_length;
    data->ecma119_map = opts->ecma119_map;

    if (data->input_charset == NULL) {
        if (opts->input_charset != NULL) {
            data->input_charset = strdup(opts->input_charset);
        } else {
            data->input_charset = strdup(data->local_charset);
        }
    }
    if (data->input_charset == NULL) {
        ret = ISO_OUT_OF_MEM;
        goto fs_cleanup;
    }
    data->auto_input_charset = opts->auto_input_charset;

    /* and finally return. Note that we keep the DataSource opened */

    *fs = ifs;
    {ret = ISO_SUCCESS; goto ex;}

fs_cleanup: ;
    ifs_fs_free(ifs);
    free(ifs);

ex:;
    LIBISO_FREE_MEM(buffer);
    return ret;
}


/* Take over aa_string from file source to node or discard it after making
   the necessary change in node->mode group permissions.
   node->mode must already be set.
*/
static
int src_aa_to_node(IsoFileSource *src, IsoNode *node, int flag)
{
    int ret;
    unsigned char *aa_string;
    ImageFileSourceData *data;
    _ImageFsData *fsdata;
    char *a_text = NULL, *d_text = NULL;

    data = (ImageFileSourceData*)src->data;
    fsdata = data->fs->data;

    /* Obtain ownership of eventual AAIP string */
    ret = iso_file_source_get_aa_string(src, &aa_string, 1);
    if (ret != 1 || aa_string == NULL)
        return 1;
    if (fsdata->aaip_load == 1) {
        /* Attach aa_string to node */
        ret = iso_node_add_xinfo(node, aaip_xinfo_func, aa_string);
        if (ret < 0)
            return ret;
    } else {
        /* Look for ACL and perform S_IRWXG mapping */
        iso_aa_get_acl_text(aa_string, node->mode, &a_text, &d_text, 16);
        if (a_text != NULL)
            aaip_cleanout_st_mode(a_text, &(node->mode), 4 | 16);
        /* Dispose ACL a_text and d_text */
        iso_aa_get_acl_text(aa_string, node->mode, &a_text, &d_text, 1 << 15);
        /* Dispose aa_string */
        aaip_xinfo_func(aa_string, 1);
    }
    return 1;
}


static
int image_builder_create_node(IsoNodeBuilder *builder, IsoImage *image,
                              IsoFileSource *src, char *in_name,
                              IsoNode **node)
{
    int ret, idx, to_copy;
    struct stat info;
    IsoNode *new = NULL;
    IsoBoot *bootcat;
    char *name = NULL;
    char *dest = NULL;
    ImageFileSourceData *data;
    _ImageFsData *fsdata;

#ifdef Libisofs_with_zliB
    /* Intimate friendship with this function in filters/zisofs.c */
    int ziso_add_osiz_filter(IsoFile *file, uint8_t header_size_div4,
                             uint8_t block_size_log2,
                             uint32_t uncompressed_size, int flag);
#endif /* Libisofs_with_zliB */


    if (builder == NULL || src == NULL || node == NULL || src->data == NULL) {
        ret = ISO_NULL_POINTER; goto ex;
    }

    data = (ImageFileSourceData*)src->data;
    fsdata = data->fs->data;

    if (in_name == NULL) {
        name = iso_file_source_get_name(src);
    } else {
        name = strdup(in_name);
        if (name == NULL) {
            ret = ISO_OUT_OF_MEM; goto ex;
        }
    }

    /* get info about source */
    ret = iso_file_source_lstat(src, &info);
    if (ret < 0) {
        goto ex;
    }

    switch (info.st_mode & S_IFMT) {
    case S_IFREG:
        {
            /* source is a regular file */

            /* El-Torito images have only one section */
            if (fsdata->eltorito && data->sections[0].block == fsdata->catblock) {

                if (image->bootcat->node != NULL) {
                    ret = iso_msg_submit(image->id, ISO_EL_TORITO_WARN, 0,
                                 "More than one catalog node has been found. "
                                 "We can continue, but that could lead to "
                                 "problems");
                    if (ret < 0)
                        goto ex;
                    iso_node_unref((IsoNode*)image->bootcat->node);
                }

                /* we create a placeholder for the catalog instead of
                 * a regular file */
                new = calloc(1, sizeof(IsoBoot));
                if (new == NULL) {
                    ret = ISO_OUT_OF_MEM; goto ex;
                }
                bootcat = (IsoBoot *) new;
                bootcat->lba = data->sections[0].block;
                bootcat->size = info.st_size;
                if (bootcat->size > 10 * BLOCK_SIZE)
                    bootcat->size = 10 * BLOCK_SIZE;
                bootcat->content = NULL;
                if (bootcat->size > 0) {
                    bootcat->content = calloc(1, bootcat->size);
                    if (bootcat->content == NULL) {
                        ret = ISO_OUT_OF_MEM; goto ex;
                    }
                    to_copy = bootcat->size;
                    if (bootcat->size > fsdata->catsize)
                        to_copy = fsdata->catsize;
                    memcpy(bootcat->content, fsdata->catcontent, to_copy);
                }

                /* and set the image node */
                image->bootcat->node = bootcat;
                new->type = LIBISO_BOOT;
                new->refcount = 1;
            } else {
                IsoStream *stream;
                IsoFile *file;

                ret = iso_file_source_stream_new(src, &stream);
                if (ret < 0)
                    goto ex;

                /* take a ref to the src, as stream has taken our ref */
                iso_file_source_ref(src);

                file = calloc(1, sizeof(IsoFile));
                if (file == NULL) {
                    iso_stream_unref(stream);
                    {ret = ISO_OUT_OF_MEM; goto ex;}
                }

                /* mark file as from old session */
                file->from_old_session = 1;

                /*
                 * and we set the sort weight based on the block on image, to
                 * improve performance on image modifying.
                 *
                 * This was too obtrusive because it occupied the highest
                 * possible weight ranks:
                 *     file->sort_weight = INT_MAX - data->sections[0].block;
                 *
                 * So a try to be more nice and rely on caching with tiles
                 * of at least 16 blocks. This occupies a range within
                 * the interval of 1 to 2 exp 28 = 268,435,456.
                 * (Dividing each number separately saves from integer
                 *  rollover problems.)
                 */
                file->sort_weight =
                       fsdata->nblocks / 16 - data->sections[0].block / 16 + 1;

                file->stream = stream;
                file->node.type = LIBISO_FILE;

#ifdef Libisofs_with_zliB

                if (data->header_size_div4 > 0) {
                    ret = ziso_add_osiz_filter(file, data->header_size_div4,
                                               data->block_size_log2,
                                               data->uncompressed_size, 0);
                    if (ret < 0) {
                        iso_stream_unref(stream);
                        goto ex;
                    }
                }

#endif /* Libisofs_with_zliB */

                new = (IsoNode*) file;
                new->refcount = 0;

                if (data->sections[0].size > 0) {
                    for (idx = 0; idx < fsdata->num_bootimgs; idx++)
                        if (fsdata->eltorito && data->sections[0].block ==
                            fsdata->bootblocks[idx])
                    break;
                } else {
                    idx = fsdata->num_bootimgs;
                }
                if (idx < fsdata->num_bootimgs) {
                    /* it is boot image node */
                    if (image->bootcat->bootimages[idx]->image != NULL) {
                        /* idx is already occupied, try to find unoccupied one
                           which has the same block address.
                        */
                        for (; idx < fsdata->num_bootimgs; idx++)
                            if (fsdata->eltorito && data->sections[0].block ==
                                fsdata->bootblocks[idx] &&
                                image->bootcat->bootimages[idx]->image == NULL)
                        break;
                    }
                    if (idx >= fsdata->num_bootimgs) {
                        ret = iso_msg_submit(image->id, ISO_EL_TORITO_WARN, 0,
             "More than one ISO node has been found for the same boot image.");
                        if (ret < 0) {
                            iso_stream_unref(stream);
                            goto ex;
                        }
                    } else {
                        /* and set the image node */
                        image->bootcat->bootimages[idx]->image = file;
                        new->refcount++;
                    }
                }
            }
        }
        break;
    case S_IFDIR:
        {
            /* source is a directory */
            new = calloc(1, sizeof(IsoDir));
            if (new == NULL) {
                {ret = ISO_OUT_OF_MEM; goto ex;}
            }
            new->type = LIBISO_DIR;
            new->refcount = 0;
        }
        break;
    case S_IFLNK:
        {
            /* source is a symbolic link */
            IsoSymlink *link;

            LIBISO_ALLOC_MEM(dest, char, LIBISOFS_NODE_PATH_MAX);

            ret = iso_file_source_readlink(src, dest, LIBISOFS_NODE_PATH_MAX);
            if (ret < 0) {
                goto ex;
            }
            link = calloc(1, sizeof(IsoSymlink));
            if (link == NULL) {
                {ret = ISO_OUT_OF_MEM; goto ex;}
            }
            link->dest = strdup(dest);
            link->node.type = LIBISO_SYMLINK;
            link->fs_id = ISO_IMAGE_FS_ID;
            link->st_dev = info.st_dev;
            link->st_ino = info.st_ino;
            new = (IsoNode*) link;
            new->refcount = 0;
        }
        break;
    case S_IFSOCK:
    case S_IFBLK:
    case S_IFCHR:
    case S_IFIFO:
        {
            /* source is an special file */
            IsoSpecial *special;
            special = calloc(1, sizeof(IsoSpecial));
            if (special == NULL) {
                ret = ISO_OUT_OF_MEM; goto ex;
            }
            special->dev = info.st_rdev;
            special->node.type = LIBISO_SPECIAL;
            special->fs_id = ISO_IMAGE_FS_ID;
            special->st_dev = info.st_dev;
            special->st_ino = info.st_ino;
            new = (IsoNode*) special;
            new->refcount = 0;
        }
        break;
    default:
        ret = ISO_BAD_ISO_FILETYPE; goto ex;
    }
    /* fill fields */
    new->refcount++;
    new->name = name; name = NULL;
    new->mode = info.st_mode;
    new->uid = info.st_uid;
    new->gid = info.st_gid;
    new->atime = info.st_atime;
    new->mtime = info.st_mtime;
    new->ctime = info.st_ctime;

    new->hidden = 0;

    new->parent = NULL;
    new->next = NULL;

    ret = src_aa_to_node(src, new, 0);
    if (ret < 0) {
        goto ex;
    }

    /* Attach ino as xinfo if valid and no IsoStream is involved */
    if (info.st_ino != 0 && (info.st_mode & S_IFMT) != S_IFREG &&
        !fsdata->make_new_ino) {
        ret = iso_node_set_ino(new, info.st_ino, 0);
        if (ret < 0)
            goto ex;
    }

    *node = new; new = NULL;
    {ret = ISO_SUCCESS; goto ex;}

ex:;
    if (name != NULL)
        free(name);
    if (new != NULL)
        iso_node_unref(new);
    LIBISO_FREE_MEM(dest);
    return ret;
}

/**
 * Create a new builder, that is exactly a copy of an old builder, but where
 * create_node() function has been replaced by image_builder_create_node.
 */
static
int iso_image_builder_new(IsoNodeBuilder *old, IsoNodeBuilder **builder)
{
    IsoNodeBuilder *b;

    if (builder == NULL) {
        return ISO_NULL_POINTER;
    }

    b = malloc(sizeof(IsoNodeBuilder));
    if (b == NULL) {
        return ISO_OUT_OF_MEM;
    }

    b->refcount = 1;
    b->create_file_data = old->create_file_data;
    b->create_node_data = old->create_node_data;
    b->create_file = old->create_file;
    b->create_node = image_builder_create_node;
    b->free = old->free;

    *builder = b;
    return ISO_SUCCESS;
}

/**
 * Create a file source to access the El-Torito boot image, when it is not
 * accessible from the ISO filesystem.
 */
static
int create_boot_img_filesrc(IsoImageFilesystem *fs, IsoImage *image, int idx,
                            IsoFileSource **src)
{
    int ret;
    struct stat atts;
    _ImageFsData *fsdata;
    IsoFileSource *ifsrc = NULL;
    ImageFileSourceData *ifsdata = NULL;

    if (fs == NULL || fs->data == NULL || src == NULL) {
        return ISO_NULL_POINTER;
    }

    fsdata = (_ImageFsData*)fs->data;

    memset(&atts, 0, sizeof(struct stat));
    atts.st_mode = S_IFREG;
    atts.st_ino = img_give_ino_number(image, 0);
    atts.st_nlink = 1;

    /*
     * this is the greater problem. We don't know the size. For now, we
     * just use a single block of data. In a future, maybe we could figure out
     * a better idea. Another alternative is to use several blocks, that way
     * is less probable that we throw out valid data.
     */
    atts.st_size = (off_t)BLOCK_SIZE;

    /* Fill last entries */
    atts.st_dev = fsdata->id;
    atts.st_blksize = BLOCK_SIZE;
    atts.st_blocks = DIV_UP(atts.st_size, BLOCK_SIZE);

    /* ok, we can now create the file source */
    ifsdata = calloc(1, sizeof(ImageFileSourceData));
    if (ifsdata == NULL) {
        ret = ISO_OUT_OF_MEM;
        goto boot_fs_cleanup;
    }
    ifsrc = calloc(1, sizeof(IsoFileSource));
    if (ifsrc == NULL) {
        ret = ISO_OUT_OF_MEM;
        goto boot_fs_cleanup;
    }

    ifsdata->sections = malloc(sizeof(struct iso_file_section));
    if (ifsdata->sections == NULL) {
        ret = ISO_OUT_OF_MEM;
        goto boot_fs_cleanup;
    }

    /* fill data */
    ifsdata->fs = fs;
    iso_filesystem_ref(fs);
    ifsdata->parent = NULL;
    ifsdata->info = atts;
    ifsdata->name = NULL;
    ifsdata->sections[0].block = fsdata->bootblocks[idx];
    ifsdata->sections[0].size = BLOCK_SIZE;
    ifsdata->nsections = 1;

    ifsrc->class = &ifs_class;
    ifsrc->data = ifsdata;
    ifsrc->refcount = 1;

    *src = ifsrc;
    return ISO_SUCCESS;

boot_fs_cleanup: ;
    free(ifsdata);
    free(ifsrc);
    return ret;
}

/** ??? >>> ts B00428 : should the max size become public ? */
#define Libisofs_boot_image_max_sizE (4096*1024)

/** Guess which of the loaded boot images contain boot information tables.
    Set boot->seems_boot_info_table accordingly.
*/
static
int iso_image_eval_boot_info_table(IsoImage *image, struct iso_read_opts *opts,
                         IsoDataSource *src, uint32_t iso_image_size, int flag)
{
    int i, j, ret, section_count, todo, chunk;
    uint32_t img_lba, img_size, boot_pvd_found, image_pvd, alleged_size;
    struct iso_file_section *sections = NULL;
    struct el_torito_boot_image *boot;
    uint8_t *boot_image_buf = NULL, boot_info_found[16], *buf = NULL;
    IsoStream *stream = NULL;
    IsoFile *boot_file;
    uint64_t blk;

    if (image->bootcat == NULL)
        {ret = ISO_SUCCESS; goto ex;}
    LIBISO_ALLOC_MEM(buf, uint8_t, BLOCK_SIZE);
    for (i = 0; i < image->bootcat->num_bootimages; i++) {
        boot = image->bootcat->bootimages[i];
        boot_file = boot->image;
        boot->seems_boot_info_table = 0;
        boot->seems_grub2_boot_info = 0;
        boot->seems_isohybrid_capable = 0;
        img_size = iso_file_get_size(boot_file);
        if (img_size > Libisofs_boot_image_max_sizE || img_size < 64)
    continue;
        img_lba = 0;
        sections = NULL;
        ret = iso_file_get_old_image_sections(boot_file,
                                              &section_count, &sections, 0);
        if (ret == 1 && section_count > 0)
            img_lba = sections[0].block;
        if (sections != NULL) {
            free(sections);
            sections = NULL;
        }
        if(img_lba == 0)
    continue;

        boot_image_buf = calloc(1, img_size);
        if (boot_image_buf == NULL) {
            ret = ISO_OUT_OF_MEM;
            goto ex;
        }
        stream = iso_file_get_stream(boot_file);
        ret = iso_stream_open(stream);
        if (ret < 0) {
            stream = NULL;
            goto ex;
        }
        for (todo = img_size; todo > 0; ) {
          if (todo > BLOCK_SIZE)
              chunk = BLOCK_SIZE;
          else
              chunk = todo;
          ret = iso_stream_read(stream, boot_image_buf + (img_size - todo),
                                chunk);
          if (ret != chunk) {
            ret = (ret < 0) ? ret : (int) ISO_FILE_READ_ERROR;
            goto ex;
          }
          todo -= chunk;
        }
        iso_stream_close(stream);
        stream = NULL;
        
        memcpy(boot_info_found, boot_image_buf + 8, 16);
        boot_pvd_found = iso_read_lsb(boot_info_found, 4);
        image_pvd = (uint32_t) (opts->block + 16);

        /* Accommodate to eventually relocated superblock */
        if (image_pvd != boot_pvd_found &&
            image_pvd == 16 && boot_pvd_found < iso_image_size) {
            /* Check whether there is a PVD at boot_pvd_found
               and whether it bears the same image size 
             */
            ret = read_pvd_block(src, boot_pvd_found, buf, &alleged_size);
            if (ret == 1 &&
                alleged_size + boot_pvd_found == iso_image_size + image_pvd)
              image_pvd = boot_pvd_found;
        }

        ret = make_boot_info_table(boot_image_buf, image_pvd,
                                   img_lba, img_size);
        if (ret < 0)
            goto ex;
        if (memcmp(boot_image_buf + 8, boot_info_found, 16) == 0)
            boot->seems_boot_info_table = 1;

        if (img_size >= Libisofs_grub2_elto_patch_poS + 8) {
            blk = 0;
            for (j = Libisofs_grub2_elto_patch_poS + 7;
                 j >= Libisofs_grub2_elto_patch_poS; j--)
                blk = (blk << 8) | boot_image_buf[j];
            if (blk == img_lba * 4 + Libisofs_grub2_elto_patch_offsT)
                boot->seems_grub2_boot_info = 1;
        }
        if (img_size >= 68 && boot->seems_boot_info_table)
            if (boot_image_buf[64] == 0xfb && boot_image_buf[65] == 0xc0 &&
                boot_image_buf[66] == 0x78 && boot_image_buf[67] == 0x70)
                boot->seems_isohybrid_capable = 1;

        free(boot_image_buf);
        boot_image_buf = NULL;
    }
    ret = 1;
ex:;
    if (boot_image_buf != NULL)
        free(boot_image_buf);
    if (stream != NULL)
        iso_stream_close(stream);
    LIBISO_FREE_MEM(buf);
    return ret;
}


static
void issue_collision_warning_summary(size_t failures)
{
    if (failures > ISO_IMPORT_COLL_WARN_MAX) {
        iso_msg_submit(-1, ISO_IMPORT_COLLISION, 0,
                       "More file name collisions had to be resolved");
    }
    if (failures > 0) {
        iso_msg_submit(-1, ISO_IMPORT_COLLISION, 0,
                       "Sum of resolved file name collisions: %.f",
                       (double) failures);
    }
}

/* Mark all non-matching combinations of head_per_cyl and sectors_per_head
   in the matches bitmap. This is a brute force approach to find the common
   intersections of up to 8 hyperbolas additionally intersected with the grid
   of integer coordinates {1..255}x{1..63}.
   Given the solution space size of only 14 bits, it seems inappropriate to
   employ any algebra.
*/
static
void iso_scan_hc_sh(uint32_t lba, int c, int h, int s, uint8_t *matches)
{
    int i, j;
    uint32_t res;

/*
    fprintf(stderr, "iso_scan_hc_sh :%d = %4d/%3d/%2d :\n", lba, c, h, s);
*/
    if (lba == ((uint32_t) s) - 1 && c == 0 && h == 0) /* trivial solutions */
        return;
    if (c == 1023 && h >= 254 && s == 63) /* Indicators for invalid CHS */
        return;

    /* matches(i=0,j=1) == 0 indicates presence of non-trivial equations */
    matches[0] &= ~1;

    for (i = 1; i <= 255; i++) {
        for (j = 1; j <= 63; j++) {
            res = ((c * i) + h) * j + (s - 1);
            if (res != lba) {
                matches[(i / 8) * 32 + (j - 1)] &= ~(1 << (i % 8));
/*
            } else {    
                 if (matches[(i / 8) * 32 + (j - 1)] & (1 << (i % 8)))
                     fprintf(stderr,
                    "iso_scan_hc_sh :%d = %4d/%3d/%2d :  H/C= %3d  S/H= %2d\n",
                            lba, c, h, s, i, j);
*/
            }
        }
    }
}

/* Pick a good remaining solution from the matches bitmap.
*/
static
void iso_get_hc_sh(uint8_t *matches, uint32_t iso_image_size,
                   int *hc, int *sh, int flag)
{
    int i, j, k;
    static int pref[][2] = {{64, 32}, {255, 63}}, prefs = 2;
    
    *hc = *sh = 0;

    if (matches[0] & 1)
        return; /* Only trivial equations seen */

    /* Look for preferred layouts */
    for (k = 0; k < prefs; k++) {
        i = pref[k][0];
        j = pref[k][1];
        if ((uint32_t) (1024 / 4 * i * j) <= iso_image_size)
    continue;
        if (matches[(i / 8) * 32 + (j - 1)] & (1 << (i % 8))) {
            *hc = i;
            *sh = j;
            return;
        }
    }

    /* Look for largest possible cylinder */
    for (i = 1; i <= 255; i++) {
        for (j = 1; j <= 63; j++) {
            if ((uint32_t) (1024 / 4 * i * j) <= iso_image_size)
        continue;
            if (matches[(i / 8) * 32 + (j - 1)] & (1 << (i % 8))) {
                if( i * j < *hc * *sh)
        continue;
                *hc = i;
                *sh = j;
            }
        }
    }
}

static
int iso_analyze_mbr_ptable(IsoImage *image, int flag)
{
    int i, j, ret, cyl_align_mode, part_after_image = 0;
    uint32_t start_h, start_s, start_c, end_h, end_s, end_c, sph = 0, hpc = 0;
    uint32_t start_lba, num_blocks, end_chs_lba, image_size, lba, cyl_size;
    uint8_t *data, pstatus, ptype, *hc_sh = NULL;
    struct iso_imported_sys_area *sai;

    /* Bitmap for finding head_per_cyl and sectors_per_head. */
    LIBISO_ALLOC_MEM(hc_sh, uint8_t, 32 * 63);
    memset(hc_sh, 0xff,  32 * 63);

    sai = image->imported_sa_info;
    image_size = sai->image_size;
    for (i = 0; i < 4; i++) {
        data = (uint8_t *) (image->system_area_data + 446 + 16 * i);
        for (j = 0; j < 16; j++)
            if (data[j])
        break;
        if (j == 16)
    continue;
        pstatus = data[0];
        ptype = data[4];
        start_c = ((data[2] & 0xc0) << 2) | data[3];
        start_h = data[1];
        start_s = data[2] & 63;
        end_c = ((data[6] & 0xc0) << 2) | data[7];
        end_h = data[5];
        end_s = data[6] & 63;
        start_lba = iso_read_lsb(data + 8, 4);
        num_blocks = iso_read_lsb(data + 12, 4);
        if (num_blocks <= 0)
    continue;
        if (sph > 0) {
            if (end_s != sph)
                sph = 0xffffffff;
        } else if (sph == 0) {
            sph = end_s;
        }
        if (hpc > 0) {
            if (end_h + 1 != hpc)
                hpc = 0xffffffff;
        } else if (hpc == 0) {
            hpc = end_h + 1;
        }
        /* Check whether start_lba + num_blocks - 1 matches chs,hpc,spc */
        end_chs_lba = ((end_c * hpc) + end_h) * sph + end_s;
        if (hpc > 0 && hpc < 0xffffffff && sph > 0 && sph < 0xffffffff)
            if (end_chs_lba != start_lba + num_blocks)
                hpc = sph = 0xffffffff;
        /* In case that end CHS does not give cylinder layout */
        iso_scan_hc_sh(start_lba, start_c, start_h, start_s, hc_sh);
        iso_scan_hc_sh(start_lba + num_blocks - 1, end_c, end_h, end_s, hc_sh);

        /* Register partition as iso_mbr_partition_request */
        if (sai->mbr_req == NULL) {
            sai->mbr_req = calloc(ISO_MBR_ENTRIES_MAX,
                                  sizeof(struct iso_mbr_partition_request *));
            if (sai->mbr_req == NULL)
                {ret = ISO_OUT_OF_MEM; goto ex;}
        }
        ret = iso_quick_mbr_entry(sai->mbr_req, &(sai->mbr_req_count),
                                  (uint64_t) start_lba, (uint64_t) num_blocks,
                                  ptype, pstatus, i + 1);
        if (ret < 0)
            goto ex;
        if ((start_lba + num_blocks + 3) / 4 > image_size)
            image_size = (start_lba + num_blocks + 3) / 4;
    }

    if (hpc > 0 && hpc < 0xffffffff && sph > 0 && sph < 0xffffffff) {
        sai->partition_secs_per_head = sph;
        sai->partition_heads_per_cyl = hpc;
    } else {
        /* Look for the best C/H/S parameters caught in scan */
        iso_get_hc_sh(hc_sh, image_size, &(sai->partition_heads_per_cyl),
                             &(sai->partition_secs_per_head), 0);
    }

    cyl_align_mode = 2; /* off */
    if (sai->partition_secs_per_head >0 && sai->partition_heads_per_cyl > 0 &&
        sai->mbr_req_count > 0) {
        /* Check for cylinder alignment */
        for (i = 0; i < sai->mbr_req_count; i++) {
             cyl_size = sai->partition_secs_per_head *
                        sai->partition_heads_per_cyl;
             lba = sai->mbr_req[i]->start_block + sai->mbr_req[i]->block_count;
             if (sai->mbr_req[i]->start_block >= sai->image_size)
                 part_after_image = 1;
             end_c = lba / cyl_size;
             if (end_c * cyl_size != lba)
        break;
        }
        if (i == sai->mbr_req_count && part_after_image)
            cyl_align_mode = 3; /* all */
        else if (i >= 1)
            cyl_align_mode = 1; /* on */
    }
    sai->system_area_options &= ~(3 << 8);
    sai->system_area_options |= (cyl_align_mode << 8);
    ret = 1;
ex:
    LIBISO_FREE_MEM(hc_sh);
    return ret;
    
}

/* @return 0= no hybrid detected
           1= ISOLINUX isohybrid (options & 2)
           2= GRUB2 MBR patching (options & (1 << 14))
*/
static
int iso_analyze_isohybrid(IsoImage *image, int flag)
{
    uint8_t *sad;
    uint32_t eltorito_lba = 0;
    uint64_t mbr_lba;
    int i, section_count, ret;
    ElToritoBootImage *boot;
    struct iso_file_section *sections;

    sad = (uint8_t *) image->system_area_data;

    /* Learn LBA of boot image */;
    if (image->bootcat == NULL)
        return 0;
    if (image->bootcat->num_bootimages < 1)
        return 0;
    boot = image->bootcat->bootimages[0];
    if (boot == NULL)
        return 0;
    ret = iso_file_get_old_image_sections(boot->image, &section_count,
                                          &sections, 0);
    if (ret < 0)
        return ret;
    if (ret > 0 && section_count > 0)
        eltorito_lba = sections[0].block;
    free(sections);
    
    /* Check MBR whether it is ISOLINUX and learn LBA to which it points */
    if (!boot->seems_isohybrid_capable)
        goto try_grub2_mbr;
    for (i= 0; i < 426; i++)
        if(strncmp((char *) (sad + i), "isolinux", 8) == 0)
    break;
    if (i < 426) { /* search text was found */
        mbr_lba = iso_read_lsb(sad + 432, 4);
        mbr_lba /= 4;
        if (mbr_lba == eltorito_lba)
           return 1;
        goto try_grub2_mbr;
    }

try_grub2_mbr:;
    /* Check for GRUB2 MBR patching */
    mbr_lba = iso_read_lsb64(sad + 0x1b0);
    if (mbr_lba / 4 - 1 == eltorito_lba)
        return 2; 

    return 0;
}

static
int iso_analyze_partition_offset(IsoImage *image, IsoDataSource *src,
                                 uint64_t start_block, int flag)
{
    int ret;
    uint8_t *buf = NULL;
    off_t p_offset;
    struct ecma119_pri_vol_desc *pvm;
    struct iso_imported_sys_area *sai;

    sai = image->imported_sa_info;

    /* Check for PVD at partition start with same end */
    LIBISO_ALLOC_MEM(buf, uint8_t, 2048);
    p_offset = start_block / 4;
    ret = src->read_block(src, p_offset + 16, buf);
    if (ret > 0) {
        pvm = (struct ecma119_pri_vol_desc *) buf;
        if (strncmp((char*) pvm->std_identifier, "CD001", 5) == 0 &&
            pvm->vol_desc_type[0] == 1 &&
            pvm->vol_desc_version[0] == 1 &&
            pvm->file_structure_version[0] == 1 &&
            iso_read_lsb(pvm->vol_space_size, 4) + p_offset == sai->image_size)
            sai->partition_offset = p_offset;
    }
    ret = 1;
ex:;
    LIBISO_FREE_MEM(buf);
    return ret;
}

static
int iso_analyze_mbr(IsoImage *image, IsoDataSource *src, int flag)
{
    int sub_type = 2, ret, is_isohybrid = 0, is_grub2_mbr = 0;
    int is_protective_label = 0;
    uint64_t part2_start;
    char *sad;
    struct iso_imported_sys_area *sai;
    struct iso_mbr_partition_request *part;

    sad = image->system_area_data;
    sai = image->imported_sa_info;

    /* Is it an MBR ? */
    if (((unsigned char *) sad)[510] != 0x55 ||
        ((unsigned char *) sad)[511] != 0xaa)
        {ret = 0; goto ex;}

    ret = iso_analyze_mbr_ptable(image, 0);
    if (ret <= 0)
        goto ex;

    ret = iso_analyze_isohybrid(image, 0);
    if (ret < 0)
        goto ex;
    if (ret == 1) {
        sub_type = 0;
        is_isohybrid = 1;
    } else if(ret == 2) {
        /* will become sub_type 0 if protective_label */
        is_grub2_mbr = 1;
    }

    if (sai->mbr_req_count == 3 && !is_isohybrid) {
        /* Check for libisofs PReP partitions :
               0xee or 0xcd from 0 to a-1
               0x41 from a to b
               0x0c or 0xcd from b+1 to end
        */
        if ((sai->mbr_req[0]->start_block == 0 &&
             (sai->mbr_req[0]->type_byte == 0xee ||
              sai->mbr_req[0]->type_byte == 0xcd)) &&
            sai->mbr_req[0]->block_count == sai->mbr_req[1]->start_block &&
            sai->mbr_req[1]->type_byte == 0x41 &&
            (sai->mbr_req[1]->start_block % 4) == 0 &&
            sai->mbr_req[1]->start_block + sai->mbr_req[1]->block_count ==
                                                sai->mbr_req[2]->start_block &&
            (sai->mbr_req[2]->type_byte == 0x0c ||
             sai->mbr_req[2]->type_byte == 0xcd) &&
            (sai->mbr_req[2]->start_block + sai->mbr_req[2]->block_count) / 4
                                                          == sai->image_size) {
            sai->prep_part_start = sai->mbr_req[1]->start_block / 4;
            sai->prep_part_size = (sai->mbr_req[1]->block_count + 3) / 4;
            sub_type = 0;
        }
    }
    if (sai->mbr_req_count >= 1 &&
        (sai->mbr_req[0]->type_byte == 0xee || !is_isohybrid) &&
        !(sai->prep_part_start > 0)) {
        part = sai->mbr_req[0];
        part2_start = 0;
        if (sai->mbr_req_count >= 2)
            part2_start = sai->mbr_req[1]->start_block;
        if (part->start_block == 1 &&
            (part->block_count + 1 == ((uint64_t) sai->image_size) * 4 ||
             (part->type_byte == 0xee &&
              part->block_count + 1 >= ((uint64_t) sai->image_size) * 4 &&
              (sai->mbr_req_count == 1 ||
               (sai->mbr_req_count == 2 &&
                sai->mbr_req[1]->type_byte == 0x00))) ||
             part->block_count + 1 == part2_start)) {
            /* libisofs protective msdos label for GRUB2 */
            is_protective_label = 1;
            sub_type = 0;
        } else if (sai->mbr_req_count == 1 && part->start_block == 0 &&
                 part->block_count <= ((uint64_t) sai->image_size) * 4 &&
                 part->block_count + 600 >= ((uint64_t) sai->image_size) * 4 &&
                 part->type_byte == 0x96) {
            /* CHRP (possibly without padding) */
            sub_type = 1;
        } else if (sai->mbr_req_count == 1 &&
                   sai->mbr_req[0]->start_block > 0 &&
                   (sai->mbr_req[0]->start_block % 4) == 0 &&
                   (sai->mbr_req[0]->start_block +
                        sai->mbr_req[0]->block_count) / 4 <= sai->image_size &&
                   part->type_byte == 0x41) {
            /* mkisofs PReP partition */
            sai->prep_part_start = sai->mbr_req[0]->start_block / 4;
            sai->prep_part_size = (sai->mbr_req[0]->block_count + 3) / 4;
            sub_type = 0;
        }
    }

    /* Check for partition offset with extra set of meta data */
    if (sai->mbr_req_count > 0) {
        part = sai->mbr_req[0];
        if ((part->status_byte == 0x80 || part->status_byte == 0) &&
            part->start_block >= 64 && part->block_count >= 72 &&
            part->start_block <= 2048 &&
            part->start_block % 4 == 0 && part->block_count % 4 == 0 &&
            (part->start_block + part->block_count) / 4 == sai->image_size) {

            ret = iso_analyze_partition_offset(image, src, part->start_block,
                                               0);
            if (ret < 0)
                goto ex;
        }
    }

    /* Set sa type 0, sub type as chosen */
    sai->system_area_options = (sai->system_area_options & 0xffff8300) |
                               is_protective_label |
                               (is_isohybrid << 1) |
                               (sub_type << 10) |
                               (is_grub2_mbr << 14);
    ret = 1;
ex:;
    return ret;
}

static
int iso_seems_usable_gpt_head(uint8_t *head, int flag)
{
    uint32_t head_size, entry_size;

    if (strncmp((char *) head, "EFI PART", 8) != 0) /* signature */
        return 0;
    if (head[8] || head[9] || head[10] != 1 || head[11]) /* revision */
        return 0;
    head_size = iso_read_lsb(head + 12, 4);
    if (head_size < 92)
        return 0;
    entry_size = iso_read_lsb(head + 84, 4);
    if (entry_size != 128)
        return 0;
    return 1;
}

static
int iso_analyze_gpt_backup(IsoImage *image, IsoDataSource *src, int flag)
{
    struct iso_imported_sys_area *sai;
    uint64_t part_start;
    uint32_t iso_block, found_crc, crc, entry_count, array_crc;
    uint8_t *head, *part_array, *b_part, *m_part;
    int ret, i, num_iso_blocks, l, j, entries_diff;
    unsigned char *buf = NULL;
    char *comments = NULL;

    sai = image->imported_sa_info;
    LIBISO_ALLOC_MEM(buf, unsigned char, 34 * 1024);
    LIBISO_ALLOC_MEM(comments, char, 4096);

    /* Read ISO block with backup head */
    if (sai->gpt_backup_lba >= ((uint64_t) sai->image_size) * 4 &&
        (sai->mbr_req_count < 1 ||
         sai->mbr_req[0]->start_block + sai->mbr_req[0]->block_count
         > sai->gpt_backup_lba + 1))
        sprintf(comments + strlen(comments), "Implausible header LBA %.f, ",
                (double) sai->gpt_backup_lba);
    iso_block = sai->gpt_backup_lba / 4;
    ret = src->read_block(src, iso_block, buf);
    if (ret < 0) {
        sprintf(comments + strlen(comments),
                "Cannot read header block at 2k LBA %.f, ",
                (double) iso_block);
        ret = 0; goto ex;
    }
    head = buf + (sai->gpt_backup_lba % 4) * 512;
    ret = iso_seems_usable_gpt_head(head, 0);
    if (ret == 0)
        strcat(comments,
               "Not a GPT 1.0 header of 92 bytes for 128 bytes per entry, ");
    if (ret <= 0) {
        ret = 0; goto ex;
    }

    /* Check head CRC */
    found_crc = iso_read_lsb(head + 16, 4);
    memset(head + 16, 0, 4);
    crc = iso_crc32_gpt((unsigned char *) head, 92, 0);
    if (found_crc != crc) {
        sprintf(comments + strlen(comments),
                "Head CRC 0x%8x wrong. Should be 0x%8x",
                found_crc, crc);
        crc = iso_crc32_gpt((unsigned char *) head, 512, 0);
        if (found_crc == crc) {
            strcat(comments, ". Matches all 512 block bytes, ");
        } else {
            strcat(comments, ", ");
            ret = 0; goto ex;
        }
    }
    for (i = 0; i < 16; i ++)
        if (head[i + 56] != sai->gpt_disk_guid[i])
    break;
    if (i < 16) {
        sprintf(comments + strlen(comments), "Disk GUID differs (");
        iso_util_bin_to_hex(comments + strlen(comments), head + 56, 16, 0);
        sprintf(comments + strlen(comments), "), ");
    }

    /* Header content will possibly be overwritten now */
    array_crc = iso_read_lsb(head + 88, 4);
    part_start = iso_read_lsb64(head + 72);
    entry_count = iso_read_lsb(head + 80, 4);
    head = NULL;

    /* Read backup array */
    if (entry_count != sai->gpt_max_entries) {
        sprintf(comments + strlen(comments),
                "Number of array entries %u differs from main GPT %u, ",
                entry_count, sai->gpt_max_entries);
        ret = 0; goto ex;
    }
    if (part_start + (entry_count + 3) / 4 != sai->gpt_backup_lba)
        sprintf(comments + strlen(comments), "Implausible array LBA %.f, ",
                (double) part_start);
    iso_block = part_start / 4;
    num_iso_blocks = (part_start + (entry_count + 3) / 4) / 4 - iso_block + 1;
    for (i = 0; i < num_iso_blocks; i++) {
        ret = src->read_block(src, iso_block + (uint32_t) i, buf + i * 2048);
        if (ret < 0) {
            sprintf(comments + strlen(comments),
                    "Cannot read array block at 2k LBA %.f, ",
                    (double) iso_block);
            ret = 0; goto ex;
        }
    }
    part_array = buf + (part_start % 4) * 512;

    crc = iso_crc32_gpt((unsigned char *) part_array, 128 * entry_count, 0);
    if (crc != array_crc)
        sprintf(comments + strlen(comments),
                "Array CRC 0x%8x wrong. Should be 0x%8x, ", array_crc, crc);

    /* Compare entries */
    entries_diff = 0;
    for (i = 0; i < (int) entry_count; i++) {
        b_part = part_array + 128 * i;
        m_part = ((uint8_t *) image->system_area_data) +
                 sai->gpt_part_start * 512 + 128 * i;
        for (j = 0; j < 128; j++)
            if (b_part[j] != m_part[j])
        break;
        if (j < 128) {
            if (!entries_diff) {
                strcat(comments, "Entries differ for partitions");
                entries_diff = 1;
            }
            sprintf(comments + strlen(comments), " %d", i + 1);
        }
    }
    if (entries_diff) {
        strcat(comments, ", ");
        ret = 0; goto ex;
    }

    ret = 1;
ex:;
    if (comments != NULL) {
        l = strlen(comments);
        if (l > 2)
            if (comments[l - 2] == ',' && comments[l - 1] == ' ')
                comments[l - 2] = 0;
        sai->gpt_backup_comments = strdup(comments);
        if (sai->gpt_backup_comments == NULL)
            ret = ISO_OUT_OF_MEM;
    }
    LIBISO_FREE_MEM(comments);
    LIBISO_FREE_MEM(buf);
    return ret;
}

static
int iso_analyze_gpt_head(IsoImage *image, IsoDataSource *src, int flag)
{
    struct iso_imported_sys_area *sai;
    uint8_t *head;
    uint32_t crc;
    uint64_t part_start;
    int ret;
    unsigned char *crc_buf = NULL;

    sai = image->imported_sa_info;
    head = ((uint8_t *) image->system_area_data) + 512;
    LIBISO_ALLOC_MEM(crc_buf, unsigned char, 512);

    /* Is this a GPT header with digestible parameters ? */
    ret = iso_seems_usable_gpt_head(head, 0);
    if (ret <= 0)
        goto ex;
    memcpy(crc_buf, head, 512);
    memset(crc_buf + 16, 0, 4); /* CRC is computed when head_crc is 0 */
    sai->gpt_head_crc_found = iso_read_lsb(head + 16, 4);
    sai->gpt_head_crc_should = iso_crc32_gpt((unsigned char *) crc_buf, 92, 0);
    if (sai->gpt_head_crc_found != sai->gpt_head_crc_should) {
        /* There was a bug during libisofs-1.2.4 to libisofs-1.2.8
           (fixed in rev 1071). So accept the buggy CRC if it matches the
           whole GPT header block. */
        crc = iso_crc32_gpt((unsigned char *) crc_buf, 512, 0);
        if (sai->gpt_head_crc_found != crc)
            {ret = 0; goto ex;}
    }
    part_start = iso_read_lsb64(head + 72);
    sai->gpt_max_entries = iso_read_lsb(head + 80, 4);
    if (part_start + (sai->gpt_max_entries + 3) / 4 > 64)
        {ret = 0; goto ex;}

    /* Fetch desired information */
    memcpy(sai->gpt_disk_guid, head + 56, 16);
    sai->gpt_part_start = part_start;
    sai->gpt_backup_lba = iso_read_lsb64(head + 32);
    sai->gpt_first_lba = iso_read_lsb64(head + 40);
    sai->gpt_last_lba = iso_read_lsb64(head + 48);
    sai->gpt_array_crc_found = iso_read_lsb(head + 88, 4);
    sai->gpt_array_crc_should =
                      iso_crc32_gpt((unsigned char *) image->system_area_data +
                                    sai->gpt_part_start * 512,
                                    sai->gpt_max_entries * 128, 0);

    ret = iso_analyze_gpt_backup(image, src, 0);
    if (ret < 0)
        goto ex;

    ret = 1;
ex:
    LIBISO_FREE_MEM(crc_buf);
    return ret;
}

static
int iso_analyze_gpt(IsoImage *image, IsoDataSource *src, int flag)
{
    int ret, i, j;
    uint64_t start_block, block_count, flags, end_block, j_end, j_start;
    uint8_t *part;
    struct iso_imported_sys_area *sai;

    sai = image->imported_sa_info;

    ret = iso_analyze_gpt_head(image, src, 0);
    if (ret <= 0)
        return ret;

    for (i = 0; i < (int) sai->gpt_max_entries; i++) {
        part = ((uint8_t *) image->system_area_data) +
               sai->gpt_part_start * 512 + 128 * i;
        for (j = 0; j < 128; j++)
            if (part[j])
        break;
        if (j >= 128) /* all zero, invalid entry */
    continue;
        start_block = iso_read_lsb64(part + 32);
        block_count = iso_read_lsb64(part + 40);
        flags = iso_read_lsb64(part + 48);
        if ((start_block == 0 && block_count == 0) ||
            block_count + 1 < start_block)
    continue;
        block_count = block_count + 1 - start_block;
        if (sai->gpt_req == NULL) {
            sai->gpt_req = calloc(ISO_GPT_ENTRIES_MAX,
                                  sizeof(struct iso_gpt_partition_request *));
            if (sai->gpt_req == NULL)
                return ISO_OUT_OF_MEM;
        }
        ret = iso_quick_gpt_entry(sai->gpt_req, &(sai->gpt_req_count),
                                  start_block, block_count,
                                  part, part + 16, flags, part + 56);
        if (ret < 0)
            return ret;
        sai->gpt_req[sai->gpt_req_count - 1]->idx = i + 1;
    }

    /* sai->gpt_req_flags :
          bit0= GPT partitions may overlap
          >>> bit1= with bit0: neatly nested partitions
                    without  : neatly divided disk 
    */
    for (i = 0; i < (int) sai->gpt_req_count && !(sai->gpt_req_flags & 1);
         i++) {
        if (sai->gpt_req[i]->block_count == 0)
    continue;
        start_block = sai->gpt_req[i]->start_block;
        end_block = start_block + sai->gpt_req[i]->block_count;
        for (j = i + 1; j < (int) sai->gpt_req_count; j++) {
            if (sai->gpt_req[j]->block_count == 0)
        continue;
            j_start = sai->gpt_req[j]->start_block;
            j_end = j_start + sai->gpt_req[j]->block_count;
            if ((start_block <= j_start && j_start < end_block) ||
                (start_block <= j_end   && j_end   < end_block) ||
                (j_start <= start_block && start_block < j_end)) {
                sai->gpt_req_flags |= 1;
        break;
            }
        }
    }

    /* Check first GPT partition for ISO partition offset */
    if (sai->partition_offset == 0 && sai->mbr_req_count > 0 &&
        sai->gpt_req_count > 0) {
        if (sai->mbr_req[0]->type_byte == 0xee &&
            sai->mbr_req[0]->start_block == 1) { /* protective MBR */
            start_block = sai->gpt_req[0]->start_block;
            block_count = sai->gpt_req[0]->block_count;
            if (start_block >= 64 && block_count >= 72 &&
                start_block <= 2048 && start_block % 4 == 0 &&
                block_count % 4 == 0 &&
                (start_block + block_count) / 4 == sai->image_size) {

                ret = iso_analyze_partition_offset(image, src, start_block, 0);
                if (ret < 0)
                    return ret;
            }
        }
    }

    return 1;
}


static
int iso_analyze_apm_head(IsoImage *image, IsoDataSource *src, int flag)
{
    struct iso_imported_sys_area *sai;
    char *sad;
    uint32_t block_size;

    sai = image->imported_sa_info;
    sad = image->system_area_data;

    if (sad[0] != 'E' || sad[1] != 'R')
        return 0;
    block_size = iso_read_msb(((uint8_t *) sad) + 2, 2);
    if (block_size != 2048 && block_size != 512)
        return 0;
    sai->apm_block_size = block_size;
    sai->apm_req_flags |= 4 | 2; /* start_block and block_count are in
                                    block_size units, do not fill gaps */
    return 1;
}

static
int iso_analyze_apm(IsoImage *image, IsoDataSource *src, int flag)
{
    int ret, i;
    uint32_t map_entries, start_block, block_count, flags;
    char *sad, *part, name[33], type_string[33];
    struct iso_imported_sys_area *sai;

    sai = image->imported_sa_info;
    sad = image->system_area_data;

    ret = iso_analyze_apm_head(image, src, 0);
    if (ret <= 0)
        return ret;

    part = sad + sai->apm_block_size;
    map_entries = iso_read_msb(((uint8_t *) part) + 4, 4);
    for (i = 0; i < (int) map_entries; i++) {
        part = sad + (i + 1) * sai->apm_block_size;
        if (part[0] != 'P' || part[1] != 'M')
    break;
        flags = iso_read_msb(((uint8_t *) part) + 88, 4);
        if (!(flags & 3))
    continue;
        memcpy(type_string, part + 48, 32);
        type_string[32] = 0;
        if(strcmp(type_string, "Apple_partition_map") == 0)
    continue;
        start_block = iso_read_msb(((uint8_t *) part) + 8, 4);
        block_count = iso_read_msb(((uint8_t *) part + 12), 4);
        memcpy(name, part + 16, 32);
        name[32] = 0;
        if (sai->apm_req == NULL) {
            sai->apm_req = calloc(ISO_APM_ENTRIES_MAX,
                                  sizeof(struct iso_apm_partition_request *));
            if (sai->apm_req == NULL)
                return ISO_OUT_OF_MEM;
        }
        ret = iso_quick_apm_entry(sai->apm_req, &(sai->apm_req_count),
                                  start_block, block_count, name, type_string);
        if (ret <= 0)
            return ret;
        if (strncmp(name, "Gap", 3) == 0 &&
            strcmp(type_string, "ISO9660_data") == 0) {
            if ('0' <= name[3] && name[3] <= '9' && (name[4] == 0 ||
                 ('0' <= name[4] && name[4] <= '9' && name[5] == 0))) {
                sai->apm_gap_count++;
                sai->apm_req_flags &= ~2;
            }
        }
    }
    return 1;
}

static
int iso_analyze_mips(IsoImage *image, IsoDataSource *src, int flag)
{
    int ret = 0, spt, bps, i, j, idx;
    uint32_t magic, chk, head_chk;
    char *sad;
    uint8_t *usad, *upart;
    struct iso_imported_sys_area *sai;
    IsoNode *node;

    sai = image->imported_sa_info;
    sad = image->system_area_data;
    usad = (uint8_t *) sad;

    magic = iso_read_msb(usad, 4);
    if (magic != 0x0be5a941)
        return 0;
    spt = iso_read_msb(usad + 38, 2);
    bps = iso_read_msb(usad + 40, 2);
    if (spt != 32 || bps != 512)
        return 0;
    chk = 0;
    for (i = 0; i < 504; i += 4)
        chk -= iso_read_msb(usad + i, 4);
    head_chk = iso_read_msb(usad + 504, 4);
    if (chk != head_chk)
        return 0;

    /* Verify that partitions 1 to 8 are empty */
    for (j = 312; j < 408; j++)
        if (sad[j])
            return 0;

    /* >>> verify that partitions 9 and 10 match the image size */;

    for (i = 0; i < 15; i++) {
        upart = usad + 72 + 16 * i;
        for (j = 0; j < 16; j++)
            if (upart[j])
        break;
        if (j == 16)
    continue;
        if (sai->mips_vd_entries == NULL) {
            sai->mips_boot_file_paths = calloc(15, sizeof(char *));
            sai->mips_vd_entries = calloc(15,
                                       sizeof(struct iso_mips_voldir_entry *));
            if (sai->mips_vd_entries == NULL ||
                sai->mips_boot_file_paths == NULL)
                return ISO_OUT_OF_MEM;
            sai->num_mips_boot_files = 0;
            for (j = 0; j < 15; j++) {
                sai->mips_boot_file_paths[j] = NULL;
                sai->mips_vd_entries[j] = NULL;
            }
        }

        /* Assess boot file entry */
        if (sai->num_mips_boot_files >= 15)
            return ISO_BOOT_TOO_MANY_MIPS;
        idx = sai->num_mips_boot_files;
        sai->mips_vd_entries[idx] =
                               calloc(1, sizeof(struct iso_mips_voldir_entry));
        if (sai->mips_vd_entries[idx] == NULL)
            return ISO_OUT_OF_MEM;
        memcpy(sai->mips_vd_entries[idx]->name, upart, 8);
        sai->mips_vd_entries[idx]->name[8] = 0;
        sai->mips_vd_entries[idx]->boot_block = iso_read_msb(upart + 8, 4);
        sai->mips_vd_entries[idx]->boot_bytes = iso_read_msb(upart + 12, 4);
        ret = iso_tree_get_node_of_block(image, NULL,
                                     sai->mips_vd_entries[idx]->boot_block / 4,
                                     &node, NULL, 0);
        if (ret > 0)
            sai->mips_boot_file_paths[idx] = iso_tree_get_node_path(node);
        sai->num_mips_boot_files++;
    }
    if (sai->num_mips_boot_files > 0)
        sai->system_area_options = (1 << 2);/* MIPS Big Endian Volume Header */

    return ret;
}

static
int iso_analyze_mipsel(IsoImage *image, IsoDataSource *src, int flag)
{
    int ret = 0, i, section_count;
    char *sad;
    uint8_t *usad;
    uint32_t magic;
    struct iso_imported_sys_area *sai;
    IsoNode *node;
    IsoFile *file;
    struct iso_file_section *sections = NULL;

    sai = image->imported_sa_info;
    sad = image->system_area_data;
    usad = (uint8_t *) sad;

    for (i = 0; i < 8; i++)
        if (sad[i])
            return 0;
    magic = iso_read_lsb(usad + 8, 4);
    if (magic != 0x0002757a)
        return 0;

    sai->mipsel_p_vaddr = iso_read_lsb(usad + 16, 4);
    sai->mipsel_e_entry = iso_read_lsb(usad + 20, 4);
    sai->mipsel_p_filesz = iso_read_lsb(usad + 24, 4) * 512;
    sai->mipsel_seg_start = iso_read_lsb(usad + 28, 4);
    ret = iso_tree_get_node_of_block(image, NULL, sai->mipsel_seg_start / 4,
                                     &node, NULL, 0);
    if (ret > 0) {
        sai->mipsel_boot_file_path = iso_tree_get_node_path(node);
        file = (IsoFile *) node;
        ret = iso_file_get_old_image_sections(file, &section_count,
                                              &sections, 0);
        if (ret > 0 && section_count > 0) {
            if (sections[0].block < (1 << 30) &&
                sections[0].block * 4 < sai->mipsel_seg_start)
                sai->mipsel_p_offset = sai->mipsel_seg_start -
                                       sections[0].block * 4;
            free(sections);
        }
    }
    /* DEC Boot Block for MIPS Little Endian */
    sai->system_area_options = (2 << 2);

    return 1;
}

static
int iso_analyze_sun(IsoImage *image, IsoDataSource *src, int flag)
{
    int ret = 0, i, idx;
    char *sad;
    uint8_t *usad, checksum[2];
    uint16_t perms;
    uint64_t last_core_block;
    struct iso_imported_sys_area *sai;
    IsoNode *node;

    sai = image->imported_sa_info;
    sad = image->system_area_data;
    usad = (uint8_t *) sad;

    if (iso_read_msb(usad + 128, 4) != 1 ||
        iso_read_msb(usad + 140, 2) != 8 ||
        iso_read_msb(usad + 188, 4) != 0x600ddeee ||
        iso_read_msb(usad + 430, 2) != 1 ||
        iso_read_msb(usad + 508, 2) != 0xdabe)
        return 0;
    if (iso_read_msb(usad + 142, 2) != 4 ||
        iso_read_msb(usad + 144, 2) != 0x10 ||
        iso_read_msb(usad + 444, 4) != 0 ||
        sai->image_size > 0x3fffffff ||
        iso_read_msb(usad + 448, 4) < ((int64_t) sai->image_size * 4) - 600 ||
        iso_read_msb(usad + 448, 4) > sai->image_size * 4)
        return 0;
    checksum[0] = checksum[1] = 0;
    for (i = 0; i < 510; i += 2) {
        checksum[0] ^= usad[i];
        checksum[1] ^= usad[i + 1];
    }
    if (checksum[0] != usad[510] || checksum[1] != usad[511])
        return 0;

    sai->sparc_disc_label = calloc(1, 129);
    if (sai->sparc_disc_label == NULL)
        return ISO_OUT_OF_MEM;
    memcpy(sai->sparc_disc_label, sad, 128);
    sai->sparc_disc_label[128] = 0;
    sai->sparc_heads_per_cyl = iso_read_msb(usad + 436, 2);
    sai->sparc_secs_per_head = iso_read_msb(usad + 438, 2);

    for (i = 0; i < 8; i++) {
        perms = iso_read_msb(usad + 144 + 4 * i, 2);
        if (perms == 0)
    continue;
        if (sai->sparc_entries == NULL) {
            sai->sparc_entries = calloc(8,
                                      sizeof(struct iso_sun_disk_label_entry));
            if (sai->sparc_entries == NULL)
                return ISO_OUT_OF_MEM;
        }
        idx = sai->sparc_entry_count;
        sai->sparc_entries[idx].idx = i + 1;
        sai->sparc_entries[idx].id_tag = iso_read_msb(usad + 142 + 4 * i, 2);
        sai->sparc_entries[idx].permissions = perms;
        sai->sparc_entries[idx].start_cyl =
                                           iso_read_msb(usad + 444 + 8 * i, 4);
        sai->sparc_entries[idx].num_blocks =
                                           iso_read_msb(usad + 448 + 8 * i, 4);
        sai->sparc_entry_count++;
    }

    /* GRUB2 SUN SPARC Core File Address */
    sai->sparc_grub2_core_adr = iso_read_msb64(usad + 552);
    sai->sparc_grub2_core_size = iso_read_msb(usad + 560, 4);
    last_core_block = (sai->sparc_grub2_core_adr +
                       sai->sparc_grub2_core_size + 2047) / 2048;
    if (last_core_block > 0)
        last_core_block--;
    if (last_core_block > 17 && last_core_block < sai->image_size) {
        ret = iso_tree_get_node_of_block(image, NULL,
                                         (uint32_t) last_core_block, &node,
                                         NULL, 0);
        if (ret > 0) {
            iso_node_ref(node);
            sai->sparc_core_node = (IsoFile *) node;
        }
    } else {
        sai->sparc_grub2_core_adr = 0;
        sai->sparc_grub2_core_size = 0;
    }

    /* SUN Disk Label for SUN SPARC */
    sai->system_area_options = (3 << 2);
    
    return 1;
}

static
int iso_analyze_hppa(IsoImage *image, IsoDataSource *src, int flag)
{
    int ret = 0, i, cmd_adr, cmd_len;
    char *sad, *paths[4];
    uint8_t *usad;
    uint16_t magic;
    uint32_t adrs[4];
    struct iso_imported_sys_area *sai;
    IsoNode *node;

    sai = image->imported_sa_info;
    sad = image->system_area_data;
    usad = (uint8_t *) sad;

    magic = iso_read_msb(usad, 2);
    if (magic != 0x8000 || strncmp(sad + 2, "PALO", 5) != 0 ||
        sad[7] < 4 || sad[7] > 5)
        return 0;

    sai->hppa_hdrversion = sad[7];
    if (sai->hppa_hdrversion == 4) {
        cmd_len = 127;
        cmd_adr = 24;
    } else {
        cmd_len = 1023;
        cmd_adr = 1024;
    }
    sai->hppa_cmdline = calloc(1, cmd_len + 1);
    if (sai->hppa_cmdline == NULL)
        return ISO_OUT_OF_MEM;
    memcpy(sai->hppa_cmdline, sad + cmd_adr, cmd_len);
        sai->hppa_cmdline[cmd_len] = 0;
    adrs[0] = sai->hppa_kern32_adr = iso_read_msb(usad + 8, 4);
    sai->hppa_kern32_len = iso_read_msb(usad + 12, 4);
    adrs[1] = sai->hppa_kern64_adr = iso_read_msb(usad + 232, 4);
    sai->hppa_kern64_len = iso_read_msb(usad + 236, 4);
    adrs[2] = sai->hppa_ramdisk_adr = iso_read_msb(usad + 16, 4);
    sai->hppa_ramdisk_len = iso_read_msb(usad + 20, 4);
    adrs[3] = sai->hppa_bootloader_adr = iso_read_msb(usad + 240, 4);
    sai->hppa_bootloader_len = iso_read_msb(usad + 244, 4);
    for (i = 0; i < 4; i++) {
        paths[i] = NULL;
        ret = iso_tree_get_node_of_block(image, NULL, adrs[i] / 2048,
                                         &node, NULL, 0);
        if (ret > 0)
            paths[i] = iso_tree_get_node_path(node);
    }
    sai->hppa_kernel_32 = paths[0];
    sai->hppa_kernel_64 = paths[1];
    sai->hppa_ramdisk = paths[2];
    sai->hppa_bootloader = paths[3];

    if (sai->hppa_hdrversion == 5)
        sai->hppa_ipl_entry = iso_read_msb(usad + 248, 4);

    /* HP-PA PALO boot sector version 4 or 5 for HP PA-RISC */
    sai->system_area_options = (sai->hppa_hdrversion << 2);

    return 1;
}

static
int iso_analyze_alpha_boot(IsoImage *image, IsoDataSource *src, int flag)
{
    int ret = 0, i, section_count;
    char *sad;
    uint8_t *usad;
    struct iso_imported_sys_area *sai;
    IsoNode *node;
    IsoFile *file;
    uint64_t checksum_found, checksum_should = 0, size;
    struct iso_file_section *sections = NULL;

    sai = image->imported_sa_info;
    sad = image->system_area_data;
    usad = (uint8_t *) sad;

    checksum_found = iso_read_lsb64(usad + 504);
    for (i = 0; i < 63; i++)
       checksum_should += iso_read_lsb64(usad + 8 * i);
    if (checksum_found != checksum_should)
       return 0;
    sai->alpha_boot_image = NULL;
    sai->alpha_boot_image_size = (uint64_t) iso_read_lsb64(usad + 480);
    sai->alpha_boot_image_adr = (uint64_t) iso_read_lsb64(usad + 488);
    ret = iso_tree_get_node_of_block(image, NULL,
                                  (uint32_t) (sai->alpha_boot_image_adr / 4),
                                  &node, NULL, 0);
    if (ret > 0) {
       if (iso_node_get_type(node) != LIBISO_FILE)
           return 0;
       file = (IsoFile *) node;
       ret = iso_file_get_old_image_sections(file, &section_count,
                                             &sections, 0);
       if (ret > 0 && section_count > 0) {
           size = sections[0].size / 512 + !!(sections[0].size % 512);
           free(sections);
           if (size != sai->alpha_boot_image_size)
               return 0;
       }
       sai->alpha_boot_image = iso_tree_get_node_path(node);
    } else if (strncmp(sad, "Linux/Alpha aboot for ISO filesystem.", 37) != 0
               || sad[37] != 0) {
        return 0; /* Want to see either boot file or genisoimage string */
    }
    sai->system_area_options = (6 << 2);
    return 1;
}


struct iso_impsysa_result {
    char *buf;
    int byte_count;
    char **lines;
    int line_count;
};

static
int iso_impsysa_result_new(struct iso_impsysa_result **r, int flag)
{
    int ret;

    LIBISO_ALLOC_MEM(*r, struct iso_impsysa_result, 1);
    (*r)->buf = NULL;
    (*r)->lines = NULL;
    ret = 1;
ex:
    if (ret <= 0) {
       LIBISO_FREE_MEM(*r);
       *r = NULL;
    }
    return ret;
}

static
void iso_impsysa_result_destroy(struct iso_impsysa_result **r, int flag)
{
    if (*r == NULL)
        return;
    if ((*r)->buf != NULL)
        free((*r)->buf);
    if ((*r)->lines != NULL)
        free((*r)->lines);
    free(*r);
    *r = NULL;
}

static
void iso_impsysa_line(struct iso_impsysa_result *target, char *msg)
{
    if (target->buf != NULL)
        strcpy(target->buf + target->byte_count, msg);
    if (target->lines != NULL)
        target->lines[target->line_count] = target->buf + target->byte_count;
    target->byte_count += strlen(msg) + 1;
    target->line_count++;
}

static
void iso_impsysa_report_text(struct iso_impsysa_result  *target,
                             char *msg, char *path, int flag)
{
    if (strlen(msg) + strlen(path) >= ISO_MAX_SYSAREA_LINE_LENGTH)
        sprintf(msg + strlen(msg), "(too long to show here)");
    else
        strcat(msg, path);
    iso_impsysa_line(target, msg);
}

static
void iso_impsysa_reduce_na(uint32_t block, uint32_t *na, uint32_t claim)
{
    if ((*na == 0 || *na > claim) && block < claim)
        *na = claim;
}

static
int iso_impsysa_reduce_next_above(IsoImage *image, uint32_t block,
                                  uint32_t *next_above, int flag)
{
    int i, section_count, ret;
    struct iso_imported_sys_area *sai;
    struct el_torito_boot_image *img;
    struct iso_file_section *sections = NULL;

    sai = image->imported_sa_info;

    /* PVD, path table, root directory of active and of first session */
    for (i = 0; i < sai->num_meta_struct_blocks; i++)
        iso_impsysa_reduce_na(block, next_above, sai->meta_struct_blocks[i]); 

    /* Partition tables */
    for (i = 0; i < sai->mbr_req_count; i++) {
        iso_impsysa_reduce_na(block, next_above,
                              (uint32_t) (sai->mbr_req[i]->start_block / 4));
        iso_impsysa_reduce_na(block, next_above,
                              (uint32_t) ((sai->mbr_req[i]->start_block +
                                           sai->mbr_req[i]->block_count) / 4));
    }
    for (i = 0; i < sai->gpt_req_count; i++) {
        iso_impsysa_reduce_na(block, next_above,
                              (uint32_t) (sai->gpt_req[i]->start_block / 4));
        iso_impsysa_reduce_na(block, next_above,
                              (uint32_t) ((sai->gpt_req[i]->start_block +
                                           sai->gpt_req[i]->block_count) / 4));
    }
    for (i = 0; i < sai->apm_req_count; i++) {
        iso_impsysa_reduce_na(block, next_above,
                              (uint32_t) (sai->apm_req[i]->start_block /
                                          (2048 / sai->apm_block_size)));
        iso_impsysa_reduce_na(block, next_above,
                              (uint32_t) ((sai->apm_req[i]->start_block +
                                           sai->apm_req[i]->block_count) /
                                           (2048 / sai->apm_block_size)));
    }
    if (image->bootcat != NULL) {
        if (image->bootcat->node != NULL)
            iso_impsysa_reduce_na(block, next_above,
                                  image->bootcat->node->lba);
        for (i= 0; i < image->bootcat->num_bootimages; i++) {
            img = image->bootcat->bootimages[i];
            ret = iso_file_get_old_image_sections(img->image, &section_count,
                                                  &sections, 0);
            if (ret > 0 && section_count > 0)
                if (block != sections[0].block)
                    iso_impsysa_reduce_na(block, next_above,
                                          sections[0].block);
            if (sections != NULL) {
                free(sections);
                sections = NULL;
            }
        }
    }

    iso_impsysa_reduce_na(block, next_above, sai->image_size);

    return ISO_SUCCESS;
}

/* @param flag bit0= try to estimate the size if no path is found
*/
static
void iso_impsysa_report_blockpath(IsoImage *image,
                                  struct iso_impsysa_result *target, char *msg,
                                  uint32_t start_block, int flag)
{
    int ret;
    char *path = NULL, *cpt;
    IsoNode *node;
    uint32_t next_above = 0;
    uint32_t size;

    ret = iso_tree_get_node_of_block(image, NULL, start_block,
                                     &node, &next_above, 0);
    if (ret <= 0) {
        if (!(flag & 1))
            return;
        /* Look for next claimed block for estimating file size.
           next_above already holds the best data file candidate.
        */
        ret = iso_impsysa_reduce_next_above(image, start_block, &next_above, 0);
        if (ret < 0)
            return;
        if (next_above == 0)
            return;
        size = next_above - start_block;

        /* Replace in msg "path" by "blks", report number in bytes */
        cpt = strstr(msg, "path");
        if (cpt == NULL)
            return;
        path = iso_alloc_mem(strlen(msg) + 20, 1, 0);
        if (path == NULL)
            return;
        strcpy(path, msg);
        memcpy(path + (cpt - msg), "blks", 4);
        sprintf(path + strlen(path), "%u", (unsigned int) size);
        iso_impsysa_report_text(target, path, "", 0);
        free(path);
        return;
    }
    path = iso_tree_get_node_path(node);
    if (path != NULL) {
        iso_impsysa_report_text(target, msg, path, 0);
        free(path);
    }
}

static
int iso_impsysa_report(IsoImage *image, struct iso_impsysa_result *target,
                       int flag)
{
    char *msg = NULL, *local_name = NULL, *path;
    int i, j, sa_type, sao, sa_sub, ret, idx;
    size_t local_len;
    struct iso_imported_sys_area *sai;
    struct iso_mbr_partition_request *part;
    struct iso_gpt_partition_request *gpt_entry;
    struct iso_apm_partition_request *apm_entry;
    static char *alignments[4] = {"auto", "on", "off", "all"};
    IsoWriteOpts *opts = NULL;
    struct iso_sun_disk_label_entry *sparc_entry;

    sai = image->imported_sa_info;

    LIBISO_ALLOC_MEM(msg, char, ISO_MAX_SYSAREA_LINE_LENGTH);

    if (sai == NULL)
        {ret = 0; goto ex;}
    if (!sai->is_not_zero)
        {ret = 0; goto ex;}
    sao = sai->system_area_options;
    sprintf(msg, "System area options: 0x%-8.8x", (unsigned int) sao);
    iso_impsysa_line(target, msg);

    /* Human readable form of system_area_options */
    sa_type = (sao >> 2) & 63;
    sa_sub = (sao >> 10) & 15;
    strcpy(msg, "System area summary:");
    if (sa_type == 0) {
        if ((sao & 3) || sa_sub == 1 || sa_sub == 2) {
            strcat(msg, " MBR");
            if (sao & 1)
                strcat(msg, " protective-msdos-label");
            else if (sao & 2)
                strcat(msg, " isohybrid");
            else if (sa_sub == 1)
                strcat(msg, " CHRP");
            if ((sao & (1 << 14)) && !(sao & 2))
                strcat(msg, " grub2-mbr");
            sprintf(msg + strlen(msg), " cyl-align-%s",
                                       alignments[(sao >> 8) & 3]);
        } else if (sai->prep_part_start > 0 && sai->prep_part_size > 0) {
            strcat(msg, " PReP");
        } else if (sai->mbr_req_count > 0) {
            strcat(msg, " MBR");
        } else {
            strcat(msg, " not-recognized");
        }
    } else if (sa_type == 1) {
        strcat(msg, " MIPS-Big-Endian");
    } else if (sa_type == 2) {
        strcat(msg, " MIPS-Little-Endian");
    } else if (sa_type == 3) {
        strcat(msg, " SUN-SPARC-Disk-Label");
    } else if (sa_type == 4 || sa_type == 5) {
        sprintf(msg + strlen(msg), " HP-PA-PALO");
    } else if (sa_type == 6) {
        sprintf(msg + strlen(msg), " DEC-Alpha");
    } else {
        sprintf(msg + strlen(msg), " unkown-system-area-type-%d", sa_type);
    }
    if (sai->gpt_req_count > 0)
        strcat(msg, " GPT");
    if (sai->apm_req_count > 0)
        strcat(msg, " APM");

    iso_impsysa_line(target, msg); /* System area summary */

    sprintf(msg, "ISO image size/512 : %.f",
                 ((double) sai->image_size) * 4.0);
    iso_impsysa_line(target, msg);
    if (sai->mbr_req_count > 0 && sa_type == 0) {
        sprintf(msg, "Partition offset   : %d", sai->partition_offset);
        iso_impsysa_line(target, msg);
    }
    if (sa_type >= 4 && sa_type <= 5) {
        sprintf(msg, "PALO header version: %d", sai->hppa_hdrversion);
        iso_impsysa_line(target, msg);
        sprintf(msg, "HP-PA cmdline      : ");
        iso_impsysa_report_text(target, msg, sai->hppa_cmdline, 0);
        sprintf(msg, "HP-PA boot files   :   ByteAddr    ByteSize  Path");
        iso_impsysa_line(target, msg);
        sprintf(msg, "HP-PA 32-bit kernel: %10u  %10u  ",
                     sai->hppa_kern32_adr, sai->hppa_kern32_len);
        iso_impsysa_report_text(target, msg,
                                sai->hppa_kernel_32 != NULL ?
                                sai->hppa_kernel_32 : "(not found in ISO)", 0);
        sprintf(msg, "HP-PA 64-bit kernel: %10u  %10u  ",
                     sai->hppa_kern64_adr, sai->hppa_kern64_len);
        iso_impsysa_report_text(target, msg,
                                sai->hppa_kernel_64 != NULL ?
                                sai->hppa_kernel_64 : "(not found in ISO)", 0);
        sprintf(msg, "HP-PA ramdisk      : %10u  %10u  ",
                     sai->hppa_ramdisk_adr, sai->hppa_ramdisk_len);
        iso_impsysa_report_text(target, msg,
                                sai->hppa_ramdisk != NULL ?
                                sai->hppa_ramdisk : "(not found in ISO)", 0);
        sprintf(msg, "HP-PA bootloader   : %10u  %10u  ",
                     sai->hppa_bootloader_adr, sai->hppa_bootloader_len);
        iso_impsysa_report_text(target, msg,
                                sai->hppa_bootloader != NULL ?
                                sai->hppa_bootloader : "(not found in ISO)", 0);
    } else if (sa_type == 6) {
        sprintf(msg, "DEC Alpha ldr size : %.f",
                     (double) sai->alpha_boot_image_size);
        iso_impsysa_line(target, msg);
        sprintf(msg, "DEC Alpha ldr adr  : %.f",
                     (double) sai->alpha_boot_image_adr);
        iso_impsysa_line(target, msg);
        if (sai->alpha_boot_image != NULL) {
            sprintf(msg, "DEC Alpha ldr path : %s", sai->alpha_boot_image);
            iso_impsysa_line(target, msg);
        }
    }
    if (sai->mbr_req_count > 0) {
        sprintf(msg, "MBR heads per cyl  : %d", sai->partition_heads_per_cyl);
        iso_impsysa_line(target, msg);
        sprintf(msg, "MBR secs per head  : %d", sai->partition_secs_per_head);
        iso_impsysa_line(target, msg);
        sprintf(msg,
            "MBR partition table:   N Status  Type        Start       Blocks");
        iso_impsysa_line(target, msg);
    }
    for (i = 0; i < sai->mbr_req_count; i++) {
        part = sai->mbr_req[i];
        sprintf(msg,
                "MBR partition      : %3d   0x%2.2x  0x%2.2x  %11.f  %11.f",
                part->desired_slot,
                (unsigned int) part->status_byte,
                (unsigned int) part->type_byte,
                (double) part->start_block, (double) part->block_count);
        iso_impsysa_line(target, msg);
    }
    for (i = 0; i < sai->mbr_req_count; i++) {
        part = sai->mbr_req[i];
        if (part->block_count == 0)
    continue;
        sprintf(msg, "MBR partition path : %3d  ", part->desired_slot);
        iso_impsysa_report_blockpath(image, target, msg,
                                     (uint32_t) (part->start_block / 4), 0);
    }
    if (sai->prep_part_start > 0 && sai->prep_part_size > 0) {
        sprintf(msg, "PReP boot partition: %u  %u",
                     sai->prep_part_start, sai->prep_part_size);
        iso_impsysa_line(target, msg);
    }

    if (sa_type == 1) {
        sprintf(msg,
                "MIPS-BE volume dir :   N      Name       Block       Bytes");
        iso_impsysa_line(target, msg);
        for (i = 0; i < sai->num_mips_boot_files; i++) {
            sprintf(msg,
                    "MIPS-BE boot entry : %3d  %8s  %10u  %10u",
                    i + 1, sai->mips_vd_entries[i]->name,
                    sai->mips_vd_entries[i]->boot_block,
                    sai->mips_vd_entries[i]->boot_bytes);
            iso_impsysa_line(target, msg);
            if (sai->mips_boot_file_paths[i] != NULL) {
                sprintf(msg, "MIPS-BE boot path  : %3d  ", i + 1);
                iso_impsysa_report_text(target, msg,
                                        sai->mips_boot_file_paths[i], 0);
            }
        }
    } else if (sa_type == 2) {
        sprintf(msg,
      "MIPS-LE boot map   :   LoadAddr    ExecAddr SegmentSize SegmentStart");
        iso_impsysa_line(target, msg);
        sprintf(msg, "MIPS-LE boot params: %10u  %10u  %10u  %10u",
                sai->mipsel_p_vaddr, sai->mipsel_e_entry, sai->mipsel_p_filesz,
                sai->mipsel_seg_start);
        iso_impsysa_line(target, msg);
        if (sai->mipsel_boot_file_path != NULL) {
            sprintf(msg, "MIPS-LE boot path  : ");
            iso_impsysa_report_text(target, msg,
                                    sai->mipsel_boot_file_path, 0);
            sprintf(msg, "MIPS-LE elf offset : %u", sai->mipsel_p_offset);
            iso_impsysa_line(target, msg);
        }
    } else if (sa_type == 3) {
        sprintf(msg, "SUN SPARC disklabel: %s", sai->sparc_disc_label);
        iso_impsysa_line(target, msg);
        sprintf(msg, "SUN SPARC secs/head: %d", sai->sparc_secs_per_head);
        iso_impsysa_line(target, msg);
        sprintf(msg, "SUN SPARC heads/cyl: %d", sai->sparc_heads_per_cyl);
        iso_impsysa_line(target, msg);
        sprintf(msg,
           "SUN SPARC partmap  :   N   IdTag   Perms    StartCyl   NumBlocks");
        iso_impsysa_line(target, msg);
        for (i = 0; i < sai->sparc_entry_count; i++) {
            sparc_entry = sai->sparc_entries + i;
            sprintf(msg,
                    "SUN SPARC partition: %3d  0x%4.4x  0x%4.4x  %10u  %10u",
                    sparc_entry->idx,
                    sparc_entry->id_tag, sparc_entry->permissions,
                    sparc_entry->start_cyl, sparc_entry->num_blocks);
            iso_impsysa_line(target, msg);
        }
        if (sai->sparc_grub2_core_adr > 0) {
            sprintf(msg, "SPARC GRUB2 core   : %.f  %u",
                         (double) sai->sparc_grub2_core_adr,
                         sai->sparc_grub2_core_size);
            iso_impsysa_line(target, msg);
            if (sai->sparc_core_node != NULL) {
                path = iso_tree_get_node_path((IsoNode *) sai->sparc_core_node);
                if (path != NULL) {
                    sprintf(msg, "SPARC GRUB2 path   : ");
                    iso_impsysa_report_text(target, msg, path, 0);
                    free(path);
                }
            }
        }
    }

    if (sai->gpt_req_count > 0) {
        sprintf(msg, "GPT                :   N  Info");
        iso_impsysa_line(target, msg);
        if (sai->gpt_head_crc_should != sai->gpt_head_crc_found) {
            sprintf(msg,
 "GPT CRC should be  :      0x%8.8x  to match first 92 GPT header block bytes",
                    sai->gpt_head_crc_should);
            iso_impsysa_line(target, msg);
            sprintf(msg,
 "GPT CRC found      :      0x%8.8x  matches all 512 bytes of GPT header block",
                    sai->gpt_head_crc_found);
            iso_impsysa_line(target, msg);
        }
        if (sai->gpt_array_crc_should != sai->gpt_array_crc_found) {
            sprintf(msg,
                 "GPT array CRC wrong:      should be 0x%8.8x , found 0x%8.8x",
                 sai->gpt_array_crc_should, sai->gpt_array_crc_found);
            iso_impsysa_line(target, msg);
        }
        if (sai->gpt_backup_comments != NULL) {
            if (sai->gpt_backup_comments[0]) {
                sprintf(msg, "GPT backup problems:      ");
                iso_impsysa_report_text(target, msg,
                                        sai->gpt_backup_comments, 0);
            }
        }
        sprintf(msg, "GPT disk GUID      :      ");
        iso_util_bin_to_hex(msg + 26, sai->gpt_disk_guid, 16, 0);
        iso_impsysa_line(target, msg);
        sprintf(msg, "GPT entry array    :      %u  %u  %s",
                     (unsigned int) sai->gpt_part_start,
                     (unsigned int) sai->gpt_max_entries,
                     sai->gpt_req_flags & 1 ? "overlapping" : "separated");
        iso_impsysa_line(target, msg);
        sprintf(msg, "GPT lba range      :      %.f  %.f  %.f",
                     (double) sai->gpt_first_lba, (double) sai->gpt_last_lba,
                     (double) sai->gpt_backup_lba);
        iso_impsysa_line(target, msg);
	ret = iso_write_opts_new(&opts, 0);
        if (ret < 0)
            goto ex;
        ret = iso_write_opts_set_output_charset(opts, "UTF-16LE");
        if (ret < 0)
            goto ex;
    }
    for (i = 0; i < sai->gpt_req_count; i++) {
        gpt_entry = sai->gpt_req[i];
        idx = gpt_entry->idx;

        sprintf(msg, "GPT partition name : %3d  ", idx);
        for (j = 72; j >= 2; j -= 2)
            if (gpt_entry->name[j - 2] || gpt_entry->name[j - 1])
        break;
        iso_util_bin_to_hex(msg + 26, gpt_entry->name, j, 0);
        iso_impsysa_line(target, msg);
        if (j > 0)
            ret = iso_conv_name_chars(opts, (char *) gpt_entry->name, j,
                                 &local_name, &local_len, 0 | 512 | (1 << 15));
        else
            ret = 0;
        if (ret == 1 && local_len <= 228) {
            sprintf(msg, "GPT partname local : %3d  ", idx);
            memcpy(msg + 26, local_name, local_len);
            LIBISO_FREE_MEM(local_name); local_name = NULL;
            msg[26 + local_len] = 0;
            iso_impsysa_line(target, msg);
        }
        sprintf(msg, "GPT partition GUID : %3d  ", idx);
        iso_util_bin_to_hex(msg + 26, gpt_entry->partition_guid, 16, 0);
        iso_impsysa_line(target, msg);
        sprintf(msg, "GPT type GUID      : %3d  ", idx);
        iso_util_bin_to_hex(msg + 26, gpt_entry->type_guid, 16, 0);
        iso_impsysa_line(target, msg);
        sprintf(msg, "GPT partition flags: %3d  0x%8.8x%8.8x", idx,
                     (unsigned int) ((gpt_entry->flags >> 32) & 0xffffffff),
                     (unsigned int) (gpt_entry->flags & 0xffffffff));
        iso_impsysa_line(target, msg);
        sprintf(msg, "GPT start and size : %3d  %.f  %.f", idx,
                     (double) gpt_entry->start_block,
                     (double) gpt_entry->block_count);
        iso_impsysa_line(target, msg);
        if (gpt_entry->block_count == 0)
    continue;
        sprintf(msg, "GPT partition path : %3d  ", idx);
        iso_impsysa_report_blockpath(image, target, msg,
                                   (uint32_t) (gpt_entry->start_block / 4), 0);
    }

    if (sai->apm_req_count > 0) {
        sprintf(msg, "APM                :   N  Info");
        iso_impsysa_line(target, msg);
        sprintf(msg, "APM block size     :      %u", sai->apm_block_size);
        iso_impsysa_line(target, msg);
        sprintf(msg, "APM gap fillers    :      %d", sai->apm_gap_count);
        iso_impsysa_line(target, msg);
    }
    for (i = 0; i < sai->apm_req_count; i++) {
        apm_entry = sai->apm_req[i];
        idx = i + 1;
        sprintf(msg, "APM partition name : %3d  %s", idx, apm_entry->name);
        iso_impsysa_line(target, msg);
        sprintf(msg, "APM partition type : %3d  %s", idx, apm_entry->type);
        iso_impsysa_line(target, msg);
        sprintf(msg, "APM start and size : %3d  %.f  %.f", idx,
                     (double) apm_entry->start_block,
                     (double) apm_entry->block_count);
        iso_impsysa_line(target, msg);
        if (apm_entry->block_count == 0)
    continue;
        sprintf(msg, "APM partition path : %3d  ", idx);
        iso_impsysa_report_blockpath(image, target, msg,
                                     (uint32_t) (apm_entry->start_block /
                                                 (2048 / sai->apm_block_size)),
                                     0);
    }

    ret = 1;
ex:
    LIBISO_FREE_MEM(local_name);
    if (opts != NULL)
        iso_write_opts_free(opts);
    LIBISO_FREE_MEM(msg);
    return ret;
}

static
int iso_report_result_destroy(char ***result, int flag)
{
    if (*result == NULL)
        return ISO_SUCCESS;
    if ((*result)[0] != NULL) /* points to the whole multi-line buffer */
        free((*result)[0]);
    free(*result);
    *result = NULL;
    return ISO_SUCCESS;
}

static
int iso_report_help(char **doc, char ***result, int *line_count, int flag)
{
    int i, count = 0;
    char *buf = NULL;

    *line_count = 0;
    for (i = 0; strcmp(doc[i], "@END_OF_DOC@") != 0; i++)
        count += strlen(doc[i]) + 1;
    if (i == 0)
        return ISO_SUCCESS;
    *result = calloc(i, sizeof(char *));
    if (*result == NULL)
        return ISO_OUT_OF_MEM;
    buf = calloc(1, count);
    if (buf == NULL) {
        free(*result);
        *result = NULL;
        return ISO_OUT_OF_MEM;
    }
    *line_count = i;
    count = 0;
    for (i = 0; strcmp(doc[i], "@END_OF_DOC@") != 0; i++) {
        strcpy(buf + count, doc[i]);
        (*result)[i] = buf + count;
        count += strlen(doc[i]) + 1;
    }
    return ISO_SUCCESS;
}

static
int iso_eltorito_report(IsoImage *image, struct iso_impsysa_result *target,
                        int flag)
{
    char *msg = NULL, emul_code[6], pltf[5], *path;
    int i, j, ret, section_count;
    uint32_t lba, *lba_mem = NULL;
    struct el_torito_boot_catalog *bootcat;
    IsoBoot *bootnode;
    struct el_torito_boot_image *img;
    struct iso_file_section *sections = NULL;
    static char emul_names[5][6] = {"none", "fd1.2", "fd1.4", "fd2.8", "hd"};
    static char pltf_names[3][5] = {"BIOS", "PPC", "Mac"};
    static int num_emuls = 5, num_pltf = 3;

    bootcat = image->bootcat;

    LIBISO_ALLOC_MEM(msg, char, ISO_MAX_SYSAREA_LINE_LENGTH);

    if (bootcat == NULL)
        {ret= 0; goto ex;}
    bootnode = image->bootcat->node;
    if (bootnode == NULL)
        {ret= 0; goto ex;}

    sprintf(msg, "El Torito catalog  : %u  %u",
                 (unsigned int) bootnode->lba,
                 (unsigned int) (bootnode->size + 2047) / 2048);
    iso_impsysa_line(target, msg);
    path = iso_tree_get_node_path((IsoNode *) bootnode);
    if (path != NULL) {
        sprintf(msg, "El Torito cat path : ");
        iso_impsysa_report_text(target, msg, path, 0);
        free(path);
    }
    if (bootcat->num_bootimages > 0) {
        sprintf(msg,
"El Torito images   :   N  Pltf  B   Emul  Ld_seg  Hdpt  Ldsiz         LBA");
        iso_impsysa_line(target, msg);
        LIBISO_ALLOC_MEM(lba_mem, uint32_t, bootcat->num_bootimages);
    }
    for (i= 0; i < bootcat->num_bootimages; i++) {
        img = bootcat->bootimages[i];
        if (img->type < num_emuls)
            strcpy(emul_code, emul_names[img->type]);
        else
            sprintf(emul_code, "0x%2.2x", (unsigned int) img->type);
        if (img->platform_id < num_pltf)
            strcpy(pltf, pltf_names[img->platform_id]);
        else if(img->platform_id == 0xef)
            strcpy(pltf, "UEFI");
        else
            sprintf(pltf, "0x%2.2x", (unsigned int) img->platform_id);
        lba = 0xffffffff;
        ret = iso_file_get_old_image_sections(img->image, &section_count,
                                              &sections, 0);
        if (ret > 0 && section_count > 0)
            lba = sections[0].block;
        lba_mem[i]= lba;
        if (sections != NULL) {
            free(sections);
            sections = NULL;
        }
        sprintf(msg,
         "El Torito boot img : %3d  %4s  %c  %5s  0x%4.4x  0x%2.2x  %5u  %10u",
              i + 1, pltf, img->bootable ? 'y' : 'n', emul_code,
              (unsigned int) img->load_seg, (unsigned int) img->partition_type,
              (unsigned int) img->load_size,
              (unsigned int) lba);
        iso_impsysa_line(target, msg);
    }
    for (i= 0; i < bootcat->num_bootimages; i++) {
        img = bootcat->bootimages[i];
        if (lba_mem[i] != 0xffffffff) {
            sprintf(msg, "El Torito img path : %3d  ", i + 1);
            iso_impsysa_report_blockpath(image, target, msg, lba_mem[i], 1);
        }
        sprintf(msg, "El Torito img opts : %3d  ", i + 1);
        if (img->seems_boot_info_table)
            strcat(msg, "boot-info-table ");
        if (img->seems_isohybrid_capable)
            strcat(msg, "isohybrid-suitable ");
        if (img->seems_grub2_boot_info)
            strcat(msg, "grub2-boot-info ");
        if (strlen(msg) > 27) {
            msg[strlen(msg) - 1] = 0;
            iso_impsysa_line(target, msg);
        }
        for (j = 0; j < (int) sizeof(img->id_string); j++)
            if (img->id_string[j])
        break;
        if (j < (int) sizeof(img->id_string)) {
            sprintf(msg, "El Torito id string: %3d  ", i + 1);
            iso_util_bin_to_hex(msg + strlen(msg),
                                img->id_string, 24 + 4 * (i > 0), 0);
        }
        for (j = 0; j < (int) sizeof(img->selection_crit); j++)
            if (img->selection_crit[j])
        break;
        if (j < (int) sizeof(img->selection_crit) && i > 0) {
            sprintf(msg, "El Torito sel crit : %3d  ", i + 1);
            iso_util_bin_to_hex(msg + strlen(msg),
                                img->selection_crit, 20, 0);
        }
    }

    ret = ISO_SUCCESS;    
ex:;
    LIBISO_FREE_MEM(msg);
    LIBISO_FREE_MEM(lba_mem);
    return ret;
}


/* API */
/* @param flag  bit1= do not report system area but rather reply help text
               bit15= dispose result from previous call
*/
static
int iso_image_report_boot_eqp(IsoImage *image, int what,
                              char ***result, int *line_count, int flag)
{
    int ret;
    char **doc;
    struct iso_impsysa_result *target = NULL;
    static char *sysarea_doc[] = { ISO_SYSAREA_REPORT_DOC ,
                                   ISO_SYSAREA_REPORT_DOC_MBR ,
                                   ISO_SYSAREA_REPORT_DOC_GPT1 ,
                                   ISO_SYSAREA_REPORT_DOC_GPT2 ,
                                   ISO_SYSAREA_REPORT_DOC_APM ,
                                   ISO_SYSAREA_REPORT_DOC_MIPS ,
                                   ISO_SYSAREA_REPORT_DOC_SUN ,
                                   ISO_SYSAREA_REPORT_DOC_HPPA ,
                                   ISO_SYSAREA_REPORT_DOC_ALPHA ,
                                   "@END_OF_DOC@" };
    static char *eltorito_doc[] = { ISO_ELTORITO_REPORT_DOC ,
                                    "@END_OF_DOC@" };

    if (flag & (1 << 15))
        return iso_report_result_destroy(result, 0);
    if (flag & 1) {
        if (what == 0)
            doc = sysarea_doc;
        else
            doc = eltorito_doc;
        return iso_report_help(doc, result, line_count, 0);
    }

    *result = NULL;
    *line_count = 0;
    ret = iso_impsysa_result_new(&target, 0);
    if (ret < 0)
        goto ex;
    if (what == 0)
        ret = iso_impsysa_report(image, target, 0);
    else
        ret = iso_eltorito_report(image, target, 0);
    if (ret <= 0)
        goto ex;
    target->buf = calloc(1, target->byte_count + 1);
    target->lines = calloc(target->line_count + 1, sizeof(char *));
    if (target->buf == NULL || target->lines == NULL)
        {ret = ISO_OUT_OF_MEM; goto ex;}
    target->lines[0] = target->buf; /* even if no lines get reported */
    target->byte_count = 0;
    target->line_count = 0;
    if (what == 0)
        ret = iso_impsysa_report(image, target, 0);
    else
        ret = iso_eltorito_report(image, target, 0);
    if (ret <= 0)
        goto ex;

    /* target to result */
    *result = target->lines;
    target->lines = NULL;
    target->buf = NULL;
    *line_count = target->line_count;

    ret = ISO_SUCCESS;
ex:
    iso_impsysa_result_destroy(&target, 0);
    return ret;
}

/* API */
/* @param flag  bit1= do not report system area but rather reply help text
               bit15= dispose result from previous call
*/
int iso_image_report_system_area(IsoImage *image,
                                 char ***result, int *line_count, int flag)
{
    return iso_image_report_boot_eqp(image, 0, result, line_count, flag);
}

static
int iso_record_pvd_blocks(IsoImage *image, IsoDataSource *src, uint32_t block,
                          int flag)
{
    int ret;
    uint8_t *buffer = NULL;
    struct iso_imported_sys_area *sai;

    LIBISO_ALLOC_MEM(buffer, uint8_t, 2048);

    sai = image->imported_sa_info;
    sai->meta_struct_blocks[sai->num_meta_struct_blocks++] = block;

    ret = src->read_block(src, block, buffer);
    if (ret < 0)
        goto ex;

    /* Verify that it is a PVD of a volume not larger than sai->image_size */
    if (buffer[0] != 1 || strncmp((char *) buffer + 1, "CD001", 5) != 0)
        {ret = 0; goto ex;}
    if (iso_read_lsb(buffer + 80, 4) > sai->image_size)
        {ret = 0; goto ex;}

    /* L pathtable, Opt L, M pathtable , Opt M, Root directory extent*/
    sai->meta_struct_blocks[sai->num_meta_struct_blocks++] =
                                                 iso_read_lsb(buffer + 140, 4);
    sai->meta_struct_blocks[sai->num_meta_struct_blocks++] =
                                                 iso_read_lsb(buffer + 144, 4);
    sai->meta_struct_blocks[sai->num_meta_struct_blocks++] =
                                                 iso_read_lsb(buffer + 148, 4);
    sai->meta_struct_blocks[sai->num_meta_struct_blocks++] =
                                                 iso_read_lsb(buffer + 152, 4);
    sai->meta_struct_blocks[sai->num_meta_struct_blocks++] =
                                                 iso_read_lsb(buffer + 158, 4);

    ret = ISO_SUCCESS;
ex:;
    LIBISO_FREE_MEM(buffer);
    return ret;
}

static
int iso_record_meta_struct_blocks(IsoImage *image, IsoDataSource *src,
                                  int flag)
{
    int ret;
    struct iso_imported_sys_area *sai;

    sai = image->imported_sa_info;
    ret = iso_record_pvd_blocks(image, src, sai->pvd_block, 0);
    if (ret < 0)
        goto ex;
    /* Try block 32 as first session PVD */
    ret = iso_record_pvd_blocks(image, src, 16, 0);
    if (ret < 0)
       goto ex;
    if (ret == 0 && sai->pvd_block > 16) {
       /* No emulated multi-session: Try block 16 as first session PVD */
       ret = iso_record_pvd_blocks(image, src, 16, 0);
       if (ret < 0)
           goto ex;
    }
    ret = ISO_SUCCESS;
ex:
    return ret;
}

static
int iso_analyze_system_area(IsoImage *image, IsoDataSource *src,
                            struct iso_read_opts *opts, uint32_t image_size, 
                            int flag)
{
    int ret, i, sao, sa_type, sa_sub;

    iso_imported_sa_unref(&(image->imported_sa_info), 0);
    ret = iso_imported_sa_new(&(image->imported_sa_info), 0);
    if (ret < 0)
        goto ex;

    for (i = 0; i < 32768; i++)
        if (image->system_area_data[i] != 0)
    break;
    if (i < 32768)
        image->imported_sa_info->is_not_zero = 1;

    image->imported_sa_info->image_size = image_size;
    image->imported_sa_info->pvd_block = opts->block + 16;

    ret = iso_analyze_mbr(image, src, 0);
    if (ret < 0)
        goto ex;
    ret = iso_analyze_gpt(image, src, 0);
    if (ret < 0)
        goto ex;
    ret = iso_analyze_apm(image, src, 0);
    if (ret < 0)
        goto ex;
    sao = image->imported_sa_info->system_area_options;
    sa_type = (sao >> 2) & 0x3f;
    sa_sub = (sao >> 10) & 0xf;
    if (sa_type == 0 && !((sao & 3) || sa_sub == 1 || sa_sub == 2)) {
        ret = iso_analyze_mips(image, src, 0);
        if (ret < 0)
           goto ex;
        if (ret == 0) {
            ret = iso_analyze_mipsel(image, src, 0);
            if (ret < 0)
                goto ex;
        }
        if (ret == 0) {
            ret = iso_analyze_sun(image, src, 0);
            if (ret < 0)
                goto ex;
        }
    }
    if (sa_type == 0 && !((sao & 3) || sa_sub == 1)) {
        /* HP-PA PALO v5 can look like generic MBR */
        ret = iso_analyze_hppa(image, src, 0);
        if (ret < 0)
            goto ex;
        /* DEC Alpha has checksum bytes where MBR has its magic number */
        if (ret == 0) {
            ret = iso_analyze_alpha_boot(image, src, 0);
            if (ret < 0)
                goto ex;
        }
    }
    ret = iso_record_meta_struct_blocks(image, src, 0);
    if (ret < 0)
        goto ex;

    ret = ISO_SUCCESS;
ex:;
    image->imported_sa_info->overall_return = ret;
    return ret;
}


/* API */
/* @param flag  bit1= do not report system area but rather reply help text
               bit15= dispose result from previous call
*/
int iso_image_report_el_torito(IsoImage *image,
                               char ***reply, int *line_count, int flag)
{
    return iso_image_report_boot_eqp(image, 1, reply, line_count, flag);
}


int iso_image_import(IsoImage *image, IsoDataSource *src,
                     struct iso_read_opts *opts,
                     IsoReadImageFeatures **features)
{
    int ret, hflag, i, idx;
    IsoImageFilesystem *fs;
    IsoFilesystem *fsback;
    IsoNodeBuilder *blback;
    IsoDir *oldroot;
    IsoFileSource *newroot;
    _ImageFsData *data;
    struct el_torito_boot_catalog *oldbootcat;
    uint8_t *rpt;
    IsoFileSource *boot_src;
    IsoNode *node;
    char *old_checksum_array = NULL;
    char checksum_type[81];
    uint32_t checksum_size, truncate_mode, truncate_length;
    size_t size, attr_value_length;
    char *attr_value;
    unsigned char *aa_string = NULL;
    void *ctx = NULL;
    char md5[16];
    struct el_torito_boot_catalog *catalog = NULL;
    ElToritoBootImage *boot_image = NULL;

    if (image == NULL || src == NULL || opts == NULL) {
        return ISO_NULL_POINTER;
    }

    opts->truncate_mode = image->truncate_mode;
    opts->truncate_length = image->truncate_length;
    ret = iso_image_filesystem_new(src, opts, image->id, &fs);
    if (ret < 0) {
        return ret;
    }
    data = fs->data;


    if (opts->keep_import_src) {
        iso_data_source_ref(src);
        image->import_src = src;
    }
    if (opts->load_system_area) {
        if (image->system_area_data != NULL)
            free(image->system_area_data);
        image->system_area_data = calloc(32768, 1);
        if (image->system_area_data == NULL) {
            iso_filesystem_unref(fs);
            return ISO_OUT_OF_MEM;
        }
        image->system_area_options = 0;
        /* Read 32768 bytes */
        for (i = 0; i < 16; i++) {
            rpt = (uint8_t *) (image->system_area_data + i * 2048);
            ret = src->read_block(src, opts->block + i, rpt);
            if (ret < 0) {
                iso_filesystem_unref(fs);
                return ret;
            }
        }
    }

    /* get root from filesystem */
    ret = fs->get_root(fs, &newroot);
    if (ret < 0) {
        iso_filesystem_unref(fs);
        return ret;
    }

    /* Lookup character set even if no AAIP loading is enabled */
    ret = iso_file_source_get_aa_string(newroot, &aa_string, 2);
    if (ret == 1 && aa_string != NULL) {
        ret = iso_aa_lookup_attr(aa_string, "isofs.cs",
                                   &attr_value_length, &attr_value, 0);
        free(aa_string);
    } else {
        ret = 0;
    }
    if (ret == 1) {
        if (data->auto_input_charset & 1) {
            if (data->input_charset != NULL)
                free(data->input_charset);
            data->input_charset = attr_value;
            iso_msg_submit(image->id, ISO_GENERAL_NOTE, 0,
                         "Learned from ISO image: input character set '%.80s'",
                          attr_value);
        } else {
            iso_msg_submit(image->id, ISO_GENERAL_NOTE, 0,
                   "Ignored character set name recorded in ISO image: '%.80s'",
                           attr_value);
            free(attr_value);
        }
        attr_value = NULL;
    }

    /* backup image filesystem, builder and root */
    fsback = image->fs;
    blback = image->builder;
    oldroot = image->root;
    oldbootcat = image->bootcat; /* could be NULL */
    image->bootcat = NULL;
    old_checksum_array = image->checksum_array;
    image->checksum_array = NULL;

    /* create new builder */
    ret = iso_image_builder_new(blback, &image->builder);
    if (ret < 0) {
        goto import_revert;
    }

    image->fs = fs;

    /* create new root, and set root attributes from source */
    ret = iso_node_new_root(&image->root);
    if (ret < 0) {
        goto import_revert;
    }
    {
        struct stat info;

        /* I know this will not fail */
        iso_file_source_lstat(newroot, &info);
        image->root->node.mode = info.st_mode;
        image->root->node.uid = info.st_uid;
        image->root->node.gid = info.st_gid;
        image->root->node.atime = info.st_atime;
        image->root->node.mtime = info.st_mtime;
        image->root->node.ctime = info.st_ctime;

        /* This might fail in iso_node_add_xinfo() */
        ret = src_aa_to_node(newroot, &(image->root->node), 0);
        if (ret < 0)
            goto import_revert;

        /* Attach ino as xinfo if valid */
        if (info.st_ino != 0 && !data->make_new_ino) {
            ret = iso_node_set_ino(&(image->root->node), info.st_ino, 0);
            if (ret < 0)
                goto import_revert;
        }
    }

    ret = iso_root_get_isofsnt(&(image->root->node), &truncate_mode,
                               &truncate_length, 0);
    if (ret == 1 && (int) truncate_mode == image->truncate_mode &&
        image->truncate_mode == 1 &&
        truncate_length >= 64 && truncate_length <= 255 &&
        (int) truncate_length != image->truncate_length) {

        data->truncate_mode = opts->truncate_mode = image->truncate_mode =
                                                                 truncate_mode;
        data->truncate_length = opts->truncate_length =
                                      image->truncate_length = truncate_length;
        iso_msg_submit(image->id, ISO_TRUNCATE_ISOFSNT, 0,
                "File name truncation length changed by loaded image info: %d",
                (int) truncate_length);
    }

    /* if old image has el-torito, add a new catalog */
    if (data->eltorito) {

        catalog = calloc(1, sizeof(struct el_torito_boot_catalog));
        if (catalog == NULL) {
            ret = ISO_OUT_OF_MEM;
            goto import_revert;
        }

        catalog->num_bootimages = 0;
        for (idx = 0; idx < data->num_bootimgs; idx++) {
            boot_image = calloc(1, sizeof(ElToritoBootImage));
            if (boot_image == NULL) {
                ret = ISO_OUT_OF_MEM;
                goto import_revert;
            }
            boot_image->image = NULL;
            boot_image->bootable = data->boot_flags[idx] & 1;
            boot_image->type = data->media_types[idx];
            boot_image->partition_type = data->partition_types[idx];
            boot_image->load_seg = data->load_segs[idx];
            boot_image->load_size = data->load_sizes[idx];
            boot_image->platform_id = data->platform_ids[idx];
            memcpy(boot_image->id_string, data->id_strings[idx], 28);
            memcpy(boot_image->selection_crit, data->selection_crits, 20);

            catalog->bootimages[catalog->num_bootimages] = boot_image;
            boot_image = NULL;
            catalog->num_bootimages++;
        }
        for ( ; idx < Libisofs_max_boot_imageS; idx++)
            catalog->bootimages[idx] = NULL;
        image->bootcat = catalog;
        catalog = NULL; /* So it does not get freed */
    }

    /* recursively add image */
    ret = iso_add_dir_src_rec(image, image->root, newroot);
    if (ret < 0) {
        /* error during recursive image addition */
        iso_node_builder_unref(image->builder);
        goto import_revert;
    }
    issue_ucs2_warning_summary(data->joliet_ucs2_failures);
    issue_collision_warning_summary(image->collision_warnings);

    /* Take over inode management from IsoImageFilesystem.
       data->inode_counter is supposed to hold the maximum PX inode number.
     */
    image->inode_counter = data->inode_counter;

    if ((data->px_ino_status & (2 | 4 | 8)) || opts->make_new_ino) {
        /* Attach new inode numbers to any node which does not have one,
           resp. to all nodes in case of opts->make_new_ino 
        */
        if (opts->make_new_ino)
            hflag = 1; /* Equip all data files with new unique inos */
        else
            hflag = 2 | 4 | 8; /* Equip any file type if it has ino == 0 */
        ret = img_make_inos(image, image->root, hflag);
        if (ret < 0) {
            iso_node_builder_unref(image->builder);
            goto import_revert;
        }
    }

    if (data->eltorito) {
        /* if catalog and boot image nodes were not filled,
           we create them here */
        for (idx = 0; idx < image->bootcat->num_bootimages; idx++) {
            if (image->bootcat->bootimages[idx]->image != NULL)
        continue;
            ret = create_boot_img_filesrc(fs, image, idx, &boot_src);
            if (ret < 0) {
                iso_node_builder_unref(image->builder);
                goto import_revert;
            }
            ret = image_builder_create_node(image->builder, image, boot_src,
                                            NULL, &node);
            iso_file_source_unref(boot_src); /* Now owned by node */
            if (ret < 0) {
                iso_node_builder_unref(image->builder);
                goto import_revert;
            }
            if (image->bootcat->bootimages[idx]->image != NULL) {
                /* Already added to bootimages in image_builder_create_node().
                 * Now it has one refcount for tree and one for bootimages.
                 * But it will not go to tree. So unref.
                 */
                iso_node_unref(node);
            } else {
                image->bootcat->bootimages[idx]->image = (IsoFile*)node;
            }
            

            /* warn about hidden images */
            iso_msg_submit(image->id, ISO_EL_TORITO_HIDDEN, 0,
                           "Found hidden El-Torito image. Its size could not "
                           "be figured out, so image modify or boot image "
                           "patching may lead to bad results.");
        }
        if (image->bootcat->node == NULL) {
            IsoNode *node;
            IsoBoot *bootcat;
            node = calloc(1, sizeof(IsoBoot));
            if (node == NULL) {
                ret = ISO_OUT_OF_MEM;
                goto import_revert;
            }
            bootcat = (IsoBoot *) node;
            bootcat->lba = data->catblock;
            bootcat->size = data->catsize;
            bootcat->content = NULL; 
            if (bootcat->size > 0) {
                bootcat->content = calloc(1, bootcat->size);
                if (bootcat->content == NULL) {
                    free(node);
                    ret = ISO_OUT_OF_MEM;
                    goto import_revert;
                }
                memcpy(bootcat->content, data->catcontent, bootcat->size);
            }
            node->type = LIBISO_BOOT;
            node->mode = S_IFREG;
            node->refcount = 1;
            image->bootcat->node = (IsoBoot*)node;
        }
    }

    iso_node_builder_unref(image->builder);

    /* set volume attributes */
    iso_image_set_volset_id(image, data->volset_id);
    iso_image_set_volume_id(image, data->volume_id);
    iso_image_set_publisher_id(image, data->publisher_id);
    iso_image_set_data_preparer_id(image, data->data_preparer_id);
    iso_image_set_system_id(image, data->system_id);
    iso_image_set_application_id(image, data->application_id);
    iso_image_set_copyright_file_id(image, data->copyright_file_id);
    iso_image_set_abstract_file_id(image, data->abstract_file_id);
    iso_image_set_biblio_file_id(image, data->biblio_file_id);
    iso_image_set_pvd_times(image, data->creation_time,
         data->modification_time, data->expiration_time, data->effective_time);

    if (features != NULL) {
        *features = malloc(sizeof(IsoReadImageFeatures));
        if (*features == NULL) {
            ret = ISO_OUT_OF_MEM;
            goto import_revert;
        }
        (*features)->hasJoliet = data->joliet;
        (*features)->hasRR = data->rr_version != 0;
        (*features)->hasIso1999 = data->iso1999;
        (*features)->hasElTorito = data->eltorito;
        (*features)->size = data->nblocks;
    }

    if (data->md5_load) {
        /* Read checksum array */
        ret = iso_root_get_isofsca((IsoNode *) image->root,
                                   &(image->checksum_start_lba),
                                   &(image->checksum_end_lba),
                                   &(image->checksum_idx_count),
                                   &checksum_size, checksum_type, 0); 
        if (ret > 0)
            if (checksum_size != 16 || strcmp(checksum_type, "MD5") != 0)
                ret = 0;
        if (ret > 0 && image->checksum_idx_count > 1) {
            size = image->checksum_idx_count / 128;
            if (size * 128 < image->checksum_idx_count)
                size++;
            image->checksum_array = calloc(size, 2048);
            if (image->checksum_array == NULL) {
                ret = ISO_OUT_OF_MEM;
                goto import_revert;
            }

            /* Load from image->checksum_end_lba */;
            for (i = 0; i < (int) size; i++) {
                rpt = (uint8_t *) (image->checksum_array + i * 2048);
                ret = src->read_block(src, image->checksum_end_lba + i, rpt);
                if (ret <= 0)
                    goto import_cleanup;
            }

            /* Compute MD5 and compare with recorded MD5 */
            ret = iso_md5_start(&ctx);
            if (ret < 0) {
                ret = ISO_OUT_OF_MEM;
                goto import_revert;
            }
            for (i = 0; i < (int) image->checksum_idx_count - 1; i++)
                iso_md5_compute(ctx, image->checksum_array + i * 16, 16);
            iso_md5_end(&ctx, md5);
            for (i = 0; i < 16; i++)
                if (md5[i] != image->checksum_array[
                                      (image->checksum_idx_count - 1) * 16 + i]
                   )
            break;
            if (i < 16) {
                iso_msg_submit(image->id, ISO_MD5_AREA_CORRUPTED, 0,
  "MD5 checksum array appears damaged and not trustworthy for verifications.");
                free(image->checksum_array);
                image->checksum_array = NULL;
                image->checksum_idx_count = 0;
            }
        }
    }

    ret = iso_image_eval_boot_info_table(image, opts, src, data->nblocks, 0);
    if (ret < 0)
        goto import_revert;

    if (opts->load_system_area && image->system_area_data != NULL) {
        ret = iso_analyze_system_area(image, src, opts, data->nblocks, 0);
        if (ret < 0) {
            iso_msg_submit(-1, ISO_SYSAREA_PROBLEMS, 0,
                      "Problem encountered during inspection of System Area:");
            iso_msg_submit(-1, ISO_SYSAREA_PROBLEMS, 0,
                           iso_error_to_msg(ret));
        }
    }

    ret = ISO_SUCCESS;
    goto import_cleanup;

    import_revert:;

    iso_node_unref((IsoNode*)image->root);
    el_torito_boot_catalog_free(image->bootcat);
    image->root = oldroot;
    oldroot = NULL;
    image->bootcat = oldbootcat;
    oldbootcat = NULL;
    image->checksum_array = old_checksum_array;
    old_checksum_array = NULL;

    import_cleanup:;

    /* recover backed fs and builder */
    image->fs = fsback;
    image->builder = blback;

    /* free old root */
    if (oldroot != NULL)
        iso_node_unref((IsoNode*)oldroot);

    /* free old boot catalog */
    if (oldbootcat != NULL)
        el_torito_boot_catalog_free(oldbootcat);

    if (catalog != NULL)
        el_torito_boot_catalog_free(catalog);
    if (boot_image != NULL)
        free((char *) boot_image);
    iso_file_source_unref(newroot);
    fs->close(fs);
    iso_filesystem_unref(fs);
    if (old_checksum_array != NULL)
        free(old_checksum_array);
    if (ctx != NULL)
        iso_md5_end(&ctx, md5);
    return ret;
}

const char *iso_image_fs_get_volset_id(IsoImageFilesystem *fs)
{
    _ImageFsData *data = (_ImageFsData*) fs->data;
    return data->volset_id;
}

const char *iso_image_fs_get_volume_id(IsoImageFilesystem *fs)
{
    _ImageFsData *data = (_ImageFsData*) fs->data;
    return data->volume_id;
}

const char *iso_image_fs_get_publisher_id(IsoImageFilesystem *fs)
{
    _ImageFsData *data = (_ImageFsData*) fs->data;
    return data->publisher_id;
}

const char *iso_image_fs_get_data_preparer_id(IsoImageFilesystem *fs)
{
    _ImageFsData *data = (_ImageFsData*) fs->data;
    return data->data_preparer_id;
}

const char *iso_image_fs_get_system_id(IsoImageFilesystem *fs)
{
    _ImageFsData *data = (_ImageFsData*) fs->data;
    return data->system_id;
}

const char *iso_image_fs_get_application_id(IsoImageFilesystem *fs)
{
    _ImageFsData *data = (_ImageFsData*) fs->data;
    return data->application_id;
}

const char *iso_image_fs_get_copyright_file_id(IsoImageFilesystem *fs)
{
    _ImageFsData *data = (_ImageFsData*) fs->data;
    return data->copyright_file_id;
}

const char *iso_image_fs_get_abstract_file_id(IsoImageFilesystem *fs)
{
    _ImageFsData *data;
    data = (_ImageFsData*) fs->data;
    return data->abstract_file_id;
}

const char *iso_image_fs_get_biblio_file_id(IsoImageFilesystem *fs)
{
    _ImageFsData *data = (_ImageFsData*) fs->data;
    return data->biblio_file_id;
}

int iso_read_opts_new(IsoReadOpts **opts, int profile)
{
    IsoReadOpts *ropts;

    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    if (profile != 0) {
        return ISO_WRONG_ARG_VALUE;
    }

    ropts = calloc(1, sizeof(IsoReadOpts));
    if (ropts == NULL) {
        return ISO_OUT_OF_MEM;
    }

    ropts->file_mode = 0444;
    ropts->dir_mode = 0555;
    ropts->noaaip = 1;
    ropts->ecma119_map = 1;
    ropts->nomd5 = 1;
    ropts->load_system_area = 0;
    ropts->keep_import_src = 0;
    ropts->truncate_mode = 1;
    ropts->truncate_length = LIBISOFS_NODE_NAME_MAX;

    *opts = ropts;
    return ISO_SUCCESS;
}

void iso_read_opts_free(IsoReadOpts *opts)
{
    if (opts == NULL) {
        return;
    }

    free(opts->input_charset);
    free(opts);
}

int iso_read_opts_set_start_block(IsoReadOpts *opts, uint32_t block)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->block = block;
    return ISO_SUCCESS;
}

int iso_read_opts_set_no_rockridge(IsoReadOpts *opts, int norr)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->norock = norr ? 1 :0;
    return ISO_SUCCESS;
}

int iso_read_opts_set_no_joliet(IsoReadOpts *opts, int nojoliet)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->nojoliet = nojoliet ? 1 :0;
    return ISO_SUCCESS;
}

int iso_read_opts_set_no_iso1999(IsoReadOpts *opts, int noiso1999)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->noiso1999 = noiso1999 ? 1 :0;
    return ISO_SUCCESS;
}

int iso_read_opts_set_no_aaip(IsoReadOpts *opts, int noaaip)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->noaaip = noaaip ? 1 : 0;
    return ISO_SUCCESS;
}

int iso_read_opts_set_no_md5(IsoReadOpts *opts, int no_md5)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->nomd5 = no_md5 == 2 ? 2 : no_md5 == 1 ? 1 : 0;
    return ISO_SUCCESS;
}


int iso_read_opts_set_new_inos(IsoReadOpts *opts, int new_inos)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->make_new_ino = new_inos ? 1 : 0;
    return ISO_SUCCESS;
}

int iso_read_opts_set_preferjoliet(IsoReadOpts *opts, int preferjoliet)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->preferjoliet = preferjoliet ? 1 :0;
    return ISO_SUCCESS;
}

int iso_read_opts_set_ecma119_map(IsoReadOpts *opts, int ecma119_map)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    if (ecma119_map < 0 || ecma119_map > 3)
        return 0;
    opts->ecma119_map = ecma119_map;
    return ISO_SUCCESS;
}

int iso_read_opts_set_default_uid(IsoReadOpts *opts, uid_t uid)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->uid = uid;
    return ISO_SUCCESS;
}

int iso_read_opts_set_default_gid(IsoReadOpts *opts, gid_t gid)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->gid = gid;
    return ISO_SUCCESS;
}

int iso_read_opts_set_default_permissions(IsoReadOpts *opts, mode_t file_perm,
                                          mode_t dir_perm)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->file_mode = file_perm;
    opts->dir_mode = dir_perm;
    return ISO_SUCCESS;
}

int iso_read_opts_set_input_charset(IsoReadOpts *opts, const char *charset)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->input_charset = charset ? strdup(charset) : NULL;
    return ISO_SUCCESS;
}

int iso_read_opts_auto_input_charset(IsoReadOpts *opts, int mode)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->auto_input_charset = mode;
    return ISO_SUCCESS;
}

int iso_read_opts_load_system_area(IsoReadOpts *opts, int mode)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->load_system_area = mode & 1;
    return ISO_SUCCESS;
}

int iso_read_opts_keep_import_src(IsoReadOpts *opts, int mode)
{
    if (opts == NULL) {
        return ISO_NULL_POINTER;
    }
    opts->keep_import_src = mode & 1;
    return ISO_SUCCESS;
}

/**
 * Destroy an IsoReadImageFeatures object obtained with iso_image_import.
 */
void iso_read_image_features_destroy(IsoReadImageFeatures *f)
{
    if (f) {
        free(f);
    }
}

/**
 * Get the size (in 2048 byte block) of the image, as reported in the PVM.
 */
uint32_t iso_read_image_features_get_size(IsoReadImageFeatures *f)
{
    return f->size;
}

/**
 * Whether RockRidge extensions are present in the image imported.
 */
int iso_read_image_features_has_rockridge(IsoReadImageFeatures *f)
{
    return f->hasRR;
}

/**
 * Whether Joliet extensions are present in the image imported.
 */
int iso_read_image_features_has_joliet(IsoReadImageFeatures *f)
{
    return f->hasJoliet;
}

/**
 * Whether the image is recorded according to ISO 9660:1999, i.e. it has
 * a version 2 Enhanced Volume Descriptor.
 */
int iso_read_image_features_has_iso1999(IsoReadImageFeatures *f)
{
    return f->hasIso1999;
}

/**
 * Whether El-Torito boot record is present present in the image imported.
 */
int iso_read_image_features_has_eltorito(IsoReadImageFeatures *f)
{
    return f->hasElTorito;
}


/**
 * Get the start addresses and the sizes of the data extents of a file node
 * if it was imported from an old image.
 *
 * @param file
 *      The file
 * @param section_count
 *      Returns the number of extent entries in sections arrays
 * @param sections
 *      Returns the array of file sections. Apply free() to dispose it.
 * @param flag
 *      Reserved for future usage, submit 0
 * @return
 *      1 if there are valid extents (file comes from old image),
 *      0 if file was newly added, i.e. it does not come from an old image,
 *      < 0 error
 */
int iso_file_get_old_image_sections(IsoFile *file, int *section_count,
                                   struct iso_file_section **sections,
                                   int flag)
{
    if (file == NULL || section_count == NULL || sections == NULL) {
        return ISO_NULL_POINTER;
    }
    if (flag != 0) {
        return ISO_WRONG_ARG_VALUE;
    }
    *section_count = 0;
    *sections = NULL;
    if (file->from_old_session != 0) {

        /*
         * When file is from old session, we retrieve the original IsoFileSource
         * to get the sections. This break encapsultation, but safes memory as
         * we don't need to store the sections in the IsoFile node.
         */
        IsoStream *stream = file->stream, *input_stream;
        FSrcStreamData *data;
        ImageFileSourceData *ifsdata;

        /* Get the most original stream */
        while (1) {
            input_stream = iso_stream_get_input_stream(stream, 0);
            if (input_stream == NULL || input_stream == stream)
        break;
            stream = input_stream;
        }

        /* From here on it must be a stream with FSrcStreamData. */
        /* ??? Shall one rather check :
                stream->class == extern IsoStreamIface fsrc_stream_class
               (its storage location is global in stream.c)
         */
        if (stream->class->type[0] != 'f' || stream->class->type[1] != 's' ||
            stream->class->type[2] != 'r' || stream->class->type[3] != 'c')
            return 0;

        data = stream->data;
        ifsdata = data->src->data;

        *section_count = ifsdata->nsections;
        if (*section_count <= 0)
            return 1;
        *sections = malloc(ifsdata->nsections *
                                sizeof(struct iso_file_section));
        if (*sections == NULL) {
            return ISO_OUT_OF_MEM;
        }
        memcpy(*sections, ifsdata->sections,
               ifsdata->nsections * sizeof(struct iso_file_section));
        return 1;
    }
    return 0;
}

/* Rank two IsoFileSource by their eventual old image LBAs if still non-zero.
   Other IsoFileSource classes and zeroized LBAs will be ranked only roughly.
   flag bit0 preserves transitivity of the caller by evaluating ifs_class with
   non-zero block address as smaller than anything else.
   flag bit1 could harm reproducibility of ISO image output.
   @param flag bit0= if s1 exor s2 is of applicable class, then enforce
                     a valid test result by comparing classes
               bit1= if both are applicable but also have sections[].block == 0
                     then enforce a valid test result by comparing object                            addresses.
*/
int iso_ifs_sections_cmp(IsoFileSource *s1, IsoFileSource *s2, int *cmp_ret,
                         int flag)
{
    int i;
    ImageFileSourceData *d1 = NULL, *d2 = NULL;
    IsoFileSourceIface *class1 = NULL, *class2 = NULL;

    /* Newly created IsoFileSrc from imported IsoFile (e.g. boot image)
       is not an applicable source. It must be kept from causing a decision
       with other non-applicables.
    */
    if (s1 != NULL) {
        class1 = (IsoFileSourceIface *) s1->class;
        if (class1 == &ifs_class) {
            d1 = (ImageFileSourceData *) s1->data;
            if (d1->nsections > 0)
                if (d1->sections[0].block == 0)
                    class1 = NULL;
        }
    }
    if (s2 != NULL) {
        class2 = (IsoFileSourceIface *) s2->class;
        if (class2 == &ifs_class) {
            d2 = (ImageFileSourceData *) s2->data;
            if (d2->nsections > 0)
                if (d2->sections[0].block == 0)
                    class2 = NULL;
        }
    }

    if (class1 != &ifs_class && class2 != &ifs_class) {
        *cmp_ret = 0;
        return 0;
    }
    if (class1 != class2) {
        *cmp_ret = (class1 == &ifs_class ? -1 : 1);
        if (flag & 1)
            return 1;
        return 0;
    }

    if (d1->nsections != d2->nsections) {
        *cmp_ret = d1->nsections < d2->nsections ? -1 : 1;
        return 1;
    }
    if (d1->nsections == 0) {
       *cmp_ret = 0;
       return 1;
    }
    if (d1->sections[0].size < 1 || d2->sections[0].size < 1) {
        if (d1->sections[0].size > d2->sections[0].size)
            *cmp_ret = 1;
        else if (d1->sections[0].size < d2->sections[0].size)
            *cmp_ret = -1;
        else
            *cmp_ret = 0;
        return 1;
    }

    for (i = 0; i < d1->nsections; i++) {
        if (d1->sections[i].block != d2->sections[i].block) {
            *cmp_ret = (d1->sections[i].block < d2->sections[i].block ? -1 : 1);
            return 1;
        }
        if (d1->sections[i].size != d2->sections[i].size) {
            *cmp_ret = (d1->sections[i].size < d2->sections[i].size ? -1 : 1);
            return 1;
        }
    }
    *cmp_ret = 0;
    return 1;
}

