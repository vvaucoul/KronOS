/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 19:02:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/11 14:04:43 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <multiboot/multiboot.h>
#include <system/panic.h>

#include <system/pit.h>

bool multiboot_check_magic_number(hex_t magic_number)
{
    if (magic_number != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        kernel_log_info("CHK", "MAGIC NUMBER IS INVALID");
        return (false);
    }
    else if (__DISPLAY_INIT_LOG__)
        kernel_log_info("CHK", "MAGIC NUMBER IS VALID");
    return (true);
}

static void multiboot_check_device(MultibootInfo *mboot_ptr)
{
    uint32_t device = mboot_ptr->boot_device >> 24;
    printk(_END"\t\t\t  -"_GREEN " DEVICE: " _END);
    switch (device)
    {
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

int multiboot_init(MultibootInfo *mboot_ptr)
{
    if (CHECK_FLAG(mboot_ptr->flags, 0) == false)
    {
        printk(_GREEN "[CHECK] FLAGS: " _RED "INVALID " _END "\n");
        return (1);
    }
    if (CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_MEMORY) == false)
    {
        printk(_GREEN "[CHECK] MEMORY: " _RED "INVALID " _END "\n");
        return (1);
    }
    if (CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_BOOTDEV) == false)
    {
        printk(_GREEN "[CHECK] BOOTDEV: " _RED "INVALID " _END "\n");
        return (1);
    }
    // if (CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_ACPI_MEMORY) == false)
    // {
    //     printk(_GREEN "[CHECK] ACPI_MEMORY: " _RED "INVALID " _END "\n");
    //     return (1);
    // }
    if (CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_DEVICE) == false)
    {
        printk(_GREEN "[CHECK] DEVICE: " _RED "INVALID " _END "\n");
        return (1);
    }
    else
    {
        multiboot_check_device(mboot_ptr);
    }

    // if (CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_VBE) == false)
    // {
    //     printk(_GREEN "[CHECK] VBE: " _RED "INVALID " _END "\n");
    //     return (1);
    // }
    // else
    // {
    //     printk(_GREEN "VBE address: " _END "0x%x" _END "\n", mboot_ptr->vbe_control_info);
    //     if (mboot_ptr->vbe_mode_info)
    //     {
    //         printk(_GREEN "VBE mode: " _END "0x%x" _END "\n", mboot_ptr->vbe_mode);
    //         printk(_GREEN "VBE interface segment: " _END "0x%x" _END "\n", mboot_ptr->vbe_interface_seg);
    //         printk(_GREEN "VBE interface offset: " _END "0x%x" _END "\n", mboot_ptr->vbe_interface_off);
    //         printk(_GREEN "VBE interface length: " _END "0x%x" _END "\n", mboot_ptr->vbe_interface_len);
    //     }
    //     else
    //     {
    //         printk(_GREEN "VBE mode: " _END "0x%x" _END "\n", mboot_ptr->vbe_mode);
    //     }
    // }
    return (0);
}