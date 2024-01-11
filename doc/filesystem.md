# Filesystem Documentation

### Drivers: ATA / PATA etc.
In a computer system, drivers are essential components that allow the operating system to interact with hardware devices. In the context of filesystems, drivers such as ATA (Advanced Technology Attachment), PATA (Parallel ATA), SATA (Serial ATA), etc., are used to manage the communication between the operating system and the storage devices (like hard drives or SSDs).

These drivers handle the low-level details of reading and writing data to the storage devices. They operate at the block level, meaning they read or write blocks of data at a time.

### Block Structures: Nodes, Inodes
A filesystem organizes data in a structured way using several components, including blocks, nodes, and inodes.

- **Blocks**: A block is the smallest unit of data that a filesystem can read or write at a time. The size of a block can vary depending on the filesystem.

- **Nodes**: A node is a data structure that represents a file or a directory in the filesystem. Each node contains information like the name of the file or directory, its size, permissions, and the location of its data blocks.

- **Inodes**: An inode (index node) is a data structure in a Unix-style filesystem that describes a file or directory. It includes information about the file or directory, such as its size, permissions, creation time, owner, and the location of its data blocks.

### Filesystem: ext2
The ext2 (second extended filesystem) is a filesystem used by the Linux kernel. It was designed to overcome the limitations of the original ext filesystem.

- **ext2.c**: This file contains the implementation of the ext2 filesystem. It includes functions for reading and writing data, managing inodes, and handling directories.

- **ext2.h**: This header file contains the definitions and structures related to the ext2 filesystem. It includes the definition of the ext2 superblock, inode, and directory entry structures.

### Initrd
The initrd (initial ramdisk) is a temporary root filesystem that is loaded into memory during the Linux boot process.

- **initrd.c**: This file contains the implementation of the initrd filesystem. It includes functions for initializing the filesystem, reading and writing data, and managing files and directories.

- **initrd.h**: This header file contains the definitions and structures related to the initrd filesystem. It includes the definition of the initrd header and file header structures.

### Virtual File System (VFS)
The Virtual File System (VFS) is an abstraction layer that provides a common interface for interacting with various filesystems.

- **vfs.c**: This file contains the implementation of the VFS. It includes functions for initializing the VFS, mounting and unmounting filesystems, and reading and writing data.

- **vfs.h**: This header file contains the definitions and structures related to the VFS. It includes the definition of the VFS node, file operations, and filesystem operations structures.
