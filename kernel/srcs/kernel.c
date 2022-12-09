/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 15:11:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <shell/ksh.h>

#include <system/gdt.h>
#include <system/idt.h>
#include <system/isr.h>
#include <system/irq.h>
#include <system/pit.h>
#include <system/tss.h>
#include <system/kerrno.h>
#include <system/serial.h>
#include <system/panic.h>
#include <system/sections.h>
#include <system/fpu.h>
#include <system/bsod.h>
#include <system/scheduler.h>

#include <drivers/keyboard.h>
#include <drivers/display.h>
#include <drivers/vesa.h>

#include <multiboot/multiboot.h>

#include <memory/memory.h>
#include <memory/memory_map.h>
#include <memory/kheap.h>
#include <memory/paging.h>

#include <workflows/workflows.h>

MultibootInfo *__multiboot_info = NULL;

static inline void ksh_header(void)
{
    printk(_RED "\n \
   \t\t\t\t\t\t\t##   ###   ##  \n \
   \t\t\t\t\t\t\t ##  ###  ##   \n \
   \t\t\t\t\t\t\t  ## ### ##    \n \
   \t\t\t\t\t\t\t  ## ### ##    \n \
   \t\t\t\t\t\t\t  ## ### ##    \n \
   \t\t\t\t\t\t\t ##  ###  ##   \n \
   \t\t\t\t\t\t\t##   ###   ##  \n \
    \n" _END);
    printk(_RED);
    terminal_write_n_char('#', VGA_WIDTH);
    printk(_END);
    printk("\n");
}

void kernel_log_info(const char *part, const char *name)
{
    if (__DISPLAY_INIT_LOG__)
        printk(_YELLOW "[%s] " _END "- " _GREEN "[INIT] " _CYAN "%s " _END "\n", part, name);
}

static void __hhk_log(void)
{
    if (__HIGHER_HALF_KERNEL__ == true)
        kernel_log_info("LOG", "HHK: TRUE");
    else
        kernel_log_info("LOG", "HHK: FALSE");
}

static int init_kernel(hex_t magic_number, hex_t addr)
{
    terminal_initialize();
    ksh_header();
    __hhk_log();
    kernel_log_info("LOG", "TERMINAL");
    init_kerrno();
    kernel_log_info("LOG", "KERRNO");

    /* Check Magic Number and assign multiboot info */
    if (multiboot_check_magic_number(magic_number) == false)
        return (__BSOD_UPDATE("Multiboot Magic Number is invalid") | 1);
    // Todo: Fix multiboot with boot.s (no multiboot info -> due to idt works)
    else
    {
        printk("Addr: %x\n", addr);
        // __multiboot_info = (MultibootInfo *)(addr);
        // assert(__multiboot_info != NULL);
        // if (multiboot_init(__multiboot_info))
        //     __PANIC("Error: multiboot_init failed");
        // kernel_log_info("LOG", "MULTIBOOT");
        // if (get_memory_map(__multiboot_info))
        //     __PANIC("Error: kernel memory map failed");
        // kernel_log_info("LOG", "KERNEL MEMORY MAP");
        // // display_multiboot_infos();
    }
    gdt_install();
    kernel_log_info("LOG", "GDT");

    tss_init(5, 0x10, 0x0);
    kernel_log_info("LOG", "TSS");

    idt_install();
    kernel_log_info("LOG", "IDT");
    isrs_install();
    kernel_log_info("LOG", "ISRS");
    irq_install();
    kernel_log_info("LOG", "IRQ");

    timer_install();
    kernel_log_info("LOG", "TIMER");
    keyboard_install();
    kernel_log_info("LOG", "KEYBOARD");
    enable_fpu();
    kernel_log_info("LOG", "FPU");

    init_paging();
    kernel_log_info("LOG", "PAGING");

    // SMP -> Wait KFS -> Threads, processus
    // kpause();
    // Require x64 Broadwell Intel (5th Gen) or higher
    // smp_init();
    // kernel_log_info("LOG", "SMP");
    // kpause();

    return (0);
}
int init_multiboot_kernel(hex_t magic_number, hex_t addr)
{
    __UNUSED(addr);
    if (multiboot_check_magic_number(magic_number) == false)
        return (1);
    return (0);
}

int kmain(hex_t magic_number, hex_t addr)
{
    ASM_CLI();
    if ((init_kernel(magic_number, addr)))
        return (1);
    if (__DISPLAY_INIT_LOG__)
        printk("\n");
    ASM_STI();

    // /* Raise exception: Divide by zero */

    __asm__ volatile("int $0x0");
    __asm__ volatile("int $0x1");

    printk("Hello World!\n");

    // interrupts_test();

    // process_test();

    kronos_shell();
    return (0);
}