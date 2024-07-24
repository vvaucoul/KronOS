/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vga.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:31:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/23 14:42:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/vga.h>

size_t terminal_row;
size_t terminal_column;
uint16_t *terminal_buffer;

uint8_t __vga_foreground_color = VGA_COLOR_LIGHT_GREY;
uint8_t __vga_background_color = VGA_COLOR_BLACK;

void vga_init(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_buffer = __VGA_MEMORY;
    terminal_clear_screen();
}

void terminal_set_color(uint8_t color) {
    __vga_foreground_color = (color & 0x0F);
}

void terminal_set_background_color(uint8_t color) {
    __vga_background_color = color;
}

static void terminal_newline(void) {
    ++terminal_row;
    terminal_column = 0;
    if (terminal_row >= VGA_HEIGHT) {
        terminal_move_offset_down();
        terminal_row = VGA_HEIGHT - 1;
    }
}

static void terminal_tab(void) {
    terminal_column += 4;
    if (terminal_column >= VGA_WIDTH) {
        terminal_newline();
    }
}

void terminal_putchar(char c) {
    switch (c) {
    case '\n':
        terminal_newline();
        break;
    case '\t':
        terminal_tab();
        break;
    default:
        terminal_putentryat(c, terminal_column, terminal_row);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            terminal_newline();
        }
        break;
    }
    update_cursor(terminal_column, terminal_row);
}

static void terminal_write(const char *data, size_t size) {
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char *data) {
    terminal_write(data, strlen(data));
}

void terminal_writestring_location(const char *data, size_t x, size_t y) {
    size_t ux = x, uy = y;
    for (size_t i = 0; i < strlen(data); i++) {
        terminal_putentryat(data[i], ux, uy);
        if (++ux == VGA_WIDTH) {
            ux = x;
            if (++uy == VGA_HEIGHT)
                uy = y;
        }
    }
}

void update_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void refresh_cursor(void) {
    update_cursor(terminal_column, terminal_row);
}

void terminal_write_n_char(char c, size_t count) {
    for (size_t i = 0; i < count; i++) {
        terminal_putchar(c);
    }
}

void terminal_move_offset_down(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            TERMINAL_CHAR(x, y) = TERMINAL_CHAR(x, y + 1);
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        TERMINAL_CHAR(x, VGA_HEIGHT - 1) = VGA_ENTRY(' ', VGA_ENTRY_COLOR(__vga_foreground_color, __vga_background_color));
    }
}

void vga_enable_cursor(void) {
    outb(VGA_CRTC_INDEX, VGA_CURSOR_START_REG);
    outb(VGA_CRTC_DATA, (inb(VGA_CRTC_DATA) & 0xC0) | 14);
    outb(VGA_CRTC_INDEX, VGA_CURSOR_END_REG);
    outb(VGA_CRTC_DATA, (inb(VGA_CRTC_DATA) & 0xE0) | 15);
}

void vga_disable_cursor(void) {
    outb(VGA_CRTC_INDEX, VGA_CURSOR_START_REG);
    outb(VGA_CRTC_DATA, 0x20);
}