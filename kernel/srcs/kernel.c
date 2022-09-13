/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/13 21:36:06 by vvaucoul         ###   ########.fr       */
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
#include <memory/memory_map.h>
#include <memory/pmm.h>

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

static void ksh_log_info(const char *part, const char *name)
{
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[%s] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "%s " COLOR_END "\n", part, name);
}

static int init_kernel(hex_t magic_number, hex_t addr)
{
    terminal_initialize();
    ksh_header();
    ksh_log_info("LOG", "TERMINAL");

    /* Check Magic Number and assign multiboot info */
    if (multiboot_check_magic_number(magic_number) == false)
        return (1);
    else
        __multiboot_info = (MultibootInfo *)(addr);
    init_kerrno();
    ksh_log_info("LOG", "KERRNO");
    gdt_install();
    ksh_log_info("LOG", "GDT");
    idt_install();
    ksh_log_info("LOG", "IDT");
    isrs_install();
    ksh_log_info("LOG", "ISRS");
    irq_install();
    ksh_log_info("LOG", "IRQ");
    timer_install();
    ksh_log_info("LOG", "TIMER");
    keyboard_install();
    ksh_log_info("LOG", "KEYBOARD");

    if (__multiboot_info == NULL)
        __PANIC("Error: __multiboot struct is NULL");
    if (multiboot_init(__multiboot_info))
        __PANIC("Error: multiboot_init failed");
    ksh_log_info("LOG", "MULTIBOOT");
    if (get_kernel_memory_map(__multiboot_info))
        __PANIC("Error: kernel memory map failed");
    ksh_log_info("LOG", "KERNEL MEMORY MAP");

    pmm_init(KMAP.available.start_addr, KMAP.available.length);
    ksh_log_info("LOG", "PMM");

    init_kernel_memory();
    ksh_log_info("LOG", "KERNEL MEMORY");
    enable_fpu();
    ksh_log_info("LOG", "FPU");
    return (0);
}
int init_multiboot_kernel(hex_t magic_number, hex_t addr)
{
    if (multiboot_check_magic_number(magic_number) == false)
        return (1);
    __multiboot_info = (MultibootInfo *)(addr);
    return (0);
}

int kmain(hex_t magic_number, hex_t addr)
{
    ASM_CLI();
    if ((init_kernel(magic_number, addr)))
        return (1);
    kprintf("\n");
    ASM_STI();
    kronos_shell();
    return (0);
}