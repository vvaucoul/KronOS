/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user_syscall.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 11:01:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/24 17:38:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#include <stdint.h>

static inline int32_t syscall(int32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    int32_t result;
    __asm__ volatile("int $0x80" : "=a"(result) : "a"(syscall_number), "b"(arg1), "c"(arg2), "d"(arg3));
    return result;
}

static inline int32_t write(const char *str) {
    return syscall(0x4, (uint32_t)str, 0, 0);
}


#endif /* !USER_SYSCALL_H */