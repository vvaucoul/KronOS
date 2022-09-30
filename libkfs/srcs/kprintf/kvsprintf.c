/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kvsprintf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 10:44:27 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/29 10:58:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kprintf.h>

static void __kvsprintf(const char *str, void (*write)(const char), const char *format, uint32_t *pos, va_list arg)
{
    for (size_t i = 0; str[i]; i++)
    {
        if (str[i] == '%')
        {
            i++;
            if (str[i] == 's')
            {
                const char *s = va_arg(arg, const char *);
                __kvsprintf(s, write, format, pos, arg);
            }
            else if (str[i] == 'd')
            {
                char buffer[__KITOA_BUFFER_LENGTH__];
                int d = va_arg(arg, int);
                kitoa(d, buffer);
                char *s = buffer;
                __kvsprintf(s, write, format, pos, arg);
            }
            else if (str[i] == 'c')
            {
                char c = va_arg(arg, int);
                write(c);
            }
            else if (str[i] == 'x')
            {
                char buffer[__KITOA_BUFFER_LENGTH__];
                int d = va_arg(arg, int);
                kitoa_base(d, 16, buffer);
                char *s = buffer;
                __kvsprintf(s, write, format, pos, arg);
            }
            else if (str[i] == 'p')
            {
                char buffer[__KITOA_BUFFER_LENGTH__];
                int d = va_arg(arg, int);
                kitoa_base(d, 16, buffer);
                char *s = buffer;
                __kvsprintf(s, write, format, pos, arg);
            }
            else if (str[i] == 'u')
            {
                char buffer[__KITOA_BUFFER_LENGTH__];
                int d = va_arg(arg, uint32_t);
                kitoa_base(d, 10, buffer);
                char *s = buffer;
                __kvsprintf(s, write, format, pos, arg);
            }
            else if (str[i] == '%')
            {
                write('%');
            }
        }
        else
        {
            write(str[i]);
        }
    }
}

void kvsprintf(const char *str, void (*write)(const char), const char *format, va_list arg)
{
    uint32_t pos = 0;
    __kvsprintf(str, write, format, &pos, arg);
}