/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 13:23:25 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 14:22:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/vfs/vfs.h>
#include <syscall/stat.h>

#warning "Stat: Replace vfs->fs_root with current directory node" 

int stat(const char *path, struct stat *buf) {
    memset(buf, 0, sizeof(struct stat));

    Vfs *vfs = vfs_get_current_fs();

    // Todo: Replace vfs->fs_root with current directory node
    VfsNode *node = vfs_finddir(vfs, vfs->fs_root, path);
    if (node == NULL) {
        return (-ENOENT);
    }
    return (vfs_get_node_stat(vfs, node, buf));
}