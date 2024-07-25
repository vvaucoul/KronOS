/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/25 00:28:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 10:05:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <syscall/syscall.h>
#include <system/fs/file.h>

int sys_write(int fd, const void *buf, uint32_t count) {
    if (fd < 0 || fd >= MAX_FDS) {
        return (-EBADF);
    } else if (buf == NULL) {
        return (-EFAULT);
    } else if (count == 0) {
        return (0);
    }

    // Get file descriptor
    File *file = get_file_descriptor(fd);

    if (file == NULL) {
        return (-EBADF);
    }

    // Write to file
    if (file->f_ops->write == NULL) {
        return (-EACCES);
    } else {

        return (file->f_ops->write(file->inode, file->pos, count, buf));

        // Todo: reformat write function fs like the following line
        // return (file->f_ops->write(file, buf, count));
    }
}