/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/28 17:42:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <shell/ksh.h>

/* Multitasking */
#include <multitasking/process.h>
#include <multitasking/scheduler.h>

/* System */
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
#include <system/random.h>
#include <system/sections.h>
#include <system/serial.h>
#include <system/signal.h>
#include <system/threads.h>
#include <system/time.h>
#include <system/tss.h>

/* Drivers */
#include <drivers/device/char.h>
#include <drivers/device/devices.h>
#include <drivers/device/floppy.h>
#include <drivers/device/ide.h>
#include <drivers/display.h>
#include <drivers/keyboard.h>
#include <drivers/vesa.h>

#include <memory/mmap.h>
#include <multiboot/multiboot.h>

/* Memory */
#include <memory/kheap.h>
#include <memory/memory.h>
#include <memory/memory_map.h>
#include <memory/paging.h>

/* Filesystem */
#include <filesystem/ext2/ext2.h>
#include <filesystem/initrd.h>
#include <filesystem/tinyfs/tinyfs.h>
#include <filesystem/vfs/vfs.h>

#include <workflows/workflows.h>

/* Apps */
#include <apps/kfe/kfe.h>

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
        // printk(_CURSOR_MOVE_UP);
    }

    // DEBUG ONLY
    if (irq_check_install(IRQ_PIT))
        kmsleep(100);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   KERNEL INIT                                  ||
// ! ||--------------------------------------------------------------------------------||

static int check_multiboot(hex_t magic_number, hex_t addr, uint32_t *kstack) {
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

    kernel_stack = kstack;
    initial_esp = (uint32_t)kstack;
    uint32_t kernel_stack_size = (uint32_t)kernel_stack / 1024 / 1024;
    printk("       - "_YELLOW
           "[LOG] " _END "- "_END _GREEN "[INIT] " _END "STACK: "_GREEN
           "%u MB " _END "(%u bytes)\n" _END,
           kernel_stack_size, kernel_stack);
    return (0);
}

static int init_system_components(void) {
    gdt_init();
    kernel_log_info("LOG", "GDT");

    tss_init(7, 0x10, 0x0);
    kernel_log_info("LOG", "TSS");

    idt_install();
    kernel_log_info("LOG", "IDT");

    isrs_install();
    kernel_log_info("LOG", "ISR");

    irq_install();
    kernel_log_info("LOG", "IRQ");

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

    random_init();
    kernel_log_info("LOG", "RANDOM");
    return (0);
}

static int init_multitasking(void) {
    init_scheduler();
    kernel_log_info("LOG", "SCHEDULER");

    init_tasking();
    kernel_log_info("LOG", "TASKING");

    thread_init();
    kernel_log_info("LOG", "THREADS");
    return (0);
}

static int init_filesystems(uint32_t initrd_location, uint32_t initrd_end) {

    // Init devices
    devices_init();
    kernel_log_info("LOG", "DEVICES");

    if ((vfs_init()) != 0) {
        __WARND("Error: vfs_init failed, (Kernel will not use VFS)");
    } else {
        kernel_log_info("LOG", "VFS");
    }

    // Init initrd filesystem
    if (__multiboot_info->mods_count > 0) {
        if ((initrd_init(initrd_location, initrd_end)) != 0) {
            __PANIC("Error: initrd_init failed");
            __BSOD_UPDATE("Error: initrd_init failed");
            bsod("INITRD INIT FAILED", __FILE__);
            return (1);
        } else {
            kernel_log_info("LOG", "INITRD");
        }
    }
    // initrd_display_hierarchy();
    // kpause();

    /*
    **  IDE INIT
    **
    **  Integrated Drive Electronics initialization
    **  IDE Driver initialization
    */

    if ((ide_init()) != 0) {
        __WARND("Error: ide_init failed, (Kernel will not use IDE Driver)");
    } else {
        /* Workflow IDE must be called after IDE init */
        workflow_ide();
        kernel_log_info("LOG", "IDE");
    }

/**
 * FLOPPY INIT
 *
 * Floppy Driver initialization
 */
#if FLOPPY_DRIVER == 1
    // fdc_initialize();
    kernel_log_info("LOG", "FLOPPY");

    ksleep(1);

    // workflow_fdc();
    // kernel_log_info("LOG", "FLOPPY READ/WRITE");
#else
    __INFOD("FLOPPY Driver is disabled");
#endif

#if __TINYFS__ == 1
    Device *dev = device_get_by_id(0);
    TinyFS *tfs = NULL;

    /* Check if device is available */
    if (dev == NULL) {
        __WARND("Error: device_get_by_id failed, (Kernel will not use TINYFS)");
    } else {
        /* Init TINYFS */
        if ((tfs = tinyfs_init(dev)) == NULL) {
            __WARND("Error: tinyfs_init failed, (Kernel will not use TINYFS)");
        } else {
            /* Format TINYFS */
            if ((tinyfs_formater(tfs, false)) != 0) { // TMP: Hard format is disabled
                __WARND("Error: tinyfs_formater failed, (Kernel will not use TINYFS)");
            }
            /* Mount TINYFS on device */
            if ((vfs_mount(tfs->fs.vfs)) != 0) {
                __WARND("Error: vfs_mount failed, (Kernel will not use TINYFS)");
            } else {
                kernel_log_info("LOG", "TINYFS");
            }
        }
    }
#else
    __INFOD("TINYFS is disabled");
#endif

    /*
    **  EXT2 INIT
    **
    **  EXT2 Filesystem initialization
    */
#if __EXT2__ == 1
    // Todo:
    if ((ext2_init()) != 0) {
        __WARND("Error: vfs_create_fs failed, (Kernel will not use EXT2 Filesystem)");
    } else {
        kernel_log_info("LOG", "FILESYSTEM EXT2");
    }
    kpause();
#else
    __INFOD("EXT2 is disabled, (Kernel will not use EXT2 Filesystem)");
#endif
    return (0);
}

static int init_kernel(hex_t magic_number, hex_t addr, uint32_t *kstack) {
    vga_init();
    ksh_header();
    kernel_log_info("LOG", "VGA - (80x25)");
    kernel_log_info("LOG", "TERMINAL");

    time_init();
    kernel_log_info("LOG", "TIME");

    // bga_init();
    // init_vbe_mode();

    printk("       - "_YELLOW
           "[LOG] " _END "- "_END _GREEN "[CHK] " _END " HHK: "_GREEN
           "%s " _END "\n" _END,
           __HIGHER_HALF_KERNEL__ == true ? "true" : "false");

    kernel_log_info("LOG", "TERMINAL");
    init_kerrno();
    kernel_log_info("LOG", "KERRNO");

    check_multiboot(magic_number, addr, kstack);
    init_system_components();

    /**
     * INIT INITRD
     *
     * Init initrd if multiboot modules are found
     * Initrd -> initial ramdisk
     */
    uint32_t initrd_location = 0;
    uint32_t initrd_end = 0;
    if (__multiboot_info->mods_count > 0) {
        initrd_location = *((uint32_t *)__multiboot_info->mods_addr);
        initrd_end = *(uint32_t *)(__multiboot_info->mods_addr + 4);
        placement_addr = initrd_end;
    } else {
        __WARND("No multiboot modules found, kernel will not use initrd.");
    }

    init_paging();
    kernel_log_info("LOG", "PAGING");
    kernel_log_info("LOG", "HEAP");

    init_syscall();
    kernel_log_info("LOG", "SYSCALL");

    init_signals();
    kernel_log_info("LOG", "SIGNALS");

    init_filesystems(initrd_location, initrd_end);
    init_multitasking();

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

int kmain(hex_t magic_number, hex_t addr, uint32_t *kstack) {
    ASM_CLI();
    if ((init_kernel(magic_number, addr, kstack)))
        return (1);
    if (__DISPLAY_INIT_LOG__) {
        printk("\n");
        printk(_RED);
        terminal_write_n_char('#', VGA_WIDTH);
        printk(_END);
        printk("\n\n");
    }
    ASM_STI();

    tm_t date = gettime();
    printk("Date: " _GREEN "%04u-%02u-%u:%02u-%02u-%02u\n\n" _END, date.year + 2000, date.month, date.day, date.hours + 1, date.minutes, date.seconds);

    // printk("__TEST_HEPHAISTOS__ = %d\n", __TEST_HEPHAISTOS__);

#if __TEST_HEPHAISTOS__ == 1
    __INFOD("Test Hephaistos is enabled");
    return hephaistos_workflow();
#else // !__TEST_HEPHAISTOS__
    __INFOD("Test Hephaistos is disabled");
#endif

    switch_to_user_mode();

    while (1) {
    }

    // threads_test();
    // process_test();

    // uint32_t esp;

    // GET_ESP(esp);
    // printk("ESP: " _GREEN "0x%x\n" _END, esp);

    // tss_set_stack_pointer(esp);
    // tss_set_stack_segment(0x10);

    // switch_to_user_mode();

    // // Todo: Fix priority
    // task_set_priority(pid_tmp, TASK_PRIORITY_LOW);
    // task_set_priority(pid_tmp2, TASK_PRIORITY_LOW);
    // task_set_priority(pid_tmp3, TASK_PRIORITY_LOW);

    pid_t pid = fork();
    if (pid == 0) {
        // Todo: Must enter in user space instead of using kernel space
        kronos_shell();
    } else {

        /*
        ** Task 0 -> Kernel
        ** Must infinite loop
        */

        while (1) {
        }
    }

    kpause();

    print_gdt();
    print_tss();
    check_gdt_tss();
    print_idt_entry(0x80);
    switch_to_user_mode();

    while (1) {
    }
}

//     // Display initrd files

//     // list the contents of /

//     // Todo: KFS-6
//     // initrd_display_hierarchy();

//     // uint32_t esp;
//     // GET_ESP(esp);
//     // tss_init(7, 0x10, esp);
//     // kernel_log_info("LOG", "TSS");
//     // switch_to_user_mode();

//     // threads_test();
//     // process_test();

//     // uint32_t esp;

//     // GET_ESP(esp);
//     // printk("ESP: " _GREEN "0x%x\n" _END, esp);

//     // tss_set_stack_pointer(esp);
//     // tss_set_stack_segment(0x10);

//     // switch_to_user_mode();

//     // Todo: Fix priority
//     // task_set_priority(pid_tmp, TASK_PRIORITY_LOW);
//     // task_set_priority(pid_tmp2, TASK_PRIORITY_LOW);
//     // task_set_priority(pid_tmp3, TASK_PRIORITY_LOW);

//     // switch_to_user_mode();

//     Vfs *__tiny_vfs = vfs_get_fs(TINYFS_FILESYSTEM_NAME);
//     VfsNode *root = __tiny_vfs->fs_root;

//     // vfs_mkdir(__tiny_vfs, root, "/test", 0755);

//     __tiny_vfs->fops->mkdir(root, "test", 0755);
//     __tiny_vfs->fops->mkdir(root, "test2", 0755);
//     __tiny_vfs->fops->mkdir(root, "test3", 0755);
//     __tiny_vfs->fops->mkdir(root, "test4", 0755);

//     Dirent *dir = NULL;
//     uint32_t i = 0;

//     while ((dir = __tiny_vfs->fops->readdir(root, i)) != NULL) {
//         if (strcmp(dir->d_name, "test") == 0) {
//             VfsNode *newroot = vfs_finddir(__tiny_vfs, root, "test");
//             __tiny_vfs->fops->mkdir(newroot, "subtest", 0755);
//             __tiny_vfs->fops->create(newroot, "file.txt", 0755);
//         }
//         ++i;
//     }
//     tinyfs_display_hierarchy((TinyFS_Inode *)(root), 0);
//     // kpause();

//     printk("\n---------------------\n\n");

// #include <cmds/cd.h>
// #include <cmds/ls.h>
// #include <cmds/pwd.h>
// #include <syscall/chdir.h>
// #include <syscall/getcwd.h>

//     pwd(0, NULL);
//     ls(0, NULL);

//     if ((sys_chdir("/test")) != 0) {
//         printk("Error: sys_chdir failed\n");
//     }

//     printk("PWD: ");
//     pwd(0, NULL);
//     printk("Real PWD: %s\n", __tiny_vfs->nops->get_name(__tiny_vfs->fs_current_node));

//     // tinyfs_display_hierarchy((TinyFS_Inode *)(__tiny_vfs->fs_current_node), 0);
//     sys_chdir("/test/subtest");

//     printk("PWD: ");
//     pwd(0, NULL);
//     printk("Real PWD: %s\n", __tiny_vfs->nops->get_name(__tiny_vfs->fs_current_node));
//     kpause();

//     // pwd(0, NULL);
//     // ls(0, NULL);
//     // workflow_tinyfs();

//     kpause();

//     kfe(0, NULL);
//     printk("End of KFE\n");

//     pid_t pid2 = fork();
//     if (pid2 == 0) {

//         printk("ls: ");
//         ls(0, NULL);
//         ksleep(1);
//         printk("\npwd: ");
//         pwd(0, NULL);
//         ksleep(1);

//         cd(1, (char *[]){"cd", "test"}); // Todo: implement /... (ex: /test)
//         ksleep(1);

//         printk("ls: ");
//         ls(0, NULL);
//         ksleep(1);
//         printk("\npwd: ");
//         pwd(0, NULL);
//         ksleep(1);
//     } else {
//         int status;

//         waitpid(pid2, &status, 0);
//         printk("Child process exited with status: %d\n", status);
//     }
//     pause();

//     pid_t pid = fork();
//     if (pid == 0) {
//         // Todo: Must enter in user space instead of using kernel space
//         kronos_shell();
//     } else {

//         /*
//         ** Task 0 -> Kernel
//         ** Must infinite loop
//         */

//         while (1) {
//         }
//     }
//     return (0);
// }