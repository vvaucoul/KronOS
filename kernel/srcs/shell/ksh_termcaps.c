/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_termcaps.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 19:43:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/20 17:06:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh_termcaps.h>

/*******************************************************************************
 *                               CURSOR MOVEMENT                               *
 ******************************************************************************/

extern void ksh_move_cursor_up(void) {
	ksh_history_up();
	return;
	// if (ksh_current_line <= ksh_min_line)
	//     return;
	// tty_row--;
	// if (tty_row < __HEADER_HEIGHT__)
	//     tty_row = __HEADER_HEIGHT__;
	// tty_column = __PROMPT_ASCII_LEN__;
	// ksh_current_line--;
}

extern void ksh_move_cursor_down(void) {
	return;
	// if (ksh_current_line >= ksh_current_max_line)
	//     return;
	// tty_row++;
	// if (tty_row >= VGA_HEIGHT)
	//     tty_row = VGA_HEIGHT - 1;
	// tty_column = __PROMPT_ASCII_LEN__;
	// ksh_current_line++;
}

extern void ksh_move_cursor_left(void) {
	if (tty_column <= __PROMPT_ASCII_LEN__)
		return;
	else
		tty_column--;
	tty_refresh_cursor();
}

extern void ksh_move_cursor_right(void) {
	if (tty_column >= ksh_get_last_character_index() || tty_column - __PROMPT_ASCII_LEN__ >= ksh_line_index)
		return;
	else
		tty_column++;
	tty_refresh_cursor();
}

/*******************************************************************************
 *                            SPECIAL CHAR KEYBOARD                            *
 ******************************************************************************/

extern void ksh_del_one(void) {
	if (tty_column <= __PROMPT_ASCII_LEN__)
		return;
	else {
		tty_column--;
		ksh_del_char_location(tty_column, tty_row);
		ksh_buffer_delete_char_at(tty_column - __PROMPT_ASCII_LEN__);
		tty_refresh_cursor();
	}
}

extern void ksh_new_line(void) {
	tty_column = 0;
	if (tty_row >= VGA_HEIGHT - 1) {
		tty_move_cursor_down();
		tty_row = VGA_HEIGHT - 1;
		tty_column = 0;
	} else {
		tty_row++;
		if (ksh_current_line == ksh_current_max_line)
			ksh_current_max_line++;
		ksh_current_line++;
	}
	if (ksh_current_line == ksh_current_max_line)
		ksh_execute_command();
	else {
		tty_column = __PROMPT_ASCII_LEN__;
		tty_refresh_cursor();
	}
}

extern void ksh_suppr_char(void) {
	// TODO FIX SUPPR CHAR

	ksh_del_char_location(tty_column, tty_row);
	ksh_buffer_delete_char_at(tty_column - __PROMPT_ASCII_LEN__);
}

/*******************************************************************************
 *                                 WRITE CHAR                                  *
 ******************************************************************************/

extern void ksh_write_char(char c) {
	if (tty_column >= VGA_WIDTH - 1)
		return;
	else if (tty_column < __PROMPT_ASCII_LEN__)
		return;
	else {
		ksh_insert_char_location(c, tty_column, tty_row);
		ksh_insert_char_at(c, tty_column - __PROMPT_ASCII_LEN__ - 1);
	}
}
