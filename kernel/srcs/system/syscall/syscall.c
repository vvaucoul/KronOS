/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:30:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/10 01:15:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/syscall.h>

syscall_t __syscall[SYSCALL_SIZE];

static void __add_syscall(uint32_t id, const char *name, sysfn_t *fn)
{
    __syscall[id].id = id;
    __syscall[id].name = (char *)name;
    __syscall[id].function = fn;
}

void init_syscall(void)
{
    memset(__syscall, 0, SYSCALL_SIZE);

    // TMP syscall fn -> NULL -> Wait for KFS-5 & KFS-7
    __add_syscall(SYSCALL_RESTART, "restart", NULL);
    __add_syscall(SYSCALL_EXIT, "exit", NULL);
    __add_syscall(SYSCALL_FORK, "fork", NULL);
    __add_syscall(SYSCALL_READ, "read", NULL);
    __add_syscall(SYSCALL_WRITE, "write", NULL);
}
