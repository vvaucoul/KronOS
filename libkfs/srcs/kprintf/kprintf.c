/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kprintf.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 15:06:11 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/03 21:45:32 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kprintf.h>

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

static int kprint_mod(const char *format, size_t i)
{

    /* CHECK SPECIAL DELIMITERS */

    // SPACES
    if (format[i] == SPE_DEL_MIN)
    {
        _g_kprintf.__is_neg_space = true;
        ++i;
    }
    else
        _g_kprintf.__is_neg_space = false;
    int nbr = 0;
    while (isdigit(format[i]))
    {
        nbr = nbr * 10 + format[i] - 0x30;
        i++;
    }
    _g_kprintf.__space = nbr;

    /* CHECK BASIC DELIMITERS */

    if (format[i] == MOD_DEL)
        i = __kpf_manage_mod(format, i);
    else if (format[i] == DEL_C)
        __kpf_manage_char();
    else if (format[i] == DEL_D || format[i] == DEL_I)
        __kpf_manage_nbr();
    else if (format[i] == DEL_S)
        __kpf_manage_str();
    else if (format[i] == DEL_P)
        __kpf_manage_ptr();
    else if (format[i] == DEL_U)
        __kpf_manage_unsigned();
    else if (format[i] == DEL_X)
        __kpf_manage_hexa();
    return (i);
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
            i = kprint_mod(format, i);
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

    _g_kprintf.__is_neg_space = false;
    _g_kprintf.__space = 0;

    va_start(_g_kprintf.args, format);
    ret = kprintf_loop(format);
    va_end(_g_kprintf.args);
    return (ret);
}