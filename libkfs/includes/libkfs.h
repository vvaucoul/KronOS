/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libkfs.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 13:52:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/27 19:36:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _LIBKFS_H
#define _LIBKFS_H

#include "kprintf.h"

#include "../libs/stdbool/stdbool.h"
#include "../libs/ctypes/ctypes.h"
#include "../libs/string/string.h"
#include "../libs/stddef/stddef.h"

extern void *kmemset(void *ptr, int value, size_t length);
extern void *kbzero(void *ptr, size_t len);
extern void *kmemcpy(void *dst, const void *src, size_t length);
extern void *kmemmove(void *dst, const void *src, size_t length);

extern void kputs(const char *str);
extern void kuputs(const unsigned char *str);
extern int katoi(const char *str);

#define __KITOA_BUFFER_LENGTH__ 11
extern int kitoa(int nbr, char str[__KITOA_BUFFER_LENGTH__]);
extern int kitoa_base(int nbr, int base, char str[__KITOA_BUFFER_LENGTH__]);

extern void kputchar(char c);
extern void kputstr(const char *str);
extern void kputendl(const char *str);
extern void kputnbr(int n);
extern void kputnbr_base(int n, int base);
extern void kputunbr(unsigned int n);
extern void kputptr(void *ptr);

static inline void kputnbr_hex(int n)
{
    kputnbr_base(n, 16);
}

static inline void kputnbr_bin(int n)
{
    kputnbr_base(n, 2);
}

#endif /* _LIBKFS_H */