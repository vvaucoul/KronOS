/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 09:45:15 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 12:58:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/initrd.h>
#include <memory/memory.h>

initrd_header_t *initrd_header;     // The header.
initrd_file_header_t *file_headers; // The list of file headers.
Ext2Inode *initrd_root;             // Our root directory node.
Ext2Inode *initrd_dev;              // We also add a directory node for /dev, so we can mount devfs later on.
Ext2Inode *root_nodes;              // List of file nodes.
int nroot_nodes;                    // Number of file nodes.

struct dirent dirent;

static uint32_t initrd_read(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    initrd_file_header_t header = file_headers[node->inode];
    if (offset > header.length)
        return 0;
    if (offset + size > header.length)
        size = header.length - offset;
    memcpy(buffer, (uint8_t *)(header.offset + offset), size);
    return size;
}

static struct dirent *initrd_readdir(Ext2Inode *node, uint32_t index) {
    if (node == initrd_root && index == 0) {
        strcpy(dirent.name, "dev");
        dirent.ino = 0;
        return &dirent;
    }

    if (index - 1 >= (uint32_t)nroot_nodes)
        return 0;
    strcpy(dirent.name, root_nodes[index - 1].name);
    dirent.ino = root_nodes[index - 1].inode;
    return &dirent;
}

static Ext2Inode *initrd_finddir(Ext2Inode *node, char *name) {
    if (node == initrd_root && !strcmp(name, "dev"))
        return initrd_dev;

    for (int i = 0; i < nroot_nodes; i++)
        if (!strcmp(name, root_nodes[i].name))
            return &root_nodes[i];
    return 0;
}

static uint32_t initrd_write(Ext2Inode *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    initrd_file_header_t header = file_headers[node->inode];
    if (offset + size > header.length)
        return 0;  // Not enough space
    memcpy((uint8_t *)(header.offset + offset), buffer, size);
    return size;
}

Ext2Inode *initialise_initrd(uint32_t location) {
    // Initialise the main and file header pointers and populate the root directory.
    initrd_header = (initrd_header_t *)location;
    file_headers = (initrd_file_header_t *)(location + sizeof(initrd_header_t));
    // Initialise the root directory.
    initrd_root = (Ext2Inode *)kmalloc(sizeof(Ext2Inode));
    strcpy(initrd_root->name, "initrd");
    initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
    initrd_root->flags = FS_DIRECTORY;
    initrd_root->read = 0;
    initrd_root->write = 0;
    initrd_root->open = 0;
    initrd_root->close = 0;
    initrd_root->readdir = &initrd_readdir;
    initrd_root->finddir = &initrd_finddir;
    initrd_root->ptr = 0;
    initrd_root->impl = 0;
    // Initialise the /dev directory (required!)
    initrd_dev = (Ext2Inode *)kmalloc(sizeof(Ext2Inode));
    strcpy(initrd_dev->name, "dev");
    initrd_dev->mask = initrd_dev->uid = initrd_dev->gid = initrd_dev->inode = initrd_dev->length = 0;
    initrd_dev->flags = FS_DIRECTORY;
    initrd_dev->read = 0;
    initrd_dev->write = 0;
    initrd_dev->open = 0;
    initrd_dev->close = 0;
    initrd_dev->readdir = &initrd_readdir;
    initrd_dev->finddir = &initrd_finddir;
    initrd_dev->ptr = 0;
    initrd_dev->impl = 0;

    root_nodes = (Ext2Inode *)kmalloc(sizeof(Ext2Inode) * initrd_header->nfiles);
    nroot_nodes = initrd_header->nfiles;
    // For every file...
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
        root_nodes[i].read = &initrd_read;
        root_nodes[i].write = &initrd_write;
        root_nodes[i].readdir = 0;
        root_nodes[i].finddir = 0;
        root_nodes[i].open = 0;
        root_nodes[i].close = 0;
        root_nodes[i].impl = 0;
    }
    return initrd_root;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 DEBUG FUNCTION                                 ||
// ! ||--------------------------------------------------------------------------------||

void read_disk(void) {
    int i = 0;
    struct dirent *node = 0;
    while ((node = readdir_fs(fs_root, i)) != 0) {
        printk("Found file ");
        printk(node->name);
        Ext2Inode *fsnode = finddir_fs(fs_root, node->name);

        if ((fsnode->flags & 0x7) == FS_DIRECTORY) {
            printk("\n\t(directory)\n");
        } else {
            printk("\n\t contents: \"");
            uint8_t buf[256];
            uint32_t sz = read_fs(fsnode, 0, 256, buf);
            uint32_t j;
            for (j = 0; j < sz; j++) {
                char temp_str[2] = {buf[j], '\0'};
                printk(temp_str);
            }

            printk("\"\n");
        }
        i++;
    }
}