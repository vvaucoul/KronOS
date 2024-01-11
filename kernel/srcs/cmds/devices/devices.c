/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   devices.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 17:53:00 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/11 18:02:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/disks.h>
#include <drivers/device/devices.h>

#include <memory/memory.h>
#include <shell/ksh_args.h>

// ! ||--------------------------------------------------------------------------------||
// ! ||                               CMD DEVICES DISPLAY                              ||
// ! ||--------------------------------------------------------------------------------||

static void __display_help(void) {
    printk("Usage: devices [OPTION]...\n");
    printk("List devices\n");
    printk("\n");
    printk("  -h, --help\t\tDisplay this help and exit\n");
    printk("  -v, --version\t\tDisplay version information and exit\n");
}

static void __display_version(void) {
    printk("devices version 1.0\n");
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                CMD DEVICES UTILS                               ||
// ! ||--------------------------------------------------------------------------------||

// ! ||--------------------------------------------------------------------------------||
// ! ||                                CMD DEVICES MAIN                                ||
// ! ||--------------------------------------------------------------------------------||

static void cmd_devices(void) {
    uint32_t devices_count = device_get_devices_count();

    printk("Devices:\n");
    for (uint32_t i = 0; i < devices_count; ++i) {
        Device *device = device_get(i);
        printk("\t- %s\n", device->name);
        printk("\t\t- UID: %d\n", device->uid);
        printk("\t\t- Type: %d (%s)\n", device->type, device->type == DEVICE_BLOCK 
                                                    ? "Block" : device->type == DEVICE_CHAR 
                                                    ? "Char"
                                                    : "Unknown");
    }
}

int __devices(int argc, char **argv) {
    if (argc == 1) {
        cmd_devices();
        return (0);
    } else {
        if (ksh_has_arg(argc, argv, "--help") || ksh_has_arg(argc, argv, "-h")) {
            __display_help();
            return (0);
        } else if (ksh_has_arg(argc, argv, "--version") || ksh_has_arg(argc, argv, "-v")) {
            __display_version();
            return (0);
        } else {
            printk("devices: invalid option\n");
            printk("Try 'devices --help' for more information.\n");
            return (1);
        }
    }
    return (0);
}
