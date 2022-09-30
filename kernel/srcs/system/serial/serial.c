/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serial.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/01 16:14:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/29 10:57:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <system/serial.h>

static int __is_transmit_empty(void)
{
    return (inportb(PORT_COM1 + 5) & 0x20);
}

static void __write_serial(const char c)
{
    while (__is_transmit_empty() == false);
    outportb(PORT_COM1, c);
}

void qemu_printf(const char *str, ...)
{
    va_list(ap);
    va_start(ap, str);
    kvsprintf(str, __write_serial, str, ap);
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