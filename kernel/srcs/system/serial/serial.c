/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serial.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/01 16:14:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/28 10:13:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <system/serial.h>

#include <stdio.h>
#include <macros.h>

static int __is_transmit_empty(void)
{
    return (inportb(PORT_COM1 + 5) & 0x20);
}

__unused__ static void __write_serial(const char c)
{
    while (__is_transmit_empty() == false)
        ;
    outportb(PORT_COM1, c);
}

void qemu_printf(const char *str, ...)
{
    va_list(ap);
    va_start(ap, str);
    // vsprintk(str, str, ap);
    va_end(ap);
}

void serial_init(void)
{
    outportb(PORT_COM1 + 1, 0x00);
    outportb(PORT_COM1 + 3, 0x80);
    outportb(PORT_COM1 + 0, 0x03);
    outportb(PORT_COM1 + 1, 0x00);
    outportb(PORT_COM1 + 3, 0x03);
    outportb(PORT_COM1 + 2, 0xC7);
    outportb(PORT_COM1 + 4, 0x0B);
}