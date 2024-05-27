/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:50:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/03/14 20:12:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/vfs/vfs.h>

#include <filesystem/ext2/ext2.h>
#include <filesystem/initrd.h>

#include <memory/memory.h>
#include <multitasking/process.h>

Hashtable *vfs_mounts = NULL;

// ! ||--------------------------------------------------------------------------------||
// ! ||                                       VFS                                      ||
// ! ||--------------------------------------------------------------------------------||

int vfs_init(void) {
    vfs_mounts = hashtable_create();
    if (vfs_mounts == NULL) {
        __THROW("VFS: Failed to init VFS", 1);
    }
    return (0);
}

Vfs *vfs_get_current_fs(void) {
    // Todo: return current fs
    return (hashtable_get(vfs_mounts, "TinyFS"));
    // return (hashtable_get(vfs_mounts, "ext2"));
}

Vfs *vfs_get_fs(const char *fs_name) {
    return (hashtable_get(vfs_mounts, fs_name));
}

Vfs *vfs_create_fs(VfsFS *fs, VfsInfo *fs_info, VfsFsOps *fsops, VfsFileOps *fops, VfsNodeOps *nops) {
    Vfs *vfs = kmalloc(sizeof(Vfs));

    if (vfs == NULL) {
        __THROW("VFS: Failed to create VFS", NULL);
    } else {
        memset(vfs, 0, sizeof(Vfs));

        vfs->fs = fs;
        vfs->fs_info = fs_info;
        vfs->fs_root = NULL;
        vfs->fs_current_node = NULL;

        vfs->use_vfs_cache = 0;
        vfs->vfs_cache = NULL;

        vfs->fsops = fsops;
        vfs->fops = fops;
        vfs->nops = nops;

        return (vfs);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    VFS NODES                                   ||
// ! ||--------------------------------------------------------------------------------||

VfsNode *vfs_create_node(Vfs *vfs, VfsNode *root_node, const char *node_name) {
    VfsNode *node = vfs->nops->create_node(root_node, node_name);

    if (vfs->use_vfs_cache == 1) {
        VfsNode *cache_node = vfs_create_cache_link(vfs->vfs_cache, node);

        if ((vfs_add_node(vfs->vfs_cache, root_node, cache_node)) == NULL) {
            __WARN("VFS: Failed to add node to VFS Cache", NULL);
        }
        return (node);
    }
    return (node);
}

int vfs_delete_node(Vfs *vfs, VfsNode *node) {
    if (vfs->use_vfs_cache == 1) {
        int st = 0;

        if ((st = vfs_remove_node(vfs->vfs_cache, node)) != 0) {
            __WARN("VFS: Failed to remove node from VFS", 1);
        }
        if ((st += vfs_destroy_cache_link(vfs->vfs_cache, node)) != 0) {
            __WARN("VFS: Failed to destroy node from VFS Cache", 1);
        }
        return (st);
    }
    return (vfs->nops->remove_node(node));
}

void *vfs_custom_nops(Vfs *vfs, VfsNode *node, uint8_t nops_index, ...) {
    if (vfs->use_vfs_cache == 1) {
        // VfsANode *a_node = (VfsANode *)node;

        // va_list args;
        // int i = 0;
        // va_start(args, i);

        // void *result = vfs->nops->custom_ops[nops_index](a_node->data, args);

        // va_end(args);
        // return (result);
    }
    return (NULL);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                              VFS NODES OPERATIONS                              ||
// ! ||--------------------------------------------------------------------------------||

/* File operations */
int vfs_open(Vfs *vfs, VfsNode *node, uint32_t flags) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->open == NULL) {
        return (-1);
    } else {
        return (vfs->fops->open(node, flags));
    }
}

int vfs_close(Vfs *vfs, VfsNode *node) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->close == NULL) {
        return (-1);
    } else {
        return (vfs->fops->close(node));
    }
}

int vfs_read(Vfs *vfs, VfsNode *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->read == NULL) {
        return (-1);
    } else {
        return (vfs->fops->read(node, offset, size, buffer));
    }
}

int vfs_write(Vfs *vfs, VfsNode *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->write == NULL) {
        return (-1);
    } else {
        return (vfs->fops->write(node, offset, size, buffer));
    }
}

/* Directory operations */
int vfs_opendir(Vfs *vfs, VfsNode *node) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->opendir == NULL)
        return (-1);
    else {
        return (vfs->fops->opendir(node));
    }
}

int vfs_closedir(Vfs *vfs, VfsNode *node) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->closedir == NULL)
        return (-1);
    else {
        return (vfs->fops->closedir(node));
    }
}

Dirent *vfs_readdir(Vfs *vfs, VfsNode *node, uint32_t index) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->readdir == NULL) {
        return (NULL);
    } else {
        return (vfs->fops->readdir(node, index));
    }
}

VfsNode *vfs_finddir(Vfs *vfs, VfsNode *node, const char *name) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->finddir == NULL) {
        return (NULL);
    } else {
        return (vfs->fops->finddir(node, name));
    }
}

/* Create and delete files and directories */
int vfs_create(Vfs *vfs, VfsNode *node, const char *name, uint16_t permission) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->create == NULL) {
        return (-1);
    } else {
        return (vfs->fops->create(node, name, permission));
    }
}

int vfs_unlink(Vfs *vfs, VfsNode *node, const char *name) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->unlink == NULL) {
        return (-1);
    } else {
        return (vfs->fops->unlink(node, name));
    }
}

int vfs_mkdir(Vfs *vfs, VfsNode *node, const char *name, uint16_t permission) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->mkdir == NULL) {
        return (-1);
    } else {
        return (vfs->fops->mkdir(node, name, permission));
    }
}

int vfs_rmdir(Vfs *vfs, VfsNode *node, const char *name) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->rmdir == NULL) {
        return (-1);
    } else {
        return (vfs->fops->rmdir(node, name));
    }
}

/* Other operations */
int vfs_move(Vfs *vfs, VfsNode *node, const char *name, const char *new_name) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->move == NULL) {
        return (-1);
    } else {
        return (vfs->fops->move(node, name, new_name));
    }
}

int vfs_chmod(Vfs *vfs, VfsNode *node, uint16_t permission) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->chmod == NULL) {
        return (-1);
    } else {
        return (vfs->fops->chmod(node, permission));
    }
}

int vfs_chown(Vfs *vfs, VfsNode *node, uint16_t owner) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->chown == NULL) {
        return (-1);
    } else {
        return (vfs->fops->chown(node, owner));
    }
}

/* Link operations */
int vfs_link(Vfs *vfs, VfsNode *node, const char *name, const char *new_name) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->link == NULL) {
        return (-1);
    } else {
        return (vfs->fops->link(node, name, new_name));
    }
}

int vfs_symlink(Vfs *vfs, VfsNode *node, const char *name, const char *new_name) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->symlink == NULL) {
        return (-1);
    } else {
        return (vfs->fops->symlink(node, name, new_name));
    }
}

int vfs_readlink(Vfs *vfs, VfsNode *node, const char *name, const char *new_name) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->readlink == NULL) {
        return (-1);
    } else {
        return (vfs->fops->readlink(node, name, new_name));
    }
}

/* Utils operations */
int vfs_chdir(Vfs *vfs, const char *path) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->chdir == NULL) {
        return (-1);
    } else {
        return (vfs->fops->chdir(vfs, path));
    }
    //     if (vfs == NULL) {
    //     return (-1);
    // } else {
    //     VfsNode *node = vfs_finddir(vfs, vfs->fs_root, path);
    //     if (node == NULL) {
    //         printk("Failed to change directory (node not found)\n");
    //         return (-1);
    //     }
    //     task_t *task = get_current_task();

    //     if (task == NULL) {
    //         printk("Failed to change directory (no current task)\n");
    //         return -1;
    //     } else {
    //         memscpy(task->env.pwd, 64, path, strlen(path));
    //     }
    // }
    // return (0);
}

int vfs_stat(Vfs *vfs, VfsNode *node, struct stat *buf) {
    if (vfs == NULL || vfs->fops == NULL || vfs->fops->stat == NULL) {
        return (-1);
    } else {
        return (vfs->fops->stat(node, buf));
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                               VFS MOUNT / UNMOUNT                              ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief Mount a filesystem
 *
 * @param vfs
 * @return int
 */
int vfs_mount(Vfs *vfs) {
    if (vfs == NULL || vfs->fsops == NULL || vfs->fsops->mount == NULL) {
        return (-1);
    } else {
        if ((vfs->fsops->mount(vfs)) != 0) {
            return (1);
        }
        hashtable_insert(vfs_mounts, vfs->fs_info->name, vfs);
    }
    return (0);
}

/**
 * @brief Unmount a filesystem
 *
 * @param vfs
 * @return int
 */
int vfs_unmount(Vfs *vfs) {
    if (vfs == NULL || vfs->fsops == NULL || vfs->fsops->unmount == NULL) {
        return (-1);
    } else {
        if ((vfs->fsops->mount(vfs)) != 0) {
            return (1);
        }
        hashtable_remove(vfs_mounts, vfs->fs_info->name);
    }
    return (0);
}