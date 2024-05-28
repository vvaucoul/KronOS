/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portb.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:31:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/28 14:27:11 by vvaucoul         ###   ########.fr       */
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

void insw(uint16_t port, void *addr, uint32_t word_cnt) {
    __asm__ volatile("rep insw" : "+D"(addr), "+c"(word_cnt) : "d"(port) : "memory");
}

uint32_t inl(uint32_t _port) {
    uint32_t _v;

    __asm__ volatile("inl %%dx, %%eax"
                     : "=a"(_v)
                     : "d"(_port));
    return _v;
}

void insl(uint16_t port, void *addr, uint32_t dword_cnt) {
    __asm__ volatile("rep insl"
                     : "+D"(addr), "+c"(dword_cnt)
                     : "d"(port)
                     : "memory");
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

void outsw(uint16_t port, const void *addr, uint32_t word_cnt) {
    __asm__ volatile("rep outsw" : "+S"(addr), "+c"(word_cnt) : "d"(port));
}

void outl(uint32_t _port, uint32_t _data) {
    __asm__ volatile("outl %%eax, %%dx"
                     :
                     : "d"(_port), "a"(_data));
}

void outdd(uint16_t _port, uint32_t _data) {
    __asm__ volatile("outl %0, %1"
                     :
                     : "a"(_data), "Nd"(_port));
}

uint32_t indd(uint16_t _port) {
    uint32_t _v;

    __asm__ volatile("inl %%dx, %%eax"
                     : "=a"(_v)
                     : "d"(_port));
    return _v;
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