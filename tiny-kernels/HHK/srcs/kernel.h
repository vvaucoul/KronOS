/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/19 19:26:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/19 20:02:52 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_H
#define KERNEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 *                                     VGA                                     *
 ******************************************************************************/

#define VGA_ADDRESS 0xC00B8000

extern void terminal_initialize(void);
extern void terminal_setcolor(uint8_t color);
extern void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
extern void terminal_putchar(char c);
extern void terminal_write(const char *data, size_t size);
extern void terminal_writestring(const char *data);
extern void terminal_clear_screen(void);

#endif /* !KERNEL_H */