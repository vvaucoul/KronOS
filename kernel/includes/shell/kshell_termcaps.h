/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kshell_termcaps.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/24 15:20:57 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 12:31:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KSHELL_TERMCAPS_H
#define KSHELL_TERMCAPS_H

#include "kshell.h"

/*******************************************************************************
 *                               CURSOR MOVEMENT                               *
 ******************************************************************************/

static inline void kshell_move_cursor_up(void)
{
    terminal_row--;
    if (terminal_row < __HEADER_HEIGHT__)
        terminal_row = __HEADER_HEIGHT__;
    terminal_column = __PROMPT_ASCII_LEN__;
}

static inline void kshell_move_cursor_down(void)
{
    terminal_row++;
    if (terminal_row >= VGA_HEIGHT)
        terminal_row = VGA_HEIGHT - 1;
    terminal_column = __PROMPT_ASCII_LEN__;
}

static inline void kshell_move_cursor_left(void)
{
    if (terminal_column <= __PROMPT_ASCII_LEN__)
        return;
    else
        terminal_column--;
}

static inline void kshell_move_cursor_right(void)
{
    if (terminal_column >= kshell_get_last_character_index())
        return;
    else
        terminal_column++;
}

static inline void kshell_del_one(void)
{
    if (terminal_column <= __PROMPT_ASCII_LEN__)
        return;
    else
    {
        terminal_column--;
        kshell_del_char_location(terminal_column, terminal_row);
        UPDATE_CURSOR();
    }
}

static inline void kshell_new_line(void)
{
    terminal_column = 0;
    terminal_row++;
    DISPLAY_PROMPT();
    terminal_column = __PROMPT_ASCII_LEN__;
    UPDATE_CURSOR();
}

static inline void kshell_write_char(char c)
{
    if (terminal_column >= VGA_WIDTH - 1)
        return;
    else if (terminal_column < __PROMPT_ASCII_LEN__)
        return;
    else
    {
        if (IS_CHAR() == true)
            kshell_insert_char_location(c, terminal_column, terminal_row);
        else
            kputchar(c);
    }
}

static inline void kshell_suppr_char(void)
{
    kshell_del_char_location(terminal_column, terminal_row);
}

#endif // !KSHELL_TERMCAPS_H