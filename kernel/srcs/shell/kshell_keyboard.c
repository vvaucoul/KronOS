/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kshell_keyboard.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 22:55:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/27 13:11:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/shell/kshell.h"

void kshell_init(void)
{
    for (size_t y = 0; y < kshell_max_line; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = TERMINAL_CURSOR_AT_LOCATION(x, y);
            kshell_buffer[index] = VGA_ENTRY(' ', terminal_color);
        }
    }
}