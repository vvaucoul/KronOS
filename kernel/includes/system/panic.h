/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   panic.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/09 12:26:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 14:04:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PANIC_H
#define PANIC_H

#include <kernel.h>
#include <system/kerrno.h>

#define __USE_KERRNO_HELPER__ false

#define __PANIC_HEADER__ (const char *)(_RED "KERNEL - PANIC: " _END)

/*******************************************************************************
 *                                PANIC MACROS                                 *
 ******************************************************************************/

static bool __panic_handler = false;
#define __DISPLAY_HEADER__()          \
    {                                 \
        if (__panic_handler == false) \
        {                             \
            __panic_handler = true;   \
            printk(__PANIC_HEADER__); \
        }                             \
    }

#define __PANIC_LOOP_HANDLER__() \
    {                            \
        while (1)                \
        {                        \
            ;                    \
        }                        \
    }

/*******************************************************************************
 *                               PANIC FUNCTIONS                               *
 ******************************************************************************/

extern void kernel_panic(const char *str);
extern void kernel_panic_multistr(const char *str[], size_t count);

/*******************************************************************************
 *                            PANIC EXTERN DEFINES                             *
 ******************************************************************************/

#define __kernel_panic(str) kernel_panic(str)
#define __PANIC(str) __kernel_panic(str)
#define __PANIC_MULTISTR(str, count) kernel_panic_multistr(str, count)

#endif /* PANIC_H */