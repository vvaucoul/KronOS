/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:32:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 23:33:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IO_H
# define IO_H

// #include "../kernel.h"
#include "../../../libkfs/libs/stddef/stddef.h"

extern unsigned char inportb(unsigned short _port);
extern void outportb(unsigned short _port, unsigned char _data);
extern void poweroff(void);
extern void halt();
extern void reboot();
extern void outb(uint16_t port, uint8_t val);
extern void outw(uint16_t port, uint16_t val);

#endif