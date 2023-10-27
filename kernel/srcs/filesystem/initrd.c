/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 09:45:15 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 16:02:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/initrd.h>
#include <memory/memory.h>

InitrdHeader *initrd_header;    // The header.
InitrdFileHeader *file_headers; // The list of file headers.
Ext2Inode *initrd_root;         // Our root directory node.
Ext2Inode *initrd_dev;          // We also add a directory node for /dev, so we can mount devfs later on.
Ext2Inode *root_nodes;          // List of file nodes.
uint32_t nroot_nodes;           // Number of file nodes.

struct dirent dirent;

static void initrd_flush(Ext2Inode *node) {
    __UNUSED(node);
}

static void initrd_open(Ext2Inode *node) {
    node->flags |= FS_OPEN;
}

static void initrd_close(Ext2Inode *node) {
    node->flags &= ~FS_OPEN;
}

static struct dirent *initrd_readdir(Ext2Inode *node, uint32_t index) {
    if (node == initrd_root && index == 0) {
        strcpy(dirent.name, "dev");
        dirent.inode = 0;
        return &dirent;
    }

    if (index - 1 >= (uint32_t)nroot_nodes)
        return 0;
    strcpy(dirent.name, root_nodes[index - 1].name);
    dirent.inode = root_nodes[index - 1].inode;
    return &dirent;
}

static Ext2Inode *initrd_finddir(Ext2Inode *node, char *name) {
    // if (node == initrd_root && !strcmp(name, "dev")) {
    //     return initrd_dev;
    // }

    for (uint32_t i = 0; i < nroot_nodes; i++)
        if (!strcmp(name, root_nodes[i].name)) {
            return &root_nodes[i];
        } else if (node->flags & FS_DIRECTORY && node->childs != NULL) {
            for (uint32_t j = 0; j < node->n_children; j++) {
                initrd_finddir(node->childs[j], name);
            }
        }
    return 0;
}

static uint32_t initrd_read(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    InitrdFileHeader header = file_headers[node->inode];
    if (offset > header.length)
        return 0;
    if (offset + size > header.length)
        size = header.length - offset;
    memcpy(buffer, (uint8_t *)(header.offset + offset), size);
    return size;
}

static uint32_t initrd_write(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    InitrdFileHeader header = file_headers[node->inode];
    if (offset + size > header.length) {
        __WARN("Not enough space", 1);
    }
    memcpy((uint8_t *)(header.offset + offset), buffer, size);
    return size;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                      MKDIR                                     ||
// ! ||--------------------------------------------------------------------------------||

static void link_directory(Ext2Inode *parent, Ext2Inode *child) {
    // Link the child to the parent

    if (parent->parent == NULL) {
        parent->parent = child;
    } else {
        Ext2Inode *tmp = parent->parent;
        while (tmp->parent != NULL) {
            tmp = tmp->parent;
        }
        tmp->parent = child;
    }
}

static uint32_t intird_mkdir(Ext2Inode *node, char *name, uint16_t permission) {
    Ext2Inode *new_dir = (Ext2Inode *)kmalloc(sizeof(Ext2Inode));
    if (new_dir == NULL) {
        return 1; // Return an error code
    }

    // Initialize the new directory inode
    strcpy(new_dir->name, name);
    new_dir->flags = FS_DIRECTORY;
    new_dir->fops.read = &initrd_read;
    new_dir->fops.write = &initrd_write;
    new_dir->fops.open = &initrd_open;
    new_dir->fops.close = &initrd_close;
    new_dir->fops.readdir = &initrd_readdir;
    new_dir->fops.finddir = &initrd_finddir;
    new_dir->fops.flush = 0;
    new_dir->impl = 0;

    // Link the new directory to the parent directory (this is pseudocode and will depend on your specific FS layout)
    link_directory(node, new_dir);

    return 0;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  INITRD - INIT                                 ||
// ! ||--------------------------------------------------------------------------------||

static void __initrd_setup_directory_fops(Ext2Inode *node) {
    node->fops.read = 0;
    node->fops.write = 0;
    node->fops.open = 0;
    node->fops.close = 0;
    node->fops.readdir = &initrd_readdir;
    node->fops.finddir = &initrd_finddir;
    node->fops.flush = 0;
    node->fops.mkdir = 0;
}

static void __initrd_setup_file_fops(Ext2Inode *node) {
    node->fops.read = &initrd_read;
    node->fops.write = &initrd_write;
    node->fops.open = &initrd_open;
    node->fops.close = &initrd_close;
    node->fops.readdir = &initrd_readdir;
    node->fops.finddir = &initrd_finddir;
    node->fops.flush = &initrd_flush;
    node->fops.mkdir = &intird_mkdir;
}

static void create_node(Ext2Inode *parent, const char *name, uint32_t flags) {
    Ext2Inode *node = (Ext2Inode *)kmalloc(sizeof(Ext2Inode));
    strcpy(node->name, name);
    node->mask = node->uid = node->gid = node->inode = node->length = 0;
    node->flags = flags;
    node->fops.read = flags == FS_DIRECTORY ? 0 : &initrd_read;
    node->fops.write = flags == FS_DIRECTORY ? 0 : &initrd_write;
    node->fops.readdir = &initrd_readdir;
    node->fops.finddir = &initrd_finddir;
    node->fops.open = 0;
    node->fops.close = 0;
    node->parent = parent;
    node->impl = 0;

    int n = parent->n_children++;

    // Add this node to parent's children Array
    if (parent->childs == NULL) {
        parent->childs = (Ext2Inode **)kmalloc(sizeof(Ext2Inode *));
        parent->childs[0] = node;
    } else {
        parent->childs = (Ext2Inode **)krealloc(parent->childs, parent->n_children * sizeof(Ext2Inode *));
        parent->childs[n] = node;
    }
    printk("Created node: "_GREEN
           "[%s]\n"_END,
           node->name);
}

static uint32_t __initrd_initialize_hierarchy(void) {
    create_node(initrd_root, "dev", FS_DIRECTORY);
    create_node(initrd_root, "bin", FS_DIRECTORY);
    create_node(initrd_root, "usr", FS_DIRECTORY);
    create_node(initrd_root, "etc", FS_DIRECTORY);

    Ext2Inode *dev_node = initrd_finddir(initrd_root, "dev");

    if (dev_node == NULL) {
        __THROW("Cannot find /dev directory", 1);
    }
    create_node(dev_node, "null", FS_FILE);
    create_node(dev_node, "zero", FS_FILE);

    Ext2Inode *bin_node = initrd_finddir(initrd_root, "bin");

    if (bin_node == NULL) {
        __THROW("Cannot find /bin directory", 1);
    }
    create_node(bin_node, "ls", FS_FILE);
    create_node(bin_node, "cat", FS_FILE);
    
    return 0;
}

Ext2Inode *initrd_init(uint32_t location) {
    // Initialise the main and file header pointers and populate the root directory.
    initrd_header = (InitrdHeader *)location;
    file_headers = (InitrdFileHeader *)(location + sizeof(InitrdHeader));
    // Initialise the root directory.
    initrd_root = (Ext2Inode *)kmalloc(sizeof(Ext2Inode));

    if (initrd_root == NULL) {
        __THROW("Failed to allocate memory for initrd_root", NULL);
    }

    /* Initialise root directory */
    strcpy(initrd_root->name, "initrd");
    initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
    initrd_root->flags = FS_DIRECTORY;
    __initrd_setup_directory_fops(initrd_root);

    initrd_root->parent = NULL;
    initrd_root->impl = 0;

    /* Initialise /dev directory */
    initrd_dev = (Ext2Inode *)kmalloc(sizeof(Ext2Inode));
    strcpy(initrd_dev->name, "dev");
    initrd_dev->mask = initrd_dev->uid = initrd_dev->gid = initrd_dev->inode = initrd_dev->length = 0;
    __initrd_setup_directory_fops(initrd_dev);
    initrd_dev->parent = initrd_root;
    initrd_dev->impl = 0;
    initrd_dev->flags = FS_DIRECTORY;

    if ((__initrd_initialize_hierarchy())) {
        __THROW("Failed to initialize filesystem hierarchy", NULL);
    }

    root_nodes = (Ext2Inode *)kmalloc(sizeof(Ext2Inode) * initrd_header->nfiles);
    nroot_nodes = initrd_header->nfiles;

    printk("Initrd files: "_GREEN
           "[%d]\n"_END,
           initrd_header->nfiles);
    for (uint32_t i = 0; i < initrd_header->nfiles; i++) {
        // Edit the file's header - currently it holds the file offset
        // relative to the start of the ramdisk. We want it relative to the start
        // of memory.
        file_headers[i].offset += location;

        // Create a new file node.
        strcpy(root_nodes[i].name, (char *)&file_headers[i].name);
        root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
        root_nodes[i].length = file_headers[i].length;
        root_nodes[i].inode = i;
        root_nodes[i].flags = FS_FILE;
        __initrd_setup_file_fops(&root_nodes[i]);
        root_nodes[i].parent = initrd_root;
        root_nodes[i].impl = 0;
    }
    return initrd_root;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 DEBUG FUNCTION                                 ||
// ! ||--------------------------------------------------------------------------------||

void initrd_debug_read_disk(void) {
    int i = 0;
    struct dirent *node = 0;

    while ((node = ext2_readdir_fs(fs_root, i)) != 0) {
        Ext2Inode *fsnode = ext2_finddir_fs(fs_root, node->name);

        if (!fsnode) {
            printk("Error: Couldn't find node for file %s\n", node->name);
            continue;
        }

        printk("Found file: %s ", node->name);
        printk("(Inode: %d, Size: %d bytes)\n", fsnode->inode, fsnode->length);

        if ((fsnode->flags & 0x7) == FS_DIRECTORY) {
            Ext2Dirent *dirent = fsnode->fops.readdir(fsnode, 0);
            printk("\tType: directory:\n");

            if (dirent == NULL) {
                printk("Error: Couldn't read directory %s\n", node->name);
                continue;
            } else {
                printk("\tContents:\n");
                while (dirent != NULL) {
                    printk("\t\t%s\n", dirent->name);
                    dirent = fsnode->fops.readdir(fsnode, dirent->inode + 1);
                }
            }
        } else {
            printk("\tType: file\n");
            printk("\tContents: \"");

            // Be careful with large files, this is for debug purposes
            uint8_t buf[fsnode->length < 256 ? fsnode->length : 256];
            uint32_t sz = ext2_read_fs(fsnode, 0, sizeof(buf), buf);

            for (uint32_t j = 0; j < sz; j++) {
                if (buf[j] >= 32 && buf[j] < 127) { // printable ASCII chars
                    printk("%c", buf[j]);
                } else {
                    printk("\\x%02x", buf[j]);
                }
            }

            printk("\"\n");
        }

        printk("----\n");
        i++;
    }
}