/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 09:40:25 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 00:34:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_H
#define FILE_H

#include <fs/vfs/vfs.h>
#include <system/fs/file.h>

typedef long off_t;

typedef struct file {
    int fd;            // File descriptor
    off_t pos;         // Current position in the file
    int mode;          // Opening mode (read, write, etc.)
    VfsNode *inode;    // Pointer to the corresponding inode
    VfsFileOps *f_ops; // Pointer to file operations
} File;

extern File *allocate_file_structure(VfsNode *node, int flags);
extern void free_file_structure(File *file);
extern File *get_file_descriptor(int fd);

#endif /* !FILE_H */