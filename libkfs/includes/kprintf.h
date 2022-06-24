/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kprintf.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 15:07:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/23 12:15:15 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KPRINTF_H
#define KPRINTF_H

#include "../../kernel/includes/terminal.h"
#include <stdarg.h>

#define MOD_DEL 0x25
#define DEL_D 0x64
#define DEL_I 0x69
#define DEL_C 0x63
#define DEL_S 0x73
#define DEL_P 0x70
#define DEL_U 0x75
#define DEL_X 0x78

#define COLOR_END "\x1b[0m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"

typedef struct s_kprintf
{
    va_list args;
} t_kprintf;

extern t_kprintf _g_kprintf;

int kprintf(const char *format, ...);

#endif // !KPRINTF_H