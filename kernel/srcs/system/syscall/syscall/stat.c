/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 13:23:25 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 00:25:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/vfs/vfs.h>
#include <system/fs/stat.h>

/**
 * @brief Retrieves information about a file or directory.
 *
 * This function retrieves information about the file or directory specified by the given pathname
 * and stores it in the provided statbuf structure.
 *
 * @param pathname The path to the file or directory.
 * @param statbuf A pointer to the stat structure where the information will be stored.
 * @return 0 on success, -1 on failure.
 */
static int stat(const char *pathname, struct stat *statbuf) {
    memset(statbuf, 0, sizeof(struct stat));

    Vfs *vfs = vfs_get_current_fs();

    VfsNode *node = vfs_finddir(vfs, vfs->fs_current_node, pathname);
    if (node == NULL) {
        return (-ENOENT);
    }
    return (vfs_get_node_stat(vfs, node, statbuf));
}

/**
 * @brief Get file status information.
 *
 * This function retrieves the status of the file associated with the given file descriptor.
 *
 * @param fd The file descriptor of the file.
 * @param statbuf Pointer to a struct stat object where the file status information will be stored.
 *
 * @return 0 on success, -1 on failure.
 */
static int fstat(int fd, struct stat *statbuf) {
#warning "fstat() not implemented"
    return (0);
}

/**
 * @brief Retrieves information about a file or symbolic link, using its pathname.
 *
 * This function is similar to the `stat()` function, but if the specified pathname is a symbolic link, then information about the link itself is returned, rather than the file or directory to which the link points.
 *
 * @param pathname The pathname of the file or symbolic link.
 * @param statbuf Pointer to a `struct stat` object where the retrieved information will be stored.
 * @return 0 on success, -1 on failure.
 */
static int lstat(const char *pathname, struct stat *statbuf) {
#warning "lstat() not implemented"
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                SYSCALL FUNCTIONS                               ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief System call to retrieve information about a file.
 *
 * This function retrieves information about the file specified by the given pathname
 * and stores it in the provided statbuf structure.
 *
 * @param pathname The path to the file.
 * @param statbuf Pointer to the stat structure where the file information will be stored.
 * 
 * @return On success, 0 is returned. On error, -1 is returned and errno is set appropriately.
 */
int sys_stat(const char *pathname, struct stat *statbuf) {
    return (stat(pathname, statbuf));
}

/**
 * @brief Retrieves information about a file or directory associated with a file descriptor.
 *
 * This function retrieves information about the file or directory associated with the file descriptor `fd`
 * and stores it in the `statbuf` structure.
 *
 * @param fd The file descriptor of the file or directory.
 * @param statbuf A pointer to the `struct stat` object where the retrieved information will be stored.
 * 
 * @return On success, 0 is returned. On error, -1 is returned and `errno` is set appropriately.
 */
int sys_fstat(int fd, struct stat *statbuf) {
    return (fstat(fd, statbuf));
}

/**
 * @brief Perform a lstat system call.
 *
 * This function retrieves information about a file or symbolic link, using the given pathname.
 * The retrieved information is stored in the provided statbuf structure.
 *
 * @param pathname The path of the file or symbolic link to retrieve information about.
 * @param statbuf A pointer to the stat structure where the retrieved information will be stored.
 * @return On success, 0 is returned. On error, -1 is returned and errno is set appropriately.
 */
int sys_lstat(const char *pathname, struct stat *statbuf) {
    return (lstat(pathname, statbuf));
}