/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_multiboot.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 10:15:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 13:56:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multiboot/multiboot.h>
#include <workflows/workflows.h>
#include <memory/memory.h>
#include <memory/memory_map.h>

static void multiboot_check_device(MultibootInfo *mboot_ptr)
{
    uint32_t device = mboot_ptr->boot_device >> 24;
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

void display_multiboot_infos(void)
{
    __WORKFLOW_HEADER();

    printk(_CYAN "Flags:"_END
                  " 0x%x\n",
            __multiboot_info->flags);

    printk(_CYAN "Memory: "_END
                  "%uKo - %uMo\n",
            __multiboot_info->mem_lower, __multiboot_info->mem_upper / 1024);
    printk(_CYAN "Boot Device:"_END
                  " 0x%x\n",
            __multiboot_info->boot_device);
    printk(_CYAN "Cmdline:"_END
                  " %s\n",
            __multiboot_info->cmdline);
    printk(_CYAN "Mods "_END
                  "%u - 0x%x\n",
            __multiboot_info->mods_count, __multiboot_info->mods_addr);
    printk(_CYAN "Drives: "_END
                  "%u - 0x%x - ",
            __multiboot_info->drives_length, __multiboot_info->drives_addr);
    multiboot_check_device(__multiboot_info);

    printk(_CYAN "Boot Loader Name:"_END
                  " %s\n",
            __multiboot_info->boot_loader_name);

    printk(_CYAN "VBE: %u, %u, %u, %u, %u, %u\n"_END, __multiboot_info->vbe_control_info, __multiboot_info->vbe_mode_info, __multiboot_info->vbe_mode, __multiboot_info->vbe_interface_seg, __multiboot_info->vbe_interface_off, __multiboot_info->vbe_interface_len);

    printk(_CYAN "FrameBuffer: %u, %u, %u, %u, %u, %u\n"_END, __multiboot_info->framebuffer_addr, __multiboot_info->framebuffer_pitch, __multiboot_info->framebuffer_width, __multiboot_info->framebuffer_height, __multiboot_info->framebuffer_bpp, __multiboot_info->framebuffer_type);

    __WORKFLOW_FOOTER();
}
