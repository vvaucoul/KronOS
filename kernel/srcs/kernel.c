/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/16 16:24:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <shell/ksh.h>

#include <system/gdt.h>
#include <system/idt.h>
#include <system/isr.h>
#include <system/irq.h>
#include <system/pit.h>

#include <drivers/keyboard.h>

#include <memory/memory.h>

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
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "TERMINAL " COLOR_END "\n");
    gdt_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "GDT " COLOR_END "\n");
    idt_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "IDT " COLOR_END "\n");
    isrs_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "ISR " COLOR_END "\n");
    irq_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "IRQ " COLOR_END "\n");
    timer_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "PIT " COLOR_END "\n");
    keyboard_install();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "KEYBOARD " COLOR_END "\n");
    init_kernel_memory();
    kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "MEMORY " COLOR_END "\n");
}

void kmain(void)
{
    ASM_CLI();
    init_kernel();
    kprintf("\n");
    khexdump(0x00000800 - 64, 142);
    kprintf("\n");
    ASM_STI();

    kprintf("Test Alloc: \n");
    char *str = kmalloc(1024);
    str[0] = 'A';
    str[1] = 'B';
    str[2] = 'C';

    kprintf("str = %s\n", str);

    kronos_shell();
}