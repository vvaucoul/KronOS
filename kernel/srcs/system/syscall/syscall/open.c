/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 23:36:08 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 20:42:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <syscall/syscall.h>

#include <fs/vfs/vfs.h>

#include <system/fs/cache.h>
#include <system/fs/file.h>
#include <system/fs/open.h>
#include <system/fs/path.h>
#include <system/fs/perm.h>

#include <multitasking/process.h>

int sys_open(const char *path, int flags, mode_t mode) {
    if (path == NULL || strlen(path) == 0) {
        return (-EINVAL);
    } else if (strlen(path) > PATH_MAX) {
        return (-ENAMETOOLONG);
    }

    path_result_t path_result = path_split(path);

    Vfs *vfs = vfs_get_current_fs();
    VfsNode *current_node = vfs->fs_current_node;
    VfsNode *node = vfs_find_node(vfs, path);

    if (node == NULL) {
        if (flags & O_CREAT) {
            // Todo: add mode for vfs_create_node (mode_t / permissions)
            node = vfs_create_node(vfs, current_node, path_result.file);
            if (node == NULL) {
                return -ENOENT;
            }
        } else {
            return -ENOENT;
        }
    }

    task_t *proc = get_current_task();
    if (proc == NULL) {
        return -EACCES;
    }

    // uint32_t perm = proc->task_id.uid;
    uint32_t perm = O_RDWR; // tmp: set to read/write

    // Check permissions
    if (check_permissions(perm, mode) == 0) {
        return -EACCES;
    }

    // Setup File cache (retreive faster file)
    file_cache_t *cache = get_file_cache(vfs->fs_info->name);

    if (cache) {
        File *cache_file = cache_open_file(cache, path, mode);

        if (cache_file) {
            return (cache_file->fd);
        }
    }

    // Allocate file descriptor
    int fd = allocate_file_descriptor(path, mode);

    if (fd < 0) {
        return -EMFILE;
    }

    // Allocate file structure
    File *file = allocate_file_structure(node, flags);

    if (file == NULL) {
        release_file_descriptor(fd);
        return -ENOMEM;
    }

    // Todo: Lock file
    // Lock file
    // if (flags & O_LOCK) {
    // lock_file(inode);
    // }

    return fd;
}