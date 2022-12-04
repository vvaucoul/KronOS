/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_history.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 12:48:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 14:02:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh.h>
#include <shell/ksh_termcaps.h>

static uint8_t __current_history_line = 0; /* Current history line */
static uint8_t __current_history_selection = 0; /* Current history selection */

char *ksh_history[__MAX_HISTORY_LENGTH__] = {0};

void ksh_add_line_history(const char *str)
{
    return ;
    if (__current_history_line >= __MAX_HISTORY_LENGTH__)
        __current_history_line = __MAX_HISTORY_LENGTH__;
    memcpy(ksh_history[__current_history_line], str, strlen(str));
    printk("History[%d] = %s\n", __current_history_line, ksh_history[__current_history_line]);
    ++__current_history_line;
    __current_history_selection = __current_history_line;
}

void ksh_history_up(void)
{
    return ;
    __current_history_selection = __current_history_selection - 1;
    if (__current_history_selection > 0)
    {
        const uint32_t __len = strlen(ksh_history[__current_history_selection]);
        for (uint32_t i = 0; i < __len; i++)
        {
            ksh_write_char((unsigned char)(ksh_history[__current_history_selection][i]));
        }
    }
}

void ksh_history_down(void)
{
    __current_history_selection++;
}