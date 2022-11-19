/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kprintf.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 15:07:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/18 15:16:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _KPRINTF_H
#define _KPRINTF_H

#include "../../kernel/includes/terminal.h"
#include "../libs/string/string.h"
#include "../libs/stdbool/stdbool.h"

#include <stdarg.h>

#define MOD_DEL 0x25
#define DEL_D 0x64
#define DEL_I 0x69
#define DEL_C 0x63
#define DEL_S 0x73
#define DEL_P 0x70
#define DEL_U 0x75
#define DEL_X 0x78
#define DEL_F 0X66

#define SPE_DEL_MIN 0x2d
#define SPE_DEL_ZERO 0x30

#define COLOR_END "\x1b[0m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"

#define _END COLOR_END
#define _RED COLOR_RED
#define _GREEN COLOR_GREEN
#define _YELLOW COLOR_YELLOW
#define _BLUE COLOR_BLUE
#define _MAGENTA COLOR_MAGENTA
#define _CYAN COLOR_CYAN

#define CURSOR_MOVE_UP "\x1b[1A"
#define CURSOR_MOVE_DOWN "\x1b[1B"
#define CURSOR_MOVE_RIGHT "\x1b[1C"
#define CURSOR_MOVE_LEFT "\x1b[1D"

typedef struct s_kprintf
{
    va_list args;

    int __space;
    bool __is_neg_space;
    bool __use_zero;
} t_kprintf;

extern t_kprintf _g_kprintf;

extern int kprintf(const char *format, ...);
extern void kvsprintf(const char *str, void (*write)(const char), const char *format, va_list arg);

extern size_t __kptrlen(const void *ptr);

extern size_t __kpf_manage_mod(const char *format, size_t i);
extern void __kpf_manage_char();
extern void __kpf_manage_nbr();
extern void __kpf_manage_str();
extern void __kpf_manage_ptr();
extern void __kpf_manage_hexa();
extern void __kpf_manage_unsigned();
extern void __kpf_manage_float();

extern void __kpf_manage_space_front(const int arg_len);
extern void __kpf_manage_space_back(const int arg_len);

#endif /* _KPRINTF_H */