/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs_node.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 11:51:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/13 18:18:32 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/vfs/vfs.h>
#include <memory/memory.h>
#include <multitasking/process_env.h>

/**
 * @brief Get the name of a VFS node
 *
 * @param vfs The VFS
 * @param node The VFS node
 * @return char* The name of the VFS node
 */
char *vfs_get_node_path(Vfs *vfs, VfsNode *node) {
    char *path = NULL;
    VfsNode *tmp = node;

    while (tmp) {
        char *name = vfs->nops->get_name(tmp); // Get the name of the current node.

        if (path == NULL) {
            // First node: simply start with its name.
            path = kmalloc(strlen(name) + 1);
            strcpy(path, name);
        } else if (strcmp(name, "/") != 0) { // If the name is not the root.
            // Build the new path with a slash if necessary.
            size_t newPathLength = strlen(name) + strlen(path) + 2; // +2 for the slash and '\0'.
            char *newPath = kmalloc(newPathLength);
            strcpy(newPath, name); // Copy the name of the current node.
            strcat(newPath, "/"); // Add a slash.
            strcat(newPath, path); // Add the previous path.
            kfree(path); // Free the old path.
            path = newPath; // Update the path pointer.
        }
        // kfree(name); // Free the name if necessary (depending on the get_name implementation).
        tmp = vfs->nops->get_parent(tmp); // Move to the parent of the node.
    }

    // If path is still NULL, we are at the root or the node was NULL.
    if (path == NULL) {
        path = kmalloc(2); // For "/" and '\0'.
        strcpy(path, "/");
    } else {
        // Make sure the path starts with "/".
        char *finalPath = kmalloc(strlen(path) + 2); // +2 for potential initial "/" and '\0'.
        finalPath[0] = '/';
        strcpy(finalPath + 1, path); // Concatenate "/" at the beginning if it's not already there.
        kfree(path);
        path = finalPath;
    }

    return path;
}
/**
 * @brief Find a node in the VFS by its path
 *
 * @param vfs
 * @param path
 * @return VfsNode*
 */
extern VfsNode *vfs_find_node(Vfs *vfs, const char *path) {
    if (vfs == NULL || path == NULL) {
        return NULL;
    }

    VfsNode *node = (path[0] == '/') ? vfs->fs_root : vfs->fs_current_node;
    if (strcmp(path, "/") == 0) {
        return node;
    }

    char *path_copy = kmalloc(strlen(path) + 1);
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
                printk("Node: %s\n", vfs->nops->get_name(node));
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
 * @brief Set the current node in the VFS (set fs_current_node to node)
 *
 * @param vfs
 * @param node
 * @return int
 */
extern int vfs_set_current_node(Vfs *vfs, VfsNode *node) {
    if (vfs == NULL || node == NULL) {
        return (-1);
    } else {
        vfs->fs_current_node = node;
    }
    return (0);
}

/**
 * @brief Get stat of a VFS node (Use sys_stat syscall to get file stats)
 *
 * @param vfs
 * @param node
 * @param buf
 * @return int
 */
int vfs_get_node_stat(Vfs *vfs, VfsNode *node, struct stat *buf) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->stat == NULL) {
        return (-1);
    }
    return (vfs->fops->stat(node, buf));
}
