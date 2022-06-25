/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kshell.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 14:40:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 15:23:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/shell/kshell.h"

size_t kshell_max_line = VGA_HEIGHT - __HEADER_HEIGHT__;
size_t kshell_min_line = __HEADER_HEIGHT__;
size_t kshell_current_line = __HEADER_HEIGHT__;
size_t kshell_current_max_line = __HEADER_HEIGHT__;

static inline void kshell_header(void)
{
    kprintf(COLOR_RED "\n \
   \t\t\t\t\t\t\t##   ###   ##  \n \
   \t\t\t\t\t\t\t ##  ###  ##   \n \
   \t\t\t\t\t\t\t  ## ### ##    \n \
   \t\t\t\t\t\t\t  ## ### ##    \n \
   \t\t\t\t\t\t\t  ## ### ##    \n \
   \t\t\t\t\t\t\t ##  ###  ##   \n \
   \t\t\t\t\t\t\t##   ###   ##  \n \
    \n" COLOR_END);
    kprintf(COLOR_RED);
    terminal_write_n_char('#', VGA_WIDTH);
    kprintf(COLOR_END);
    kprintf("\n");
}

void kronos_shell(void)
{
    kshell_header();
    DISPLAY_PROMPT();
    while (1)
    {
    }
}