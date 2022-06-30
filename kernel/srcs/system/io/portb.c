/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portb.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:31:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/30 15:01:38 by vvaucoul         ###   ########.fr       */
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

uint8_t inb(uint32_t _port)
{
    uint8_t _v;

    asm volatile("inb %%dx, %%al"
                 : "=a"(_v)
                 : "d"(_port));
    return _v;
}

uint16_t inw(uint32_t _port)
{
    uint16_t _v;

    asm volatile("inw %%dx, %%ax"
                 : "=a"(_v)
                 : "d"(_port));
    return _v;
}

uint32_t inl(uint32_t _port)
{
    uint32_t _v;

    asm volatile("inl %%dx, %%eax"
                 : "=a"(_v)
                 : "d"(_port));
    return _v;
}

void outb(uint16_t _port, uint8_t _data)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(_data), "Nd"(_port));
}

void outw(uint16_t _port, uint16_t _data)
{
    asm volatile("outw %0, %1"
                 :
                 : "a"(_data), "Nd"(_port));
}

void outl(uint32_t _port, uint32_t _data)
{
    asm volatile("outl %%eax, %%dx"
                 :
                 : "d"(_port), "a"(_data));
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