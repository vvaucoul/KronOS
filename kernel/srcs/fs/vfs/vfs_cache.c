/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs_cache.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 14:15:15 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:06:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fs/vfs/vfs.h>

#include <mm/mm.h>

/**
 * @brief Creates a Vfs cache.
 * 
 * This function creates a Vfs cache for the specified Vfs, using the specified cache function.
 * 
 * @param vfs The Vfs for which to create the cache.
 * @param cfn The cache function to use.
 * @return The created Vfs cache.
 */
VfsCache *vfs_create_cache(Vfs *vfs, VfsCacheFn cfn) {
    VfsCache *vfsc = kmalloc(sizeof(VfsCache));

    if (vfsc == NULL) {
        __THROW("VFS: Failed to create VFS cache", NULL);
    } else {
        memset(vfsc, 0, sizeof(VfsCache));
        vfsc->root_node = NULL;
        vfsc->nodes_count = 0;

        vfsc->cfn.vfs_set_cache_links = cfn.vfs_set_cache_links;
        vfsc->cfn.vfs_get_cache_links = cfn.vfs_get_cache_links;

        vfs->vfs_cache = vfsc;
        vfs->use_vfs_cache = 1;
    }
    return (vfsc);
}

/**
 * @brief Destroys the cache associated with the given VfsCache object.
 *
 * @param vfsc The VfsCache object whose cache needs to be destroyed.
 * @return void
 */
int vfs_destroy_cache(VfsCache *vfsc) {
    if (vfsc->root_node != NULL) {
        vfs_destroy_cache_link(vfsc, vfsc->root_node);
    }
    kfree(vfsc);
    return (0);
}

/**
 * @brief Creates a Vfs cache link.
 *
 * This function creates a Vfs cache link for the specified Vfs cache and Vfs node.
 *
 * @param vfsc The Vfs cache for which to create the link.
 * @param node The Vfs node for which to create the link.
 * @return The created Vfs cache link.
 */
VfsCacheLinks *vfs_create_cache_link(VfsCache *vfsc, VfsNode *node) {
    if (node == NULL) {
        __THROW("VFS: Failed to create VFS cache node", NULL);
    } else {
        VfsCacheLinks *cache_node = kmalloc(sizeof(VfsCacheLinks));

        if (cache_node == NULL) {
            __THROW("VFS: Failed to create VFS cache node", NULL);
        } else {
            memset(cache_node, 0, sizeof(VfsCacheLinks));

            cache_node->node = node;
            cache_node->cache = vfsc;

            cache_node->parent = NULL;
            cache_node->childrens = NULL;

            cache_node->childrens_count = 0;

            if ((vfsc->cfn.vfs_set_cache_links(node, cache_node)) != 0) {
                kfree(cache_node);
                return (NULL);
            }
            return (cache_node);
        }
    }
}

/**
 * @brief Destroys the cache link associated with the given VfsCache object and Vfs node.
 *
 * @param vfsc The VfsCache object whose cache link needs to be destroyed.
 * @param node The Vfs node whose cache link needs to be destroyed.
 * @return void
 */
int vfs_destroy_cache_link(VfsCache *vfsc, VfsNode *node) {
    if (vfsc->cfn.vfs_get_cache_links(node)->childrens != NULL) {
        for (uint32_t i = 0; i < vfsc->cfn.vfs_get_cache_links(node)->childrens_count; i++) {
            vfs_destroy_cache_link(vfsc, vfsc->cfn.vfs_get_cache_links(node)->childrens[i]);
        }
        kfree(vfsc->cfn.vfs_get_cache_links(node)->childrens);
    }
    return (0);
}

/**
 * @brief Adds a node to the Vfs cache.
 *
 * This function adds a node to the Vfs cache.
 *
 * @param vfsc The Vfs cache to which to add the node.
 * @param root_node The root node of the Vfs cache.
 * @param node The node to add to the Vfs cache.
 * @return The added node.
 */
VfsNode *vfs_add_node(VfsCache *vfsc, VfsNode *root_node, VfsCacheLinks *node) {
    if (root_node == NULL) {
        vfsc->root_node = node;
        node->parent = NULL;
        vfsc->nodes_count = 1;
    } else {

        if (vfsc->cfn.vfs_get_cache_links(root_node)->childrens == NULL) {
            vfsc->cfn.vfs_get_cache_links(root_node)->childrens = kmalloc(sizeof(VfsNode *) * VFS_CACHE_ALLOC_OFFSET);
            if (vfsc->cfn.vfs_get_cache_links(root_node)->childrens == NULL) {
                __THROW("VFS: Failed to add node to VFS cache", NULL);
            }
            vfsc->cfn.vfs_get_cache_links(root_node)->childrens[0] = node;
            node->parent = root_node;
            vfsc->cfn.vfs_get_cache_links(root_node)->childrens_count = 1;
        } else {
            if (vfsc->cfn.vfs_get_cache_links(root_node)->childrens_count % VFS_CACHE_ALLOC_OFFSET == 0) {
                vfsc->cfn.vfs_get_cache_links(root_node)->childrens = krealloc(vfsc->cfn.vfs_get_cache_links(root_node)->childrens, sizeof(VfsNode *) * (vfsc->cfn.vfs_get_cache_links(root_node)->childrens_count + VFS_CACHE_ALLOC_OFFSET + 1));
                if (vfsc->cfn.vfs_get_cache_links(root_node)->childrens == NULL) {
                    __THROW("VFS: Failed to add node to VFS cache", NULL);
                }
            }
            vfsc->cfn.vfs_get_cache_links(root_node)->childrens[vfsc->cfn.vfs_get_cache_links(root_node)->childrens_count] = node;
            node->parent = root_node;
            vfsc->cfn.vfs_get_cache_links(root_node)->childrens_count++;
        }
        vfsc->nodes_count++;
    }
    return (node);
}

/**
 * @brief Removes a node from the Vfs cache.
 *
 * This function removes a node from the Vfs cache.
 *
 * @param vfsc The Vfs cache from which to remove the node.
 * @param node The node to remove from the Vfs cache.
 * @return void
 */
int vfs_remove_node(VfsCache *vfsc, VfsCacheLinks *node) {
    VfsNode *parent = node->parent;

    if (parent == NULL) {
        __THROW("VFS: Failed to remove node from VFS cache", 1);
    } else if (vfsc->cfn.vfs_get_cache_links(parent)->childrens == NULL) {
        __THROW("VFS: Failed to remove node from VFS cache", 1);
    } else {
        uint32_t i = 0;

        while (vfsc->cfn.vfs_get_cache_links(parent)->childrens[i] != node) {
            ++i;
        }
        vfsc->cfn.vfs_get_cache_links(parent)->childrens[i] = NULL;
        vfsc->cfn.vfs_get_cache_links(parent)->childrens = krealloc(vfsc->cfn.vfs_get_cache_links(parent)->childrens, sizeof(VfsNode *) * (i + 1));
        if (vfsc->cfn.vfs_get_cache_links(parent)->childrens == NULL) {
            __THROW("VFS: Failed to remove node from VFS cache", 1);
        }
        vfsc->cfn.vfs_get_cache_links(parent)->childrens_count--;
    }
    vfsc->nodes_count--;
    return (0);
}