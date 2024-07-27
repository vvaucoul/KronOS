/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 14:40:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/26 22:49:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KSH_H
#define KSH_H

#include <kernel.h>
#include <drivers/vga.h>

#include <system/pit.h>

#include <string.h>

#include "ksh_buffer.h"
#include "ksh_builtins.h"

#define __PROMPT__ _END "KSH" _GREEN " $> " _END
#define __PROMPT_LEN__ (sizeof(__PROMPT__) - 1)
#define __PROMPT_ASCII_LEN__ (size_t)(7)

#define __HEADER_WIDTH__ (size_t)(VGA_WIDTH)
#define __HEADER_HEIGHT__ (size_t)(11)

#define DISPLAY_PROMPT() printk(__PROMPT__)
#define KSH_CHAR(x, y) ksh_buffer[(y)*VGA_WIDTH + (x)]
#define KSH_LINE(y) (ksh_buffer + (y)*VGA_WIDTH)

#define KSH_CLR_TERM_SH() ksh_clear_terminal_shell_buffer()
#define KSH_UPDATE_TERM_SH() ksh_update_terminal_shell_buffer()

extern void kronos_shell(void);

extern uint32_t *ksh_buffer;

extern uint32_t ksh_max_line;         // Max line of the shell
extern uint32_t ksh_current_max_line; // Current max line of the shell
extern uint32_t ksh_min_line;         // Min line of the shell
extern uint32_t ksh_current_line;     // Current line of the shell

/*******************************************************************************
 *                                   HISTORY                                   *
 ******************************************************************************/

#define __MAX_HISTORY_LENGTH__ 16
extern char *ksh_history[__MAX_HISTORY_LENGTH__];

extern void ksh_add_line_history(const char *str);
extern void ksh_history_up(void);
extern void ksh_history_down(void);

/*******************************************************************************
 *                                 GET - LINE                                  *
 ******************************************************************************/

extern void ksh_execute_command(void);

/*******************************************************************************
 *                                    UTILS                                    *
 ******************************************************************************/

extern bool ksh_is_running(void);

extern void ksh_init(void);
extern void ksh_clear_terminal_shell_buffer(void);

extern size_t ksh_get_last_character_index(void);
extern void ksh_del_char_location(size_t x, size_t y);
extern void ksh_insert_char_location(char c, size_t x, size_t y);
extern void ksh_save_line(size_t y);

extern void ksh_clear(void);

#endif /* !KSH_H */