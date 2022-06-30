/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_keyboard.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 22:55:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/30 12:27:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/shell/ksh.h"

void ksh_init(void)
{
    for (size_t y = 0; y < ksh_max_line; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = TERMINAL_CURSOR_AT_LOCATION(x, y);
            ksh_buffer[index] = VGA_ENTRY(' ', terminal_color);
        }
    }
}

void ksh_clear_terminal_shell_buffer(void)
{
    for (size_t y = __HEADER_HEIGHT__; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            TERMINAL_CHAR(x, y) = VGA_ENTRY(' ', terminal_color);
        }
    }
}

void ksh_update_terminal_shell_buffer(void)
{
    // size_t __terminal_column = 0;
    size_t __terminal_row = __HEADER_HEIGHT__;

    // size_t __ksh_column = 0;
    // size_t __ksh_row = 0;

    for (size_t y = __terminal_row; y < VGA_HEIGHT; y++)
    {
        DISPLAY_PROMPT();
        kprintf("%s\n", KSH_LINE(y));
    }
}

void ksh_move_buffer_offset_down(size_t offset)
{
    (void)offset;
    KSH_CLR_TERM_SH();
    kprintf("Update buffer down\n");
    // KSH_UPDATE_TERM_SH();
    // terminal_row = VGA_HEIGHT - 1;
    // terminal_column = __PROMPT_ASCII_LEN__;
    // UPDATE_CURSOR();
}