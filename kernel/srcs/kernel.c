/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/29 17:38:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <shell/ksh.h>

#include <multitasking/process.h>
#include <multitasking/scheduler.h>

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

static int init_kernel(hex_t magic_number, hex_t addr, uint32_t *kstack)
{
    terminal_initialize();
    ksh_header();

    kernel_stack = kstack;

    // bga_init();
    // init_vbe_mode();
    // kpause();

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
    kernel_log_info("LOG", "ISR");
    irq_install();
    kernel_log_info("LOG", "IRQ");

    timer_install();
    kernel_log_info("LOG", "TIMER");

    if ((init_cpuid()) == true)
    {
        kernel_log_info("LOG", "CPUID");
        get_cpu_topology();
        kernel_log_info("LOG", "CPU TOPOLOGY");
    }

    // init_scheduler();

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

    kheap_test();
    kpause();

    return (0);
}
int init_multiboot_kernel(hex_t magic_number, hex_t addr)
{
    __UNUSED(addr);
    if (multiboot_check_magic_number(magic_number) == false)
        return (1);
    return (0);
}

__attribute__((unused)) void test_user_function()
{
    printk("Hello from user space!\n");
}

void yield()
{
    scheduler();
}

void process_02(void)
{
    while (1)
    {
        printk("Hello from dummy function!\n");
        ksleep(1);
        // yield();
    }
}

void process_01(void)
{
    while (1)
    {
        terminal_writestring("Hello from task 0 !\n");
        ksleep(1);
        // yield();
    }
}

void exec_fn(uint32_t *addr, uint32_t *function, uint32_t size)
{
    uint32_t *ptr = (uint32_t *)addr;
    for (uint32_t i = 0; i < size; ++i)
    {
        ptr[i] = function[i];
    }
}

int kmain(hex_t magic_number, hex_t addr, uint32_t *kstack)
{
    ASM_CLI();
    if ((init_kernel(magic_number, addr, kstack)))
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

    printk("\n");

    printk("\n\nInit Scheduler\n");
    init_scheduler();

    kernel_log_info("LOG", "PROCESS");

    // switch_to_user_mode();

    create_process(process_01);
    // create_process(process_02);

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