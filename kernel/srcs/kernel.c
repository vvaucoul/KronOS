/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/02 14:58:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <shell/ksh.h>

#include <system/gdt.h>
#include <system/idt.h>
#include <system/isr.h>
#include <system/irq.h>
#include <system/pit.h>
#include <system/kerrno.h>
#include <system/serial.h>

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
    init_kerrno();
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "KERRNO " COLOR_END "\n");
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "TERMINAL " COLOR_END "\n");
    gdt_install();
    poweroff();
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "GDT " COLOR_END "\n");
    idt_install();
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "IDT " COLOR_END "\n");
    isrs_install();
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "ISR " COLOR_END "\n");
    irq_install();
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "IRQ " COLOR_END "\n");
    timer_install();
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "PIT " COLOR_END "\n");
    keyboard_install();
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "KEYBOARD " COLOR_END "\n");
    init_kernel_memory();
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "MEMORY " COLOR_END "\n");
}

void kmain(void)
{
    ASM_CLI();
    init_kernel();
    if (__DISPLAY_INIT_LOG__)
        kprintf("\n");
    // khexdump(0x00000800 - 64, 142);
    if (__DISPLAY_INIT_LOG__)
        kprintf("\n");
    qemu_printf("Salut !\n");
    ASM_STI();

    // kprintf("\n");
    // kprintf("Test Alloc: \n");
    // char *str = kmalloc(4);

    // kbzero(str, 4);
    // str[0] = 'A';
    // str[1] = 'B';
    // str[2] = 'C';

    // kprintf("str = %s\n", str);

    // char *str2 = kmalloc(4);

    // kbzero(str2, 4);
    // str2[0] = 'F';   
    // str2[1] = 'G';
    // str2[2] = 'H';

    // kprintf("str = %s\n", str);
    // kprintf("str2 = %s\n", str2);

    // kprintf("\n");

    // kprintf("str addr: %p\n", str);
    // kprintf("str 2 addr: %p\n", str2);

    // str[3] = 'D';
    // str[4] = 'E';
    // str[5] = 0;

    // kprintf("str = %s\n", str);
    // kprintf("str2 = %s\n", str2);

    kronos_shell();
}