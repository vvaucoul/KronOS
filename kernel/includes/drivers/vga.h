/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vga.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 22:07:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/28 11:31:19 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VGA_H
#define VGA_H

/**
 * @file vga_driver.h
 * @brief VGA driver for controlling the video graphics array display.
 *
 * This file provides functions for initializing and controlling the VGA display.
 * The VGA display is a hardware text mode display that is used to output text to the
 * screen. The VGA display is a 80x25 character display with 16 colors.
 *
 * @note This driver is specific to the VGA display and may not work with other types
 * of displays.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <system/io.h>

/* Hardware text mode color constants. */
enum vga_color {
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

#define VGA_ENTRY(uc, color) (((unsigned char)uc) | ((uint8_t)color) << 8)
#define VGA_ENTRY_COLOR(fg, bg) (((enum vga_color)fg) | ((enum vga_color)bg) << 4)
#define __VGA_MEMORY ((uint16_t *)(0x000B8000))

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* CRTC (Cathode Ray Tube Controller) Defines */
#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA  0x3D5

#define VGA_CURSOR_START_REG 0x0A
#define VGA_CURSOR_END_REG   0x0B

extern void vga_init(void);
extern void terminal_putchar(char c);
extern void terminal_writestring(const char *data);
extern void terminal_set_color(uint8_t color);
extern void terminal_writestring_location(const char *data, size_t x, size_t y);
extern void update_cursor(int x, int y);
extern void refresh_cursor(void);
extern void terminal_write_n_char(char c, size_t count);
extern void terminal_move_offset_down(void);
extern void terminal_set_background_color(uint8_t color);

extern void vga_enable_cursor(void);
extern void vga_disable_cursor(void);

extern size_t terminal_row;
extern size_t terminal_column;
extern uint16_t *terminal_buffer;

extern uint8_t __vga_foreground_color;
extern uint8_t __vga_background_color;

#define TERMINAL_CHAR(x, y) (terminal_buffer[(y) * VGA_WIDTH + (x)])

static inline void terminal_putentryat(char c, size_t x, size_t y) {
    uint8_t color = VGA_ENTRY_COLOR(__vga_foreground_color, __vga_background_color);
    TERMINAL_CHAR(x, y) = VGA_ENTRY(c, color);
    update_cursor(terminal_column, terminal_row);
}

static inline void terminal_clear_screen(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', x, y);
        }
    }
    terminal_column = 0;
    terminal_row = 0;
    update_cursor(terminal_column, terminal_row);
}

static inline void terminal_move_cursor_left(void) {
    if (terminal_column > 0) {
        terminal_column--;
    }
    update_cursor(terminal_column, terminal_row);
}

static inline void terminal_move_cursor_right(void) {
    if (terminal_column < VGA_WIDTH) {
        terminal_column++;
    }
    update_cursor(terminal_column, terminal_row);
}

static inline void terminal_move_cursor_up(void) {
    if (terminal_row > 0) {
        terminal_row--;
    }
    update_cursor(terminal_column, terminal_row);
}

static inline void terminal_move_cursor_down(void) {
    if (terminal_row < VGA_HEIGHT) {
        terminal_row++;
    }
    update_cursor(terminal_column, terminal_row);
}

#endif /* VGA_H */