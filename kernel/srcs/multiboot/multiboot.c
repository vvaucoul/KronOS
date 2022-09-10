/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 19:02:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/10 20:44:06 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <multiboot/multiboot.h>

bool multiboot_check_magic_number(hex_t magic_number)
{
    if (magic_number != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[CHECK] " COLOR_RED "MAGIC NUMBER IS INVALID " COLOR_END "\n");
        UPDATE_CURSOR();
        return (false);
    }
    else
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[CHECK] " COLOR_CYAN "MAGIC NUMBER IS VALID " COLOR_END "\n");
    return (true);
}

int multiboot_init(MultibootInfo *mboot_ptr)
{
    kprintf("Flags: 0x%x\n", mboot_ptr->flags);
    if ((CHECK_FLAG(mboot_ptr->flags, 0)) == false)
    {
        kprintf(COLOR_GREEN "[CHECK] FLAGS: " COLOR_RED "INVALID " COLOR_END "\n");
        return (1);
    }
    return (0);

    if (mboot_ptr->flags & MULTIBOOT_INFO_MEMORY)
    {
        mboot_ptr->mem_lower = mboot_ptr->mem_lower * 1024;
        mboot_ptr->mem_upper = mboot_ptr->mem_upper * 1024;
    }
    else
    {
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[CHECK] " COLOR_RED "MEMORY IS NOT VALID " COLOR_END "\n");
        return (1);
    }
    return (0);
}