/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2_nops.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 10:22:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/19 10:29:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/ext2/ext2.h>
#include <multitasking/process.h>
#include <memory/memory.h>

/**
 * @brief Creates a new node.
 *
 * This function creates a new node with the specified name.
 *
 * @param root_node The root node of the file system.
 * @param node_name The name of the node to create.
 * @return The created node.
 */
VfsNode *ext2_create_node(VfsNode *root_node, const char *node_name) {
    Ext2Node *ext2_node = kmalloc(sizeof(Ext2Node));

    if (ext2_node == NULL) {
        return (NULL);
    }

    memset(ext2_node, 0, sizeof(Ext2Node));
    memcpy(ext2_node->name, node_name, strlen(node_name));

    if (get_current_task() != NULL) {
        ext2_node->owner = get_current_task()->pid;
    } else {
        ext2_node->owner = 0;
    }

    return (ext2_node);
}

/**
 * @brief Removes a node.
 *
 * This function removes the specified node.
 *
 * @param node The node to remove.
 * @return 0 on success, 1 on failure.
 */
int ext2_remove_node(VfsNode *node) {
    kfree(node);
    return (0);
}