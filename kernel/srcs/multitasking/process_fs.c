/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_fs.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 17:28:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 12:26:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/vfs/vfs.h>
#include <multitasking/process_fs.h>

int process_fs_init(task_t *task) {
    Vfs *fs = vfs_get_current_fs();
    char *root_name = fs->nops->get_name(fs->fs_root);
    
    memscpy(task->env.pwd, 64, root_name, strlen(root_name));
    return (0);
}