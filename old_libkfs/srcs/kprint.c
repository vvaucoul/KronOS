/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kprint.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 12:19:39 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/18 15:29:41 by vvaucoul         ###   ########.fr       */
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

/* ----------- */

void kftoa(char *buffer, float value)
{
    uint32_t count = 1;
    const uint32_t DEFAULT_DECIMAL_COUNT = 0;
    char int_part_buffer[16];
    char *p;

    kbzero(int_part_buffer, 16);

    int x = (int)value;
    kitoa(x, int_part_buffer);
    p = int_part_buffer;
    while (*p)
        *buffer++ = *p++;
    *buffer++ = '.';


    float decimal = value - x;
    if (decimal == 0)
        *buffer++ = '0';
    else
    {
        while (decimal > 0)
        {
            uint32_t y = decimal * 10;
            *buffer++ = y + '0';
            decimal = (decimal * 10) - y;
            count++;
            if (count == DEFAULT_DECIMAL_COUNT)
                break;
        }
    }
}

void kputf(const float value)
{
    char buffer[32];
    kbzero(buffer, 32);    
    kftoa(buffer, value);
    kputstr(buffer);
}