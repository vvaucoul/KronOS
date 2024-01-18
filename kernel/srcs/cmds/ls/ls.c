/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ls.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 17:38:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/18 22:11:18 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/ls.h>

#include <filesystem/vfs/vfs.h>

/**
 * @brief List files in directory
 *
 * @param argc
 * @param argv
 * @return int
 *
 * @note
 * - ls : list files in current directory
 * basic implementation of ls system command
 */
int ls(int argc, char **argv) {

    // Todo: implement ls arguments

    Vfs *vfs = vfs_get_current_fs();

    if (vfs == NULL) {
        __THROW("ls: no filesystem mounted", 1);
    }

    VfsNode *node = vfs_finddir(vfs, vfs->fs_root, ".");

    if (node == NULL) {
        __THROW("ls: no directory found", 1);
    }

    Dirent *dir;
    uint32_t i = 0;

    while ((dir = vfs_readdir(vfs, node, i)) != NULL) {
        printk("%s\n", dir->name);
    }

    return (0);
}