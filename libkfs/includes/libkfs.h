/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libkfs.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 13:52:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/27 11:38:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _LIBKFS_H
#define _LIBKFS_H

#include "kprintf.h"
#include "../libs/stdbool/stdbool.h"
#include "../libs/ctypes/ctypes.h"
#include "../libs/string/string.h"
#include "../libs/stddef/stddef.h"
#include "../libs/macros/macros.h"

extern void kputs(const char *str);
extern void kuputs(const uint8_t *str);
extern int katoi(const char *str);
extern void kftoa(char *buffer, float value);

#define __KITOA_BUFFER_LENGTH__ 11
extern int kitoa(int nbr, char str[__KITOA_BUFFER_LENGTH__]);
extern int kitoa_base(int nbr, int base, char str[__KITOA_BUFFER_LENGTH__]);

extern void kputchar(char c);
extern void kputstr(const char *str);
extern void kputendl(const char *str);
extern void kputnbr(int n);
extern void kputnbr_base(int n, int base);
extern void kputunbr(uint32_t n);
extern void kputunbr_base(uint32_t n, uint32_t base);
extern void kputptr(void *ptr);
extern void kputf(const float value);

/*******************************************************************************
 *                                PUTNBR - HEX                                 *
 ******************************************************************************/

static inline void kputnbr_hex(int n)
{
    kputnbr_base(n, 16);
}

static inline void kputnbr_bin(int n)
{
    kputnbr_base(n, 2);
}

/*******************************************************************************
 *                            UNSIGNED PUTNBR - HEX                            *
 ******************************************************************************/

static inline void kputunbr_hex(unsigned int n)
{
    kputunbr_base(n, 16);
}

static inline void kputunbr_bin(unsigned int n)
{
    kputunbr_base(n, 2);
}

extern void khexdump(uint32_t addr, int limit);

#endif /* _LIBKFS_H */