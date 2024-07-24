/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 10:48:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 10:47:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/vfs/vfs.h>

int cd(int argc, char **argv) {
    if (argc > 2) {
        __WARND("cd: too many arguments");
        return (1);
    }
    Vfs *vfs = vfs_get_current_fs();
    if (vfs == NULL) {
        __THROW("cd: no filesystem mounted", 1);
    }

    return (vfs_chdir(vfs, argv[1]));
}