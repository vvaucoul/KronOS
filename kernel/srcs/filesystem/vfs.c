/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 21:16:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/vfs.h>

#include <memory/memory.h>

Vfs *vfs = NULL;

int vfs_init(void) {
    if ((vfs = kmalloc(sizeof(Vfs))) == NULL) {
        __THROW("vfs_init: malloc failed", 1);
    }
    vfs->fs_name = FILESYSTEM_NAME;
    vfs->fs_root = fs_root;
    return (0);
}

void vfs_delete_file(VfsInode *inode, char *name) {
    if (inode->fops.unlink != 0) {
        inode->fops.unlink(inode, name);
    } else {
        __WARND("Cannot delete file");
    }
}

// int vfs_read(VfsFile *file, void *buffer, uint32_t len) {
//     return file->inode->fops->read(file->inode, 0, len, buffer);
// }
// int vfs_write(VfsFile *file, void *buffer, uint32_t len) {
//     return file->inode->fops->write(file->inode, 0, len, buffer);
// }
