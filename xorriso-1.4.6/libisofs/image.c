/*
 * Copyright (c) 2007 Vreixo Formoso
 * Copyright (c) 2009 - 2015 Thomas Schmitt
 *
 * This file is part of the libisofs project; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2 
 * or later as published by the Free Software Foundation. 
 * See COPYING file for details.
 */

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include "libisofs.h"
#include "image.h"
#include "node.h"
#include "messages.h"
#include "eltorito.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int iso_imported_sa_new(struct iso_imported_sys_area **boots, int flag)
{
    struct iso_imported_sys_area *b;

    *boots = NULL;
    b = calloc(1, sizeof(struct iso_imported_sys_area));
    if (b == NULL)
        return ISO_OUT_OF_MEM;

    b->mbr_req = NULL;
    b->apm_req = NULL;

    b->gpt_req = NULL;
    b->gpt_backup_comments = NULL;

    b->mips_boot_file_paths = NULL;
    b->mips_vd_entries = NULL;

    b->sparc_disc_label = NULL;
    b->sparc_core_node = NULL;
    b->sparc_entries = NULL;

    b->hppa_cmdline = NULL;
    b->hppa_bootloader = NULL;
    b->hppa_kernel_32 = NULL;
    b->hppa_kernel_64 = NULL;
    b->hppa_ramdisk = NULL;

    b->alpha_boot_image = NULL;

    *boots = b;
    return 1;
}

int iso_imported_sa_unref(struct iso_imported_sys_area **boots, int flag)
{
    int i;
    struct iso_imported_sys_area *b;

    b = *boots;
    if (b == NULL)
        return 2;
    if (b->refcount > 0)
        b->refcount--;
    if (b->refcount > 0)
        return 2;

    if (b->mbr_req != NULL) {
        for (i = 0; i < b->mbr_req_count; i++)
            LIBISO_FREE_MEM(b->mbr_req[i]);
        LIBISO_FREE_MEM(b->mbr_req);
    }
    if (b->apm_req != NULL) {
        for (i = 0; i < b->apm_req_count; i++)
            LIBISO_FREE_MEM(b->apm_req[i]);
        LIBISO_FREE_MEM(b->apm_req);
    }
    if (b->gpt_req != NULL) {
        for (i = 0; i < b->gpt_req_count; i++)
            LIBISO_FREE_MEM(b->gpt_req[i]);
        LIBISO_FREE_MEM(b->gpt_req);
    }
    LIBISO_FREE_MEM(b->gpt_backup_comments);

    if (b->mips_boot_file_paths != NULL) {
        for (i = 0; i < b->num_mips_boot_files; i++)
            LIBISO_FREE_MEM(b->mips_boot_file_paths[i]);
        LIBISO_FREE_MEM(b->mips_boot_file_paths);
    }
    if (b->mips_vd_entries != NULL) {
        for (i = 0; i < b->num_mips_boot_files; i++)
            LIBISO_FREE_MEM(b->mips_vd_entries[i]);
        LIBISO_FREE_MEM(b->mips_vd_entries);
    }
    LIBISO_FREE_MEM(b->mipsel_boot_file_path);

    LIBISO_FREE_MEM(b->sparc_disc_label);
    if (b->sparc_core_node != NULL)
        iso_node_unref((IsoNode *) b->sparc_core_node);
    LIBISO_FREE_MEM(b->sparc_entries);

    LIBISO_FREE_MEM(b->hppa_cmdline);
    LIBISO_FREE_MEM(b->hppa_bootloader);
    LIBISO_FREE_MEM(b->hppa_kernel_32);
    LIBISO_FREE_MEM(b->hppa_kernel_64);
    LIBISO_FREE_MEM(b->hppa_ramdisk);
    LIBISO_FREE_MEM(b->alpha_boot_image);
    LIBISO_FREE_MEM(b);
    *boots = NULL;
    return 1;
}


/**
 * Create a new image, empty.
 *
 * The image will be owned by you and should be unref() when no more needed.
 *
 * @param name
 *     Name of the image. This will be used as volset_id and volume_id.
 * @param image
 *     Location where the image pointer will be stored.
 * @return
 *     1 success, < 0 error
 */
int iso_image_new(const char *name, IsoImage **image)
{
    int res, i;
    IsoImage *img;

    if (image == NULL) {
        return ISO_NULL_POINTER;
    }

    img = calloc(1, sizeof(IsoImage));
    if (img == NULL) {
        return ISO_OUT_OF_MEM;
    }

    /* local filesystem will be used by default */
    res = iso_local_filesystem_new(&(img->fs));
    if (res < 0) {
        free(img);
        return ISO_OUT_OF_MEM;
    }

    /* use basic builder as default */
    res = iso_node_basic_builder_new(&(img->builder));
    if (res < 0) {
        iso_filesystem_unref(img->fs);
        free(img);
        return ISO_OUT_OF_MEM;
    }

    /* fill image fields */
    res = iso_node_new_root(&img->root);
    if (res < 0) {
        iso_node_builder_unref(img->builder);
        iso_filesystem_unref(img->fs);
        free(img);
        return res;
    }
    img->refcount = 1;
    img->id = iso_message_id++;

    if (name != NULL) {
        img->volset_id = strdup(name);
        img->volume_id = strdup(name);
    }
    memset(img->application_use, 0, 512);
    img->system_area_data = NULL;
    img->system_area_options = 0;
    img->num_mips_boot_files = 0;
    for (i = 0; i < 15; i++)
         img->mips_boot_file_paths[i] = NULL;
    img->sparc_core_node = NULL;
    img->hppa_cmdline= NULL;
    img->hppa_bootloader = NULL;
    img->hppa_kernel_32 = NULL;
    img->hppa_kernel_64 = NULL;
    img->hppa_ramdisk = NULL;
    img->alpha_boot_image = NULL;
    img->import_src = NULL;
    img->builder_ignore_acl = 1;
    img->builder_ignore_ea = 1;
    img->truncate_mode = 1;
    img->truncate_length = LIBISOFS_NODE_NAME_MAX;
    img->truncate_buffer[0] = 0;
    img->inode_counter = 0;
    img->used_inodes = NULL;
    img->used_inodes_start = 0;
    img->checksum_start_lba = 0;
    img->checksum_end_lba = 0;
    img->checksum_idx_count = 0;
    img->checksum_array = NULL;
    img->generator_is_running = 0;
    for (i = 0; i < ISO_HFSPLUS_BLESS_MAX; i++)
        img->hfsplus_blessed[i] = NULL;
    img->collision_warnings = 0;
    img->imported_sa_info = NULL;

    *image = img;
    return ISO_SUCCESS;
}

/**
 * Increments the reference counting of the given image.
 */
void iso_image_ref(IsoImage *image)
{
    ++image->refcount;
}

/**
 * Decrements the reference counting of the given image.
 * If it reaches 0, the image is free, together with its tree nodes (whether
 * their refcount reach 0 too, of course).
 */
void iso_image_unref(IsoImage *image)
{
    int nexcl, i;

    if (--image->refcount == 0) {
        /* we need to free the image */

        if (image->user_data_free != NULL) {
            /* free attached data */
            image->user_data_free(image->user_data);
        }
        for (nexcl = 0; nexcl < image->nexcludes; ++nexcl) {
            free(image->excludes[nexcl]);
        }
        free(image->excludes);
        for (i = 0; i < ISO_HFSPLUS_BLESS_MAX; i++)
            if (image->hfsplus_blessed[i] != NULL)
                iso_node_unref(image->hfsplus_blessed[i]);
        iso_node_unref((IsoNode*)image->root);
        iso_node_builder_unref(image->builder);
        iso_filesystem_unref(image->fs);
        el_torito_boot_catalog_free(image->bootcat);
        iso_image_give_up_mips_boot(image, 0);
        if (image->sparc_core_node != NULL)
            iso_node_unref((IsoNode *) image->sparc_core_node);
        iso_image_set_hppa_palo(image, NULL, NULL, NULL, NULL, NULL, 1);
        if (image->alpha_boot_image != NULL)
            free(image->alpha_boot_image);
        if (image->import_src != NULL)
            iso_data_source_unref(image->import_src);
        free(image->volset_id);
        free(image->volume_id);
        free(image->publisher_id);
        free(image->data_preparer_id);
        free(image->system_id);
        free(image->application_id);
        free(image->copyright_file_id);
        free(image->abstract_file_id);
        free(image->biblio_file_id);
        free(image->creation_time);
        free(image->modification_time);
        free(image->expiration_time);
        free(image->effective_time);
        if (image->used_inodes != NULL)
            free(image->used_inodes);
        if (image->system_area_data != NULL)
            free(image->system_area_data);
        iso_image_free_checksums(image, 0);
        iso_imported_sa_unref(&(image->imported_sa_info), 0);
        free(image);
    }
}


int iso_image_free_checksums(IsoImage *image, int flag)
{
    image->checksum_start_lba = 0;
    image->checksum_end_lba = 0;
    image->checksum_idx_count = 0;
    if (image->checksum_array != NULL)
        free(image->checksum_array);
    image->checksum_array = NULL;
    return 1;
}


/**
 * Attach user defined data to the image. Use this if your application needs
 * to store addition info together with the IsoImage. If the image already
 * has data attached, the old data will be freed.
 *
 * @param data
 *      Pointer to application defined data that will be attached to the
 *      image. You can pass NULL to remove any already attached data.
 * @param give_up
 *      Function that will be called when the image does not need the data
 *      any more. It receives the data pointer as an argumente, and eventually
 *      causes data to be free. It can be NULL if you don't need it.
 */
int iso_image_attach_data(IsoImage *image, void *data, void (*give_up)(void*))
{
    if (image == NULL) {
        return ISO_NULL_POINTER;
    }

    if (image->user_data != NULL) {
        /* free previously attached data */
        if (image->user_data_free != NULL) {
            image->user_data_free(image->user_data);
        }
        image->user_data = NULL;
        image->user_data_free = NULL;
    }

    if (data != NULL) {
        image->user_data = data;
        image->user_data_free = give_up;
    }
    return ISO_SUCCESS;
}

/**
 * The the data previously attached with iso_image_attach_data()
 */
void *iso_image_get_attached_data(IsoImage *image)
{
    return image->user_data;
}

IsoDir *iso_image_get_root(const IsoImage *image)
{
    return image->root;
}

void iso_image_set_volset_id(IsoImage *image, const char *volset_id)
{
    free(image->volset_id);
    image->volset_id = strdup(volset_id);
}

const char *iso_image_get_volset_id(const IsoImage *image)
{
    if (image->volset_id == NULL)
        return "";
    return image->volset_id;
}

void iso_image_set_volume_id(IsoImage *image, const char *volume_id)
{
    free(image->volume_id);
    image->volume_id = strdup(volume_id);
}

const char *iso_image_get_volume_id(const IsoImage *image)
{
    if (image->volume_id == NULL)
        return "";
    return image->volume_id;
}

void iso_image_set_publisher_id(IsoImage *image, const char *publisher_id)
{
    free(image->publisher_id);
    image->publisher_id = strdup(publisher_id);
}

const char *iso_image_get_publisher_id(const IsoImage *image)
{
    if (image->publisher_id == NULL)
        return "";
    return image->publisher_id;
}

void iso_image_set_data_preparer_id(IsoImage *image,
                                    const char *data_preparer_id)
{
    free(image->data_preparer_id);
    image->data_preparer_id = strdup(data_preparer_id);
}

const char *iso_image_get_data_preparer_id(const IsoImage *image)
{
    if (image->data_preparer_id == NULL)
        return "";
    return image->data_preparer_id;
}

void iso_image_set_system_id(IsoImage *image, const char *system_id)
{
    free(image->system_id);
    image->system_id = strdup(system_id);
}

const char *iso_image_get_system_id(const IsoImage *image)
{
    if (image->system_id == NULL)
        return "";
    return image->system_id;
}

void iso_image_set_application_id(IsoImage *image, const char *application_id)
{
    free(image->application_id);
    image->application_id = strdup(application_id);
}

const char *iso_image_get_application_id(const IsoImage *image)
{
    if (image->application_id == NULL)
        return "";
    return image->application_id;
}

void iso_image_set_copyright_file_id(IsoImage *image,
                                     const char *copyright_file_id)
{
    free(image->copyright_file_id);
    image->copyright_file_id = strdup(copyright_file_id);
}

const char *iso_image_get_copyright_file_id(const IsoImage *image)
{
    if (image->copyright_file_id == NULL)
        return "";
    return image->copyright_file_id;
}

void iso_image_set_abstract_file_id(IsoImage *image,
                                    const char *abstract_file_id)
{
    free(image->abstract_file_id);
    image->abstract_file_id = strdup(abstract_file_id);
}

const char *iso_image_get_abstract_file_id(const IsoImage *image)
{
    if (image->abstract_file_id == NULL)
        return "";
    return image->abstract_file_id;
}

void iso_image_set_biblio_file_id(IsoImage *image, const char *biblio_file_id)
{
    free(image->biblio_file_id);
    image->biblio_file_id = strdup(biblio_file_id);
}

const char *iso_image_get_biblio_file_id(const IsoImage *image)
{
    if (image->biblio_file_id == NULL)
        return "";
    return image->biblio_file_id;
}

int iso_image_set_pvd_times(IsoImage *image,
                            char *creation_time, char *modification_time,
                            char *expiration_time, char *effective_time)
{
    if (creation_time == NULL || modification_time == NULL ||
        expiration_time == NULL || effective_time == NULL)
        return ISO_NULL_POINTER;
    image->creation_time = calloc(18, 1);   /* Surely including a trailing 0 */
    image->modification_time = calloc(18, 1);
    image->expiration_time = calloc(18, 1);
    image->effective_time = calloc(18, 1);
    if (image->creation_time == NULL || image->modification_time == NULL ||
        image->expiration_time == NULL || image->effective_time == NULL)
        return ISO_OUT_OF_MEM;
    /* (If the string is too short, a non-zero timezone will not be stored) */
    strncpy(image->creation_time, creation_time, 17);
    strncpy(image->modification_time, modification_time, 17);
    strncpy(image->expiration_time, expiration_time, 17);
    strncpy(image->effective_time, effective_time, 17);
    return ISO_SUCCESS;
}

int iso_image_get_pvd_times(IsoImage *image,
                            char **creation_time, char **modification_time,
                            char **expiration_time, char **effective_time)
{
    if (image->creation_time == NULL || image->modification_time == NULL ||
        image->expiration_time == NULL || image->effective_time == NULL)
        return ISO_NULL_POINTER;
    *creation_time = image->creation_time;
    *modification_time = image->modification_time;
    *expiration_time = image->expiration_time;
    *effective_time = image->effective_time;
    return ISO_SUCCESS;
}

void iso_image_set_app_use(IsoImage *image, const char *app_use_data,
                           int count)
{
    if (count < 0)
        count= 0;
    else if(count > 512)
        count= 512;
    if (count > 0)
        memcpy(image->application_use, app_use_data, count);
    if (count < 512)
        memset(image->application_use + count, 0, 512 - count);
}

int iso_image_get_msg_id(IsoImage *image)
{
    return image->id;
}

int iso_image_get_system_area(IsoImage *img, char system_area_data[32768],
                              int *options, int flag)
{
    *options = img->system_area_options;
    if (img->system_area_data == NULL)
        return 0;
    memcpy(system_area_data, img->system_area_data, 32768);
    return 1;
}

static
int dir_update_size(IsoImage *image, IsoDir *dir)
{
    IsoNode *pos;
    int ret;

#ifdef Libisofs_update_sizes_abortablE
    char *path= NULL;
    IsoStream *base_stream;
    int cancel_ret, ret;
    uint32_t lba;
#endif

    pos = dir->children;
    while (pos) {
        if (pos->type == LIBISO_FILE) {
            ret = iso_stream_update_size(ISO_FILE(pos)->stream);
        } else if (pos->type == LIBISO_DIR) {
            /* recurse */
            ret = dir_update_size(image, ISO_DIR(pos));

#ifdef Libisofs_update_sizes_abortablE
            if (ret == ISO_CANCELED)
                return ret; /* Message already issued by dir_update_size */
#endif

        } else {
            ret = 1;
        }

#ifdef Libisofs_update_sizes_abortablE

        /* This would report error and abort according to severity threshold.
           But it is desirable to let the update_size crawler continue
           its work after e.g. a file has vanished from hard disk.
           So normally this macro case should be disabled.
        */

        if (ret < 0) {
            cancel_ret = iso_msg_submit(image->id, ret, 0, NULL);
            path = iso_tree_get_node_path(pos);
            if (path != NULL) {
                iso_msg_submit(image->id, ret, 0,
                               "ISO path  : %s", path);
                free(path);
            }
            /* Report source path with streams which do not come from
               the loaded ISO filesystem */
            if (pos->type == LIBISO_FILE &&
                iso_node_get_old_image_lba(pos, &lba, 0) == 0) {
                base_stream = iso_stream_get_input_stream(
                                                     ISO_FILE(pos)->stream, 1);
                if (base_stream == NULL)
                    base_stream = ISO_FILE(pos)->stream;
                path = iso_stream_get_source_path(base_stream, 0);
                if (path != NULL) {
                    iso_msg_submit(image->id, ret, 0,
                                   "Local path: %s", path);
                    free(path);
                }
            }
            if (cancel_ret < 0)
                return cancel_ret; /* cancel due error threshold */
        }

#else

        if (ret < 0)
            ret = 1; /* ignore error */

#endif /* ! Libisofs_update_sizes_abortablE */

        pos = pos->next;
    }
    return ISO_SUCCESS;
}

int iso_image_update_sizes(IsoImage *image)
{
    if (image == NULL) {
        return ISO_NULL_POINTER;
    }

    return dir_update_size(image, image->root);
}


void iso_image_set_ignore_aclea(IsoImage *image, int what)
{
    image->builder_ignore_acl = (what & 1);
    image->builder_ignore_ea = !!(what & 2);
}


static
int img_register_ino(IsoImage *image, IsoNode *node, int flag)
{
    int ret;
    ino_t ino;
    unsigned int fs_id;
    dev_t dev_id;

    ret = iso_node_get_id(node, &fs_id, &dev_id, &ino, 1);
    if (ret < 0)
       return ret;
    if (ret > 0 && ino >= image->used_inodes_start &&
        ino <= image->used_inodes_start + (ISO_USED_INODE_RANGE - 1)) {
                                   /* without -1 : rollover hazard on 32 bit */
        image->used_inodes[(ino - image->used_inodes_start) / 8]
                                                           |= (1 << (ino % 8));
    }
    return 1;
}


/* Collect the bitmap of used inode numbers in the range of
   _ImageFsData.used_inodes_start + ISO_USED_INODE_RANGE
   @param flag bit0= recursion is active
*/
int img_collect_inos(IsoImage *image, IsoDir *dir, int flag)
{
    int ret, register_dir = 1;
    IsoDirIter *iter = NULL;
    IsoNode *node;
    IsoDir *subdir;

    if (dir == NULL)
        dir = image->root;
    if (image->used_inodes == NULL) {
        image->used_inodes = calloc(ISO_USED_INODE_RANGE / 8, 1);
        if (image->used_inodes == NULL)
            return ISO_OUT_OF_MEM;
    } else if(!(flag & 1)) {
        memset(image->used_inodes, 0, ISO_USED_INODE_RANGE / 8);
    } else {
        register_dir = 0;
    }
    if (register_dir) {
        node = (IsoNode *) dir;
        ret = img_register_ino(image, node, 0);
        if (ret < 0)
            return ret;
    }

    ret = iso_dir_get_children(dir, &iter);
    if (ret < 0)
        return ret;
    while (iso_dir_iter_next(iter, &node) == 1 ) {
        ret = img_register_ino(image, node, 0);
        if (ret < 0)
            goto ex;
        if (iso_node_get_type(node) == LIBISO_DIR) {
            subdir = (IsoDir *) node;
            ret = img_collect_inos(image, subdir, flag | 1);
            if (ret < 0)
                goto ex;
        }
    }
    ret = 1;
ex:;
    if (iter != NULL)
        iso_dir_iter_free(iter);
    return ret;
}


/**
 * A global counter for Rock Ridge inode numbers in the ISO image filesystem.
 *
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
uint32_t img_give_ino_number(IsoImage *image, int flag)
{
    int ret;
    uint64_t new_ino, ino_idx;
    static uint64_t limit = 0xffffffff;

    if (flag & 1) {
        image->inode_counter = 0;
        if (image->used_inodes != NULL)
            free(image->used_inodes);
        image->used_inodes = NULL;
        image->used_inodes_start = 0;
    }
    new_ino = ((uint64_t) image->inode_counter) + 1;
    if (image->used_inodes == NULL) {
        if (new_ino > 0 && new_ino <= limit) {
            image->inode_counter = (uint32_t) new_ino;
            return image->inode_counter;
        }
    }
    /* Look for free number in used territory */
    while (1) {
        if (new_ino <= 0 || new_ino > limit ||
            new_ino >= image->used_inodes_start + ISO_USED_INODE_RANGE ) {

            /* Collect a bitmap of used inode numbers ahead */

            image->used_inodes_start += ISO_USED_INODE_RANGE;
            if (image->used_inodes_start > 0xffffffff ||
                image->used_inodes_start <= 0) 
                image->used_inodes_start = 0;
            ret = img_collect_inos(image, NULL, 0);
            if (ret < 0)
                goto return_result; /* >>> need error return value */

            new_ino = image->used_inodes_start + !image->used_inodes_start;
        }
        ino_idx = (new_ino - image->used_inodes_start) / 8;
        if (!(image->used_inodes[ino_idx] & (1 << (new_ino % 8)))) {
            image->used_inodes[ino_idx] |= (1 << (new_ino % 8));
    break;
        }
        new_ino++;
    }
return_result:;
    image->inode_counter = new_ino;
    return image->inode_counter;
}


/* @param flag bit0= overwrite any ino, else only ino == 0
               bit1= install inode with non-data, non-directory files
               bit2= install inode with directories
*/
static
int img_update_ino(IsoImage *image, IsoNode *node, int flag)
{
    int ret;
    ino_t ino;
    unsigned int fs_id;
    dev_t dev_id;

    ret = iso_node_get_id(node, &fs_id, &dev_id, &ino, 1);
    if (ret < 0)
        return ret;
    if (ret == 0)
       ino = 0;
    if (((flag & 1) || ino == 0) &&
        (iso_node_get_type(node) == LIBISO_FILE || (flag & (2 | 4))) &&
        ((flag & 4) || iso_node_get_type(node) != LIBISO_DIR)) {
        ret = iso_node_set_unique_id(node, image, 0);
        if (ret < 0)
            return ret;
    }
    return 1;
}


/* @param flag bit0= overwrite any ino, else only ino == 0
               bit1= install inode with non-data, non-directory files
               bit2= install inode with directories
               bit3= with bit2: install inode on parameter dir
*/
int img_make_inos(IsoImage *image, IsoDir *dir, int flag)
{
    int ret;
    IsoDirIter *iter = NULL;
    IsoNode *node;
    IsoDir *subdir;

    if (flag & 8) {
        node = (IsoNode *) dir;
        ret = img_update_ino(image, node, flag & 7);
        if (ret < 0)
            goto ex;
    }
    ret = iso_dir_get_children(dir, &iter);
    if (ret < 0)
        return ret;
    while (iso_dir_iter_next(iter, &node) == 1) {
        ret = img_update_ino(image, node, flag & 7);
        if (ret < 0)
            goto ex;
        if (iso_node_get_type(node) == LIBISO_DIR) {
            subdir = (IsoDir *) node;
            ret = img_make_inos(image, subdir, flag & ~8);
            if (ret < 0)
                goto ex;
        }
    }
    ret = 1;
ex:;
    if (iter != NULL)
        iso_dir_iter_free(iter);
    return ret;
}


/* API */
int iso_image_get_session_md5(IsoImage *image, uint32_t *start_lba,
                              uint32_t *end_lba, char md5[16], int flag)
{
    if (image->checksum_array == NULL || image->checksum_idx_count < 1)
        return 0;
    *start_lba = image->checksum_start_lba;
    *end_lba = image->checksum_end_lba;
    memcpy(md5, image->checksum_array, 16);
    return ISO_SUCCESS;
}

int iso_image_set_checksums(IsoImage *image, char *checksum_array,
                            uint32_t start_lba, uint32_t end_lba,
                            uint32_t idx_count, int flag)
{
    iso_image_free_checksums(image, 0);
    image->checksum_array = checksum_array;
    image->checksum_start_lba = start_lba;
    image->checksum_end_lba = end_lba;
    image->checksum_idx_count = idx_count;
    return 1;
}

int iso_image_generator_is_running(IsoImage *image)
{
    return image->generator_is_running;
}


/* API */
int iso_image_add_mips_boot_file(IsoImage *image, char *path, int flag)
{
    if (image->num_mips_boot_files >= 15)
        return ISO_BOOT_TOO_MANY_MIPS;
    image->mips_boot_file_paths[image->num_mips_boot_files] = strdup(path);
    if (image->mips_boot_file_paths[image->num_mips_boot_files] == NULL)
        return ISO_OUT_OF_MEM;
    image->num_mips_boot_files++;
    return ISO_SUCCESS;
}

/* API */
int iso_image_get_mips_boot_files(IsoImage *image, char *paths[15], int flag)
{
    int i;

    for (i = 0; i < image->num_mips_boot_files; i++)
         paths[i] = image->mips_boot_file_paths[i];
    for (; i < 15; i++)
         paths[i] = NULL;
    return image->num_mips_boot_files;
}

/* API */
int iso_image_give_up_mips_boot(IsoImage *image, int flag)
{
    int i;

    for (i = 0; i < image->num_mips_boot_files; i++)
        if (image->mips_boot_file_paths[i] != NULL) {
            free(image->mips_boot_file_paths[i]);
            image->mips_boot_file_paths[i] = NULL;
        }
    image->num_mips_boot_files = 0;
    return ISO_SUCCESS;
}

static void unset_blessing(IsoImage *img, unsigned int idx)
{
    if (img->hfsplus_blessed[idx] != NULL)
        iso_node_unref(img->hfsplus_blessed[idx]);
    img->hfsplus_blessed[idx] = NULL;
}

/* API */
int iso_image_hfsplus_bless(IsoImage *img, enum IsoHfsplusBlessings blessing,
                            IsoNode *node, int flag)
{
    unsigned int i, ok = 0;

    if (flag & 2) {
        /* Delete any blessing */
        for (i = 0; i < ISO_HFSPLUS_BLESS_MAX; i++) {
            if (img->hfsplus_blessed[i] == node || node == NULL) {
                unset_blessing(img, i);
                ok = 1;
            }
        }
        return ok;
    }
    if (blessing == ISO_HFSPLUS_BLESS_MAX)
        return ISO_WRONG_ARG_VALUE;
    if (flag & 1) {
        /* Delete a particular blessing */
        if (img->hfsplus_blessed[blessing] == node || node == NULL) {
            unset_blessing(img, (unsigned int) blessing);
            return 1;
        }
        return 0;
    }

    if (node == NULL) {
        unset_blessing(img, (unsigned int) blessing);
        return 1;
    }
        
    /* No two hats on one node */
    for (i = 0; i < ISO_HFSPLUS_BLESS_MAX && node != NULL; i++)
        if (i != blessing && img->hfsplus_blessed[i] == node)
            return 0;
    /* Enforce correct file type */
    if (blessing == ISO_HFSPLUS_BLESS_INTEL_BOOTFILE) {
        if (node->type != LIBISO_FILE)
            return 0;
    } else {
        if (node->type != LIBISO_DIR)
            return 0;
    }

    unset_blessing(img, (unsigned int) blessing);
    img->hfsplus_blessed[blessing] = node;
    if (node != NULL)
        iso_node_ref(node);
    return 1;
}


/* API */
int iso_image_hfsplus_get_blessed(IsoImage *img, IsoNode ***blessed_nodes,
                                  int *bless_max, int flag)
{
    *blessed_nodes = img->hfsplus_blessed;
    *bless_max = ISO_HFSPLUS_BLESS_MAX;
    return 1;
}


/* API */
int iso_image_set_sparc_core(IsoImage *img, IsoFile *sparc_core, int flag)
{
    if (img->sparc_core_node != NULL)
        iso_node_unref((IsoNode *) img->sparc_core_node);
    img->sparc_core_node = sparc_core;
    if (sparc_core != NULL)
        iso_node_ref((IsoNode *) sparc_core);
    return 1;
}


/* API */
int iso_image_get_sparc_core(IsoImage *img, IsoFile **sparc_core, int flag)
{
    *sparc_core = img->sparc_core_node;
    return 1;
}


/* @param flag
           bit0= Let NULL parameters free the corresponding image properties.
                 Else only the non-NULL parameters of this call have an effect.
*/
static int hppa_palo_set_path(IsoImage *img, char *path, char **target,
                              char *what, int flag)
{
    int ret, err;
    IsoNode *node;
    IsoFile *file;

    if (path == NULL && !(flag & 1))
        return ISO_SUCCESS;
    if (iso_clone_mgtd_mem(path, target, 0) < 0)
        return ISO_OUT_OF_MEM;
    if (path == NULL)
        return ISO_SUCCESS;
    ret = iso_tree_path_to_node(img, path, &node);
    if (ret < 0)
        return ret;
    if (ret == 0) {
        iso_msg_submit(img->id, ISO_BOOT_FILE_MISSING, 0,
                       "Cannot find in ISO image: %s file '%s'", what, path);
        return ISO_BOOT_FILE_MISSING;
    }
    if (iso_node_get_type(node) != LIBISO_FILE) {
        err = ISO_HPPA_PALO_NOTREG;
        if (strncmp(what, "DEC Alpha", 9) == 0)
            err = ISO_ALPHA_BOOT_NOTREG;
        iso_msg_submit(img->id, err, 0,
                       "%s file is not a data file: '%s'", what, path);
        return err;
    }
    file = (IsoFile *) node;
    if (!(file->explicit_weight || file->from_old_session))
        file->sort_weight = 2;
    return ISO_SUCCESS;
}


/* API */
/* @param flag
          Bitfield for control purposes
           bit0= Let NULL parameters free the corresponding image properties.
                 Else only the non-NULL parameters of this call have an effect.
*/
int iso_image_set_hppa_palo(IsoImage *img, char *cmdline, char *bootloader,
                            char *kernel_32, char *kernel_64, char *ramdisk,
                            int flag)
{
    int ret;
    static char *what = "HP-PA PALO";

    if (cmdline != NULL || (flag & 1))
        if (iso_clone_mgtd_mem(cmdline, &(img->hppa_cmdline), 0) < 0)
            return ISO_OUT_OF_MEM;
    ret = hppa_palo_set_path(img, bootloader, &(img->hppa_bootloader), what,
                             flag & 1);
    if (ret < 0)
        return ret;
    ret = hppa_palo_set_path(img, kernel_32, &(img->hppa_kernel_32), what,
                             flag & 1);
    if (ret < 0)
        return ret;
    ret = hppa_palo_set_path(img, kernel_64, &(img->hppa_kernel_64), what,
                             flag & 1);
    if (ret < 0)
        return ret;
    ret = hppa_palo_set_path(img, ramdisk, &(img->hppa_ramdisk), what,
                             flag & 1);
    if (ret < 0)
        return ret;
    return ISO_SUCCESS;
}


/* API */
int iso_image_get_hppa_palo(IsoImage *img, char **cmdline, char **bootloader,
                            char **kernel_32, char **kernel_64, char **ramdisk)
{
    *cmdline = img->hppa_cmdline;
    *bootloader = img->hppa_bootloader;
    *kernel_32 = img->hppa_kernel_32;
    *kernel_64 = img->hppa_kernel_64;
    *ramdisk  = img->hppa_ramdisk;
    return ISO_SUCCESS;
}


/* API */
int iso_image_set_alpha_boot(IsoImage *img, char *boot_loader_path, int flag)
{
    int ret;

    ret = hppa_palo_set_path(img, boot_loader_path, &(img->alpha_boot_image),
                             "DEC Alpha Bootloader", 1);
    if (ret < 0)
        return ret;
    return ISO_SUCCESS;
}


/* API */
int iso_image_get_alpha_boot(IsoImage *img, char **boot_loader_path)
{
    *boot_loader_path = img->alpha_boot_image;
    return ISO_SUCCESS;
}


/* API */
int iso_image_set_truncate_mode(IsoImage *img, int mode, int length)
{
    if (mode < 0 || mode > 1)
        return ISO_WRONG_ARG_VALUE;
    if (length < 64 || length > LIBISOFS_NODE_NAME_MAX)
        return ISO_WRONG_ARG_VALUE;
    img->truncate_mode = mode;
    img->truncate_length = length;
    return ISO_SUCCESS;
}

/* API */
int iso_image_get_truncate_mode(IsoImage *img, int *mode, int *length)
{
    *mode = img->truncate_mode;
    *length = img->truncate_length;
    return ISO_SUCCESS;
}

/* Warning: Not thread-safe */
int iso_image_truncate_name(IsoImage *image, const char *name, char **namept,
                            int flag)
{
    int ret;

    if (name == NULL) 
        return ISO_NULL_POINTER;

    if ((int) strlen(name) <= image->truncate_length) {
        *namept = (char *) name;
        return ISO_SUCCESS;
    }
    *namept = image->truncate_buffer;
    if (name != image->truncate_buffer)
        strncpy(image->truncate_buffer, name, 4095);
    image->truncate_buffer[4095] = 0;
    ret = iso_truncate_rr_name(image->truncate_mode, image->truncate_length,
                               image->truncate_buffer, 0);
    return ret;
}

