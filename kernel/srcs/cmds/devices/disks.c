/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 10:52:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/11 18:56:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/disks.h>
#include <drivers/device/ata.h>

#include <memory/memory.h>
#include <shell/ksh_args.h>

// ! ||--------------------------------------------------------------------------------||
// ! ||                                CMD DISKS DISPLAY                               ||
// ! ||--------------------------------------------------------------------------------||

static void __display_help(void) {
    printk("Usage: disks [OPTION]...\n");
    printk("List disks\n");
    printk("\n");
    printk("  -h, --help\t\tDisplay this help and exit\n");
    printk("  -v, --version\t\tDisplay version information and exit\n");
    printk("  -c, --count\t\tDisplay number of disks\n");
    printk("  -d, --details\t\tDisplay details of disks\n");
    printk("  -s, --size\t\tDisplay size of disks\n");
    printk("  -i, --index\t\tSpecify index of disks\n");
    printk("  -e, --state\t\tCheck state of disks\n");
}

static void __display_version(void) {
    printk("disks version 1.0\n");
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 CMD DISKS UTILS                                ||
// ! ||--------------------------------------------------------------------------------||

#define for_each_disks(index, cmd, disk_index, inc) \
    int *disks = __get_disks_index(cmd);            \
    for (index = 0; (disk_index = disks[index]) != -1; inc)

static int *__get_disks_index(cmd_disks_t *cmd) {
    int *index = kmalloc(sizeof(int) * 4);

    if (!index) {
        return (NULL);
    } else {
        if (BIT_CHECK(cmd->flags, CMD_DISKS_INDEX)) {
            index[0] = cmd->index;
            index[1] = -1;
            return (index);
        } else {
            for (int i = 0; i < 4; ++i) {
                index[i] = i;
            }
            index[4] = -1;
        }
    }
    return (index);
}

static void __cmd_disks_states(cmd_disks_t *cmd) {
    uint32_t i = 0;
    int disk_index = 0;
    for_each_disks(i, cmd, disk_index, i++) {
        ATADevice *device = ata_get_device(i);

        if (device == NULL) {
            printk("\t\t- Disk "_GREEN
                   "[%d]:"_END
                   " not found\n",
                   disk_index);
        } else {
            printk("\t\t- Disk "_GREEN
                   "[%d]:"_END
                   "\n",
                   disk_index);
            ATADevice *device = ata_get_device(i);
            __ata_display_disk_state(device);
        }
    }
}

static void __cmd_disks_count(cmd_disks_t *cmd) {
    uint32_t i = 0;
    uint32_t count = 0;
    int disk_index = 0;
    for_each_disks(i, cmd, disk_index, i++) {
        ATADevice *device = ata_get_device(i);

        if (device != NULL) {
            ++count;
        }
    }
    printk("Disks count: "_GREEN
           "[%d]"_END
           "\n",
           count);
}

static void __cmd_disks_details(cmd_disks_t *cmd) {
    uint32_t i = 0;
    int disk_index = 0;
    for_each_disks(i, cmd, disk_index, i++) {
        ATADevice *device = ata_get_device(i);

        if (device == NULL) {
            printk("\t\t- Disk "_GREEN
                   "[%d]:"_END
                   " not found\n",
                   disk_index);
        } else {
            printk("\t\t- Disk "_GREEN
                   "[%d]:"_END
                   "\n",
                   disk_index);
            ata_disk_details(disk_index);
        }
    }
}

static void __cmd_disks_size(cmd_disks_t *cmd) {
    uint32_t i = 0;
    int disk_index = 0;
    for_each_disks(i, cmd, disk_index, i++) {
        printk("Disk "_GREEN
               "[%d]:"_END
               "\n",
               disk_index);
        ata_disk_size(disk_index);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 CMD DISKS MAIN                                 ||
// ! ||--------------------------------------------------------------------------------||

static void cmd_disks(cmd_disks_t *cmd) {
    if (BIT_CHECK(cmd->flags, CMD_DISKS_COUNT)) {
        __cmd_disks_count(cmd);
    }
    if (BIT_CHECK(cmd->flags, CMD_DISKS_DETAILS)) {
        __cmd_disks_details(cmd);
    }
    if (BIT_CHECK(cmd->flags, CMD_DISKS_SIZE)) {
        __cmd_disks_size(cmd);
    }
    if (BIT_CHECK(cmd->flags, CMD_DISKS_STATE)) {
        __cmd_disks_states(cmd);
    }
}

/**
 * @brief List disks
 *
 * @param argc
 * @param argv
 * @return int
 *
 * @note
 * - disks : list disks
 * basic implementation of disks system command
 */

int disks(int argc, char **argv) {
    if (argc < 2) {
        __display_help();
        return (1);
    } else {
        cmd_disks_t cmd = {
            .flags = 0,
            .index = -1};

        if (ksh_has_arg(argc, argv, "-h") || ksh_has_arg(argc, argv, "--help")) {
            __display_help();
            return (0);
        }

        if (ksh_has_arg(argc, argv, "-v") || ksh_has_arg(argc, argv, "--version")) {
            __display_version();
            return (0);
        }

        if (ksh_has_arg(argc, argv, "-c") || ksh_has_arg(argc, argv, "--count")) {
            BIT_SET(cmd.flags, CMD_DISKS_COUNT);
        }

        if (ksh_has_arg(argc, argv, "-d") || ksh_has_arg(argc, argv, "--details")) {
            BIT_SET(cmd.flags, CMD_DISKS_DETAILS);
        }

        if (ksh_has_arg(argc, argv, "-s") || ksh_has_arg(argc, argv, "--size")) {
            BIT_SET(cmd.flags, CMD_DISKS_SIZE);
        }

        if (ksh_contain_arg(argc, argv, "-i") || ksh_contain_arg(argc, argv, "--index")) {
            BIT_SET(cmd.flags, CMD_DISKS_INDEX);
            cmd.index = atoi((char *)ksh_get_arg_value(argc, argv, "-i", '='));
        }

        if (ksh_has_arg(argc, argv, "-e") || ksh_has_arg(argc, argv, "--state")) {
            BIT_SET(cmd.flags, CMD_DISKS_STATE);
        }

        cmd_disks(&cmd);
    }

    return (0);
}
