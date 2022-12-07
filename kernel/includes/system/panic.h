/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   panic.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/09 12:26:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/06 22:22:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PANIC_H
#define PANIC_H

#include <kernel.h>
#include <system/kerrno.h>

typedef enum e_panic_type
{
    ABORT,
    FAULT,
    INTERRUPT,
    TRAP
} panic_t;

#define __USE_KERRNO_HELPER__ false

#define __PANIC_HEADER__ (const char *)(_RED "KERNEL - PANIC: " _END)
#define __FAULT_HEADER__ (const char *)(_RED "KERNEL - FAULT: " _END)
#define __TRAP_HEADER__ (const char *)(_RED "KERNEL - TRAP: " _END)
#define __INTERRUPT_HEADER__ (const char *)(_RED "KERNEL - INTERRUPT: " _END)

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

#define __DISPLAY_HEADER_FAULT__() \
    {                              \
        printk(__FAULT_HEADER__);  \
    }

#define __DISPLAY_HEADER_TRAP__() \
    {                             \
        printk(__TRAP_HEADER__);  \
    }

#define __DISPLAY_HEADER_INTERRUPT__() \
    {                                  \
        printk(__INTERRUPT_HEADER__);  \
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
extern void kernel_fault(const char *str);
extern void kernel_trap(const char *str);
extern void kernel_interrupt(const char *str);
extern void kernel_panic_multistr(const char *str[], size_t count);

/*******************************************************************************
 *                            PANIC EXTERN DEFINES                             *
 ******************************************************************************/

#define __kernel_panic(str) kernel_panic(str)
#define __PANIC(str) __kernel_panic(str)
#define __PANIC_MULTISTR(str, count) kernel_panic_multistr(str, count)

#define __FAULT(str) kernel_fault(str)
#define __TRAP(str) kernel_trap(str)
#define __INTERRUPT(str) kernel_interrupt(str)

#endif /* PANIC_H */