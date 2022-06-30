/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_buffer.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 19:18:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/29 20:16:06 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/shell/ksh_buffer.h"

char ksh_line_buffer[__KSH_INPUT_BUFFER_SIZE__] = {0};
size_t ksh_line_index = 0;

extern void ksh_buffer_add_char(char c)
{
    ksh_line_buffer[ksh_line_index] = c;
    ksh_line_buffer[ksh_line_index + 1] = CHAR_ENDLINE;
    ksh_line_index++;
}

extern void ksh_update_buffer(const char *str, size_t length)
{
    kbzero(ksh_line_buffer, __KSH_INPUT_BUFFER_SIZE__);
    
    if (length > __KSH_INPUT_BUFFER_SIZE__)
        length = __KSH_INPUT_BUFFER_SIZE__;
    for (size_t i = 0; i < length; i++)
    {
        ksh_line_buffer[i] = str[i];
    }
}

extern void ksh_insert_char_at(char c, size_t index)
{
    for (size_t i = ksh_line_index; i > index; i--)
    {
        ksh_line_buffer[i] = ksh_line_buffer[i - 1];
    }
    ksh_line_buffer[index] = c;
    ksh_line_index++;
}

extern void ksh_buffer_delete_char_at(size_t index)
{
    for (size_t i = index; i < ksh_line_index; i++)
    {
        ksh_line_buffer[i] = ksh_line_buffer[i + 1];
    }
    ksh_line_index--;
}

extern void ksh_buffer_delete_last_char(void)
{
    if (ksh_line_index > 0 && ksh_line_index < __KSH_INPUT_BUFFER_SIZE__)
    {
        ksh_line_buffer[ksh_line_index] = CHAR_ENDLINE;
        ksh_line_index--;
    }
}

extern void ksh_buffer_clear(void)
{
    kbzero(ksh_line_buffer, __KSH_INPUT_BUFFER_SIZE__);
    ksh_line_index = 0;
}

extern void ksh_buffer_init(void)
{
    kbzero(ksh_line_buffer, __KSH_INPUT_BUFFER_SIZE__);
    ksh_line_index = 0;
}