/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 23:54:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 00:34:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/vfs/vfs.h>
#include <memory/memory.h>
#include <multitasking/process.h>
#include <system/fs/fd.h>
#include <system/fs/file.h>

/**
 * Retrieves the file operations associated with a given VfsNode.
 *
 * @param node The VfsNode for which to retrieve the file operations.
 * @return A pointer to the VfsFileOps structure representing the file operations.
 */
static VfsFileOps *get_file_operations(void) {
    Vfs *vfs = vfs_get_current_fs();

    if (vfs == NULL) {
        return NULL;
    }

    return (vfs->fops);
}

/**
 * @brief Finds a free file descriptor in the given file descriptor table.
 *
 * This function searches for a free file descriptor in the specified file descriptor table.
 *
 * @param fd_table The file descriptor table to search in.
 * @return The index of the free file descriptor, or -1 if no free file descriptor is found.
 */
static int find_free_fd(fd_table_t *fd_table) {
    for (int i = 0; i < MAX_FDS; ++i) {
        if (fd_table->files[i] == NULL) {
            return i;
        }
    }

    return -1;
}

/**
 * Initializes a file descriptor table.
 *
 * @return A pointer to the initialized file descriptor table.
 */
fd_table_t *fd_table_init(void) {
    fd_table_t *fd_table = (fd_table_t *)kmalloc(sizeof(fd_table_t));

    if (fd_table == NULL) {
        return NULL; // Failed to allocate memory for fd_table
    }

    for (int i = 0; i < MAX_FDS; ++i) {
        fd_table->files[i] = NULL;
    }

    return fd_table;
}

/**
 * @brief Allocates a file descriptor for the specified file path and mode.
 *
 * @param path The path of the file.
 * @param mode The mode of the file descriptor.
 * @return The allocated file descriptor.
 */
int allocate_file_descriptor(const char *path, int mode) {
    File *file = (File *)kmalloc(sizeof(File));
    task_t *proc = get_current_task();
    Vfs *vfs = vfs_get_current_fs();

    if (file == NULL) {
        return -1; // Failed to allocate memory for file
    } else if (proc == NULL) {
        kfree(file);
        return -1; // Failed to get current task
    } else if (vfs == NULL) {
        kfree(file);
        return -1; // No filesystem mounted
    }

    // File object initialization (fd, inode, pos, mode, f_ops)
    file->fd = find_free_fd(proc->fd_table); // Find a free FD
    if (file->fd == -1) {
        kfree(file);
        return -1; // No free FD
    }
    file->inode = vfs_find_node(vfs, path); // Get the inode of the path
    file->pos = 0;
    file->mode = mode;
    file->f_ops = get_file_operations(); // Get file operations

    if (file->f_ops && file->f_ops->open) {
        if (file->f_ops->open(file->inode, mode) != 0) {
            kfree(file);
            return -1;
        }
    }

    proc->fd_table->files[file->fd] = file; // Add the file to the FD table
    return file->fd;
}

/**
 * @brief Releases a file descriptor.
 *
 * This function releases the specified file descriptor, making it available for reuse.
 *
 * @param fd The file descriptor to be released.
 * @return Returns 0 on success, or a negative value on failure.
 */
int release_file_descriptor(int fd) {
    task_t *proc = get_current_task();

    if (proc == NULL) {
        return -1;
    }

    if (fd < 0 || fd >= MAX_FDS || proc->fd_table->files[fd] == NULL) {
        return -1;
    }
    File *file = proc->fd_table->files[fd];

    if (file == NULL) {
        return -1; // Invalid file descriptor
    }

    if (file->f_ops && file->f_ops->close) {
        if (file->f_ops->close(file->inode) != 0) {
            return -1;
        }
    }

    kfree(file);
    proc->fd_table->files[fd] = NULL;
    return 0;
}

/**
 * Retrieves the file descriptor associated with the given file descriptor number.
 *
 * @param fd The file descriptor number to retrieve.
 * @return A pointer to the File structure representing the file descriptor, or NULL if not found.
 */
File *get_file_descriptor(int fd) {
    task_t *proc = get_current_task();

    if (proc == NULL) {
        return NULL;
    }

    if (fd < 0 || fd >= MAX_FDS || proc->fd_table->files[fd] == NULL) {
        return NULL;
    }

    return proc->fd_table->files[fd];
}