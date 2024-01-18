# Virtual File System (VFS) Documentation

## Introduction

The Virtual File System (VFS) is an abstraction layer on top of a more concrete file system. The purpose of a VFS is to allow client applications to access different types of concrete file systems in a uniform way.

## VFS Architecture

The VFS architecture consists of the following abstract components:

- `VfsFileOps *`: Pointer to a structure containing the operations for manipulating files.
  - eg. `read`, `write`, `open`, `close`
- `VfsNode *`: Pointer to a structure representing a file or directory.
- `VfsFsOps *`: Pointer to a structure containing the operations for manipulating the filesystem.
  - `mount`: Mount the filesystem at a specific mount point.
  - `unmount`: Unmount the filesystem.

- `VfsNodeOps *`: Pointer to a structure containing the operations for manipulating nodes.
  - eg. `create_node`, `delete_node`, `get_node`

## VFS Operations

The VFS provides a set of operations for manipulating files and directories. These operations include:

- `read`: Read data from a file.
- `write`: Write data to a file.
- `open`: Open a file or directory.
- `close`: Close a file or directory.
- `mkdir`: Create a new directory.
- `rmdir`: Remove a directory.
- `readdir`: Read the contents of a directory.
- `finddir`: Find a directory by name.

The VFS provides a set of operations for manipulating the filesystem. These operations include:

- `mount`: Mount the filesystem at a specific mount point.
- `unmount`: Unmount the filesystem.

## Implementing a VFS

To implement a VFS, you need to:

1. Define the `Vfs` structure and the `VfsInfo`, `VfsFsOps`, `VfsNodeOps` structures.
2. Create a new node using the `create_node` operation.
   1. `create_node`: use the `VfsNodeOps` structure to create a new node.
3. Implement the VFS operations.
4. Implement the filesystem-specific operations.
5. Mount the filesystem at a specific mount point.

## Code Example

```c
#include <filesystem/vfs.h>

// Define the Vfs structure.
Vfs *vfs;

// Define the VfsFsOps structure.
VfsFsOps *fs_ops = {
    .mount = fs_mount,
    .unmount = fs_unmount,
};

// Define the VfsNodeOps structure.
VfsNodeOps *node_ops = {
    .create_node = fs_create_node,
    .delete_node = fs_delete_node,
    .get_node = fs_get_node,
};

// Define the VfsInfo structure.
VfsInfo *fs_info = {
    .name = "myfs",
    .d_name = "My new Filesystem",
    .version = "1.0",
    .type = FS_TYPE_MYFS, // Refer to vfs.h for a list of filesystem types.
};

// Create a new Vfs.
vfs = vfs_create(fs_info, fs_ops, node_ops);

// Mount the filesystem at a specific mount point.
vfs_mount(vfs, "/mnt");
```

## VFS Code explanation

1. Create a new Vfs using the `vfs_create` function.
2. Mount the filesystem at a specific mount point using the `vfs_mount` function.
3. Create a new node using the `vfs_create_node` function.
   - The `vfs_create_node` function uses the `VfsNodeOps` structure to create a new node. (Refer to the `VfsNodeOps` structure for more information.)
   - If the user wants to use `VFS` algorithms, the `VFS` will store the new `VfsNode` in a struture called `VfsCache`.
   - The `vfs_create_node` function returns a pointer to the new `VfsNode`.
4. Implement the VFS operations.

## Conclusion

The VFS provides a uniform interface for accessing different types of filesystems. By implementing a VFS, you can make your application filesystem-agnostic, allowing it to work with any type of filesystem.