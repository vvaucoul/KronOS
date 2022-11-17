/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 19:02:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/17 12:50:09 by vvaucoul         ###   ########.fr       */
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
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[CHK]  " COLOR_RED "MAGIC NUMBER IS INVALID " COLOR_END "\n");
        UPDATE_CURSOR();
        return (false);
    }
    else if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[CHK]  " COLOR_CYAN "MAGIC NUMBER IS VALID " COLOR_END "\n");
    return (true);
}

static void multiboot_check_device(MultibootInfo *mboot_ptr)
{
    uint32_t device = mboot_ptr->boot_device >> 24;
    kprintf(COLOR_GREEN "\t\tDEVICE: " COLOR_END);
    switch (device)
    {
    case 0xE0:
        kprintf(COLOR_END "CD" COLOR_END "\n");
        break;
    case 0x00:
        kprintf(COLOR_END "Floppy Disk" COLOR_END "\n");
        break;
    case 0x80:
        kprintf(COLOR_END "Hard Disk" COLOR_END "\n");
        break;
    default:
        kprintf(COLOR_END "Unknown" COLOR_END "\n");
        break;
    }
}

int multiboot_init(MultibootInfo *mboot_ptr)
{
    if (CHECK_FLAG(mboot_ptr->flags, 0) == false)
    {
        kprintf(COLOR_GREEN "[CHECK] FLAGS: " COLOR_RED "INVALID " COLOR_END "\n");
        return (1);
    }
    if (CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_MEMORY) == false)
    {
        kprintf(COLOR_GREEN "[CHECK] MEMORY: " COLOR_RED "INVALID " COLOR_END "\n");
        return (1);
    }
    if (CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_BOOTDEV) == false)
    {
        kprintf(COLOR_GREEN "[CHECK] BOOTDEV: " COLOR_RED "INVALID " COLOR_END "\n");
        return (1);
    }
    if (CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_ACPI_MEMORY) == false)
    {
        kprintf(COLOR_GREEN "[CHECK] ACPI_MEMORY: " COLOR_RED "INVALID " COLOR_END "\n");
        return (1);
    }
    if (CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_DEVICE) == false)
    {
        kprintf(COLOR_GREEN "[CHECK] DEVICE: " COLOR_RED "INVALID " COLOR_END "\n");
        return (1);
    }
    else
    {
        multiboot_check_device(mboot_ptr);
    }

    // if (CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_VBE) == false)
    // {
    //     kprintf(COLOR_GREEN "[CHECK] VBE: " COLOR_RED "INVALID " COLOR_END "\n");
    //     return (1);
    // }
    // else
    // {
    //     kprintf(COLOR_GREEN "VBE address: " COLOR_END "0x%x" COLOR_END "\n", mboot_ptr->vbe_control_info);
    //     if (mboot_ptr->vbe_mode_info)
    //     {
    //         kprintf(COLOR_GREEN "VBE mode: " COLOR_END "0x%x" COLOR_END "\n", mboot_ptr->vbe_mode);
    //         kprintf(COLOR_GREEN "VBE interface segment: " COLOR_END "0x%x" COLOR_END "\n", mboot_ptr->vbe_interface_seg);
    //         kprintf(COLOR_GREEN "VBE interface offset: " COLOR_END "0x%x" COLOR_END "\n", mboot_ptr->vbe_interface_off);
    //         kprintf(COLOR_GREEN "VBE interface length: " COLOR_END "0x%x" COLOR_END "\n", mboot_ptr->vbe_interface_len);
    //     }
    //     else
    //     {
    //         kprintf(COLOR_GREEN "VBE mode: " COLOR_END "0x%x" COLOR_END "\n", mboot_ptr->vbe_mode);
    //     }
    // }
    return (0);
}