/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cat.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 10:48:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:06:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/tinyfs/tinyfs.h>
#include <fs/vfs/vfs.h>
#include <mm/mm.h>
#include <multitasking/process.h>

#include <system/fs/open.h>

#include <cmds/cat.h>
#include <string.h>

int cat(int argc, char **argv) {
    if (argc == 0) {
        __WARND("cat: no file specified");
        return (1);
    } else {
        Vfs *vfs = vfs_get_current_fs();

        if (vfs == NULL) {
            __THROW("cat: no filesystem mounted", 1);
        } else if (argc > 2) {
            __THROW("cat: too many arguments", 1);
        } else if (argc < 2) {
            __THROW("cat: too few arguments", 1);
        } else if (!argv) {
            __THROW("cat: no file specified", 1);
        }

        // task_t *task = get_task(getpid());
        char path[64] = {0};
        memscpy(path, 64, argv[1], strlen(argv[1]));
        // memscpy(path, 64, task->env.pwd, strlen(task->env.pwd));
        // memjoin(path, argv[1], strlen(path), strlen(argv[1]));

        printk("cat: %s\n", path);
        VfsNode *node = vfs_find_node(vfs, path);

        // Todo: must work with vfs (inode->block_pointers[0] and inode->size)
        TinyFS_Inode *inode = (TinyFS_Inode *)(node);

        if ((vfs_open(vfs, node, O_RDONLY)) != 0) {
            __THROW("cat: file not found", 1);
        } else if (node == NULL) {
            __THROW("cat: node not found", 1);
        } else {
            char buffer[1024] = {0};
            uint32_t offset = inode->block_pointers[0];
            uint32_t size = inode->size;

            printk("cat: node: %s, offset: %d, size: %d\n", inode->name, offset, size);
            if ((vfs_read(vfs, node, offset, size, (uint8_t *)buffer)) == 0) {
                buffer[size] = '\0';
                printk("%s", buffer);
                printk("\n");
            } else {
                __WARN("Failed to read fs", 1);
            }

            vfs_close(vfs, node);
        }
    }

    return (0);
}