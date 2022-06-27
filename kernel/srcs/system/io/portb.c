/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portb.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:31:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 23:33:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/system/io.h"

unsigned char inportb(unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__("inb %1, %0"
                         : "=a"(rv)
                         : "dN"(_port));
    return rv;
}

void outportb(unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__("outb %1, %0"
                         :
                         : "dN"(_port), "a"(_data));
}

void outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

void outw(uint16_t port, uint16_t val)
{
    asm volatile("outw %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

void halt()
{
loop:
    __asm__ __volatile__("hlt");
    goto loop;
}

void reboot()
{
    uint8_t rebootTemp = 0x02;
    while (rebootTemp & 0x02)
        rebootTemp = inportb(0x64);
    outportb(0x64, 0xFE);
    halt();
}