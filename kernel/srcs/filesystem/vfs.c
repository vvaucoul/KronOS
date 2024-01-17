/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/16 17:10:43 by vvaucoul         ###   ########.fr       */
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

            if (vfs->init && vfs->init() != 0) {
                __THROW("VFS: Failed to init VFS for fs [%s]", NULL, fs_name);
            } else {
                return (vfs);
            }
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

        // Init childs
        vfs_node->childs.childs = NULL;
        vfs_node->childs.child_cnt = 0;

        // Init parent
        vfs_node->parent.parent = NULL;
        vfs_node->parent.ref_cnt = 0;
        return (vfs_node);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                CHILDS / PARENTS                                ||
// ! ||--------------------------------------------------------------------------------||

static void parent_ref_inc(VfsNode *node) {
    if (node == NULL) {
        return;
    } else {
        if (node->parent.parent && node->parent.parent->childs.child_cnt != 0) {
            node->parent.ref_cnt += node->parent.parent->childs.child_cnt;
        }
    }
}

static void parent_ref_dec(VfsNode *node) {
    if (node == NULL) {
        return;
    } else {
        if (node->parent.parent && node->parent.parent->childs.child_cnt != 0) {
            node->parent.ref_cnt -= node->parent.parent->childs.child_cnt;
        }
    }
}

static void childlist_alloc(VfsNode *node) {
    if (node == NULL) {
        return;
    } else {
        if (node->childs.childs == NULL) {
            node->childs.childs = kmalloc(sizeof(VfsNode *) * VFS_NODE_CHILD_ALLOC);
            if (node->childs.childs == NULL) {
                __THROW_NO_RETURN("VFS: Failed to alloc childs");
            } else {
                node->childs.alloced = VFS_NODE_CHILD_ALLOC;
            }
        } else if (node->childs.child_cnt >= node->childs.alloced) {
            node->childs.childs = krealloc(node->childs.childs, sizeof(VfsNode *) * (node->childs.alloced + VFS_NODE_CHILD_ALLOC));
            if (node->childs.childs == NULL) {
                __THROW_NO_RETURN("VFS: Failed to realloc childs");
            } else {
                node->childs.alloced += VFS_NODE_CHILD_ALLOC;
            }
        }
    }
}

static void childlist_append(VfsNode *parent, VfsNode *node) {
    if (parent == NULL || node == NULL) {
        return;
    } else {
        parent->childs.childs[parent->childs.child_cnt] = node;
        parent->childs.child_cnt++;
    }
}

static void childlist_remove(VfsNode *parent, VfsNode *node) {
    if (parent == NULL || node == NULL) {
        return;
    } else {
        for (uint32_t i = 0; i < parent->childs.child_cnt; i++) {
            if (parent->childs.childs[i] == node) {
                parent->childs.childs[i] = NULL;
                parent->childs.child_cnt--;
            }
        }
        kfree(node);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    VFS FSOPS                                   ||
// ! ||--------------------------------------------------------------------------------||

int vfs_add_node(VfsNode *root_node, VfsNode *node) {
    if (root_node == NULL || node == NULL) {
        return (-1);
    }

    node->parent.parent = root_node;

    childlist_alloc(root_node);
    childlist_append(root_node, node);
    parent_ref_inc(node);
    return (0);
}

int vfs_remove_node(VfsNode *root_node, VfsNode *node) {
    if (root_node || node == NULL) {
        return (-1);
    }

    childlist_alloc(root_node);
    childlist_remove(root_node, node);
    parent_ref_dec(node);
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