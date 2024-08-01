/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kfe.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 12:50:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 19:09:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <apps/kfe/kfe.h>
#include <cmds/pwd.h>
#include <drivers/keyboard.h>
#include <drivers/vga.h>
#include <fs/vfs/vfs.h>
#include <mm/mm.h>
#include <system/pit.h>

#include <string.h>

static void __attribute__((constructor)) kfe_constructor(void) {
    printk("+------------------------------------------------------------------------------+");
    printk("|                            %s                              |", KFE_NAME);
    printk("+------------------------------------------------------------------------------+");
    printk("| %-76s |", "/"); // Todo: get_pwd()
    printk("+------------------------------------------------------------------------------+");
    printk("| Inode | Type      | Name                              | Size       | Blocks  |");
    printk("+------------------------------------------------------------------------------+");
}

static void kfe_print_files(Kfe *kfe) {
    // Print files
    Vfs *vfs = kfe->vfs;
    VfsNode *__selected_node = kfe->current_node;
    Dirent *dir = NULL;

    uint32_t i = 0;
    while ((dir = vfs_readdir(vfs, __selected_node, i)) != NULL) {
        VfsNode *node = vfs_finddir(vfs, __selected_node, dir->d_name);

        if (node == NULL) {
            printk("Error: [%s] File not found\n", dir->d_name);
            ++i;
            continue;
        }

        struct stat st;
        errno_t err = 0;

        if ((err = vfs_get_node_stat(vfs, node, &st)) != 0) {
            if (err == -ENOENT) {
                printk("Error: File [%s] does not exist\n", dir->d_name);
            } else {
                printk("Error: Unknown error\n");
            }
            ++i;
            continue;
        }

        printk("|");
        if (kfe->selected_inode == st.st_ino) {
            printk(_LGREEN);
            printk("->");
            printk(_END);
        } else {
            printk("  ");
        }
        printk(" %-6.4d ", st.st_ino);
        printk("|");
        if ((st.st_mode & VFS_DIRECTORY) != 0) {
            printk(_BG_BLUE);
        } else {
            printk(_BG_YELLOW);
        }
        printk(" %-9s ", (st.st_mode & VFS_DIRECTORY) ? "Directory" : "File");

        printk(_END);
        printk("|");
        printk(" %-33s ", dir->d_name);
        printk("|");
        printk(" %10d ", st.st_size);
        printk("|");
        printk(" %-7d ", st.st_blocks);
        printk("|");

        printk(_END);

        // printk("| %-8.4d | %-9s | %-33s | %-8.4d O | %-7d |", i, (st.st_mode & VFS_DIRECTORY) ? "Directory" : "File", dir->d_name, st.st_size * 1000, st.st_blocks);
        ++i;
    }
    printk("+------------------------------------------------------------------------------+");
    update_cursor(0, VGA_HEIGHT - 3);
    printk("+------------------------------------------------------------------------------+");
    printk("| Commands: [w]: Up | [s]: Down | [a]: Back | [Enter]: Select | [q]: Quit      |");
    printk("+------------------------------------------------------------------------------+");
}

Kfe *kfe_create(void) {
    Kfe *kfe = kmalloc(sizeof(Kfe));

    if (kfe == NULL) {
        __THROW("KFE: Failed to create KFE", NULL);
    } else {
        memset(kfe, 0, sizeof(Kfe));
        kfe->selected_inode = 1;
        kfe->vfs = vfs_get_current_fs();
        kfe->current_node = kfe->vfs->fs_root;
        memcpy(kfe->name, KFE_NAME, strlen(KFE_NAME));
        memcpy(kfe->version, KFE_VERSION, strlen(KFE_VERSION));
    }
    return (kfe);
}

static int __kfe_controller(Kfe *kfe) {
    bool should_exit = false;

    while (should_exit == false) {
        int c = 0;

        // Todo: Instead of clearing the screen, clear old cursor position
        terminal_clear_screen();
        kfe_constructor();
        kfe_print_files(kfe);

        if ((c = getchar()) != 0) {
            switch (c) {
            case 'q':
                should_exit = true;
                break;
            case 'w':
                if (kfe->selected_inode > 1) {
                    --kfe->selected_inode;
                }
                break;
            case 's':
                ++kfe->selected_inode;
                break;
            case 'a':
                break;
            case '\n': {
                /**
                 * Todo: ...
                 *
                 * - If selected file is a directory
                 *    - Change current node to selected directory
                 *    - Update current node
                 * - If selected file is a file
                 *   - Open file
                 *   - choose action
                 *      - Edit file
                 *      - Delete file
                 *      - Copy file
                 *      - Display file content
                 */

                // VfsNode *__selected_node = kfe->current_node;
                VfsNode *__selected_node = kfe->vfs->nops->get_links(kfe->vfs, kfe->current_node)[kfe->selected_inode];

                struct stat st;
                if ((vfs_get_node_stat(kfe->vfs, __selected_node, &st)) != 0) {
                    printk("Error: stat\n");
                    continue;
                }
                if (st.st_mode & VFS_DIRECTORY) {
                    uint32_t i = 0;
                    Dirent *dir = NULL;
                    while ((dir = vfs_readdir(kfe->vfs, __selected_node, i)) != NULL) {
                        VfsNode *node = vfs_finddir(kfe->vfs, __selected_node, dir->d_name);

                        if (i == kfe->selected_inode) {
                            kfe->current_node = node;
                            break;
                        }
                        i++;
                    }

                } else if (st.st_mode & VFS_FILE) {
                }

            } break;

            default:
                break;
            }
        }
    }

    return (0);
}

int kfe(__unused__ int argc, __unused__ char **argv) {
    Kfe *kfe = kfe_create();

    if (kfe == NULL) {
        __THROW("KFE: Failed to create KFE", 1);
    }

    __kfe_controller(kfe);

    return (0);
}