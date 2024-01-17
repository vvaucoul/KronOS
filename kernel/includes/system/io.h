/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:32:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/17 10:42:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IO_H
#define IO_H

#include <kernel.h>

extern uint8_t inportb(uint16_t _port);
extern void outportb(uint16_t _port, uint8_t _data);
extern void outb(uint16_t _port, uint8_t _data);
extern void outw(uint16_t _port, uint16_t _data);
extern void outsw(uint16_t port, const void *addr, uint32_t word_cnt);
extern void outl(uint32_t _port, uint32_t _data);
extern uint8_t inb(uint32_t _port);
extern uint16_t inw(uint32_t _port);
extern void insw(uint16_t port, void *addr, uint32_t word_cnt);
extern uint32_t inl(uint32_t _port);
extern void insl(uint16_t port, void *addr, uint32_t dword_cnt);
extern void outdd(uint16_t _port, uint32_t _data);
extern uint32_t indd(uint16_t _port);

extern void poweroff(void);
extern void halt();
extern void reboot();

#endif /* !IO_H */