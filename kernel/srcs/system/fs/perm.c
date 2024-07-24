/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perm.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 23:50:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 20:06:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/fs/perm.h>

/**
 * @brief Check if the given permissions match the specified mode.
 *
 * This function compares the permissions (perm) with the specified mode.
 * It returns 1 if the permissions match the mode, otherwise it returns 0.
 *
 * @param perm The permissions to be checked.
 * @param mode The specified mode to compare with.
 * @return 1 if the permissions match the mode, 0 otherwise.
 */
int check_permissions(uint32_t perm, uint32_t mode) {
    return ((perm & mode) == mode);
}

/**
 * @brief Check the permissions of an inode.
 *
 * This function checks the permissions of the given inode against the specified mode.
 *
 * @param inode The VfsNode representing the inode to check.
 * @param mode The mode to check against.
 * @return An integer indicating the result of the permission check.
 */
int check_inode_permissions(VfsNode *inode, uint32_t mode) {
    struct stat st;
    uint32_t perm;

    if ((vfs_get_node_stat(vfs_get_current_fs(), inode, &st)) != 0) {
        return (0);
    }
    perm = st.st_mode & 0777;

    return (check_permissions(perm, mode));
}