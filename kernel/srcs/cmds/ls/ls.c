/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ls.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 17:38:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 20:37:40 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/ls.h>

#include <fs/vfs/vfs.h>
#include <syscall/syscall.h>

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
int ls(__unused__ int argc, __unused__ char **argv) {

    // Todo: implement ls arguments

    Vfs *vfs = vfs_get_current_fs();

    if (vfs == NULL) {
        __THROW("ls: no filesystem mounted", 1);
    }

    // Todo: implement current directory (pwd / cd)
    task_t *task = get_task(getpid());
    char path[64] = {0};
    memscpy(path, 64, task->env.pwd, strlen(task->env.pwd));

    // if (path[strlen(path) - 1] != '/') {
    //     memjoin(path, "/", strlen(path), 1);
    // }

    Dirent *dir;
    uint32_t i = 0;

    printk("    ");
    while ((dir = vfs_readdir(vfs, vfs->fs_current_node, i)) != NULL) {
        // VfsNode *node = vfs_finddir(vfs, vfs->fs_current_node, dir->d_name);

        // Todo: Implement stat command / syscall to get file mode / type / size / permissions etc..
        // todo: implement colors for file types

        struct stat st;
        // char *path = vfs_get_node_path(vfs, node); // Todo: replace name by path (update stat function to take path as argument)
        char *name = dir->d_name;

        if ((sys_stat(name, &st)) != 0) {
            __WARND("ls: failed to get file stats");
            i++;
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            printk(_LBLUE); // Dossier
        } else if (S_ISREG(st.st_mode)) {
            if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH)) {
                printk(_GREEN); // Fichier exécutable
            } else {
                printk(_END); // Fichier régulier
            }
        } else if (S_ISLNK(st.st_mode)) {
            printk(_CYAN); // Lien symbolique
        }

        printk("%s", dir->d_name);
        // Permissions with -l arg
        // if (st.st_mode & S_IRUSR)
        //     printk("r");
        // else
        //     printk("-");
        // if (st.st_mode & S_IWUSR)
        //     printk("w");
        // else
        //     printk("-");
        // if (st.st_mode & S_IXUSR)
        //     printk("x");
        // else
        //     printk("-");
        printk(_END);
        printk(" ");
        i++;
    }
    printk("\n" _END);

    return (0);
}