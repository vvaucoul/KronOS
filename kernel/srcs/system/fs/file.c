/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 20:14:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:06:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/fs/file.h>
#include <mm/mm.h>

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
 * @brief Allocates a file structure for a given VfsNode with specified flags.
 *
 * This function is responsible for allocating a file structure for a given VfsNode
 * with the specified flags. The file structure is used to represent a file in the
 * file system.
 *
 * @param node The VfsNode for which the file structure is being allocated.
 * @param flags The flags to be set for the file structure.
 * @return A pointer to the allocated File structure.
 */
File *allocate_file_structure(VfsNode *node, int flags) {
    File *file = (File *)kmalloc(sizeof(File));
    if (file == NULL) {
        return NULL;
    }

    file->fd = 0;
    file->pos = 0;
    file->mode = flags;
    file->inode = node;
    file->f_ops = get_file_operations();

    return file;
}

/**
 * @brief Frees the memory allocated for a file structure.
 *
 * This function frees the memory allocated for a file structure.
 *
 * @param file A pointer to the file structure to be freed.
 */
void free_file_structure(File *file) {
    kfree(file);
}

// Fonction pour verrouiller un fichier
void lock_file(VfsNode *inode) {
    // Implémentation du verrouillage, par exemple en utilisant un mutex
    // pthread_mutex_lock(&inode->lock);

    #warning "lock_file() not implemented"
}

// Fonction pour déverrouiller un fichier
void unlock_file(VfsNode *inode) {
    // Implémentation du déverrouillage
    // pthread_mutex_unlock(&inode->lock);

    #warning "lock_file() not implemented"
}