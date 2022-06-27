/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kprintf.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 15:06:11 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 19:40:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/kprintf.h"

t_kprintf _g_kprintf;

static int check_colors(const char *str)
{
    if (kstrncmp(str, COLOR_END, kstrlen(COLOR_END)) == 0)
    {
        terminal_setcolor(VGA_COLOR_LIGHT_GREY);
        return (kstrlen(COLOR_END));
    }
    else if (kstrncmp(str, COLOR_RED, kstrlen(COLOR_RED)) == 0)
    {
        terminal_setcolor(VGA_COLOR_RED);
        return (kstrlen(COLOR_RED));
    }
    else if (kstrncmp(str, COLOR_GREEN, kstrlen(COLOR_GREEN)) == 0)
    {
        terminal_setcolor(VGA_COLOR_GREEN);
        return (kstrlen(COLOR_GREEN));
    }
    else if (kstrncmp(str, COLOR_BLUE, kstrlen(COLOR_BLUE)) == 0)
    {
        terminal_setcolor(VGA_COLOR_BLUE);
        return (kstrlen(COLOR_BLUE));
    }
    else if (kstrncmp(str, COLOR_YELLOW, kstrlen(COLOR_YELLOW)) == 0)
    {
        terminal_setcolor(VGA_COLOR_BROWN);
        return (kstrlen(COLOR_YELLOW));
    }
    else if (kstrncmp(str, COLOR_MAGENTA, kstrlen(COLOR_MAGENTA)) == 0)
    {
        terminal_setcolor(VGA_COLOR_MAGENTA);
        return (kstrlen(COLOR_MAGENTA));
    }
    else if (kstrncmp(str, COLOR_CYAN, kstrlen(COLOR_CYAN)) == 0)
    {
        terminal_setcolor(VGA_COLOR_CYAN);
        return (kstrlen(COLOR_CYAN));
    }
    
    return (0);
}

static int kprintf_loop(const char *format)
{
    size_t i = 0;
    int ret = 0;

    while (format[i])
    {
        if ((ret = (check_colors(format + i))) != 0)
        {
            i += ret;
            continue;
        }
        else
            ret = 0;
        if (format[i] == MOD_DEL)
        {
            i++;
            if (format[i] == MOD_DEL)
            {
                kputchar(MOD_DEL);
            }
            else if (format[i] == DEL_D || format[i] == DEL_C || format[i] == DEL_I)
            {
                int nbr = va_arg(_g_kprintf.args, int);
                kputnbr(nbr);
            }
            else if (format[i] == DEL_S)
            {
                char *str = va_arg(_g_kprintf.args, char *);
                kputstr(str);
            }
            else if (format[i] == DEL_P)
            {
                void *ptr = va_arg(_g_kprintf.args, void *);
                kputptr(ptr);
            }
            else if (format[i] == DEL_U)
            {
                unsigned int nbr = va_arg(_g_kprintf.args, unsigned int);
                kputnbr(nbr);
            }
            else if (format[i] == DEL_X)
            {
                unsigned int nbr = va_arg(_g_kprintf.args, unsigned int);
                kputnbr_hex(nbr);
            }
        }
        else
        {
            terminal_putchar(format[i]);
        }
        ++i;
    }
    return (0);
}

int kprintf(const char *format, ...)
{
    int ret;

    va_start(_g_kprintf.args, format);
    ret = kprintf_loop(format);
    va_end(_g_kprintf.args);
    return (ret);
}