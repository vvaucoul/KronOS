/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs_node.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 11:51:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 13:35:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/vfs/vfs.h>
#include <memory/memory.h>

/**
 * @brief Get the name of a VFS node
 *
 * @param vfs The VFS
 * @param node The VFS node
 * @return char* The name of the VFS node
 */
char *vfs_get_node_path(Vfs *vfs, VfsNode *node) {
    char *path = NULL;

    while ((node = vfs->nops->get_parent(node)) != NULL) {
        char *tmp = path;
        char *name = vfs->nops->get_name(node);

        path = kmalloc(strlen(name) + 1 + (path ? strlen(path) : 0) + 1);
        if (tmp) {
            strcpy(path, name);
            strcat(path, "/");
            strcat(path, tmp);
            kfree(tmp);
        } else {
            strcpy(path, name);
        }
    }
    return (path);
}