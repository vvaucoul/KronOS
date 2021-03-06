/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kprintf_manager.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/30 15:54:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/07/11 21:01:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kprintf.h>

size_t __kpf_manage_mod(const char *format, size_t i)
{
    __kpf_manage_space_front(1);
    --i;
    if (format[i + 1] != MOD_DEL)
        kputchar(MOD_DEL);
    ++i;
    __kpf_manage_space_back(1);
    return (i);
}

void __kpf_manage_char()
{
    char c = va_arg(_g_kprintf.args, int);

    __kpf_manage_space_front(1);
    kputchar(c);
    __kpf_manage_space_back(1);
}
void __kpf_manage_nbr()
{
    int nbr = va_arg(_g_kprintf.args, int);

    __kpf_manage_space_front(knbrlen(nbr));
    kputnbr(nbr);
    __kpf_manage_space_back(knbrlen(nbr));
}

void __kpf_manage_str()
{
    char *str = va_arg(_g_kprintf.args, char *);
    __kpf_manage_space_front(kstrlen(str));
    kputstr(str);
    __kpf_manage_space_back(kstrlen(str));
}

void __kpf_manage_ptr()
{
    void *ptr = va_arg(_g_kprintf.args, void *);
    __kpf_manage_space_front(__kptrlen(ptr));
    kputptr(ptr);
    __kpf_manage_space_back(__kptrlen(ptr));
}

void __kpf_manage_hexa()
{
    unsigned int nbr = va_arg(_g_kprintf.args, unsigned int);
    __kpf_manage_space_front(knbrlen(nbr));
    kputnbr_hex(nbr);
    __kpf_manage_space_back(knbrlen(nbr));
}

void __kpf_manage_unsigned()
{
    unsigned int nbr = va_arg(_g_kprintf.args, unsigned int);
    __kpf_manage_space_front(knbrlen(nbr));
    kputnbr(nbr);
    __kpf_manage_space_back(knbrlen(nbr));
}