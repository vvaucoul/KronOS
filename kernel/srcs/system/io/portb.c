/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portb.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:31:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/io.h>

uint8_t inportb(uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__("inb %1, %0"
                         : "=a"(rv)
                         : "dN"(_port));
    return rv;
}

void outportb(uint16_t _port, uint8_t _data) {
    __asm__ __volatile__("outb %1, %0"
                         :
                         : "dN"(_port), "a"(_data));
}

uint8_t inb(uint32_t _port) {
    uint8_t _v;

    __asm__ volatile("inb %%dx, %%al"
                     : "=a"(_v)
                     : "d"(_port));
    return _v;
}

uint16_t inw(uint32_t _port) {
    uint16_t _v;

    __asm__ volatile("inw %%dx, %%ax"
                     : "=a"(_v)
                     : "d"(_port));
    return _v;
}

uint32_t inl(uint32_t _port) {
    uint32_t _v;

    __asm__ volatile("inl %%dx, %%eax"
                     : "=a"(_v)
                     : "d"(_port));
    return _v;
}

void outb(uint16_t _port, uint8_t _data) {
    __asm__ volatile("outb %0, %1"
                     :
                     : "a"(_data), "Nd"(_port));
}

void outw(uint16_t _port, uint16_t _data) {
    __asm__ volatile("outw %0, %1"
                     :
                     : "a"(_data), "Nd"(_port));
}

void outl(uint32_t _port, uint32_t _data) {
    __asm__ volatile("outl %%eax, %%dx"
                     :
                     : "d"(_port), "a"(_data));
}

void halt() {
loop:
    __asm__ __volatile__("hlt");
    goto loop;
}

void reboot() {
    uint8_t rebootTemp = 0x02;
    while (rebootTemp & 0x02)
        rebootTemp = inportb(0x64);
    outportb(0x64, 0xFE);
    halt();
}