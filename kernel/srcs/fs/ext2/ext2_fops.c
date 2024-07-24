/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2_fops.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 11:17:41 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 10:47:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/ext2/ext2.h>

// ! ||--------------------------------------------------------------------------------||
// ! ||                              EXT2 FILES OPERATIONS                             ||
// ! ||--------------------------------------------------------------------------------||

/**
 * File operations
 */

uint32_t ext2_read(Ext2Node *node, uint32_t offset, uint32_t size, uint8_t *buffer) {

}

uint32_t ext2_write_full(Ext2Node *node, uint32_t size, uint8_t *buffer) {

}

uint32_t ext2_write(Ext2Node *node, uint32_t offset, uint32_t size, uint8_t *buffer) {

}

uint32_t ext2_unlink(Ext2Node *inode, char *name) {

}

/**
 * Directory operations
 */

void ext2_open(Ext2Node *node) {

}

void ext2_close(Ext2Node *node) {

}

struct dirent *ext2_readdir(Ext2Node *node, uint32_t index) {

}

Ext2Node *ext2_finddir(Ext2Node *node, char *name) {

}

uint32_t ext2_mkdir(Ext2Node *inode, char *name, mode_t permission) {

}

uint32_t ext2_rmdir(Ext2Node *inode, char *name) {

}

/**
 * Other operations
 */

uint32_t ext2_move(Ext2Node *inode, char *name, char *new_name) {

}

uint32_t ext2_chmod(Ext2Node *inode, mode_t mode) {
}

uint32_t ext2_chown(Ext2Node *inode, uid_t uid, gid_t gid) {

}
