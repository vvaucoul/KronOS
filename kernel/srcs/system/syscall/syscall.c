/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:30:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/30 12:54:02 by vvaucoul         ###   ########.fr       */
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

static bool is_user_address_valid(void *addr, size_t size) {
    uint32_t start_addr = (uint32_t)addr;
    uint32_t end_addr = start_addr + size;

    for (uint32_t current_addr = start_addr; current_addr < end_addr; current_addr += PAGE_SIZE) {
        page_t *page = get_page(current_addr, current_directory);
        if (!page || !page->present || !page->user) {
            return false;
        }
    }
    return true;
}

extern int syscall_write(const char *str) {
    if (!is_user_address_valid((void *)str, strlen(str) + 1)) {
        __PANIC("Invalid user address in syscall_write");
    }
    return printk("%s", str);
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

void syscall_handler(struct regs *r) {
    printk("Syscall %d Received\n", r->eax);
    int32_t syscall_number = r->eax;
    uint32_t arg1 = r->ebx;
    uint32_t arg2 = r->ecx;
    uint32_t arg3 = r->edx;

    if (syscall_number >= SYSCALL_SIZE) {
        __THROW_NO_RETURN("Invalid syscall number: %u\n", syscall_number);
    }

    syscall_t *syscall = &(syscalls[syscall_number]);
    if (syscall && syscall->function) {
        r->eax = syscall->function((void *)arg1, (void *)arg2, (void *)arg3);
    } else {
        printk("Syscall not implemented: %u\n", syscall_number);
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

    // isr_register_interrupt_handler(0x80, &syscall_handler);
    idt_set_gate(0x80, (uint32_t)isr80_handler, 0x08, 0xEE);
}