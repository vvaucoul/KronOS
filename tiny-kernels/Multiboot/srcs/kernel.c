/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/19 14:58:23 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 12:55:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"
#include "gdt.h"
#include "multiboot.h"

int kmain(uint32_t magic_number, uint32_t *multiboot_info)
{
    terminal_initialize();
    if (magic_number != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        terminal_writestring("Error: Invalid magic number");
        return (1);
    }
    terminal_clear_screen();
    terminal_writestring("Hello, kernel World!\n");
    gdt_init();
    multiboot_init((uint32_t *)((uint32_t)multiboot_info + 0xC0000000));
    if (multiboot_check() == 1)
    {
        terminal_writestring("Error: Invalid multiboot info");
        return (1);
    }
    while (1)
        ;
}