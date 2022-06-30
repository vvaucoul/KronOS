/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_termcaps.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/24 15:20:57 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/29 19:45:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KSH_TERMCAPS_H
#define KSH_TERMCAPS_H

#include "ksh.h"

/*******************************************************************************
 *                              BUFFER MANAGEMENT                              *
 ******************************************************************************/

// TODO ! ksh Buffer before
// Stocker le buffer dans ksh buffer et afficher ksh buffer dans terminal buffer

extern void ksh_move_buffer_offset_down(size_t offset);

// static void ksh_copy_line(size_t y, size_t ny)
// {
//     for (size_t i = 0; i < VGA_WIDTH - 1; i++)
//     {
//         TERMINAL_CHAR(i, y) = TERMINAL_CHAR(i, ny);
//     }
// }

// static inline void ksh_move_offset_buffer_up(size_t offset)
// {
//     for (size_t y = __HEADER_HEIGHT__; y < VGA_HEIGHT; y++)
//     {
//         ksh_copy_line(y, y + offset);
//     }
// }

// static inline void ksh_move_offset_buffer_down(size_t offset)
// {
//     for (size_t y = __HEADER_HEIGHT__; y < VGA_HEIGHT; y++)
//     {
//         ksh_copy_line(y, y + offset);
//     }
// }

/*******************************************************************************
 *                               CURSOR MOVEMENT                               *
 ******************************************************************************/

extern void ksh_move_cursor_up(void);
extern void ksh_move_cursor_down(void);
extern void ksh_move_cursor_left(void);
extern void ksh_move_cursor_right(void);

/*******************************************************************************
 *                            SPECIAL CHAR KEYBOARD                            *
 ******************************************************************************/

extern void ksh_del_one(void);
extern void ksh_new_line(void);
extern void ksh_suppr_char(void);

/*******************************************************************************
 *                                 WRITE CHAR                                  *
 ******************************************************************************/

extern void ksh_write_char(char c);

#endif /* KSH_TERMCAPS_H */