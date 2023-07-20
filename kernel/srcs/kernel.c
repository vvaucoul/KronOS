/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 23:38:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <shell/ksh.h>

#include <multitasking/process.h>
#include <multitasking/scheduler.h>

#include <syscall/syscall.h>
#include <system/bsod.h>
#include <system/cmos.h>
#include <system/cpu.h>
#include <system/fpu.h>
#include <system/gdt.h>
#include <system/idt.h>
#include <system/ipc.h>
#include <system/irq.h>
#include <system/isr.h>
#include <system/kerrno.h>
#include <system/mutex.h>
#include <system/panic.h>
#include <system/pit.h>
#include <system/sections.h>
#include <system/serial.h>
#include <system/signal.h>
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

#include <memory/mmap.h>

#include <workflows/workflows.h>

#if __HIGHER_HALF_KERNEL__ == true
#error "Higher Half Kernel is not supported yet"
#endif

uint32_t initial_esp;
uint32_t *kernel_stack = NULL;

// ! ||--------------------------------------------------------------------------------||
// ! ||                             KERNEL UTILS FUNCTIONS                             ||
// ! ||--------------------------------------------------------------------------------||

static inline void ksh_header(void) {
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

void kernel_log_info(const char *part, const char *name) {
    if (__DISPLAY_INIT_LOG__) {
        tm_t tm = gettime();

        uint64_t diff_time = difftime(&tm, &startup_tm);
        printk(_END "[0:%02u] "_END
                    "- "_YELLOW
                    "[%s] " _END "- " _GREEN "[INIT] " _CYAN "%s " _END "\n",
               diff_time, part, name);
    }

    // DEBUG ONLY
    if (irq_check_install(IRQ_PIT))
        kmsleep(100);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   KERNEL INIT                                  ||
// ! ||--------------------------------------------------------------------------------||

static int init_kernel(hex_t magic_number, hex_t addr, uint32_t *kstack) {
    terminal_initialize();
    ksh_header();

    time_init();
    kernel_log_info("LOG", "TIME");

    kernel_stack = kstack;
    initial_esp = (uint32_t)kstack;
    uint32_t kernel_stack_size = (uint32_t)kernel_stack / 1024 / 1024;
    printk("       - "_YELLOW
           "[LOG] " _END "- "_END _GREEN "[INIT] " _END "STACK: "_GREEN
           "%u MB " _END "(%u bytes)\n" _END,
           kernel_stack_size, kernel_stack);

    // bga_init();
    // init_vbe_mode();

    printk("       - "_YELLOW
           "[LOG] " _END "- "_END _GREEN "[CHK] " _END "HHK: "_GREEN
           "%s " _END "\n" _END,
           __HIGHER_HALF_KERNEL__ == true ? "true" : "false");

    kernel_log_info("LOG", "TERMINAL");
    init_kerrno();
    kernel_log_info("LOG", "KERRNO");
    /* Check Magic Number and assign multiboot info */
    if (multiboot_check_magic_number(magic_number) == false)
        return (__BSOD_UPDATE("Multiboot Magic Number is invalid") | 1);

#if __HIGHER_HALF_KERNEL__ == true
    __multiboot_info = (MultibootInfo *)((hex_t *)((hex_t)addr + KERNEL_VIRTUAL_BASE));
#else
    __multiboot_info = (MultibootInfo *)((hex_t *)((hex_t)addr));
#endif

    if (__multiboot_info == NULL)
        return (__BSOD_UPDATE("Multiboot Info is invalid") | 1);
    printk("       - "_YELLOW
           "[LOG] " _END "- "_END _GREEN "[CHK] " _END "MULTIBOOT: "_GREEN
           "0x%x " _END "\n" _END,
           __multiboot_info);

    if (multiboot_init(__multiboot_info))
        return (__BSOD_UPDATE("Error: multiboot_init failed") | 1);
    kernel_log_info("LOG", "MULTIBOOT");
    if (get_memory_map(__multiboot_info))
        return (__BSOD_UPDATE("Error: kernel memory map failed") | 1);
    kernel_log_info("LOG", "KERNEL MEMORY MAP");

    gdt_install();
    kernel_log_info("LOG", "GDT");

    idt_install();
    kernel_log_info("LOG", "IDT");

    isrs_install();
    kernel_log_info("LOG", "ISR");

    irq_install();
    kernel_log_info("LOG", "IRQ");

    tss_init(5, 0x10, 0x0);
    kernel_log_info("LOG", "TSS");

    timer_install();
    kernel_log_info("LOG", "TIMER");

    if ((init_cpuid()) == true) {
        kernel_log_info("LOG", "CPUID");
        get_cpu_topology();
        kernel_log_info("LOG", "CPU TOPOLOGY");
    }

    keyboard_install();
    kernel_log_info("LOG", "KEYBOARD");
    enable_fpu();
    kernel_log_info("LOG", "FPU");

    init_paging();
    kernel_log_info("LOG", "PAGING");
    kernel_log_info("LOG", "HEAP");

    init_syscall();
    kernel_log_info("LOG", "SYSCALL");

    init_signals();
    kernel_log_info("LOG", "SIGNALS");

    // SMP -> Wait KFS -> Threads, processus
    // kpause();
    // Require x64 Broadwell Intel (5th Gen) or higher
    // smp_init();
    // kernel_log_info("LOG", "SMP");

    // ksleep(1);
    // kheap_test();
    // kpause();

    init_scheduler();
    init_tasking();
    kernel_log_info("LOG", "PROCESS");

    return (0);
}
int init_multiboot_kernel(hex_t magic_number, hex_t addr) {
    __UNUSED(addr);
    if (multiboot_check_magic_number(magic_number) == false)
        return (1);
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   KERNEL MAIN                                  ||
// ! ||--------------------------------------------------------------------------------||


void test_03(void) {
    while (1) {
        printk("[%d] Test 03\n", getpid());
        ksleep(1);
    }
}

void test_02(void) {
    pid_t pid_tmp3 = init_task(test_03);
    while (1) {
        printk("[%d] Test 02\n", getpid());
        ksleep(1);
    }
}

void test_01(void) {
    pid_t pid_tmp2 = init_task(test_02);
    while (1) {
        printk("[%d] Test 01\n", getpid());
        ksleep(1);
    }
}


uint32_t placement_address;

int kmain(hex_t magic_number, hex_t addr, uint32_t *kstack) {
    ASM_CLI();
    if ((init_kernel(magic_number, addr, kstack)))
        return (1);
    if (__DISPLAY_INIT_LOG__)
        printk("\n");
    ASM_STI();

    tm_t date = gettime();
    printk("Date: " _GREEN "%04u-%02u-%u:%02u-%02u-%02u\n\n" _END, date.year + 2000, date.month, date.day, date.hours + 1, date.minutes, date.seconds);

    // Todo: EXT2 VFS
    // assert(__multiboot_info->mods_count > 0);
    // uint32_t initrd_location = *((uint32_t *)__multiboot_info->mods_addr);
    // uint32_t initrd_end = *(uint32_t *)(__multiboot_info->mods_addr + 4);
    // // Don't trample our module with placement accesses, please!
    // placement_address = initrd_end;

    // process_test();


    // Todo: la structure de donnee ne peut prendre en compte qu'un seul parent et un seul enfent
    pid_t pid_tmp = init_task(test_01);
    // pid_t pid_tmp2 = init_task(test_02);
    // pid_t pid_tmp3 = init_task(test_03);
    
    ksleep(2);
    // Todo: Debug: lorsque l'on envoie un signal (SIGKILL)
    // Cela tue le processus mais aussi tous les processus parent
    // EX: Kill PID 2 -> Kill PID 1 
    //     Kill PID 3 -> Kill PID 2 -> Kill PID 1
    //     Kill PID 4 -> Kill PID 3 -> Kill PID 2 -> Kill PID 1
    signal(3, SIGKILL);

    // init_task(test_01);
    // init_task(test_02);

    while (1)
        ;

    pid_t pid = fork();
    if (pid == 0) {
        // Todo: Must enter in user space
        kronos_shell();
    } else {

        /*
        ** Task 0 -> Kernel
        ** Must infinite loop
        */

        while (1) {
        }
    }
    return (0);
}