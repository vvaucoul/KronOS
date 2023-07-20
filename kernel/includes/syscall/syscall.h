/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:30:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 13:31:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <kernel.h>

#include <syscall/exit.h>
#include <syscall/fork.h>
#include <syscall/kill.h>
#include <syscall/wait.h>

/*
All x86 syscall: https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md#x86-32_bit
*/

#define SYSCALL_SIZE 0x180 // 384 syscall
#define SYSCALL_IRQ 0x7F

typedef void sysfn_t;

typedef struct s_syscall {
    uint32_t id;
    char *name;
    sysfn_t *function;
} syscall_t;

extern syscall_t __syscall[SYSCALL_SIZE];

extern void init_syscall(void);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 SYSCALL MACROS                                 ||
// ! ||--------------------------------------------------------------------------------||

#define _syscall0(type, name)                 \
    type name(void) {                         \
        type __res;                           \
        __asm__ volatile("int $0x80"          \
                         : "=a"(__res)        \
                         : "0"(__NR_##name)); \
        if (__res >= 0)                       \
            return __res;                     \
        return -1;                            \
    }

#define _syscall(type, name, atype, a)                \
    type name(atype a) {                              \
        type __res;                                   \
        __asm__ volatile("int $0x80"                  \
                         : "=a"(__res)                \
                         : "0"(__NR_##name), "b"(a)); \
        if (__res >= 0)                               \
            return __res;                             \
        return -1;                                    \
    }

#define _syscall1(type, name, atype, a, btype, b)             \
    type name(atype a, btype b) {                             \
        type __res;                                           \
        __asm__ volatile("int $0x80"                          \
                         : "=a"(__res)                        \
                         : "0"(__NR_##name), "b"(a), "c"(b)); \
        if (__res >= 0)                                       \
            return __res;                                     \
        return -1;                                            \
    }

#define _syscall2(type, name, atype, a, btype, b, ctype, c)           \
    type name(atype a, btype b, ctype c) {                            \
        type __res;                                                   \
        __asm__ volatile("int $0x80"                                  \
                         : "=a"(__res)                                \
                         : "0"(__NR_##name), "b"(a), "c"(b), "d"(c)); \
        if (__res >= 0)                                               \
            return __res;                                             \
        return -1;                                                    \
    }

#define _syscall3(type, name, atype, a, btype, b, ctype, c, dtype, d)         \
    type name(atype a, btype b, ctype c, dtype d) {                           \
        type __res;                                                           \
        __asm__ volatile("int $0x80"                                          \
                         : "=a"(__res)                                        \
                         : "0"(__NR_##name), "b"(a), "c"(b), "d"(c), "S"(d)); \
        if (__res >= 0)                                                       \
            return __res;                                                     \
        return -1;                                                            \
    }

#define _syscall4(type, name, atype, a, btype, b, ctype, c, dtype, d, etype, e)       \
    type name(atype a, btype b, ctype c, dtype d, etype e) {                          \
        type __res;                                                                   \
        __asm__ volatile("int $0x80"                                                  \
                         : "=a"(__res)                                                \
                         : "0"(__NR_##name), "b"(a), "c"(b), "d"(c), "S"(d), "D"(e)); \
        if (__res >= 0)                                                               \
            return __res;                                                             \
        return -1;                                                                    \
    }

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  SYSCALL LIST                                  ||
// ! ||--------------------------------------------------------------------------------||

#define SYSCALL_RESTART 0x00
#define __NR_restart 0x00
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

#define SYSCALL_READ 0x03
#define __NR_read 0x03
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
#define __NR_write 0x04
/*
** EAX: 0x04
** EBX: unsigned int fd
** ECX: const char *buff
** EDX: unsigned int count
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

// ... Continue ...

#define SYSCALL_FORK 0x20
#define __NR_fork 0x20
/*
** EAX: 0x20
** EBX: 0x00
** ECX: 0x00
** EDX: 0x00
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

#define SYSCALL_WAIT 0x21
#define __NR_wait 0x21
/*
** EAX: 0x21
** EBX: int pid
** ECX: 0x00
** EDX: 0x00
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

#define SYSCALL_WAITPID 0x22
#define __NR_waitpid 0x22
/*
** EAX: 0x22
** EBX: int pid
** ECX: int options
** EDX: int status
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

#define SYSCALL_KILL 0x3E
#define __NR_kill 0x3E
/*
** EAX: int signum
** EBX: sighandler_t handler
** ECX: 0x00
** EDX: 0x00
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

#define SYSCALL_GETUID 0x66
#define __NR_getuid 0x66
/*
** EAX: 0x66
** EBX: 0x00
** ECX: 0x00
** EDX: 0x00
** ESI: 0x00
** EDI: 0x00
** EBP: 0x00
*/

#endif /* !SYSCALL_H */