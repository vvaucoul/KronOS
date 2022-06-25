/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kshell_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/24 15:41:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 15:37:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/shell/kshell.h"

size_t kshell_get_last_character_index(void)
{
    // TODO Debug

    size_t i = VGA_WIDTH - 1;
    while (i > __PROMPT_ASCII_LEN__ && isprint(TERMINAL_CHAR(terminal_column, terminal_row)) == true && TERMINAL_CHAR(terminal_column, terminal_row) == ' ')
    {
        i--;
    }
    return (i);
}

static void kshell_move_offset_line_right(size_t x, size_t y, size_t offset)
{
    for (size_t i = VGA_WIDTH - 2; i > __PROMPT_ASCII_LEN__ && i > x; i--)
    {
        TERMINAL_CHAR(i, y) = TERMINAL_CHAR(i - offset, y);
    }
}

static void kshell_move_offset_line_left(size_t x, size_t y, size_t offset)
{
    for (size_t i = x; i < VGA_WIDTH - 1; i++)
    {
        TERMINAL_CHAR(i, y) = TERMINAL_CHAR(i + offset, y);
    }
}

void kshell_del_char_location(size_t x, size_t y)
{
    kshell_move_offset_line_left(x, y, 1);
    UPDATE_CURSOR();
}

void kshell_insert_char_location(char c, size_t x, size_t y)
{
    kshell_move_offset_line_right(x, y, 1);
    kputchar(c);
    UPDATE_CURSOR();
}