/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 10:19:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 20:41:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <syscall/syscall.h>
#include <system/fs/cache.h>
#include <system/fs/file.h>

int sys_close(int fd) {
    File *file = get_file_descriptor(fd);
    if (file == NULL) {
        return -EBADF;
    }

    Vfs *vfs = vfs_get_current_fs();

    // Todo: Release file cache
    // Release file cache
    // release_file_cache(file);

    // Setup File cache (retreive faster file)
    file_cache_t *cache = get_file_cache(vfs->fs_info->name);

    if (cache) {
        if (cache_has_file_by_fd(cache, fd) == 0) {
            return -EBADF;
        }
        
        free_file_structure(file);
        return (cache_close_file(cache, fd));
    }

    // Todo: Unlock file
    // Unlock file
    // if (file->flags & O_LOCK) {
    //     unlock_file(file->inode);
    // }

    free_file_structure(file);
    release_file_descriptor(fd);

    return 0;
}