/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kprintf_manager.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/30 15:54:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/30 16:48:54 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/kprintf.h"

void __kpf_manage_mod()
{
    kputchar(MOD_DEL);
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