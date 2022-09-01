/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serial.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/01 16:14:50 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/01 16:17:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERIAL_H
#define SERIAL_H

#include <kernel.h>
#include <system/io.h>

#define PORT_COM1 0x3f8

void serial_init(void);

void qemu_printf(const char *str, ...);

#endif /* SERIALH_H */