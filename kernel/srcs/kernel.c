/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/25 13:39:37 by vvaucoul         ###   ########.fr       */
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

#include <filesystem/ext2/vfs_ext2.h>
#include <filesystem/initrd.h>

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

    tss_init(7, 0x10, 0x0);
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

    /* Basic INITRD - VFS - EXT2 Implementation */
    if (__multiboot_info->mods_count > 0) {
        kernel_log_info("LOADING", "INITRD - FILESYSTEM");
        // ksleep(2);
        uint32_t initrd_location = *((uint32_t *)__multiboot_info->mods_addr);
        uint32_t initrd_end = *(uint32_t *)(__multiboot_info->mods_addr + 4);
        fs_root = initialise_initrd(initrd_location);
        placement_addr = initrd_end;
        kernel_log_info("LOG", "INITRD");
        // ksleep(2);
        kernel_log_info("LOG", "VFS - EXT2");
        // ksleep(2);
    } else {
        __WARND("No multi boot modules found, kernel will not use filesystem.");
    }

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
    kernel_log_info("LOG", "SCHEDULER");

    init_tasking();
    kernel_log_info("LOG", "TASKING");

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

    printk("Stringify: " _GREEN "%s\n" _END, (STRINGIFY(Hello World !)));

    // Display initrd files

    // list the contents of /

    //Todo: KFS-6
    // printk("Initrd files:\n");
    // read_disk();
    // uint8_t buffer[] = "Hello, World!";
    // write_fs(fs_root, 0, strlen((const char *)buffer), buffer);
    // read_disk();

    // uint32_t esp;
    // GET_ESP(esp);
    // tss_init(7, 0x10, esp);
    // kernel_log_info("LOG", "TSS");
    // switch_to_user_mode();
    
        
    process_test();
    kpause();

    // Todo: Fix priority
    // task_set_priority(pid_tmp, TASK_PRIORITY_LOW);
    // task_set_priority(pid_tmp2, TASK_PRIORITY_LOW);
    // task_set_priority(pid_tmp3, TASK_PRIORITY_LOW);

    switch_to_user_mode();

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