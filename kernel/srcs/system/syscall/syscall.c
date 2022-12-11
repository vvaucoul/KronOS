/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:30:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/11 13:32:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/syscall.h>
#include <system/irq.h>

syscall_t __syscall[SYSCALL_SIZE];

static sysfn_t __syscall_exit_test(void)
{
    printk("Syscall exit test\n");
}

static void __add_syscall(uint32_t id, const char *name, sysfn_t *fn)
{
    __syscall[id].id = id;
    __syscall[id].name = (char *)name;
    __syscall[id].function = fn;
}

static void __syscall_handler(struct regs *r)
{
    printk("Syscall %d called\n", r->eax);
    assert(r->eax < SYSCALL_SIZE);

    uint32_t id = r->eax;

    // Update current process

    uint32_t ret;

    __asm__ volatile(
        "push %1\n"
        "push %2\n"
        "push %3\n"
        "push %4\n"
        "push %5\n"
        "push %6\n"
        "call *%6\n"
        "pop %%ebx\n"
        "pop %%ebx\n"
        "pop %%ebx\n"
        "pop %%ebx\n"
        "pop %%ebx\n"
        : "=a"(ret)
        : "r"(r->edi), "r"(r->esi), "r"(r->edx), "r"(r->ecx), "r"(r->ebx), "r"(id));

    // todo: update current process
    // r = current_process->regs;
    r->eax = ret;
}

void init_syscall(void)
{
    memset(__syscall, 0, SYSCALL_SIZE);

    // TMP syscall fn -> NULL -> Wait for KFS-5 & KFS-7
    __add_syscall(SYSCALL_RESTART, "restart", NULL);
    __add_syscall(SYSCALL_EXIT, "exit", &__syscall_exit_test);
    __add_syscall(SYSCALL_FORK, "fork", NULL);
    __add_syscall(SYSCALL_READ, "read", NULL);
    __add_syscall(SYSCALL_WRITE, "write", NULL);

    irq_install_handler(SYSCALL_IRQ, &__syscall_handler);

    // call syscall
    // type __res;
    // __asm__ volatile("int $0x80"
    //                  : "=a"(__res)
    //                  : "0"(__NR_##name), "b"(a));
    // if (__res >= 0)
    //     return __res;
    // errno = -__res;
    // return -1;
}
