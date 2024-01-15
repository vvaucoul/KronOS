/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/14 01:28:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/ext2/ext2.h>
#include <filesystem/vfs.h>

#include <memory/memory.h>

Vfs *vfs_init(const char *fs_name, VfsNode *fs_root, int (*init)(void)) {
    Vfs *vfs = kmalloc(sizeof(Vfs));

    if (vfs == NULL) {
        __THROW("VFS: Failed to create VFS", NULL);
    } else {
        memset(vfs, 0, sizeof(Vfs));
        memcpy(vfs->fs_name, fs_name, strlen(fs_name));
        vfs->fs_root = fs_root;
        vfs->init = init;
        vfs->fsops = kmalloc(sizeof(VfsFsOps));

        if (vfs->fsops == NULL) {
            __THROW("VFS: Failed to create VFS", NULL);
        } else {
            memset(vfs->fsops, 0, sizeof(VfsFsOps));
        }
        return (vfs);
    }
}

VfsNode *vfs_create_node(const char *node_name) {
    VfsNode *vfs_node = kmalloc(sizeof(VfsNode));

    if (vfs_node == NULL) {
        __THROW("VFS: Failed to create VFS", NULL);
    } else {
        memset(vfs_node, 0, sizeof(VfsNode));

        memset(vfs_node->name, 0, VFS_NODE_FILE_LEN);
        memcpy(vfs_node->name, node_name, strlen(node_name));

        vfs_node->next = NULL;
        return (vfs_node);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    VFS FSOPS                                   ||
// ! ||--------------------------------------------------------------------------------||

int vfs_add_node(VfsNode *root_node, VfsNode *node) {
    if (root_node == NULL || node == NULL) {
        return (-1);
    }
    node->parent = root_node;

    // Add node to the list of childs
    if (root_node->next == NULL) {
        root_node->next = node;
    } else {
        VfsNode *tmp = root_node->next;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = node;
    }
    root_node->child_count++;
    return (0);
}

int vfs_remove_node(VfsNode *root_node, VfsNode *node) {
    if (root_node || node == NULL) {
        return (-1);
    }
    VfsNode *tmp = root_node->next;
    VfsNode *prev = NULL;

    while (tmp != NULL) {
        if (tmp == node) {
            if (prev == NULL) {
                root_node->next = tmp->next;
            } else {
                prev->next = tmp->next;
            }
            tmp->next = NULL;
            root_node->child_count--;
            return (0);
        }
        prev = tmp;
        tmp = tmp->next;
    }

    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                              VFS NODES OPERATIONS                              ||
// ! ||--------------------------------------------------------------------------------||

Dirent *vfs_readdir(VfsNode *node) {
    if (node == NULL || node->fops.readdir == NULL) {
        return (NULL);
    } else {
        if ((node->flags & VFS_DIRECTORY) != 0 && node->fops.readdir != NULL) {
            return (node->fops.readdir(node));
        }
    }
    return (NULL);
}

VfsNode *vfs_finddir(VfsNode *node, const char *name) {
    if (node == NULL || node->fops.finddir == NULL) {
        return (NULL);
    } else {
        if ((node->flags & VFS_DIRECTORY) != 0 && node->fops.finddir != NULL) {
            return (node->fops.finddir(node, name));
        }
    }
    return (NULL);
}

int vfs_opendir(VfsNode *node) {
    node->dir_index = 0;
    return (0);
}

int vfs_closedir(VfsNode *node) {
    node->dir_index = 0;
    return (0);
}

int vfs_open(VfsNode *node, uint32_t flags) {
    return (0);
}

int vfs_close(VfsNode *node) {
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                               VFS MOUNT / UNMOUNT                              ||
// ! ||--------------------------------------------------------------------------------||

int vfs_mount(Vfs *vfs) {
    if (vfs == NULL || vfs->fsops == NULL) {
        return (-1);
    } else {
        vfs->fsops()->mount();
    }
    return (0);
}

int vfs_unmount(Vfs *vfs) {
    if (vfs == NULL || vfs->fsops == NULL) {
        return (-1);
    } else {
        vfs->fsops()->unmount();
    }
    return (0);
}