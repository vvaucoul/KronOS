/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/04 19:45:36 by vvaucoul         ###   ########.fr       */
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

MultibootInfo *_multiboot_info = NULL;

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
    // init_kernel_memory();
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[LOG] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "MEMORY " COLOR_END "\n");
    enable_fpu();
}

void kmain(hex_t magic_number, hex_t addr)
{
    ASM_CLI();
    init_kernel();
    if (__check_magic_number(magic_number) == false)
        return;
    else
    {
        _multiboot_info = (MultibootInfo *)(&addr);
        if (_multiboot_info == NULL)
            __PANIC("Error: _multiboot_info is NULL");
    }

    // __display_multiboot_infos();
    // display_sections();
    UPDATE_CURSOR();


    // kprintf("Value 1: %f\n", 1.73728);
    // kprintf("Value 2: %f\n", 123.00237);
    // kprintf("Value 3: %f\n", 936624);
    // kprintf("Value 4: %f\n", 0.000352);
    // kprintf("Value 5: %f\n", 89347.403402361);
   
    ASM_STI();
    while (1);

    // if (__DISPLAY_INIT_LOG__)
    //     kprintf("\n");
    // khexdump(0x00000800 - 64, 142);
    // if (__DISPLAY_INIT_LOG__)
    //     kprintf("\n");
    // kprintf("Using video type: %d\n", get_bios_area_video_type());

    kronos_shell();
}