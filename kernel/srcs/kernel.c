/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/11 23:11:17 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <shell/ksh.h>

#include <multitasking/scheduler.h>
#include <multitasking/tasking.h>

#include <system/bsod.h>
#include <system/cmos.h>
#include <system/cpu.h>
#include <system/fpu.h>
#include <system/gdt.h>
#include <system/idt.h>
#include <system/irq.h>
#include <system/isr.h>
#include <system/kerrno.h>
#include <system/panic.h>
#include <system/pit.h>
#include <system/sections.h>
#include <system/serial.h>
#include <system/syscall.h>
#include <system/time.h>
#include <system/tss.h>

#include <drivers/display.h>
#include <drivers/keyboard.h>
#include <drivers/vesa.h>

#include <multiboot/multiboot.h>

#include <memory/kheap.h>
#include <memory/memory.h>
#include <memory/memory_map.h>
#include <memory/paging.h>

#include <workflows/workflows.h>

#if __HIGHER_HALF_KERNEL__ == true
#error "Higher Half Kernel is not supported yet"
#endif

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
    {
        tm_t tm = gettime();

        uint64_t diff_time = difftime(&tm, &startup_tm);
        printk(_END "[0:%02u] "_END
                    "- "_YELLOW
                    "[%s] " _END "- " _GREEN "[INIT] " _CYAN "%s " _END "\n",
               diff_time, part, name);
    }
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

    // bga_init();
    // vesa_init();

    time_init();
    kernel_log_info("LOG", "TIME");

    __hhk_log();
    kernel_log_info("LOG", "TERMINAL");
    init_kerrno();
    kernel_log_info("LOG", "KERRNO");

    /* Check Magic Number and assign multiboot info */
    if (multiboot_check_magic_number(magic_number) == false)
        return (__BSOD_UPDATE("Multiboot Magic Number is invalid") | 1);
    else
    {
        __multiboot_info = (MultibootInfo *)((hex_t *)((hex_t)addr));
        assert(__multiboot_info != NULL);
        if (multiboot_init(__multiboot_info))
            __PANIC("Error: multiboot_init failed");
        kernel_log_info("LOG", "MULTIBOOT");
        if (get_memory_map(__multiboot_info))
            __PANIC("Error: kernel memory map failed");
        kernel_log_info("LOG", "KERNEL MEMORY MAP");
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

    if ((init_cpuid()) == true)
    {
        kernel_log_info("LOG", "CPUID");

        printk(_END "\t\t\t   -"_GREEN
                    " VENDOR: " _END "%s" _END "\n",
               cpu_vendor);
        printk(_END "\t\t\t   -"_GREEN
                    " NAME: " _END "%s" _END "\n",
               cpu_brand);
        printk(_END "\t\t\t   -"_GREEN
                    " HYPERVISOR: " _END "%s" _END "\n",
               hypervisor);
    }
    get_cpu_topology();
    kernel_log_info("LOG", "CPU TOPOLOGY");

    timer_install();
    kernel_log_info("LOG", "TIMER");
    keyboard_install();
    kernel_log_info("LOG", "KEYBOARD");
    enable_fpu();
    kernel_log_info("LOG", "FPU");
    init_paging();
    kernel_log_info("LOG", "PAGING");

    init_syscall();
    kernel_log_info("LOG", "SYSCALL");

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

void test_user_function()
{
    printk("Hello from user space!\n");
}

int kmain(hex_t magic_number, hex_t addr)
{
    ASM_CLI();
    if ((init_kernel(magic_number, addr)))
        return (1);
    if (__DISPLAY_INIT_LOG__)
        printk("\n");
    ASM_STI();


    /* Raise exception: Divide by zero */
    // __asm__ volatile("int $0x0");

    // interrupts_test();

    // process_test();

    // tm_t date = gettime();
    // printk("Mktime: %u\n", mktime(&date));
    // printk("Date: %s\n\n", asctime(&date));

    // exit();

    tm_t date = gettime();
    printk(_GREEN "%04u-%02u-%u:%02u-%02u-%02u\n\n" _END, date.year + 2000, date.month, date.day, date.hours + 1, date.minutes, date.seconds);

    get_cpu_informations();

    tasking_init();

    // TODO: fork and exec
    /* fork
    ** exec kronos_shell
    ** infinite pause
    */

    // switch to user mode

    // switch_user_mode();
    // kpause();

    kronos_shell();
    return (0);
}