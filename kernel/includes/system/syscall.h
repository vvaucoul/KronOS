/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:30:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/10 21:34:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <kernel.h>

/*
All x86 syscall: https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md#x86-32_bit
*/

#define SYSCALL_SIZE 0x180 // 384 syscall
#define SYSCALL_IRQ 0x7F

typedef void sysfn_t;

typedef struct s_syscall
{
    uint32_t id;
    char *name;
    sysfn_t *function;
} syscall_t;

extern syscall_t __syscall[SYSCALL_SIZE];

extern void init_syscall(void);

/*******************************************************************************
 *                               SYSCALL MACROS                                *
 ******************************************************************************/

#define _syscall0(type, name)                 \
    type name(void)                           \
    {                                         \
        type __res;                           \
        __asm__ volatile("int $0x80"          \
                         : "=a"(__res)        \
                         : "0"(__NR_##name)); \
        if (__res >= 0)                       \
            return __res;                     \
        return -1;                            \
    }

#define _syscall(type, name, atype, a)                \
    type name(atype a)                                \
    {                                                 \
        type __res;                                   \
        __asm__ volatile("int $0x80"                  \
                         : "=a"(__res)                \
                         : "0"(__NR_##name), "b"(a)); \
        if (__res >= 0)                               \
            return __res;                             \
        return -1;                                    \
    }

/*******************************************************************************
 *                              ALL SYSCALL LIST                               *
 ******************************************************************************/

#define SYSCALL_RESTART 0x00
/*
** EAX: 0x00
** EBX: 0x00
** ECX: 0x00
** EDX: 0x00
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

#define SYSCALL_EXIT 0x01
#define __NR_exit 0x01

// volatile void exit(int error_code);
/*
** EAX: 0x01
** EBX: int error_code
** ECX: 0x00
** EDX: 0x00
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

#define SYSCALL_FORK 0x02
#define __NR_fork 0x02
/*
** EAX: 0x02
** EBX: 0x00
** ECX: 0x00
** EDX: 0x00
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

#define SYSCALL_READ 0x03
/*
** EAX: 0x03
** EBX: unsigned int fd
** ECX: char *buff
** EDX: unsigned int count
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

#define SYSCALL_WRITE 0x04
/*
** EAX: 0x04
** EBX: unsigned int fd
** ECX: const char *buff
** EDX: unsigned int count
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

#endif /* !SYSCALL_H */