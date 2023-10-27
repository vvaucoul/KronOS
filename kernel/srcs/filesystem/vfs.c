/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 12:56:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/vfs.h>

int vfs_read(VfsFile *file, void *buffer, uint32_t len) {
    return file->inode->fops->read(buffer, len);
}
int vfs_write(VfsFile *file, void *buffer, uint32_t len) {
    return file->inode->fops->write(buffer, len);
}
