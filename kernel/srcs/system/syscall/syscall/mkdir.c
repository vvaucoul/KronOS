/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mkdir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:04:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/18 22:11:18 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <syscall/syscall.h>

#include <filesystem/vfs/vfs.h>

int sys_mkdir(char *path, mode_t __unused__ permission) {
    Ext2Node *node __unused__ = ext2_finddir(fs_root, path);

    /*
    - Check if path already exist
        - If yes, return error
    - Check if parent directory exist
        - If yes
            - Check if path is a directory
                - If yes, return error
                - Else, create directory
        - If no, return error
    */


    #warning "Todo: Implement VFS"
    #warning "Todo: Implement PWD"

    return (0);
}