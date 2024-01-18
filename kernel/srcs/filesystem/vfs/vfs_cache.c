/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs_cache.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 14:15:15 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/18 23:59:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/vfs/vfs.h>

#include <memory/memory.h>

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

int vfs_destroy_cache(VfsCache *vfsc) {
    if (vfsc->root_node != NULL) {
        vfs_destroy_cache_link(vfsc, vfsc->root_node);
    }
    kfree(vfsc);
    return (0);
}

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

int vfs_destroy_cache_link(VfsCache *vfsc, VfsNode *node) {
    if (vfsc->cfn.vfs_get_cache_links(node)->childrens != NULL) {
        for (uint32_t i = 0; i < vfsc->cfn.vfs_get_cache_links(node)->childrens_count; i++) {
            vfs_destroy_cache_link(vfsc, vfsc->cfn.vfs_get_cache_links(node)->childrens[i]);
        }
        kfree(vfsc->cfn.vfs_get_cache_links(node)->childrens);
    }
    return (0);
}

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

// /**
//  * @brief Create a new VFS algorithm node
//  *
//  * @param node
//  * @return VfsANode*
//  *
//  * @note This function is used to create a new VFS algorithm node
//  */
// VfsANode *vfs_create_anode(VfsNode *node) {
//     VfsANode *anode = kmalloc(sizeof(VfsANode));

//     if (anode == NULL) {
//         __WARN("VFS: Failed to create VFS algorithm node", NULL);
//     } else {
//         memset(anode, 0, sizeof(VfsANode));
//         anode->data = node;
//         anode->childrens = NULL;
//         anode->parent = NULL;
//         anode->childrens_count = 0;
//     }
//     return (anode);
// }

// /**
//  * @brief Add a node to the VFS
//  *
//  * @param vfs
//  * @param root_node
//  * @param node
//  * @return int
//  *
//  * @note This function is used to add a node to the VFS
//  * All nodes must be VfsANode type (use vfs algorithm)
//  */
// VfsANode *vfs_add_node(Vfs *vfs, VfsNode *root_node, VfsANode *node) {
//     VfsANode *a_root_node = (VfsANode *)root_node;

//     // If root node is NULL, set node as root node
//     if (a_root_node == NULL) {
//         return (node);
//     }
//     // If root node has no childrens, create childrens and add node
//     else if (a_root_node->childrens == NULL) {
//         a_root_node->childrens = kmalloc((sizeof(VfsANode *) * VFS_ALGORITHM_ALLOC_OFFSET));
//         if (a_root_node->childrens == NULL) {
//             __WARN("VFS: Failed to add node to VFS", NULL);
//         }
//         a_root_node->childrens[0] = node;
//         node->parent = a_root_node;
//         a_root_node->childrens_count = 1;
//     }
//     // Else add node to childrens
//     else {
//         if (a_root_node->childrens_count % VFS_ALGORITHM_ALLOC_OFFSET == 0) {
//             a_root_node->childrens = krealloc(a_root_node->childrens, sizeof(VfsANode *) * (a_root_node->childrens_count + VFS_ALGORITHM_ALLOC_OFFSET + 1));
//             if (a_root_node->childrens == NULL) {
//                 __WARN("VFS: Failed to add node to VFS", NULL);
//             }
//         }
//         a_root_node->childrens[a_root_node->childrens_count] = node;
//         node->parent = a_root_node;
//         a_root_node->childrens_count++;
//     }
//     return (node);
// }

// /**
//  * @brief Remove a node from the VFS
//  *
//  * @param vfs
//  * @param node
//  * @return int
//  *
//  * @note This function is used to remove a node from the VFS
//  * All nodes must be VfsANode type (use vfs algorithm)
//  */
// int vfs_remove_node(Vfs *vfs, VfsNode *node) {
//     VfsANode *a_node = (VfsANode *)node;
//     VfsANode *a_parent = a_node->parent;

//     if (a_parent == NULL) {
//         __WARN("VFS: Failed to remove node from VFS", 1);
//     } else if (a_parent->childrens == NULL) {
//         __WARN("VFS: Failed to remove node from VFS", 1);
//     } else {
//         int i = 0;

//         while (a_parent->childrens[i] != a_node) {
//             ++i;
//         }
//         a_parent->childrens[i] = NULL;
//         a_parent->childrens = krealloc(a_parent->childrens, sizeof(VfsANode *) * (i + 1));
//         if (a_parent->childrens == NULL) {
//             __WARN("VFS: Failed to remove node from VFS", 1);
//         }
//         a_parent->childrens_count--;
//     }
//     return (0);
// }