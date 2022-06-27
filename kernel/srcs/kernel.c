/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/27 12:41:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/kernel.h"
#include "../includes/shell/kshell.h"
#include "../includes/system/gdt.h"
#include "../includes/system/idt.h"
#include "../includes/system/isr.h"
#include "../includes/system/irq.h"
#include "../includes/system/pit.h"
#include "../includes/drivers/keyboard.h"

static void init_kernel(void)
{
    terminal_initialize();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "TERMINAL " COLOR_END"\n");
    gdt_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "GDT " COLOR_END"\n");
    idt_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "IDT " COLOR_END"\n");
    isrs_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "ISR " COLOR_END"\n");
    irq_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "IRQ " COLOR_END"\n");
    timer_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "PIT " COLOR_END"\n");
    keyboard_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "KEYBOARD " COLOR_END"\n");

    kprintf("" COLOR_YELLOW "[LOG] " COLOR_GREEN "Kernel initialized\n" COLOR_END "");
    kprintf(COLOR_END "\nLoading Kernel...\n");
}

void kmain(void)
{
    __asm__ __volatile__("cli");
    init_kernel();
    __asm__ __volatile__("sti");
    ksleep(1);
    terminal_clear_screen();

    kronos_shell();

    // kputs("\n");
    // terminal_writestring("Hello from kernel!\n");

    // kprintf("Hello from %s [%d]\n\n", "kprintf", 42);
    // kprintf(COLOR_RED "Hello RED!\n" COLOR_END);
    // kprintf(COLOR_GREEN "Hello GREEN!\n" COLOR_END);
    // kprintf(COLOR_BLUE "Hello BLUE!\n" COLOR_END);
    // kprintf(COLOR_YELLOW "Hello YELLOW!\n" COLOR_END);
    // kprintf(COLOR_MAGENTA "Hello MAGENTA!\n" COLOR_END);
    // kprintf(COLOR_CYAN "Hello CYAN!\n" COLOR_END);
    // kprintf(COLOR_END "Hello END!\n");

    // for (;;)
    //     ;
}