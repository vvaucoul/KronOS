/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kshell_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/24 15:41:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 12:31:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/shell/kshell.h"

size_t kshell_get_last_character_index(void)
{
    // TODO Debug

    // char buffer[__KITOA_BUFFER_LENGTH__];
    // kmemset(buffer, 0, __KITOA_BUFFER_LENGTH__);
    // kitoa(0, buffer);
    // terminal_writestring_location("X: ", 60, 3);
    // terminal_writestring_location(buffer, 62, 3);
    // kmemset(buffer, 0, __KITOA_BUFFER_LENGTH__);
    // kitoa(terminal_row, buffer);
    // terminal_writestring_location("Y: ", 60, 4);
    // terminal_writestring_location(buffer, 62, 4);

    size_t i = VGA_WIDTH - 1;
    while (i > __PROMPT_ASCII_LEN__ && isprint(terminal_buffer[TERMINAL_CURSOR_AT_LOCATION(i, terminal_row)]) == true && terminal_buffer[TERMINAL_CURSOR_AT_LOCATION(i, terminal_row)] == ' ')
    {
        i--;
    }
    // kmemset(buffer, 0, __KITOA_BUFFER_LENGTH__);
    // kitoa(i, buffer);
    // terminal_writestring_location("I: ", 60, 5);
    // terminal_writestring_location(buffer, 62, 5);
    return (i);
}

static void kshell_move_offset_line_right(size_t x, size_t y, size_t offset)
{
    for (size_t i = VGA_WIDTH - 2; i > __PROMPT_ASCII_LEN__ && i > x; i--)
    {
        terminal_buffer[TERMINAL_CURSOR_AT_LOCATION(i, y)] = terminal_buffer[TERMINAL_CURSOR_AT_LOCATION(i - offset, y)];
    }
}

static void kshell_move_offset_line_left(size_t x, size_t y, size_t offset)
{
    for (size_t i = x; i < VGA_WIDTH - 1; i++)
    {
        terminal_buffer[TERMINAL_CURSOR_AT_LOCATION(i, y)] = terminal_buffer[TERMINAL_CURSOR_AT_LOCATION(i + offset, y)];
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