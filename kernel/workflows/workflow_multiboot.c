/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_multiboot.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 10:15:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 19:02:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multiboot/multiboot.h>
#include <workflows/workflows.h>
#include <memory/memory.h>

void display_multiboot_infos(void)
{
    kprintf("- " COLOR_CYAN "multiboot_info" COLOR_END ": " COLOR_GREEN "%p\n" COLOR_END, &__multiboot_info);
    kprintf("- " COLOR_CYAN "flags" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __multiboot_info->flags);
    kprintf("- " COLOR_CYAN "mem_low" COLOR_END ": " COLOR_GREEN "%u KB\n" COLOR_END, __multiboot_info->mem_lower);
    kprintf("- " COLOR_CYAN "mem_upper" COLOR_END ": " COLOR_GREEN "%u KB\n" COLOR_END, __multiboot_info->mem_upper);
    kprintf("- " COLOR_CYAN "mem length" COLOR_END ": " COLOR_GREEN "%u MB\n" COLOR_END, (__multiboot_info->mem_upper + __multiboot_info->mem_lower) / 1024);
    kprintf("- " COLOR_CYAN "boot_device" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __multiboot_info->boot_device);
    kprintf("- " COLOR_CYAN "cmdline" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __multiboot_info->cmdline);
    kprintf("- " COLOR_CYAN "modules_count" COLOR_END ": " COLOR_GREEN "%d\n" COLOR_END, __multiboot_info->mods_count);
    kprintf("- " COLOR_CYAN "modules_addr" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __multiboot_info->mods_addr);
    kprintf("- " COLOR_CYAN "mmap_length" COLOR_END ": " COLOR_GREEN "%d\n" COLOR_END, __multiboot_info->mmap_length);
    kprintf("- " COLOR_CYAN "mmap_addr" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __multiboot_info->mmap_addr);
    if (__multiboot_info->mmap_length > 0)
        kprintf("- " COLOR_CYAN "Memory Map:\n" COLOR_END);
    else
        kprintf("- " COLOR_CYAN "Memory Map: " COLOR_RED "none" COLOR_END "\n");
    // kprintf("- " COLOR_CYAN "Start ADDR: 0x%x\n" COLOR_END, KMAP.available.start_addr);
    
    // todo !

    // for (uint32_t i = 0; i < __multiboot_info->mmap_length + KERNEL_VIRTUAL_BASE; i += sizeof(MultibootMemoryMap))
    // {
    //     MultibootMemoryMap *mmap = (MultibootMemoryMap *)(__multiboot_info->mmap_addr + KERNEL_VIRTUAL_BASE + i);
    //     if (mmap->type == __MULTIBOOT_MEMORY_AVAILABLE)
    //     {
    //         kprintf("  - " COLOR_CYAN "size: " COLOR_GREEN "%u" COLOR_CYAN ", addr: " COLOR_GREEN "0x%x%x" COLOR_CYAN ", len: " COLOR_GREEN "%u%u" COLOR_CYAN ", type: " COLOR_YELLOW "%d" COLOR_CYAN "\n" COLOR_END,
    //                 mmap->size, mmap->addr_low, mmap->addr_high, mmap->len_low, mmap->len_high, mmap->type);
    //     }
    //     else
    //     {
    //         continue;
    //     }
    // }
    kprintf("\n" COLOR_END);
    return ;
    kprintf("- " COLOR_CYAN "vbe_control_info" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __multiboot_info->vbe_control_info);
    kprintf("- " COLOR_CYAN "vbe_mode_info" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __multiboot_info->vbe_mode_info);
    kprintf("- " COLOR_CYAN "vbe_mode" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __multiboot_info->vbe_mode);

    kprintf("- " COLOR_CYAN "framebuffer_addr" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __multiboot_info->framebuffer_addr);
    kprintf("- " COLOR_CYAN "framebuffer_width" COLOR_END ": " COLOR_GREEN "%d\n" COLOR_END, __multiboot_info->framebuffer_width);
    kprintf("- " COLOR_CYAN "framebuffer_height" COLOR_END ": " COLOR_GREEN "%d\n" COLOR_END, __multiboot_info->framebuffer_height);
    kprintf("- " COLOR_CYAN "framebuffer_type" COLOR_END ": " COLOR_GREEN "%d\n" COLOR_END, __multiboot_info->framebuffer_type);
}
