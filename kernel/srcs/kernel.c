/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/30 22:34:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/kernel.h"
#include "../includes/shell/ksh.h"
#include "../includes/system/gdt.h"
#include "../includes/system/idt.h"
#include "../includes/system/isr.h"
#include "../includes/system/irq.h"
#include "../includes/system/pit.h"
#include "../includes/drivers/keyboard.h"

static inline void ksh_header(void)
{
    kprintf(COLOR_RED "\n \
   \t\t\t\t\t\t\t##   ###   ##  \n \
   \t\t\t\t\t\t\t ##  ###  ##   \n \
   \t\t\t\t\t\t\t  ## ### ##    \n \
   \t\t\t\t\t\t\t  ## ### ##    \n \
   \t\t\t\t\t\t\t  ## ### ##    \n \
   \t\t\t\t\t\t\t ##  ###  ##   \n \
   \t\t\t\t\t\t\t##   ###   ##  \n \
    \n" COLOR_END);
    kprintf(COLOR_RED);
    terminal_write_n_char('#', VGA_WIDTH);
    kprintf(COLOR_END);
    kprintf("\n");
}

static void init_kernel(void)
{
    terminal_initialize();
    ksh_header();
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

    kprintf(COLOR_GREEN "Kernel initialized\n\n" COLOR_END "");
}

void kmain(void)
{
    __asm__ __volatile__("cli");
    init_kernel();
    __asm__ __volatile__("sti");

    kprintf("Test: [%5d]\n", 42);
    kprintf("Test: [%-5d]\n", 42);

    kronos_shell();
}