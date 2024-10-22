/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tty.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 22:07:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/21 14:52:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TTY_H
#define TTY_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
// #define VGA_MEMORY (uint16_t *)0xB8000
#define VGA_MEMORY (uint16_t *)0xC00B8000;

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

extern uint32_t tty_row;
extern uint32_t tty_column;
extern uint8_t tty_color;
extern uint16_t *tty_buffer;

extern void tty_move_cursor(uint32_t x, uint32_t y);
extern void tty_putentryat(char c, uint8_t color, uint32_t x, uint32_t y);
extern void tty_putchar(char c);
extern void tty_write(const char *data, uint32_t size);
extern void tty_writestring(const char *data);
extern void tty_setcolor(uint8_t color);
extern void tty_set_background_color(uint8_t color);
extern void tty_clear_screen();
extern void tty_init(void);
extern void tty_move_cursor_up(void);
extern void tty_move_cursor_down(void);
extern void tty_move_cursor_left(void);
extern void tty_move_cursor_right(void);
extern void tty_refresh_cursor(void);

extern uint16_t tty_get_char(uint32_t x, uint32_t y);
void tty_set_char(uint32_t x, uint32_t y, uint16_t c);
extern void tty_enable_cursor(void);
extern void tty_disable_cursor(void);

#endif /* TTY_H */