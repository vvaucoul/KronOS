/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chdir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 17:14:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 19:36:43 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/vfs/vfs.h>
#include <kernel.h>
#include <multitasking/process.h>
#include <syscall/syscall.h>

static int __is_valid_path(const char *path) {
    Vfs *vfs = vfs_get_current_fs();

    if (vfs == NULL) {
        return (-1);
    }
    return (vfs_find_node(vfs, path) != NULL ? 0 : -1);
}

static int __chdir(const char *path) {
    task_t *task = get_task(getpid());

    if (task == NULL) {
        return (-1);
    } else {

        Vfs *vfs = vfs_get_current_fs();
        VfsNode *node = vfs_find_node(vfs, path);

        if ((vfs_set_current_node(vfs, node)) != 0) {
            __WARN("Failed to set current node for VFS", -1);
        }

        char *path = vfs_get_node_path(vfs, node);
        memscpy(task->env.pwd, 64, path, strlen(path));

        printk("Change path: [%s] for node [%s]\n", task->env.pwd, vfs->nops->get_name(node));
    }
    return (0);
}

int sys_chdir(const char *path) {
    if (path == NULL) {
        return (-1);
    } else if (strlen(path) > PATH_MAX) {
        return (-1);
    } else if (__is_valid_path(path) != 0) {
        return (-1);
    }
    return (__chdir(path));
}