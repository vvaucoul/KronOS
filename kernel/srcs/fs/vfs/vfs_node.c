/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs_node.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 11:51:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:06:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/vfs/vfs.h>
#include <mm/mm.h>
#include <multitasking/process_env.h>

/**
 * Retrieves the path of a VfsNode within a Vfs.
 *
 * @param vfs The Vfs instance.
 * @param node The VfsNode for which to retrieve the path.
 * @return The path of the VfsNode.
 */
char *vfs_get_node_path(Vfs *vfs, VfsNode *node) {
    char *path = NULL;
    VfsNode *tmp = node;

    while (tmp) {
        char *name = vfs->nops->get_name(tmp);

        if (path == NULL) {
            path = kmalloc(strlen(name) + 1);
            strcpy(path, name);
        } else if (strcmp(name, "/") != 0) {
            uint32_t new_path_length = strlen(name) + strlen(path) + 2;
            char *newPath = kmalloc(new_path_length);
            strcpy(newPath, name);
            strcat(newPath, "/");
            strcat(newPath, path);
            kfree(path);
            path = newPath;
        }
        tmp = vfs->nops->get_parent(tmp);
    }

    if (path == NULL) {
        path = kmalloc(2);
        strcpy(path, "/");
    } else {
        char *finalPath = kmalloc(strlen(path) + 2);
        finalPath[0] = '/';
        strcpy(finalPath + 1, path);
        kfree(path);
        path = finalPath;
    }

    return path;
}

/**
 * @brief Find a VfsNode in the specified Vfs using the given path.
 *
 * This function searches for a VfsNode in the specified Vfs using the provided path.
 *
 * @param vfs The Vfs structure to search in.
 * @param path The path of the VfsNode to find.
 * @return A pointer to the found VfsNode, or NULL if not found.
 */
VfsNode *vfs_find_node(Vfs *vfs, const char *path) {
    if (vfs == NULL || path == NULL || *path == '\0') {
        return NULL;
    }

    VfsNode *node = (path[0] == '/') ? vfs->fs_root : vfs->fs_current_node;
    if (strcmp(path, "/") == 0) {
        return node;
    }

    char *path_copy = kmalloc(strlen(path) + 1);
    if (path_copy == NULL) {
        return NULL;
    }
    strcpy(path_copy, path);

    char *token = strtok(path_copy, "/");
    while (token != NULL) {
        Dirent *dir;
        uint32_t i = 0;
        int found = 0;

        while ((dir = vfs_readdir(vfs, node, i)) != NULL) {
            if (strcmp(dir->d_name, token) == 0) {
                // Si le nom du dossier correspond au segment actuel du chemin
                node = vfs_finddir(vfs, node, token);
                if (node == NULL) {
                    kfree(path_copy);
                    return NULL;
                }
                found = 1;
                break;
            }
            i++;
        }

        if (!found) {
            kfree(path_copy);
            return NULL;
        }

        token = strtok(NULL, "/");
    }

    kfree(path_copy);
    return node;
}

/**
 * Sets the current node in the virtual file system.
 *
 * @param vfs The virtual file system.
 * @param node The node to set as the current node.
 * @return Returns an integer indicating the success or failure of the operation.
 */
int vfs_set_current_node(Vfs *vfs, VfsNode *node) {
    if (vfs == NULL || node == NULL) {
        return (-1);
    } else {
        vfs->fs_current_node = node;
    }
    return (0);
}

// TODO: Must be deleted and use sys_stat instead

/**
 * Retrieves the file system node statistics for a given VfsNode.
 *
 * @param vfs The Vfs structure representing the file system.
 * @param node The VfsNode for which to retrieve the statistics.
 * @param buf A pointer to the struct stat where the statistics will be stored.
 * @return Returns 0 on success, or a negative error code on failure.
 */
int vfs_get_node_stat(Vfs *vfs, VfsNode *node, struct stat *buf) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->stat == NULL) {
        return (-1);
    }
    return (vfs->fops->stat(node, buf));
}