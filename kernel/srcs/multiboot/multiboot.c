/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 19:02:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <multiboot/multiboot.h>
#include <system/panic.h>

#include <system/pit.h>

MultibootInfo *__multiboot_info = NULL;

bool multiboot_check_magic_number(hex_t magic_number) {
    if (magic_number != MULTIBOOT_BOOTLOADER_MAGIC) {
        kernel_log_info("CHK", "MAGIC NUMBER IS INVALID");
        return (false);
    }
    kernel_log_info("CHK", "MAGIC NUMBER IS VALID");
    return (true);
}

static void multiboot_check_device(MultibootInfo *mboot_ptr) {
    uint32_t device = mboot_ptr->boot_device >> 24;
    printk(_END "\t\t\t   -"_GREEN
                " DEVICE: " _END);
    switch (device) {
    case 0xE0:
        printk(_END "CD" _END "\n");
        break;
    case 0x00:
        printk(_END "Floppy Disk" _END "\n");
        break;
    case 0x80:
        printk(_END "Hard Disk" _END "\n");
        break;
    default:
        printk(_END "Unknown" _END "\n");
        break;
    }
}

int multiboot_init(MultibootInfo *mboot_ptr) {
    CHECK_AND_LOG_FLAG(mboot_ptr, 0, "FLAGS");
    CHECK_AND_LOG_FLAG(mboot_ptr, MULTIBOOT_INFO_MEMORY, "MEMORY");
    CHECK_AND_LOG_FLAG(mboot_ptr, MULTIBOOT_INFO_BOOTDEV, "BOOTDEV");
    CHECK_AND_LOG_FLAG(mboot_ptr, MULTIBOOT_INFO_DEVICE, "DEVICE");

    multiboot_check_device(mboot_ptr);

    return (0);
}