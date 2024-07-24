/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs_cache.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 14:21:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/18 23:39:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VFS_CACHE_H
#define VFS_CACHE_H

#include <kernel.h>

#define VFS_CACHE_ALLOC_OFFSET 0x64

typedef void VfsNode;
typedef struct s_vfs_cache VfsCache;

typedef struct s_vfs_cache_links {
    VfsNode *node;
    VfsCache *cache;

    struct s_vfs_cache_links *parent;
    struct s_vfs_cache_links **childrens;
    uint32_t childrens_count;
} VfsCacheLinks;

typedef struct s_vfs_cache_fn {
    // Vfs Node Cache operations
    int (*vfs_set_cache_links)(VfsNode *node, VfsCacheLinks *links);
    VfsCacheLinks *(*vfs_get_cache_links)(VfsNode *node);
} VfsCacheFn;

typedef struct s_vfs_cache {
    VfsNode *root_node;
    uint32_t nodes_count;
    VfsCacheFn cfn;
} VfsCache;

typedef struct s_vfs Vfs;

extern VfsCache *vfs_create_cache(Vfs *vfs, VfsCacheFn cfn);
extern int vfs_destroy_cache(VfsCache *vfsc);

extern VfsCacheLinks *vfs_create_cache_link(VfsCache *vfsc, VfsNode *node);
extern int vfs_destroy_cache_link(VfsCache *vfsc, VfsNode *node);

extern VfsNode *vfs_add_node(VfsCache *vfsc, VfsNode *root_node, VfsCacheLinks *node);
extern int vfs_remove_node(VfsCache *vfsc, VfsCacheLinks *node);

#endif /* !VFS_CACHE_H */