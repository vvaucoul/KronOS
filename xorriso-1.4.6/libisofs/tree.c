/*
 * Copyright (c) 2007 Vreixo Formoso
 * Copyright (c) 2011 - 2015 Thomas Schmitt
 * 
 * This file is part of the libisofs project; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License version 2 
 * or later as published by the Free Software Foundation. 
 * See COPYING file for details.
 */

/*
 * Functions that act on the iso tree.
 */

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include "libisofs.h"
#include "node.h"
#include "image.h"
#include "fsource.h"
#include "builder.h"
#include "messages.h"
#include "tree.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>
#include <fnmatch.h>


/**
 * Add a new directory to the iso tree.
 * 
 * @param parent 
 *      the dir where the new directory will be created
 * @param name
 *      name for the new dir. If a node with same name already exists on
 *      parent, this functions fails with ISO_NODE_NAME_NOT_UNIQUE.
 * @param dir
 *      place where to store a pointer to the newly created dir. No extra
 *      ref is addded, so you will need to call iso_node_ref() if you really
 *      need it. You can pass NULL in this parameter if you don't need the
 *      pointer.
 * @return
 *     number of nodes in dir if succes, < 0 otherwise
 *     Possible errors:
 *         ISO_NULL_POINTER, if parent or name are NULL
 *         ISO_NODE_NAME_NOT_UNIQUE, a node with same name already exists
 */
int iso_tree_add_new_dir(IsoDir *parent, const char *name, IsoDir **dir)
{
    int ret;
    char *n;
    IsoDir *node;
    IsoNode **pos;
    time_t now;

    if (parent == NULL || name == NULL) {
        return ISO_NULL_POINTER;
    }
    if (dir) {
        *dir = NULL;
    }

    /* find place where to insert and check if it exists */
    if (iso_dir_exists(parent, name, &pos)) {
        /* a node with same name already exists */
        return ISO_NODE_NAME_NOT_UNIQUE;
    }

    n = strdup(name);
    ret = iso_node_new_dir(n, &node);
    if (ret < 0) {
        free(n);
        return ret;
    }

    /* permissions from parent */
    iso_node_set_permissions((IsoNode*)node, parent->node.mode);
    iso_node_set_uid((IsoNode*)node, parent->node.uid);
    iso_node_set_gid((IsoNode*)node, parent->node.gid);
    iso_node_set_hidden((IsoNode*)node, parent->node.hidden);

    /* current time */
    now = time(NULL);
    iso_node_set_atime((IsoNode*)node, now);
    iso_node_set_ctime((IsoNode*)node, now);
    iso_node_set_mtime((IsoNode*)node, now);

    if (dir) {
        *dir = node;
    }

    /* add to dir */
    return iso_dir_insert(parent, (IsoNode*)node, pos, ISO_REPLACE_NEVER);
}

int iso_image_add_new_dir(IsoImage *image, IsoDir *parent, const char *name,
                          IsoDir **dir)
{
    int ret;
    char *namept;

    ret = iso_image_truncate_name(image, name, &namept, 0);
    if (ret < 0)
        return ret;
    ret = iso_tree_add_new_dir(parent, namept, dir);
    return ret;
}

/**
 * Add a new symlink to the directory tree. Permissions are set to 0777, 
 * owner and hidden atts are taken from parent. You can modify any of them 
 * later.
 *  
 * @param parent 
 *      the dir where the new symlink will be created
 * @param name
 *      name for the new dir. If a node with same name already exists on
 *      parent, this functions fails with ISO_NODE_NAME_NOT_UNIQUE.
 * @param dest
 *      destination of the link
 * @param link
 *      place where to store a pointer to the newly created link. No extra
 *      ref is addded, so you will need to call iso_node_ref() if you really
 *      need it. You can pass NULL in this parameter if you don't need the
 *      pointer
 * @return
 *     number of nodes in parent if success, < 0 otherwise
 *     Possible errors:
 *         ISO_NULL_POINTER, if parent, name or dest are NULL
 *         ISO_NODE_NAME_NOT_UNIQUE, a node with same name already exists
 *         ISO_OUT_OF_MEM
 */
int iso_tree_add_new_symlink(IsoDir *parent, const char *name,
                             const char *dest, IsoSymlink **link)
{
    int ret;
    char *n, *d;
    IsoSymlink *node;
    IsoNode **pos;
    time_t now;

    if (parent == NULL || name == NULL || dest == NULL) {
        return ISO_NULL_POINTER;
    }
    if (link) {
        *link = NULL;
    }

    /* find place where to insert */
    if (iso_dir_exists(parent, name, &pos)) {
        /* a node with same name already exists */
        return ISO_NODE_NAME_NOT_UNIQUE;
    }

    n = strdup(name);
    d = strdup(dest);
    ret = iso_node_new_symlink(n, d, &node);
    if (ret < 0) {
        free(n);
        free(d);
        return ret;
    }

    /* permissions from parent */
    iso_node_set_permissions((IsoNode*)node, 0777);
    iso_node_set_uid((IsoNode*)node, parent->node.uid);
    iso_node_set_gid((IsoNode*)node, parent->node.gid);
    iso_node_set_hidden((IsoNode*)node, parent->node.hidden);

    /* current time */
    now = time(NULL);
    iso_node_set_atime((IsoNode*)node, now);
    iso_node_set_ctime((IsoNode*)node, now);
    iso_node_set_mtime((IsoNode*)node, now);

    if (link) {
        *link = node;
    }

    /* add to dir */
    return iso_dir_insert(parent, (IsoNode*)node, pos, ISO_REPLACE_NEVER);
}

int iso_image_add_new_symlink(IsoImage *image, IsoDir *parent,
                              const char *name, const char *dest,
                              IsoSymlink **link)
{
    int ret;
    char *namept;

    ret = iso_image_truncate_name(image, name, &namept, 0);
    if (ret < 0)
        return ret;
    ret = iso_tree_add_new_symlink(parent, namept, dest, link);
    return ret;
}

/**
 * Add a new special file to the directory tree. As far as libisofs concerns,
 * an special file is a block device, a character device, a FIFO (named pipe)
 * or a socket. You can choose the specific kind of file you want to add
 * by setting mode propertly (see man 2 stat).
 * 
 * Note that special files are only written to image when Rock Ridge 
 * extensions are enabled. Moreover, a special file is just a directory entry
 * in the image tree, no data is written beyond that.
 * 
 * Owner and hidden atts are taken from parent. You can modify any of them 
 * later.
 * 
 * @param parent
 *      the dir where the new special file will be created
 * @param name
 *      name for the new special file. If a node with same name already exists 
 *      on parent, this functions fails with ISO_NODE_NAME_NOT_UNIQUE.
 * @param mode
 *      file type and permissions for the new node. Note that you can't
 *      specify any kind of file here, only special types are allowed. i.e,
 *      S_IFSOCK, S_IFBLK, S_IFCHR and S_IFIFO are valid types; S_IFLNK, 
 *      S_IFREG and S_IFDIR aren't.
 * @param dev
 *      device ID, equivalent to the st_rdev field in man 2 stat.
 * @param special
 *      place where to store a pointer to the newly created special file. No 
 *      extra ref is addded, so you will need to call iso_node_ref() if you 
 *      really need it. You can pass NULL in this parameter if you don't need 
 *      the pointer.
 * @return
 *     number of nodes in parent if success, < 0 otherwise
 *     Possible errors:
 *         ISO_NULL_POINTER, if parent, name or dest are NULL
 *         ISO_NODE_NAME_NOT_UNIQUE, a node with same name already exists
 *         ISO_OUT_OF_MEM
 * 
 */
int iso_tree_add_new_special(IsoDir *parent, const char *name, mode_t mode,
                             dev_t dev, IsoSpecial **special)
{
    int ret;
    char *n;
    IsoSpecial *node;
    IsoNode **pos;
    time_t now;

    if (parent == NULL || name == NULL) {
        return ISO_NULL_POINTER;
    }
    if (S_ISLNK(mode) || S_ISREG(mode) || S_ISDIR(mode)) {
        return ISO_WRONG_ARG_VALUE;
    }
    if (special) {
        *special = NULL;
    }

    /* find place where to insert */
    if (iso_dir_exists(parent, name, &pos)) {
        /* a node with same name already exists */
        return ISO_NODE_NAME_NOT_UNIQUE;
    }

    n = strdup(name);
    ret = iso_node_new_special(n, mode, dev, &node);
    if (ret < 0) {
        free(n);
        return ret;
    }

    /* atts from parent */
    iso_node_set_uid((IsoNode*)node, parent->node.uid);
    iso_node_set_gid((IsoNode*)node, parent->node.gid);
    iso_node_set_hidden((IsoNode*)node, parent->node.hidden);

    /* current time */
    now = time(NULL);
    iso_node_set_atime((IsoNode*)node, now);
    iso_node_set_ctime((IsoNode*)node, now);
    iso_node_set_mtime((IsoNode*)node, now);

    if (special) {
        *special = node;
    }

    /* add to dir */
    return iso_dir_insert(parent, (IsoNode*)node, pos, ISO_REPLACE_NEVER);
}

int iso_image_add_new_special(IsoImage *image, IsoDir *parent,
                              const char *name, mode_t mode,
                              dev_t dev, IsoSpecial **special)
{
    int ret;
    char *namept;

    ret = iso_image_truncate_name(image, name, &namept, 0);
    if (ret < 0)
        return ret;
    ret = iso_tree_add_new_special(parent, namept, mode, dev, special);
    return ret;
}

/**
 * Add a new regular file to the iso tree. Permissions are set to 0444, 
 * owner and hidden atts are taken from parent. You can modify any of them 
 * later.
 *  
 * @param parent 
 *      the dir where the new file will be created
 * @param name
 *      name for the new file. If a node with same name already exists on
 *      parent, this functions fails with ISO_NODE_NAME_NOT_UNIQUE.
 * @param stream
 *      IsoStream for the contents of the file
 * @param file
 *      place where to store a pointer to the newly created file. No extra
 *      ref is addded, so you will need to call iso_node_ref() if you really
 *      need it. You can pass NULL in this parameter if you don't need the
 *      pointer
 * @return
 *     number of nodes in parent if success, < 0 otherwise
 *     Possible errors:
 *         ISO_NULL_POINTER, if parent, name or dest are NULL
 *         ISO_NODE_NAME_NOT_UNIQUE, a node with same name already exists
 *         ISO_OUT_OF_MEM
 * 
 * @since 0.6.4
 */
int iso_tree_add_new_file(IsoDir *parent, const char *name, IsoStream *stream, 
                          IsoFile **file)
{
    int ret;
    char *n;
    IsoFile *node;
    IsoNode **pos;
    time_t now;

    if (parent == NULL || name == NULL || stream == NULL) {
        return ISO_NULL_POINTER;
    }
    if (file) {
        *file = NULL;
    }

    /* find place where to insert */
    if (iso_dir_exists(parent, name, &pos)) {
        /* a node with same name already exists */
        return ISO_NODE_NAME_NOT_UNIQUE;
    }

    n = strdup(name);
    ret = iso_node_new_file(n, stream, &node);
    if (ret < 0) {
        free(n);
        return ret;
    }

    /* permissions from parent */
    iso_node_set_permissions((IsoNode*)node, 0444);
    iso_node_set_uid((IsoNode*)node, parent->node.uid);
    iso_node_set_gid((IsoNode*)node, parent->node.gid);
    iso_node_set_hidden((IsoNode*)node, parent->node.hidden);

    /* current time */
    now = time(NULL);
    iso_node_set_atime((IsoNode*)node, now);
    iso_node_set_ctime((IsoNode*)node, now);
    iso_node_set_mtime((IsoNode*)node, now);

    if (file) {
        *file = node;
    }

    /* add to dir */
    return iso_dir_insert(parent, (IsoNode*)node, pos, ISO_REPLACE_NEVER);
}

int iso_image_add_new_file(IsoImage *image, IsoDir *parent, const char *name,
                           IsoStream *stream, IsoFile **file)
{
    int ret;
    char *namept;

    ret = iso_image_truncate_name(image, name, &namept, 0);
    if (ret < 0)
        return ret;
    ret = iso_tree_add_new_file(parent, namept, stream, file);
    return ret;
}

/**
 * Set whether to follow or not symbolic links when added a file from a source
 * to IsoImage.
 */
void iso_tree_set_follow_symlinks(IsoImage *image, int follow)
{
    image->follow_symlinks = follow ? 1 : 0;
}

/**
 * Get current setting for follow_symlinks.
 * 
 * @see iso_tree_set_follow_symlinks
 */
int iso_tree_get_follow_symlinks(IsoImage *image)
{
    return image->follow_symlinks;
}

/**
 * Set whether to skip or not hidden files when adding a directory recursibely.
 * Default behavior is to not ignore them, i.e., to add hidden files to image.
 */
void iso_tree_set_ignore_hidden(IsoImage *image, int skip)
{
    image->ignore_hidden = skip ? 1 : 0;
}

/**
 * Get current setting for ignore_hidden.
 * 
 * @see iso_tree_set_ignore_hidden
 */
int iso_tree_get_ignore_hidden(IsoImage *image)
{
    return image->ignore_hidden;
}

void iso_tree_set_replace_mode(IsoImage *image, enum iso_replace_mode mode)
{
    image->replace = mode;
}

enum iso_replace_mode iso_tree_get_replace_mode(IsoImage *image)
{
    return image->replace;
}

/**
 * Set whether to skip or not special files. Default behavior is to not skip
 * them. Note that, despite of this setting, special files won't never be added
 * to an image unless RR extensions were enabled.
 * 
 * @param skip 
 *      Bitmask to determine what kind of special files will be skipped:
 *          bit0: ignore FIFOs
 *          bit1: ignore Sockets
 *          bit2: ignore char devices
 *          bit3: ignore block devices
 */
void iso_tree_set_ignore_special(IsoImage *image, int skip)
{
    image->ignore_special = skip & 0x0F;
}

/**
 * Get current setting for ignore_special.
 * 
 * @see iso_tree_set_ignore_special
 */
int iso_tree_get_ignore_special(IsoImage *image)
{
    return image->ignore_special;
}

/**
 * Set a callback function that libisofs will call for each file that is
 * added to the given image by a recursive addition function. This includes
 * image import.
 *  
 * @param report
 *      pointer to a function that will be called just before a file will be 
 *      added to the image. You can control whether the file will be in fact 
 *      added or ignored.
 *      This function should return 1 to add the file, 0 to ignore it and 
 *      continue, < 0 to abort the process
 *      NULL is allowed if you don't want any callback.
 */
void iso_tree_set_report_callback(IsoImage *image, 
                                  int (*report)(IsoImage*, IsoFileSource*))
{
    image->report = report;
}

/**
 * Add a excluded path. These are paths that won't never added to image,
 * and will be excluded even when adding recursively its parent directory.
 * 
 * For example, in
 * 
 * iso_tree_add_exclude(image, "/home/user/data/private");
 * iso_tree_add_dir_rec(image, root, "/home/user/data");
 * 
 * the directory /home/user/data/private won't be added to image.
 * 
 * @return
 *      1 on success, < 0 on error
 */
int iso_tree_add_exclude(IsoImage *image, const char *path)
{
    if (image == NULL || path == NULL) {
        return ISO_NULL_POINTER;
    }
    image->excludes = realloc(image->excludes, ++image->nexcludes * 
                              sizeof(void*));
    if (image->excludes == NULL) {
        return ISO_OUT_OF_MEM;
    }
    image->excludes[image->nexcludes - 1] = strdup(path);
    if (image->excludes[image->nexcludes - 1] == NULL) {
        return ISO_OUT_OF_MEM;
    }
    return ISO_SUCCESS;
}

/**
 * Remove a previously added exclude.
 * 
 * @see iso_tree_add_exclude
 * @return
 *      1 on success, 0 exclude do not exists, < 0 on error
 */
int iso_tree_remove_exclude(IsoImage *image, const char *path)
{
    size_t i, j;

    if (image == NULL || path == NULL) {
        return ISO_NULL_POINTER;
    }

    for (i = 0; (int) i < image->nexcludes; ++i) {
        if (strcmp(image->excludes[i], path) == 0) {
            /* exclude found */
            free(image->excludes[i]);
            --image->nexcludes;
            for (j = i; (int) j < image->nexcludes; ++j) {
                image->excludes[j] = image->excludes[j+1]; 
            }
            image->excludes = realloc(image->excludes, image->nexcludes * 
                                      sizeof(void*));
            return ISO_SUCCESS;
        }
    }
    return 0;
}

static
int iso_tree_add_node_builder(IsoImage *image, IsoDir *parent,
                              IsoFileSource *src, IsoNodeBuilder *builder,
                              IsoNode **node)
{
    int result;
    IsoNode *new;
    IsoNode **pos;
    char *name = NULL, *namept;

    if (parent == NULL || src == NULL || builder == NULL) {
        result = ISO_NULL_POINTER; goto ex;
    }
    if (node) {
        *node = NULL;
    }

    name = iso_file_source_get_name(src);

    result = iso_image_truncate_name(image, name, &namept, 0);
    if (result < 0)
        return result;

    /* find place where to insert */
    result = iso_dir_exists(parent, namept, &pos);
    if (result) {
        /* a node with same name already exists */
        result = ISO_NODE_NAME_NOT_UNIQUE; goto ex;
    }

    result = builder->create_node(builder, image, src, namept, &new);
    if (result < 0)
        goto ex;

    if (node) {
        *node = new;
    }

    /* finally, add node to parent */
    result = iso_dir_insert(parent, (IsoNode*)new, pos, ISO_REPLACE_NEVER);
ex:
    if (name != NULL)
        free(name);
    return result;
}

int iso_tree_add_node(IsoImage *image, IsoDir *parent, const char *path,
                      IsoNode **node)
{
    int result;
    IsoFilesystem *fs;
    IsoFileSource *file;

    if (image == NULL || parent == NULL || path == NULL) {
        return ISO_NULL_POINTER;
    }

    fs = image->fs;
    result = fs->get_by_path(fs, path, &file);
    if (result < 0) {
        return result;
    }
    result = iso_tree_add_node_builder(image, parent, file, image->builder,
                                       node);
    /* free the file */
    iso_file_source_unref(file);
    return result;
}

int iso_tree_add_new_node(IsoImage *image, IsoDir *parent, const char *name, 
                          const char *path, IsoNode **node)
{
    int result;
    IsoFilesystem *fs;
    IsoFileSource *file;
    IsoNode *new;
    IsoNode **pos;
    char *namept;

    if (image == NULL || parent == NULL || name == NULL || path == NULL) {
        return ISO_NULL_POINTER;
    }

    if (node) {
        *node = NULL;
    }

    result = iso_image_truncate_name(image, name, &namept, 0);
    if (result < 0)
        return result;

    /* find place where to insert */
    result = iso_dir_exists(parent, namept, &pos);
    if (result) {
        /* a node with same name already exists */
        return ISO_NODE_NAME_NOT_UNIQUE;
    }

    fs = image->fs;
    result = fs->get_by_path(fs, path, &file);
    if (result < 0) {
        return result;
    }

    result = image->builder->create_node(image->builder, image, file,
                                         namept, &new);
    
    /* free the file */
    iso_file_source_unref(file);
    
    if (result < 0) {
        return result;
    }
    
    if (node) {
        *node = new;
    }

    /* finally, add node to parent */
    return iso_dir_insert(parent, new, pos, ISO_REPLACE_NEVER);
}

int iso_tree_add_new_cut_out_node(IsoImage *image, IsoDir *parent, 
                                  const char *name, const char *path, 
                                  off_t offset, off_t size,
                                  IsoNode **node)
{
    int result;
    struct stat info;
    IsoFilesystem *fs;
    IsoFileSource *src;
    IsoFile *new;
    IsoNode **pos;
    IsoStream *stream;
    char *namept;

    if (image == NULL || parent == NULL || name == NULL || path == NULL) {
        return ISO_NULL_POINTER;
    }

    if (node) {
        *node = NULL;
    }

    result = iso_image_truncate_name(image, name, &namept, 0);
    if (result < 0)
        return result;

    /* find place where to insert */
    result = iso_dir_exists(parent, namept, &pos);
    if (result) {
        /* a node with same name already exists */
        return ISO_NODE_NAME_NOT_UNIQUE;
    }

    fs = image->fs;
    result = fs->get_by_path(fs, path, &src);
    if (result < 0) {
        return result;
    }

    result = iso_file_source_stat(src, &info);
    if (result < 0) {
        iso_file_source_unref(src);
        return result;
    }
    if (!S_ISREG(info.st_mode)) {
        return ISO_WRONG_ARG_VALUE;
    }
    if (offset >= info.st_size) {
        return ISO_WRONG_ARG_VALUE;
    }

    /* force regular file */
    result = image->builder->create_file(image->builder, image, src, &new);
    
    /* free the file */
    iso_file_source_unref(src);
    
    if (result < 0) {
        return result;
    }
    
    /* replace file iso stream with a cut-out-stream */
    result = iso_cut_out_stream_new(src, offset, size, &stream);
    if (result < 0) {
        iso_node_unref((IsoNode*)new);
        return result;
    }
    iso_stream_unref(new->stream);
    new->stream = stream;
    
    result = iso_node_set_name((IsoNode*)new, namept);
    if (result < 0) {
        iso_node_unref((IsoNode*)new);
        return result;
    }

    if (node) {
        *node = (IsoNode*)new;
    }

    /* finally, add node to parent */
    return iso_dir_insert(parent, (IsoNode*)new, pos, ISO_REPLACE_NEVER);
}

static
int check_excludes(IsoImage *image, const char *path)
{
    int i;

    for (i = 0; i < image->nexcludes; ++i) {
        char *exclude = image->excludes[i];
        if (exclude[0] == '/') {
            /* absolute exclude, must completely match path */
            if (!fnmatch(exclude, path, FNM_PERIOD|FNM_PATHNAME)) {
                return 1;
            }
        } else {
            /* relative exclude, it is enought if a part of the path matches */
            char *pos = (char*)path;
            while (pos != NULL) {
                pos++;
                if (!fnmatch(exclude, pos, FNM_PERIOD|FNM_PATHNAME)) {
                    return 1;
                }
                pos = strchr(pos, '/');
            }
        }
    }
    return 0;
}

static
int check_hidden(IsoImage *image, const char *name)
{
    return (image->ignore_hidden && name[0] == '.');
}

static
int check_special(IsoImage *image, mode_t mode)
{
    if (image->ignore_special != 0) {
        switch(mode &  S_IFMT) {
        case S_IFBLK:
            return image->ignore_special & 0x08 ? 1 : 0;
        case S_IFCHR:
            return image->ignore_special & 0x04 ? 1 : 0;
        case S_IFSOCK:
            return image->ignore_special & 0x02 ? 1 : 0;
        case S_IFIFO:
            return image->ignore_special & 0x01 ? 1 : 0;
        default:
            return 0;
        }
    }
    return 0;
}


static
void ascii_increment(char *name, int len, int pos, int rollover_carry)
{
     int c;

again:;
     if (pos < 0 || pos >= len)
         pos = len - 1;
     c = name[pos];
     if (c >= '0' && c < '9') {
         c++;
     } else if (c == '9') {
         c = 'A';
     } else if (c >= 'A' && c < 'Z') {
         c++;
     } else if (c == 'Z') {
         c = '_';
     } else if (c == '_') {
         c = 'a';
     } else if (c >= 'a' && c < 'z') {
         c++;
     } else if (c == 'z') {
         c = '0';
         name[pos] = c;
         pos--;
         if (pos >= 0 || rollover_carry)
             goto again;
         return;
     } else {
         if (pos == len - 1 || name[pos + 1] == '.')
             c = '_'; /* Make first change less riddling */
         else
             c = '0'; /* But else use the full range of valid characters */
     }
     name[pos] = c;
}

static
int insert_underscores(char *name, int *len, int *at_pos, int count,
                       char **new_name)
{
    int ret;

    LIBISO_ALLOC_MEM(*new_name, char, count + *len + 1);
    if (*at_pos > 0)
        memcpy(*new_name, name, *at_pos);
    if (count > 0)
        memset(*new_name + *at_pos, '_', count);
    if (*len > *at_pos)
        memcpy(*new_name + *at_pos + count, name + *at_pos, *len - *at_pos);
    (*new_name)[count + *len] = 0;
    *len += count;
    *at_pos += count;
    ret= ISO_SUCCESS;
ex:;
    return ret;
}

static
int make_incrementable_name(char **name, char **unique_name, int *low_pos,
                            int *rollover_carry, int *pre_check) 
{
    char *dpt, *npt;
    int first, len, ret;
    
    /* The incrementable part of the file shall have at least 7 characters.
       There may be up to pow(2.0,32.0)*2048/33 = 266548273400 files.
       The set of increment result characters has 63 elements.
       pow(63.0,7.0) is nearly 15 times larger than 266548273400.
    */
    static int min_incr = 7;

    /* At most two suffixes of total length up to 12, like .tar.bz2,
       shall be preserved. The incrementable part will eventually be
       padded up.
       Incrementing begins before the last suffix in any case. But when this
       rolls over on short prefixes, then long last suffixes will get used
       as high characters of the incremental part. This is indicated by
       *rollover_carry which corresponds to the parameter of ascii_increment()
       with the same name.
    */
    static int max_suffix = 12;

    *rollover_carry = 0;
    *pre_check = 0;

    len = strlen(*name);

    /* Check if the part before the first dot is long enough.
       If not, then preserve the last two short suffixes.
    */
    dpt = strchr(*name, '.');
    if (dpt != NULL)
        if ((dpt - *name) < min_incr)
            dpt = strrchr(*name, '.');
    if (dpt != NULL) {
        first= (dpt - *name);
        if (dpt > *name && len - first < max_suffix) {
            for(npt = dpt - 1; npt >= *name && *npt != '.'; npt--);
            if (npt >= *name) {
                if (len - (npt - *name) <= max_suffix) {
                    first= (npt - *name);
                    dpt = npt;
                }
            }
        }
    } else
        first= len;
    if (first < min_incr && (len - first) <= max_suffix) {
        ret = insert_underscores(*name, &len, &first, min_incr - first,
                                 unique_name);
        if (ret < 0)
            goto ex;
        *pre_check = 1; /* It might now already be unique */

    } else if (len < 64) {
        /* Insert an underscore to preserve the original name at least for the
           first few increments
        */
        ret = insert_underscores(*name, &len, &first, 1, unique_name);
        if (ret < 0)
            goto ex;
        *pre_check = 1;

    } else {
        LIBISO_ALLOC_MEM(*unique_name, char, len + 1);
        memcpy(*unique_name, *name, len);
        if (first < min_incr)
            *rollover_carry = 1; /* Do not get caged before the dots */
    }
    (*unique_name)[len] = 0;
    *low_pos = first - 1;
    ret = 1;
ex:;
    return(ret);
}

static
int make_really_unique_name(IsoDir *parent, char **name, char **unique_name,
                            IsoNode ***pos, int flag)
{
    int ret, rollover_carry = 0, pre_check = 0, ascii_idx = -1, len;

    ret = make_incrementable_name(name, unique_name, &ascii_idx,
                                  &rollover_carry, &pre_check);
    if (ret < 0)
        goto ex;
    len = strlen(*unique_name);
    while (1) {
        if (!pre_check)
            ascii_increment(*unique_name, len, ascii_idx, !!rollover_carry);
        else
            pre_check = 0;
        ret = iso_dir_exists(parent, *unique_name, pos);
        if (ret < 0)
            goto ex;
        if (ret == 0)
    break;
    }
    *name = *unique_name;
    ret = ISO_SUCCESS;
ex:;
    if (ret < 0) {
        LIBISO_FREE_MEM(*unique_name);
        *unique_name = NULL;
    }
    return ret;
}

/**
 * Recursively add a given directory to the image tree.
 * 
 * @return
 *      1 continue, < 0 error (ISO_CANCELED stop)
 */
int iso_add_dir_src_rec(IsoImage *image, IsoDir *parent, IsoFileSource *dir)
{
    int ret, dir_is_open = 0;
    IsoNodeBuilder *builder;
    IsoFileSource *file;
    IsoNode **pos;
    struct stat info;
    char *name, *path, *allocated_name = NULL;
    IsoNode *new;
    enum iso_replace_mode replace;

    ret = iso_file_source_open(dir);
    if (ret < 0) {
        path = iso_file_source_get_path(dir);
        /* instead of the probable error, we throw a sorry event */
	if (path != NULL) {
            ret = iso_msg_submit(image->id, ISO_FILE_CANT_ADD, ret, 
                                 "Can't open dir %s", path);
            free(path);
        } else {
            ret = iso_msg_submit(image->id, ISO_NULL_POINTER, ret,
                           "Can't open dir. NULL pointer caught as dir name");
        }
        goto ex;
    }
    dir_is_open = 1;

    builder = image->builder;
    
    /* iterate over all directory children */
    while (1) {
        int skip = 0;

        ret = iso_file_source_readdir(dir, &file);
        if (ret <= 0) {
            if (ret < 0) {
                /* error reading dir */
                ret = iso_msg_submit(image->id, ret, ret, "Error reading dir");
                goto ex;
            }
    break; /* End of directory */
        }

        path = iso_file_source_get_path(file);
        if (path == NULL) {
            ret = iso_msg_submit(image->id, ISO_NULL_POINTER, ret, 
                                 "NULL pointer caught as file path");
            goto ex;
        }
        name = strrchr(path, '/') + 1;

        if (image->follow_symlinks) {
            ret = iso_file_source_stat(file, &info);
        } else {
            ret = iso_file_source_lstat(file, &info);
        }
        if (ret < 0) {
            ret = iso_msg_submit(image->id, ISO_FILE_CANT_ADD, ret,
                                 "Error when adding file %s", path);
            goto dir_rec_continue;
        }

        if (check_excludes(image, path)) {
            iso_msg_debug(image->id, "Skipping excluded file %s", path);
            skip = 1;
        } else if (check_hidden(image, name)) {
            iso_msg_debug(image->id, "Skipping hidden file %s", path);
            skip = 1;
        } else if (check_special(image, info.st_mode)) {
            iso_msg_debug(image->id, "Skipping special file %s", path);
            skip = 1;
        }

        if (skip) {
            goto dir_rec_continue;
        }

        replace = image->replace;

        /* find place where to insert */
        ret = iso_dir_exists(parent, name, &pos);
        if (ret) {
            /* Resolve name collision
               e.g. caused by fs_image.c:make_hopefully_unique_name() 
            */
            LIBISO_FREE_MEM(allocated_name); allocated_name = NULL;
            ret = make_really_unique_name(parent, &name, &allocated_name, &pos,
                                          0);
            if (ret < 0)
                goto ex;
            image->collision_warnings++;
            if (image->collision_warnings < ISO_IMPORT_COLL_WARN_MAX) {
                ret = iso_msg_submit(image->id, ISO_IMPORT_COLLISION, 0, 
                         "File name collision resolved with %s . Now: %s",
                         path, name);
                if (ret < 0)
                    goto ex;
            }
        }

        /* if we are here we must insert. Give user a chance for cancel */
        if (image->report) {
            int r = image->report(image, file);
            if (r <= 0) {
                ret = (r < 0 ? ISO_CANCELED : ISO_SUCCESS);
                goto dir_rec_continue;
            }
        }
        ret = builder->create_node(builder, image, file, name, &new);
        if (ret < 0) {
            ret = iso_msg_submit(image->id, ISO_FILE_CANT_ADD, ret,
                         "Error when adding file %s", path);
            goto dir_rec_continue;
        }

        /* ok, node has correctly created, we need to add it */
        ret = iso_dir_insert(parent, new, pos, replace);
        if (ret < 0) {
            iso_node_unref(new);
            if (ret != (int) ISO_NODE_NAME_NOT_UNIQUE) {
                /* error */
                goto dir_rec_continue;
            } else {
                /* file ignored because a file with same node already exists */
                iso_msg_debug(image->id, "Skipping file %s. A node with same "
                              "file already exists", path);
                ret = 0;
            }
        } else {
            iso_msg_debug(image->id, "Added file %s", path);
        }

        /* finally, if the node is a directory we need to recurse */
        if (new->type == LIBISO_DIR && S_ISDIR(info.st_mode)) {
            ret = iso_add_dir_src_rec(image, (IsoDir*)new, file);
        }

dir_rec_continue:;
        free(path);
        iso_file_source_unref(file);
        
        /* check for error severity to decide what to do */
        if (ret < 0) {
            ret = iso_msg_submit(image->id, ret, 0, NULL);
            if (ret < 0)
                goto ex;
        }
    } /* while */

    ret = ISO_SUCCESS;
ex:;
    if (dir_is_open)
        iso_file_source_close(dir);
    LIBISO_FREE_MEM(allocated_name);
    return ret;
}

int iso_tree_add_dir_rec(IsoImage *image, IsoDir *parent, const char *dir)
{
    int result;
    struct stat info;
    IsoFilesystem *fs;
    IsoFileSource *file;

    if (image == NULL || parent == NULL || dir == NULL) {
        return ISO_NULL_POINTER;
    }

    fs = image->fs;
    result = fs->get_by_path(fs, dir, &file);
    if (result < 0) {
        return result;
    }

    /* we also allow dir path to be a symlink to a dir */
    result = iso_file_source_stat(file, &info);
    if (result < 0) {
        iso_file_source_unref(file);
        return result;
    }

    if (!S_ISDIR(info.st_mode)) {
        iso_file_source_unref(file);
        return ISO_FILE_IS_NOT_DIR;
    }
    result = iso_add_dir_src_rec(image, parent, file);
    iso_file_source_unref(file);
    return result;
}

/* @param flag bit0= truncate according to image truncate mode and length
*/
int iso_tree_path_to_node_flag(IsoImage *image, const char *path,
                               IsoNode **node, int flag)
{
    int result;
    IsoNode *n;
    IsoDir *dir;
    char *ptr, *brk_info = NULL, *component;

    if (image == NULL || path == NULL) {
        return ISO_NULL_POINTER;
    }

    /* get the first child at the root of the image that is "/" */
    dir = image->root;
    n = (IsoNode *)dir;
    if (!strcmp(path, "/")) {
        if (node) {
            *node = n;
        }
        return ISO_SUCCESS;
    }

    ptr = strdup(path);
    if (ptr == NULL)
        return ISO_OUT_OF_MEM;
    result = 0;

    /* get the first component of the path */
    component = strtok_r(ptr, "/", &brk_info);
    while (component) {
        if (n->type != LIBISO_DIR) {
            n = NULL;
            result = 0;
            break;
        }
        dir = (IsoDir *)n;

        if ((flag & 1) && image->truncate_mode == 1) {
            result = iso_dir_get_node_trunc(dir, image->truncate_length,
                                            component, &n);
        } else {
            result = iso_dir_get_node(dir, component, &n);
        }
        if (result != 1) {
            n = NULL;
            break;
        }

        component = strtok_r(NULL, "/", &brk_info);
    }

    free(ptr);
    if (node) {
        *node = n;
    }
    return result;
}

int iso_tree_path_to_node(IsoImage *image, const char *path, IsoNode **node)
{
    return iso_tree_path_to_node_flag(image, path, node, 0);
}

int iso_image_path_to_node(IsoImage *image, const char *path, IsoNode **node)
{
    return iso_tree_path_to_node_flag(image, path, node, 1);
}

char *iso_tree_get_node_path(IsoNode *node)
{
    char *path = NULL, *parent_path = NULL;

    if (node == NULL || node->parent == NULL)
        return NULL;
    
    if ((IsoNode*)node->parent == node) {
        return strdup("/");
    } else {
        parent_path = iso_tree_get_node_path((IsoNode*)node->parent);
        if (parent_path == NULL)
            goto ex;
        if (strlen(parent_path) == 1) {
            path = calloc(1, strlen(node->name) + 2);
            if (path == NULL)
                goto ex;
            sprintf(path, "/%s", node->name);
        } else {
            path = calloc(1, strlen(parent_path) + strlen(node->name) + 2);
            if (path == NULL)
                goto ex;
            sprintf(path, "%s/%s", parent_path, node->name);
        }
    }
ex:;
    if (parent_path != NULL)
        free(parent_path);
    return path;
}

/* Note: No reference is taken to the found node.
   @param flag bit0= recursion
*/
int iso_tree_get_node_of_block(IsoImage *image, IsoDir *dir, uint32_t block,
                               IsoNode **found, uint32_t *next_above, int flag)
{
    int ret, section_count, i;
    IsoDirIter *iter = NULL;
    IsoNode *node;
    IsoDir *subdir;
    IsoFile *file;
    struct iso_file_section *sections = NULL;
    uint32_t na = 0;

    if (dir == NULL)
        dir = image->root;

    ret = iso_dir_get_children(dir, &iter);
    while (iso_dir_iter_next(iter, &node) == 1 ) {

        if (ISO_NODE_IS_FILE(node)) {
            file = (IsoFile *) node;
            ret = iso_file_get_old_image_sections(file, &section_count,
                                                  &sections, 0);
            if (ret <= 0)
    continue;
            for (i = 0; i < section_count; i++) {
                if (sections[i].block <= block &&
                    block - sections[i].block <
                                  (((off_t) sections[i].size) + 2047) / 2048) {
                    *found = node;
                    ret = 1; goto ex;
                }
                if ((na == 0 || sections[i].block < na) &&
                                                     sections[i].block > block)
                    na = sections[i].block;
            }
            free(sections); sections = NULL;
        } else if (ISO_NODE_IS_DIR(node)) {
            subdir = (IsoDir *) node;
            ret = iso_tree_get_node_of_block(image, subdir, block, found, &na,
                                             1);
            if (ret != 0)
                goto ex;
        }
    }
    if (next_above != NULL && (na  > 0 || !(flag & 1)))
        if (*next_above == 0 || *next_above > na || !(flag & 1))
            *next_above = na;
    ret = 0;
ex:
    if (sections != NULL)
        free(sections);
    if (iter != NULL)
        iso_dir_iter_free(iter);
    return ret;
}


/* ------------------------- tree cloning ------------------------------ */

static
int iso_tree_copy_node_attr(IsoNode *old_node, IsoNode *new_node, int flag)
{
    int ret;

    new_node->mode = old_node->mode;
    new_node->uid = old_node->uid;
    new_node->gid = old_node->gid;
    new_node->atime = old_node->atime;
    new_node->mtime = old_node->mtime;
    new_node->ctime = old_node->ctime;
    new_node->hidden = old_node->hidden;
    ret = iso_node_clone_xinfo(old_node, new_node, 0);
    if (ret < 0)
        return ret;
    return ISO_SUCCESS;
}

/*
  @param flag bit0= merge directory with *new_node
*/
static
int iso_tree_clone_dir(IsoDir *old_dir,
                       IsoDir *new_parent, char *new_name, IsoNode **new_node,
                       int flag)
{
    IsoDir *new_dir = NULL;
    IsoNode *sub_node = NULL, *new_sub_node = NULL;
    IsoDirIter *iter = NULL;
    int ret;

    if (flag & 1) {
        new_dir = (IsoDir *) *new_node;
    } else {
        *new_node = NULL;
        ret = iso_tree_add_new_dir(new_parent, new_name, &new_dir);
        if (ret < 0)
           return ret;
    }
    /* Avoid traversal of target directory to allow cloning of old_dir to a
       subordinate of old_dir.
    */
    iso_node_take((IsoNode *) new_dir);

    ret = iso_dir_get_children(old_dir, &iter);
    if (ret < 0)
        goto ex;
    while(1) {
        ret = iso_dir_iter_next(iter, &sub_node);
        if (ret == 0)
    break;
        ret = iso_tree_clone(sub_node, new_dir, sub_node->name, &new_sub_node,
                             flag & 1);
        if (ret < 0)
            goto ex;
    }

    /* Now graft in the new tree resp. graft back the merged tree */
    ret = iso_dir_add_node(new_parent, (IsoNode *) new_dir, 0);
    if (ret < 0)
        goto ex;

    if (!(flag & 1))
        *new_node = (IsoNode *) new_dir;
    ret = ISO_SUCCESS;
ex:;
    if (iter != NULL)
        iso_dir_iter_free(iter);
    if (ret < 0 && new_dir != NULL) {
	if (flag & 1) {
            /* graft back the merged tree (eventually with half copy) */
            iso_dir_add_node(new_parent, (IsoNode *) new_dir, 0);
        } else {
            iso_node_remove_tree((IsoNode *) new_dir, NULL);
            *new_node = NULL;
        }
    }
    return ret;
}

static
int iso_tree_clone_file(IsoFile *old_file,
                        IsoDir *new_parent, char *new_name, IsoNode **new_node,
                        int flag)
{
    IsoStream *new_stream = NULL;
    IsoFile *new_file = NULL;
    int ret;

    *new_node = NULL;

    ret = iso_stream_clone(old_file->stream, &new_stream, 0);
    if (ret < 0)
        return ret;

    ret = iso_tree_add_new_file(new_parent, new_name, new_stream, &new_file);
    if (ret < 0)
        goto ex;
    new_stream = NULL; /* now owned by new_file */
    new_file->sort_weight = old_file->sort_weight;
    *new_node = (IsoNode *) new_file;
    ret = ISO_SUCCESS;
ex:;
    if (new_stream != NULL)
        iso_stream_unref(new_stream);
    return ret;
}

static
int iso_tree_clone_symlink(IsoSymlink *node,
                        IsoDir *new_parent, char *new_name, IsoNode **new_node,
                        int flag)
{
    IsoSymlink *new_sym;
    int ret;

    *new_node = NULL;

    ret = iso_tree_add_new_symlink(new_parent, new_name, node->dest, &new_sym);
    if (ret < 0)
        return ret;
    new_sym->fs_id = node->fs_id;
    new_sym->st_dev = node->st_dev;
    new_sym->st_ino = node->st_ino;
    *new_node = (IsoNode *) new_sym;
    return ISO_SUCCESS;
}

static
int iso_tree_clone_special(IsoSpecial *node,
                        IsoDir *new_parent, char *new_name, IsoNode **new_node,
                        int flag)
{
    IsoSpecial *new_spec;
    IsoNode *iso_node;
    int ret;

    iso_node = (IsoNode *) node;
    ret = iso_tree_add_new_special(new_parent, new_name, iso_node->mode,
                                   node->dev, &new_spec);
    if (ret < 0)
        return ret;
    new_spec->fs_id = node->fs_id;
    new_spec->st_dev = node->st_dev;
    new_spec->st_ino = node->st_ino;
    *new_node = (IsoNode *) new_spec;
    return ISO_SUCCESS;
}


/* @param flag bit0= Merge directories rather than ISO_NODE_NAME_NOT_UNIQUE.
               bit1= issue warning in case of truncation
*/
int iso_tree_clone_trunc(IsoNode *node, IsoDir *new_parent, 
                         char *new_name_in, IsoNode **new_node, 
                         int truncate_length, int flag)
{
    int ret = ISO_SUCCESS;
    char *new_name, *trunc = NULL;

    *new_node = NULL;
    new_name = new_name_in;
    if (truncate_length >= 64 && (int) strlen(new_name) > truncate_length) {
        trunc = strdup(new_name);
        if (trunc == 0) {
            ret = ISO_OUT_OF_MEM;
            goto ex;
        }
        ret = iso_truncate_rr_name(1, truncate_length, trunc, !(flag & 2));
        if (ret < 0)
            goto ex;
        new_name = trunc;
    }
    if (iso_dir_get_node(new_parent, new_name, new_node) == 1) {
        if (! (node->type == LIBISO_DIR && (*new_node)->type == LIBISO_DIR &&
               (flag & 1))) {
            *new_node = NULL;
            ret = ISO_NODE_NAME_NOT_UNIQUE;
            goto ex;
        }
    } else
        flag &= ~1;

    if (node->type == LIBISO_DIR) {
        ret = iso_tree_clone_dir((IsoDir *) node, new_parent, new_name,
                                 new_node, flag & 1);
    } else if (node->type == LIBISO_FILE) {
        ret = iso_tree_clone_file((IsoFile *) node, new_parent, new_name, 
                                  new_node, 0);
    } else if (node->type == LIBISO_SYMLINK) {
        ret = iso_tree_clone_symlink((IsoSymlink *) node, new_parent, new_name,
                                  new_node, 0);
    } else if (node->type == LIBISO_SPECIAL) {
        ret = iso_tree_clone_special((IsoSpecial *) node, new_parent, new_name,
                                    new_node, 0);
    } else if (node->type == LIBISO_BOOT) {
        ret = ISO_SUCCESS; /* API says they are silently ignored */
    }
    if (ret < 0)
        goto ex;
    if (flag & 1) {
        ret = 2; /* merged two directories, *new_node is not new */
        goto ex;
    }
    ret = iso_tree_copy_node_attr(node, *new_node, 0);

ex:;
    if (trunc != NULL)
        free(trunc);
    return ret;
}


/* API */
int iso_tree_clone(IsoNode *node,
                   IsoDir *new_parent, char *new_name, IsoNode **new_node,
                   int flag)
{
    return iso_tree_clone_trunc(node, new_parent, new_name, new_node, 0,
                                flag & 1); 
}


/* API */
int iso_image_tree_clone(IsoImage *image, IsoNode *node, IsoDir *new_parent,
                         char *new_name, IsoNode **new_node, int flag)
{
    int length, ret;

    if (image->truncate_mode == 0)
        length = 0;
    else
        length = image->truncate_length;
    ret = iso_tree_clone_trunc(node, new_parent, new_name, new_node, length,
                               flag & 3);
    return ret;
}


int iso_tree_resolve_symlink(IsoImage *img, IsoSymlink *sym, IsoNode **res,
                             int *depth, int flag)
{
    IsoDir *cur_dir = NULL;
    IsoNode *n, *resolved_node;
    char *dest, *dest_start, *dest_end;
    int ret = 0;
    unsigned int comp_len, dest_len;

    dest = sym->dest;
    dest_len = strlen(dest);

    if (dest[0] == '/') {

        /* ??? How to resolve absolute links without knowing the
               path of the future mount point ?
           ??? Would it be better to throw error ? 
           I can only assume that it gets mounted at / during some stage
           of booting.
        */;

        cur_dir = img->root;
        dest_end = dest;
    } else {
        cur_dir = sym->node.parent;
        if (cur_dir == NULL)
            cur_dir = img->root;
        dest_end = dest - 1;
    }

    while (dest_end < dest + dest_len) {
        dest_start = dest_end + 1;
        dest_end = strchr(dest_start, '/');
        if (dest_end == NULL)
            dest_end = dest_start + strlen(dest_start);
        comp_len = dest_end - dest_start;
        if (comp_len == 0 || (comp_len == 1 && dest_start[0] == '.'))
    continue;
        if (comp_len == 2 && dest_start[0] == '.' && dest_start[1] == '.') {
            cur_dir = cur_dir->node.parent;
            if (cur_dir == NULL) /* link shoots over root */
                return ISO_DEAD_SYMLINK;
    continue;
        }

        /* Search node in cur_dir */
        for (n = cur_dir->children; n != NULL; n = n->next)
            if (strncmp(dest_start, n->name, comp_len) == 0 &&
                strlen(n->name) == comp_len)
        break;
        if (n == NULL)
            return ISO_DEAD_SYMLINK;

        if (n->type == LIBISO_DIR) {
            cur_dir = (IsoDir *) n;
        } else if (n->type == LIBISO_SYMLINK) {
            if (*depth >= LIBISO_MAX_LINK_DEPTH)
                return ISO_DEEP_SYMLINK;
            (*depth)++;
            ret = iso_tree_resolve_symlink(img, (IsoSymlink *) n,
                                           &resolved_node, depth, 0);
            if (ret < 0)
                return ret;
            if (resolved_node->type != LIBISO_DIR) {
                n = resolved_node;
                goto leaf_type;
            }
            cur_dir = (IsoDir *) resolved_node;
        } else {
leaf_type:;
            if (dest_end < dest + dest_len) /* attempt to dive into file */
                return ISO_DEAD_SYMLINK;
            *res = n;
            return ISO_SUCCESS;
        }
    }
    *res = (IsoNode *) cur_dir;
    return ISO_SUCCESS;
}

