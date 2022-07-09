/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:31:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/07/09 12:10:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <terminal.h>

size_t terminal_row;
size_t terminal_column;
size_t terminal_screen;
uint8_t terminal_color;
uint16_t *terminal_buffer;

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_screen = 0;
    terminal_color = VGA_ENTRY_COLOR(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = __VGA_MEMORY__;
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            TERMINAL_CHAR(x, y) = VGA_ENTRY(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putchar(char c)
{
    if (c == CHAR_NEWLINE)
    {
        ++terminal_row;
        terminal_column = 0;

        // if (terminal_row >= VGA_HEIGHT - 1)
        if (terminal_row >= VGA_HEIGHT)
        {
            terminal_move_offset_down();
            terminal_row = VGA_HEIGHT - 1;
            terminal_column = 0;
        }
        return;
    }
    else if (c == CHAR_TAB)
    {
        terminal_column += 4;
        if (terminal_column >= VGA_WIDTH)
        {
            ++terminal_row;
            terminal_column = 0;
        }
        return;
    }
    else
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH)
    {
        terminal_column = 0;
        ++terminal_row;
    }
}

static void terminal_write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char *data)
{
    terminal_write(data, kstrlen(data));
}

void terminal_writestring_location(const char *data, size_t x, size_t y)
{
    size_t ux = x;
    size_t uy = y;

    for (size_t i = 0; i < kstrlen(data); i++)
    {
        char c = data[i];
        terminal_putentryat(c, terminal_color, ux, uy);
        if (++ux == VGA_WIDTH)
        {
            ux = x;
            if (++uy == VGA_HEIGHT)
                uy = y;
        }
    }
}

void update_cursor(int x, int y)
{
    uint16_t pos = y * VGA_WIDTH + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void terminal_write_n_char(char c, size_t count)
{
    for (size_t i = 0; i < count; i++)
        terminal_putchar(c);
}

void terminal_move_offset_down(void)
{
    size_t y = 0;

    for (; y < VGA_HEIGHT - 1; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            TERMINAL_CHAR(x, y) = TERMINAL_CHAR(x, y + 1);
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++)
        TERMINAL_CHAR(x, y) = VGA_ENTRY(' ', terminal_color);
}