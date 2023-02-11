/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   panic.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/09 12:26:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/11 22:30:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PANIC_H
#define PANIC_H

#include <kernel.h>
#include <system/kerrno.h>

typedef enum e_panic_type : uint8_t
{
    ABORT,
    FAULT,
    INTERRUPT,
    TRAP
} panic_t;

#define __USE_KERRNO_HELPER__ false

#define __PANIC_HEADER__ (const char *)(_RED "KERNEL "_END"-"_RED" PANIC: " _END)
#define __FAULT_HEADER__ (const char *)(_RED "KERNEL "_END"- "_YELLOW"FAULT: " _END)
#define __TRAP_HEADER__ (const char *)(_RED "KERNEL "_END"- "_YELLOW"TRAP: " _END)
#define __INTERRUPT_HEADER__ (const char *)(_RED "KERNEL "_END"- "_YELLOW"INTERRUPT: " _END)

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
extern __attribute__((no_caller_saved_registers)) void kernel_panic_interrupt(const char *str, uint32_t index, panic_t fault, uint32_t code);
extern void kernel_panic_multistr(const char *str[], size_t count);

/*******************************************************************************
 *                            PANIC EXTERN DEFINES                             *
 ******************************************************************************/

#define __kernel_panic(str) kernel_panic(str)
#define __PANIC(str) __kernel_panic(str)
#define __PANIC_MULTISTR(str, count) kernel_panic_multistr(str, count)

#define __PANIC_INTERRUPT(str, index, fault, code) kernel_panic_interrupt(str, index, fault, code)

#endif /* !PANIC_H */