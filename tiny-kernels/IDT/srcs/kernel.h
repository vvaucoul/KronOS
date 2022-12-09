/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/09 00:08:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 00:10:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

#define VGA_ADDRESS 0xB8000

extern void terminal_initialize(void);
extern void terminal_setcolor(uint8_t color);
extern void terminal_putentryat(char c, uint8_t color, uint32_t x, uint32_t y);
extern void terminal_putchar(char c);
extern void terminal_write(const char *data, uint32_t size);
extern void terminal_writestring(const char *data);
extern void terminal_clear_screen(void);

#endif /* !KERNEL_H */
