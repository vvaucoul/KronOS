## Virtual File System (VFS) - Documentation

### Overview

The `Virtual File System` (VFS) is an abstraction layer that provides a uniform interface to interact with various file systems. This document explores the `VFS` implementation and explains how it enables a seamless interaction with file systems such as `EXT2` and `INITRD`.

### Key Components

`VfsNode`

- A pointer representing a file or directory.
- Acts as the primary entity for file system operations.

`VfsFileOps`

- A structure containing file operation methods.
- Includes functions like `read`, `write`, `open`, `close`, `mkdir`, `rmdir`, `readdir`, `finddir`, etc.

`VfsFsOps`

- A structure for file system-level operations.
- Methods include `mount` and `unmount`.

`VfsNodeOps`

- Contains operations for node manipulation.
- Functions like `create_node`, `delete_node`.

### VFS Architecture
The VFS unifies the interaction with different file systems through a set of well-defined operations and structures. It abstracts the complexities of individual file systems, offering a consistent API for file and directory operations.

### Implementation Steps

1. Define VFS Structures:
   - VfsInfo provides metadata about the file system.
   - VfsFsOps handles mounting and unmounting.
   - VfsFileOps and VfsNodeOps manage file and node operations.

2. Node Creation:
   - Use create_node from VfsNodeOps to establish new nodes.
   - Nodes represent files or directories.
   - Implement File System Operations:

3. Develop functions for reading, writing, opening, and closing files.
   - Handle directory operations such as creation and navigation.

4. Mount the File System:
   - Use vfs_mount to mount the file system at a specified point.

### Code Integration

```c
#include "vfs.h"

// Initialize VFS with filesystem details
VfsInfo myFsInfo = {"myfs", "My Filesystem", "1.0", FS_TYPE_MYFS};
VfsFsOps myFsOps = {fs_mount, fs_unmount};
VfsNodeOps myNodeOps = {fs_create_node, fs_delete_node};

// Create and mount the file system
Vfs *myVfs = vfs_create_fs(&myFsInfo, &myFsOps, &myNodeOps);
vfs_mount(myVfs, "/mnt/myfs");
```

### Usage Scenario

- Upon invoking vfs_create_fs, the VFS is configured with specific filesystem operations.
- Mounting the filesystem using vfs_mount allows access to its files and directories under a unified namespace, like /mnt/myfs.

### Conclusion
The VFS is a critical component in modern operating systems, simplifying the interaction with diverse file systems. By abstracting file system operations, it provides a consistent and efficient way to manage files and directories, irrespective of the underlying file system type.