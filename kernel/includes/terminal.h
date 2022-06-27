/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:32:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 19:59:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TERMINAL_H
#define TERMINAL_H

#include "kernel.h"
#include "system/io.h"

#include "../../libkfs/libs/stdbool/stdbool.h"

/* Hardware text mode color constants. */
enum vga_color
{
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

#define CHAR_ENDLINE 0x00
#define CHAR_TAB 0x09
#define CHAR_NEWLINE 0x0A

#define VGA_ENTRY(uc, color) (((unsigned char)uc) | ((uint8_t)color) << 8)
#define VGA_ENTRY_COLOR(fg, bg) (((enum vga_color)fg) | ((enum vga_color)bg) << 4)
#define __VGA_MEMORY__ (uint16_t *)0xB8000

#define __MAX_SCREEN_SUPPORTED__ (size_t)3

extern void terminal_initialize(void);
extern void terminal_update_screen(void);
extern void terminal_putchar(char c);
extern void terminal_writestring(const char *data);
extern void terminal_setcolor(uint8_t color);
extern void terminal_clear_screen(void);
extern void terminal_writestring_location(const char *data, size_t x, size_t y);
extern void terminal_back_once(void);
extern void update_cursor(int x, int y);
extern void terminal_insert_char(char c);
extern void terminal_write_n_char(char c, size_t count);

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

extern size_t terminal_row;
extern size_t terminal_column;
extern uint8_t terminal_color;
extern uint16_t *terminal_buffer;

#define UPDATE_CURSOR(void) update_cursor(terminal_column, terminal_row)
#define IS_CHAR(void) __terminal_cursor_is_char__()

#define TERMINAL_CURSOR_AT_LOCATION(x, y) get_terminal_index((size_t)y, (size_t)x)
#define TERMINAL_CHAR(x, y) *get_terminal_char((size_t)x, (size_t)y)

#define __TERMINAL_CURSOR_LOCATION__ get_terminal_index(terminal_row, terminal_column)

static inline int get_terminal_index(size_t row, size_t column)
{
    return (row * VGA_WIDTH + column);
}

static inline bool __terminal_cursor_is_char__(void)
{
    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    return ((bool)(terminal_buffer[index] == ' ' ? false : true));
}

static inline uint16_t *get_terminal_char(size_t column, size_t row)
{
    return &(terminal_buffer[get_terminal_index(row, column)]);
}

#endif /* TERMINAL_H */