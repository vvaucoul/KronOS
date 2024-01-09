/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serial.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/01 16:14:50 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERIAL_H
#define SERIAL_H

#include <kernel.h>
#include <system/io.h>

#define PORT_COM1 0x3f8

void serial_init(void);

void qemu_printf(const char *str, ...);

#endif /* !SERIALH_H */