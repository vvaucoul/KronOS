/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_termcaps.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 19:43:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/07 01:08:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh_termcaps.h>

/*******************************************************************************
 *                               CURSOR MOVEMENT                               *
 ******************************************************************************/

extern void ksh_move_cursor_up(void)
{
    ksh_history_up();
    return;
    // if (ksh_current_line <= ksh_min_line)
    //     return;
    // terminal_row--;
    // if (terminal_row < __HEADER_HEIGHT__)
    //     terminal_row = __HEADER_HEIGHT__;
    // terminal_column = __PROMPT_ASCII_LEN__;
    // ksh_current_line--;
}

extern void ksh_move_cursor_down(void)
{
    return;
    // if (ksh_current_line >= ksh_current_max_line)
    //     return;
    // terminal_row++;
    // if (terminal_row >= VGA_HEIGHT)
    //     terminal_row = VGA_HEIGHT - 1;
    // terminal_column = __PROMPT_ASCII_LEN__;
    // ksh_current_line++;
}

extern void ksh_move_cursor_left(void)
{
    if (terminal_column <= __PROMPT_ASCII_LEN__)
        return;
    else
        terminal_column--;
    UPDATE_CURSOR();
}

extern void ksh_move_cursor_right(void)
{
    if (terminal_column >= ksh_get_last_character_index() || terminal_column - __PROMPT_ASCII_LEN__ >= ksh_line_index)
        return;
    else
        terminal_column++;
    UPDATE_CURSOR();
}

/*******************************************************************************
 *                            SPECIAL CHAR KEYBOARD                            *
 ******************************************************************************/

extern void ksh_del_one(void)
{
    if (terminal_column <= __PROMPT_ASCII_LEN__)
        return;
    else
    {
        terminal_column--;
        ksh_del_char_location(terminal_column, terminal_row);
        ksh_buffer_delete_char_at(terminal_column - __PROMPT_ASCII_LEN__);
        UPDATE_CURSOR();
    }
}

extern void ksh_new_line(void)
{
    terminal_column = 0;
    if (terminal_row >= VGA_HEIGHT - 1)
    {
        terminal_move_offset_down();
        terminal_row = VGA_HEIGHT - 1;
        terminal_column = 0;
    }
    else
    {
        terminal_row++;
        if (ksh_current_line == ksh_current_max_line)
            ksh_current_max_line++;
        ksh_current_line++;
    }
    if (ksh_current_line == ksh_current_max_line)
        ksh_execute_command();
    else
    {
        terminal_column = __PROMPT_ASCII_LEN__;
        UPDATE_CURSOR();
    }
}

extern void ksh_suppr_char(void)
{
    // TODO FIX SUPPR CHAR

    ksh_del_char_location(terminal_column, terminal_row);
    ksh_buffer_delete_char_at(terminal_column - __PROMPT_ASCII_LEN__);
}

/*******************************************************************************
 *                                 WRITE CHAR                                  *
 ******************************************************************************/

extern void ksh_write_char(char c)
{
    if (terminal_column >= VGA_WIDTH - 1)
        return;
    else if (terminal_column < __PROMPT_ASCII_LEN__)
        return;
    else
    {
        ksh_insert_char_location(c, terminal_column, terminal_row);
        ksh_insert_char_at(c, terminal_column - __PROMPT_ASCII_LEN__ - 1);
    }
}
