/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 19:02:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 13:02:34 by vvaucoul         ###   ########.fr       */
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
    else
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[CHK]  " COLOR_CYAN "MAGIC NUMBER IS VALID " COLOR_END "\n");
    return (true);
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
    return (0);
}