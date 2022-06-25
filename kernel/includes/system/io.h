/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:32:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 11:27:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IO_H
# define IO_H

#include "../kernel.h"

extern unsigned char inportb(unsigned short _port);
extern void outportb(unsigned short _port, unsigned char _data);
extern void poweroff(void);
extern void halt();
extern void reboot();
extern void outb(uint16_t port, uint8_t val);
extern void outw(uint16_t port, uint16_t val);

// static inline void poweroff(void)
// {
//     outw(0x604, 0x2000);
// }

#endif