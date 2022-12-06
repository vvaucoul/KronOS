/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lib.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 15:43:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/04 17:58:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"
#include <stdarg.h>

void *memset(void *ptr, uint32_t value, uint32_t size)
{
    unsigned char *tmp = (unsigned char *)ptr;

    printk("memset: ptr: 0x%x, value: %d, size: %u\n", ptr, value, size);
    while (size--)
        *(tmp++) = (unsigned char)value;
    return (ptr);
}

void *bzero(void *ptr, uint32_t size)
{
    unsigned char *tmp = ptr;

    while (size)
    {
        *(tmp++) = 0;
        --size;
    }
    return (ptr);
}

void kernel_panic(const char *str)
{
    terminal_writestring("KERNEL PANIC: ");
    terminal_writestring(str);
    terminal_writestring("\n");
    while (1)
        ;
    asm volatile("cli");
    while (1)
        ;
}

char *itoa_base(unsigned int value, int base)
{
    static char buf[32] = {0};

    int i = 30;
    for (uint32_t j = 0; j < 8; j++)
    {
        buf[j] = '0';
    }
    for (; value && i; --i, value /= base)
        buf[i] = "0123456789abcdef"[value % base];
    return (&buf[i + 1]);
}

char *itoa(int value)
{
    static char buf[32] = {0};

    int i = 30;
    for (; value && i; --i, value /= 10)
        buf[i] = "0123456789"[value % 10];
    return (&buf[i + 1]);
}

int printk(const char *str, ...)
{
    va_list args;
 
    va_start(args, str);
    for (int i = 0; str[i] != '\0'; i++)
    {
        switch (str[i])
        {
        case '%':
            switch (str[i + 1])
            {
            case 'd':
                terminal_writestring(itoa(va_arg(args, int)));
                i += 2;
                break;
            case 'u':
                terminal_writestring(itoa(va_arg(args, unsigned int)));
                i += 2;
                break;
            case 's':
                terminal_writestring(va_arg(args, char *));
                i += 2;
                break;
            case 'c':
                terminal_putchar(va_arg(args, int));
                i += 2;
                break;
            case 'x':
                terminal_writestring(itoa_base(va_arg(args, unsigned int), 16));
                i += 2;
                break;
            case '%':
                terminal_putchar('%');
                i += 2;
                break;
            default:
                terminal_putchar('%');
                break;
            }
            break;
        }
        terminal_putchar(str[i]);
    }
    va_end(args);
    return (0);
}