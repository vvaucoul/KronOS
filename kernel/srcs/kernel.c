/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/11 01:11:01 by vvaucoul         ###   ########.fr       */
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
#include <system/panic.h>
#include <system/sections.h>
#include <system/fpu.h>

#include <drivers/keyboard.h>
#include <drivers/display.h>

#include <multiboot/multiboot.h>

#include <memory/memory.h>

MultibootInfo *__multiboot_info = NULL;

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
    enable_fpu();
}

void kmain(hex_t magic_number, hex_t addr)
{
    ASM_CLI();
    init_kernel();
    if (multiboot_check_magic_number(magic_number) == false)
        return;
    else
    {
        __multiboot_info = (MultibootInfo *)(addr);
        if (__multiboot_info == NULL)
            __PANIC("Error: __multiboot struct is NULL");
        if (multiboot_init(__multiboot_info))
            __PANIC("Error: multiboot_init failed");
    }
    __display_multiboot_infos();
    kprintf("\n");
    ASM_STI();
    kronos_shell();
}