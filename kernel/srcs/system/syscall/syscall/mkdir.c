/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mkdir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:04:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 09:53:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/vfs/vfs.h>
#include <memory/memory.h>
#include <syscall/syscall.h>

#include <macros.h>

static VfsNode *find_or_create_dir(Vfs *vfs, VfsNode *parent, const char *name, uint16_t permission) {
    VfsNode *node = vfs_finddir(vfs, parent, name);
    if (node == NULL) {
        if (vfs->fops->mkdir(parent, name, permission) != 0) {
            return NULL;
        }
        node = vfs_finddir(vfs, parent, name);
    }
    return node;
}

/**
 * @brief Create a new directory.
 *
 * This function creates a new directory with the specified pathname and mode.
 *
 * @param pathname The path of the directory to be created.
 * @param mode The permissions to be set for the new directory.
 *
 * @return On success, 0 is returned. On error, -1 is returned and errno is set appropriately.
 */
int sys_mkdir(const char *pathname, mode_t mode) {
    Vfs *vfs = vfs_get_current_fs();
    if (vfs == NULL) {
        __WARN("No filesystem mounted", 1);
        return -1;
    }

    // Duplicate the pathname to avoid modifying the original string
    char *path_copy = strdup(pathname);
    if (path_copy == NULL) {
        __WARN("Memory allocation failed", 1);
        return -1;
    }

    VfsNode *current_node = (pathname[0] == '/') ? vfs->fs_root : vfs->fs_current_node;
    char *token = strtok(path_copy, "/");
    while (token != NULL) {
        current_node = find_or_create_dir(vfs, current_node, token, mode);
        if (current_node == NULL) {
            kfree(path_copy);
            __WARN("Failed to create directory", 1);
            return -1;
        }
        token = strtok(NULL, "/");
    }

    kfree(path_copy);
    return 0;
}

int sys_mkdirat(__unused__ int dirfd, __unused__ const char *pathname, __unused__ mode_t mode) {
    #warning "sys_mkdirat not implemented yet"
    
    return (0);
}