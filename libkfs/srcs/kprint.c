/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kprint.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 12:19:39 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/03 21:56:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libkfs.h>

void kputchar(char c)
{
    terminal_putchar(c);
}

void kputstr(const char *str)
{
    while (*str)
    {
        kputchar(*str);
        ++str;
    }
}

void kputendl(const char *str)
{
    kputstr(str);
    kputchar('\n');
}

void kputnbr(int n)
{
    if (n < 0)
    {
        kputchar('-');
        n = -n;
    }
    if (n >= 10)
        kputnbr(n / 10);
    kputchar(n % 10 + '0');
}

void kputnbr_base(int n, int base)
{
    char s_base[18] = "0123456789ABCDEF";

    if (n < 0)
    {
        kputchar('-');
        n = -n;
    }
    if (n >= base)
        kputnbr_base(n / base, base);
    kputchar(s_base[n % base]);
}

void kputunbr_base(uint32_t n, uint32_t base)
{
    char s_base[18] = "0123456789ABCDEF";

    if (n >= base)
        kputnbr_base(n / base, base);
    kputchar(s_base[n % base]);
}

void kputunbr(uint32_t n)
{
    if (n >= 10)
        kputunbr(n / 10);
    kputchar(n % 10 + '0');
}

void kputptr(void *ptr)
{
    kputstr("0x");
    kputunbr_hex((uint32_t)ptr);
}

void kuputs(const uint8_t *str)
{
    for (size_t i = 0; str[i]; i++)
        terminal_putchar(str[i]);
    terminal_putchar(CHAR_NEWLINE);
}

void kputs(const char *str)
{
    for (size_t i = 0; str[i]; i++)
        terminal_putchar(str[i]);
    terminal_putchar(CHAR_NEWLINE);
}