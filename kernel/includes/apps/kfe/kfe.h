/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kfe.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 12:49:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 10:47:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KFE_H
#define KFE_H

#include <fs/vfs/vfs.h>
#include <kernel.h>

/**
 * @file kfe.h
 * @brief Header file for KFE (Kronos File Explorer)
 *
 * The KFE (Kronos File Explorer) is a file explorer kernel-side for the filesystem in the terminal.
 * It provides functionalities to navigate, manipulate, and manage files and directories in the terminal environment.
 * It is a simple and lightweight file explorer, with a simple and intuitive interface.
 *
 * The KFE is designed to be used in the terminal environment, and is not intended to be used in a graphical environment.
 */

/*
 * Interface Example:
 *
 * +------------------------------------------------------------------------------+
 * |                             Kronos File Explorer                             |
 * +------------------------------------------------------------------------------+
 * | Inode | Type      | Name                              | Size       | Blocks  |
 * +------------------------------------------------------------------------------+
 * | 1     | Directory | home                              | -          | 2       |
 * | 2     | File      | home/readme.txt                   | 1024 bytes | 1       |
 * | 3     | Directory | bin                               | -          | 3       |
 * | 4     | File      | bin/ls                            | 2048 bytes | 2       |
 * | 5     | File      | bin/cat                           | 1024 bytes | 1       |
 * | 6     | Directory | etc                               | -          | 1       |
 * | 7     | File      | etc/config.ini                    | 512 bytes  | 1       |
 * | 8     | Directory | usr                               | -          | 2       |
 * | 9     | Directory | usr/share                         | -          | 1       |
 * | 10    | File      | usr/share/logo.txt                | 256 bytes  | 1       |
 * +------------------------------------------------------------------------------+
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |                                                                              |
 * |------------------------------------------------------------------------------|
 * | Commands: ls, cd <dir>, cat <file>, rm <file/dir>, mkdir <dir>, touch <file> |
 * +------------------------------------------------------------------------------+
 */

#define KFE_VERSION "0.1.0"
#define KFE_NAME "Kronos File Explorer"

typedef struct {
    char name[32];
    char version[8];
    uint32_t selected_inode;

    Vfs *vfs;
    VfsNode *current_node;
} __packed__ Kfe;

extern int kfe(__unused__ int argc, __unused__ char **argv);

#endif /* !KFE_H */