/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_history.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 12:48:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/05 13:19:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh.h>

static uint8_t __current_history_line = 0;
static uint8_t __current_history_selection = 0;

char *ksh_history[__MAX_HISTORY_LENGTH__];

void ksh_add_line_history(const char *str)
{
    if (__current_history_line >= __MAX_HISTORY_LENGTH__)
        __current_history_line = __MAX_HISTORY_LENGTH__;
    ksh_history[__current_history_line] = (char *)str;
    kprintf("History[%d] = %s\n", __current_history_line, ksh_history[__current_history_line]);
    ++__current_history_line;
    __current_history_selection = __current_history_line;
}

void ksh_history_up(void)
{
    
    __current_history_selection--;
}

void ksh_history_down(void)
{
    __current_history_selection++;
}