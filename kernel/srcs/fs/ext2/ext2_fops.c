/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2_fops.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 11:17:41 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 09:03:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/ext2/ext2.h>
#include <macros.h>

// ! ||--------------------------------------------------------------------------------||
// ! ||                              EXT2 FILES OPERATIONS                             ||
// ! ||--------------------------------------------------------------------------------||

/**
 * File operations
 */

uint32_t ext2_read(__unused__ Ext2Node *node, __unused__ uint32_t offset, __unused__ uint32_t size, __unused__ uint8_t *buffer) {
    return (0);
}

uint32_t ext2_write_full(__unused__ Ext2Node *node, __unused__ uint32_t size, __unused__ uint8_t *buffer) {
    return (0);
}

uint32_t ext2_write(__unused__ Ext2Node *node, __unused__ uint32_t offset, __unused__ uint32_t size, __unused__ uint8_t *buffer) {
    return (0);
}

uint32_t ext2_unlink(__unused__ Ext2Node *inode, __unused__ char *name) {
    return (0);
}

/**
 * Directory operations
 */

void ext2_open(__unused__ Ext2Node *node) {
}

void ext2_close(__unused__ Ext2Node *node) {
}

struct dirent *ext2_readdir(__unused__ Ext2Node *node, __unused__ uint32_t index) {
    return (NULL);
}

Ext2Node *ext2_finddir(__unused__ Ext2Node *node, __unused__ char *name) {
    return (NULL);
}

uint32_t ext2_mkdir(__unused__ Ext2Node *inode, __unused__ char *name, __unused__ mode_t permission) {
    return (0);
}

uint32_t ext2_rmdir(__unused__ Ext2Node *inode, __unused__ char *name) {
    return (0);
}

/**
 * Other operations
 */

uint32_t ext2_move(__unused__ Ext2Node *inode, __unused__ char *name, __unused__ char *new_name) {
    return (0);
}

uint32_t ext2_chmod(__unused__ Ext2Node *inode, __unused__ mode_t mode) {
    return (0);
}

uint32_t ext2_chown(__unused__ Ext2Node *inode, __unused__ uid_t uid, __unused__ gid_t gid) {
    return (0);
}
