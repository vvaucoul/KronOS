/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:30:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/24 17:45:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <syscall/syscall.h>
#include <system/irq.h>

#include <multitasking/process.h>

extern int syscall_restart(void) {
#warning "Syscall restart not implemented yet"
    return (0);
}
extern int syscall_exit(int32_t status) {
    task_exit(status);
    return (0);
}
extern int syscall_fork(void) {
    return (task_fork());
}
extern int syscall_wait(void) {
#warning "Syscall wait not implemented yet"
    return (0);
}

extern int syscall_read(int fd) {
#warning "Syscall read not implemented yet"
    __UNUSED(fd);
    return (0);
}
extern int syscall_write(const char *str) {
    return (printk("%s", str));
}

extern int syscall_kill(pid_t pid, int sig) {
    return (kill(pid, sig));
}

static syscall_t syscalls[SYSCALL_SIZE];

static void syscall_register(uint32_t id, const char *name, sysfn_t fn) {
    syscalls[id].id = id;
    syscalls[id].name = (char *)name;
    syscalls[id].function = fn;
    printk("\t\t\t   - Syscall " _YELLOW "[%d]" _END " - " _GREEN "%s" _END "\n", id, name);
}

static void syscall_handler(struct regs *r) {
    printk("Syscall %u Received\n", (uint32_t)r->eax);
    printk("- EBX : %x\n", r->ebx);
    printk("- ECX : %x\n", r->ecx);
    printk("- EDX : %x\n", r->edx);
    

    if (r->eax >= SYSCALL_SIZE) {
        __THROW_NO_RETURN("Invalid syscall number: %u\n", r->eax);
    }

    syscall_t *syscall = &(syscalls[r->eax]);
    if (syscall && syscall->function) {
        r->eax = syscall->function((void *)r->ebx, (void *)r->ecx, (void *)r->edx);
    } else {
        printk("Syscall not implemented: %u\n", r->eax);
        __WARND("Syscall not implemented: %u\n", r->eax);
    }
}

void init_syscall(void) {
    memset(syscalls, 0, sizeof(syscalls));

    // Enregistrer les appels système
    syscall_register(SYSCALL_WRITE, "write", (sysfn_t)syscall_write);
    // Ajouter d'autres appels système ici
    // syscall_register(SYSCALL_RESTART, "restart", (sysfn_t)syscall_restart);
    // syscall_register(SYSCALL_EXIT, "exit", (sysfn_t)syscall_exit);
    // syscall_register(SYSCALL_READ, "read", (sysfn_t)syscall_read);
    // syscall_register(SYSCALL_FORK, "fork", (sysfn_t)syscall_fork);
    // syscall_register(SYSCALL_WAIT, "wait", (sysfn_t)syscall_wait);
    // syscall_register(SYSCALL_KILL, "kill", (sysfn_t)syscall_kill);

    // idt_set_gate(0x80, (uint32_t)syscall_handler, 0x08, 0x8E);

    idt_set_gate(0x80, (uint32_t)syscall_handler, 0x08, 0b11101110);
}