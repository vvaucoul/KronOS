/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bga.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/09 22:43:08 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 23:02:19 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/bga.h>

#include <multiboot/multiboot.h>
#include <system/panic.h>

uint8_t *bga_memory;

static void __setup_bga_multiboot(void)
{
    if (__multiboot_info->vbe_mode_info != 0)
        bga_memory = (uint8_t *)((vbe_info_t *)__multiboot_info->vbe_mode_info)->physbase;
}

void bga_init(void)
{
    kernel_log_info("LOG", "BGA");

    uint32_t bit = 32;

    if (bit != 32)
        return;

    uint32_t *bga = (uint32_t *)BGA_ADDRESS;
    bga[0] = 0xA5ADFACE; // Magic number
    bga[1] = 0xFAF42943; // Magic number

    __setup_bga_multiboot();

    for (uint32_t i = 2; i < 1000; i += 2)
    {
        bga[i] = 0xFF00FF00;
        bga[i + 1] = 0x00FF00FF;
    }

    for (uint32_t offset = 0xE0000000; offset < 0xFF000000; offset += 0x01000000)
    {
        for (uint32_t i = offset; i <= offset + 0x00FF0000; i += 0x100)
        {
            // request page
        }
    }
}