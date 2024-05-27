/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ls.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 17:38:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/13 15:41:17 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/ls.h>

#include <filesystem/vfs/vfs.h>
#include <multitasking/process.h>

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

    // Todo: implement current directory (pwd / cd)
    task_t *task = get_task(getpid());
    char path[64] = {0};
    memscpy(path, 64, task->env.pwd, strlen(task->env.pwd));
    VfsNode *node = vfs_finddir(vfs, vfs->fs_root, path);

    if (node == NULL) {
        __THROW("ls: no directory found", 1);
    }

    Dirent *dir;
    uint32_t i = 0;

    printk("    ");
    while ((dir = vfs_readdir(vfs, node, i)) != NULL) {
        VfsNode *node = vfs_finddir(vfs, vfs->fs_root, dir->d_name);

        // Todo: Implement stat command / syscall to get file mode / type / size / permissions etc..
        // todo: implement colors for file types

        struct stat st;
        // char *path = vfs_get_node_path(vfs, node); // Todo: replace name by path (update stat function to take path as argument)
        char *name = dir->d_name;

        if ((stat(name, &st)) != 0) {
            __WARND("ls: failed to get file stats");
            i++;
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            printk(_BLUE);
        } else if (S_ISREG(st.st_mode)) {
            printk(_WHITE);
        } else {
            printk(_YELLOW);
        }
        printk("%s ", dir->d_name);
        printk(_END);
        i++;
    }
    printk("\n" _END);

    return (0);
}