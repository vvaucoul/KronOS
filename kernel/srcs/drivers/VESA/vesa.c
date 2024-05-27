/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vesa.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/18 18:33:27 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/27 16:27:53 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/bios.h>
#include <system/isr.h>

#include <drivers/vesa.h>

vesa_t vesa;

unsigned int *screen = (unsigned int *)0xA0000;

const uint32_t pixelwidth = 640;
const uint32_t pixelheight = 480;

/* example for 320x200 VGA */
static void putpixel(unsigned char *screen, int x, int y, int color)
{
    unsigned where = x * pixelwidth + y * pixelheight;
    screen[where] = color & 255;             // BLUE
    screen[where + 1] = (color >> 8) & 255;  // GREEN
    screen[where + 2] = (color >> 16) & 255; // RED
}

static int get_vbe_info(void)
{
    regs16_t regs_in;
    regs16_t regs_out;

    regs_in.ax = 0x4F00;
    regs_in.di = 0x9500;
    int86(0x10, &regs_in, &regs_out);
    kpause();
    memcpy(&vesa, (void *)0x9500, sizeof(vesa_t));
    return (regs_out.ax == 0x004F);
}

void init_vbe_mode(void)
{
    printk("Initializing VBE Mode\n");
    init_bios32();

    const bool vbe_info = get_vbe_info();

    kpause();
    if (!vbe_info)
    {
        printk("VESA VBE 2.0 Mode not supported\n");
        return;
    }
    else
        printk("VESA VBE 2.0 Mode supported\n");
    kpause();

    vbe_enable_cursor(0, 15);
    vbe_update_cursor(0, 0);
    for (uint32_t i = 0; i < pixelwidth; i++)
    {
        for (uint32_t j = 0; j < pixelheight; j++)
        {
            putpixel((unsigned char *)screen, i, j, 0xFF0000);
        }
    }
}