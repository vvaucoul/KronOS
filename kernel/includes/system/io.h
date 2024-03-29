/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:32:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 22:34:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IO_H
#define IO_H

#include <kernel.h>

extern uint8_t inportb(uint16_t _port);
extern void outportb(uint16_t _port, uint8_t _data);
extern void outb(uint16_t _port, uint8_t _data);
extern void outw(uint16_t _port, uint16_t _data);
extern void outl(uint32_t _port, uint32_t _data);
extern uint8_t inb(uint32_t _port);
extern uint16_t inw(uint32_t _port);
extern uint32_t inl(uint32_t _port);

extern void poweroff(void);
extern void halt();
extern void reboot();

#endif /* !IO_H */