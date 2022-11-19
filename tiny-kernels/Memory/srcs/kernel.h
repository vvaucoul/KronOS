/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/19 19:26:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/04 17:55:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_H
#define KERNEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern uint32_t _start;
extern uint32_t _end;

extern int kmain(uint32_t magic_number, uint32_t *multiboot_info);

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

/*******************************************************************************
 *                                     LIB                                     *
 ******************************************************************************/

extern void *memset(void *ptr, uint32_t value, uint32_t size);
extern void *bzero(void *ptr, uint32_t size);

extern void kernel_panic(const char *str);

extern int printk(const char *str, ...);

#endif /* !KERNEL_H */