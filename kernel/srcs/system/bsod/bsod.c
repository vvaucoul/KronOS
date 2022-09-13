/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsod.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 20:57:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/11 21:20:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/bsod.h>

uint16_t *g_bsod_buffer = BSOD_MEMORY;

void bsod(char *msg)
{
    g_bsod_buffer[0] = BSOD_VGA_ENTRY('c', VGA_COLOR_WHITE);
    g_bsod_buffer[1] = BSOD_VGA_ENTRY('r', VGA_COLOR_WHITE);

    for (int i = 0; msg[i]; i++)
    {
        g_bsod_buffer[i] = BSOD_VGA_ENTRY(msg[i], VGA_COLOR_WHITE);
    }
}
