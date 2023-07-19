/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:30:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 20:46:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/irq.h>
#include <system/syscall.h>

#include <multitasking/process.h>

extern int syscall_restart(void)
{
    #warning "Syscall restart not implemented yet"
    return (0);
}
extern int syscall_exit(void)
{
    #warning "Syscall exit not implemented yet"
    return (0);
}
extern int syscall_fork(void)
{
    #warning "Syscall fork not implemented yet"
    return (0);
}
extern int syscall_wait(void)
{
    #warning "Syscall wait not implemented yet"
    return (0);
}

extern int syscall_read(int fd)
{
    #warning "Syscall read not implemented yet"
    __UNUSED(fd);
    return (0);
}
extern int syscall_write(int fd)
{
    #warning "Syscall write not implemented yet"
    __UNUSED(fd);
    return (0);
}

syscall_t __syscall[SYSCALL_SIZE];

static void kill(void) {
    // Implementation of kill signal
}

static void __add_syscall(uint32_t id, const char *name, sysfn_t *fn) {
    __syscall[id].id = id;
    __syscall[id].name = (char *)name;
    __syscall[id].function = fn;

    printk("\t\t\t   - Syscall " _YELLOW "[%d]" _END " - " _GREEN "%s" _END "\n", id, name);
}

static void __syscall_handler(struct regs *r) {
    printk("Syscall %d Received\n", r->eax);
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

void init_syscall(void) {
    memset(__syscall, 0, sizeof(__syscall));

    // TMP syscall fn -> NULL -> Wait for KFS-5 & KFS-7
    __add_syscall(SYSCALL_RESTART, "restart", syscall_restart);
    __add_syscall(SYSCALL_EXIT, "exit", syscall_exit);
    __add_syscall(SYSCALL_READ, "read", syscall_read);
    __add_syscall(SYSCALL_WRITE, "write", syscall_write);
    __add_syscall(SYSCALL_FORK, "fork", syscall_fork);
    __add_syscall(SYSCALL_WAIT, "wait", syscall_wait);
    __add_syscall(SYSCALL_KILL, "kill", kill); // KILL = SIGNAL
    __add_syscall(SYSCALL_GETUID, "getuid", getuid);

    idt_set_gate(0x80, (uint32_t)__syscall_handler, IDT_SELECTOR, IDT_FLAG_GATE);
}
