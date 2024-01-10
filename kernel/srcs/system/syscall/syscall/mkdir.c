/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mkdir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:04:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 15:56:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <syscall/syscall.h>

#include <filesystem/vfs.h>

int sys_mkdir(char *path, mode_t permission) {
    Ext2Inode *inode = ext2_finddir_fs(fs_root, path);

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

}