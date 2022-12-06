/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kprintf_special_manager.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/30 15:59:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/18 15:13:19 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kprintf.h>

void __kpf_manage_space_front(const int arg_len)
{
    if (_g_kprintf.__space - arg_len > 0)
    {
        if (_g_kprintf.__is_neg_space == false)
        {
            for (int i = 0; i < _g_kprintf.__space - arg_len; i++)
            {
                if (_g_kprintf.__use_zero == true)
                    kputchar('0');
                else
                    kputchar(' ');
            }
            _g_kprintf.__space = 0;
        }
    }
}

void __kpf_manage_space_back(const int arg_len)
{
    if (_g_kprintf.__space - arg_len > 0)
    {
        if (_g_kprintf.__is_neg_space == true)
        {
            for (int i = 0; i < _g_kprintf.__space - arg_len; i++)
            {
                if (_g_kprintf.__use_zero == true)
                    kputchar('0');
                else
                    kputchar(' ');
            }
            _g_kprintf.__space = 0;
        }
    }
}
