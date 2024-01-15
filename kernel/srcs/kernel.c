/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/15 20:20:42 by vvaucoul         ###   ########.fr       */
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
#include <system/random.h>
#include <system/sections.h>
#include <system/serial.h>
#include <system/signal.h>
#include <system/threads.h>
#include <system/time.h>
#include <system/tss.h>

#include <drivers/display.h>
#include <drivers/keyboard.h>
#include <drivers/vesa.h>

#include <drivers/device/ata.h>
#include <drivers/device/floppy.h>
#include <drivers/device/pata.h>

#include <drivers/device/blocks.h>
#include <drivers/device/char.h>
#include <drivers/device/devices.h>

#include <multiboot/multiboot.h>

#include <memory/kheap.h>
#include <memory/memory.h>
#include <memory/memory_map.h>
#include <memory/paging.h>

#include <filesystem/ext2/ext2.h>
#include <filesystem/initrd.h>
#include <filesystem/vfs.h>

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

    random_init();
    kernel_log_info("LOG", "RANDOM");

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

    // SMP -> Wait KFS -> Threads, processus
    // kpause();
    // Require x64 Broadwell Intel (5th Gen) or higher
    // smp_init();
    // kernel_log_info("LOG", "SMP");

    // ksleep(1);
    // kheap_test();
    // kpause();

    // Init devices
    devices_init();
    kernel_log_info("LOG", "DEVICES");

    // Init initrd filesystem
    if (__multiboot_info->mods_count > 0) {

        if ((initrd_init(initrd_location, initrd_end)) == 1) {
            __PANIC("Error: initrd_init failed");
            __BSOD_UPDATE("Error: initrd_init failed");
            bsod("INITRD INIT FAILED", __FILE__);
            return (1);
        } else {
            kernel_log_info("LOG", "INITRD");
        }
    }

    /*
    **  ATA INIT
    **
    **  ATA Driver initialization
    */
#if ATA_DRIVER == 1
    if ((ata_init()) != 0) {
        __WARND("Error: ata_init failed, (Kernel will not use ATA Driver)");
    } else {
        kernel_log_info("LOG", "ATA");
    }
#else
    __INFOD("ATA Driver is disabled");
#endif

/**
 * PATA INIT
 *
 * PATA Driver initialization
 */
#if PATA_DRIVER == 1
    if ((pata_init(ATA_PRIMARY_IO, ATA_PRIMARY_DEV_CTRL)) != 0) {
        __WARND("Error: pata_init failed, (Kernel will not use PATA Driver)");
    } else {
        kernel_log_info("LOG", "PATA");

        uint8_t id = pata_identify(pata_dev);
        if (id != 0) {
            __WARND("Error: pata_identify failed, (Kernel will not use PATA Driver)");
        } else {
            kernel_log_info("LOG", "PATA IDENTIFY");
            pata_identify_device(pata_dev);

            ksleep(1);

            kernel_log_info("LOG", "PATA READ/WRITE");
        }
    }
#else
    __INFOD("PATTA Driver is disabled");
#endif

/**
 * FLOPPY INIT
 *
 * Floppy Driver initialization
 */
#if FLOPPY_DRIVER == 1
    if ((floppy_init()) != 0) {
        __WARND("Error: floppy_init failed, (Kernel will not use Floppy Driver)");
    } else {
        kernel_log_info("LOG", "FLOPPY");

        ksleep(1);

        kernel_log_info("LOG", "FLOPPY READ/WRITE");
    }
#else
    __INFOD("FLOPPY Driver is disabled");
#endif

    /*
    **  VFS INIT
    **
    **  Init VFS if filesystem is initialized
    */
#if VFS == 1
    if (fs_root != NULL) {
        if (vfs_init(EXT2_FILESYSTEM_NAME) != 0) {
            __BSOD_UPDATE("Error: vfs_init failed");
            bsod("VFS INIT FAILED", __FILE__);
            return (1);
        } else {
            kernel_log_info("LOG", "FILESYSTEM - VFS");
            ksleep(1);
        }
    }
#else
    __INFOD("VFS is disabled, kernel will not use virtual filesystem");
#endif

    // Todo: Init EXT2 filesystem here
    kernel_log_info("LOG", "FILESYSTEM EXT2");

    init_scheduler();
    kernel_log_info("LOG", "SCHEDULER");

    init_tasking();
    kernel_log_info("LOG", "TASKING");

    thread_init();
    kernel_log_info("LOG", "THREADS");

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

    // printk("__TEST_HEPHAISTOS__ = %d\n", __TEST_HEPHAISTOS__);

#if __TEST_HEPHAISTOS__ == 1
    __INFOD("Test Hephaistos is enabled");
    hephaistos_workflow();
#else // !__TEST_HEPHAISTOS__
    __INFOD("Test Hephaistos is disabled");
#endif

    /*
    {

        uint8_t write_buffer[512]; // Buffer de données à écrire
        uint8_t read_buffer[512];  // Buffer pour stocker les données lues

        // Remplir le buffer de données à écrire
        for (int i = 0; i < 512; i++) {
            write_buffer[i] = (uint8_t)i;
        }
        // Effacer le buffer de lecture
        memset(read_buffer, 0, 512);

        Device *device = device_get(0);
        ATADevice *ata_dev = ata_get_device(0);
        uint32_t test_lba = 0; // Mettez ici l'adresse LBA pour le test

        device->write(ata_dev, test_lba, 1, write_buffer);
        device->read(ata_dev, test_lba, 1, read_buffer);

        // Comparer les buffers
        bool is_equal = true;
        for (int i = 0; i < 512; ++i) {
            if (write_buffer[i] != read_buffer[i]) {
                printk("0x%x == 0x%x\n", write_buffer[i], read_buffer[i]);
                is_equal = false;
                break;
            } else {
                printk("0x%x == 0x%x\n", write_buffer[i], read_buffer[i]);
            }
        }

        // Afficher le résultat du test
        if (is_equal) {
            printk("ATA I/O Test success!\n");
        } else {
            printk("ATA I/O Test failure!\n");
        }
    }
    */

    // Display initrd files

    // list the contents of /

    // Todo: KFS-6
    printk("Initrd files:\n");
    Dirent *_d_node;

    VfsNode *node = initrd_fs->fs_root;

    vfs_opendir(node);
    while ((_d_node = vfs_readdir(node)) != NULL) {
        printk("Found Node: %s\n", _d_node->name);

        VfsNode *_f_node = vfs_finddir(node, _d_node->name);

        printk("Found Node: %s [%d]\n", _f_node->name, _f_node->flags);

        if (_f_node == NULL) {
            printk("Error: vfs_finddir failed\n");
            continue;
        }

        printk("Flags: %u\n", _f_node->flags);

        if ((_f_node->flags & VFS_DIRECTORY) != 0) {
            printk("Directory: %s\n", _d_node->name);

        } else if ((_f_node->flags & VFS_FILE) != 0) {
            printk("File: %s\n", _d_node->name);

            printk("Lenght: %u\n", _f_node->length);
            uint8_t *buffer = kmalloc(_f_node->length);

            memset(buffer, 0, _f_node->length);

            _f_node->fops.read(buffer, _f_node->length);

            printk("File content: %s\n", buffer);
            kfree(buffer);
        }
    }
    vfs_closedir(node);

    printk("Done!\n");
    kpause();

    // initrd_debug_read_disk();

    // uint32_t esp;
    // GET_ESP(esp);
    // tss_init(7, 0x10, esp);
    // kernel_log_info("LOG", "TSS");
    // switch_to_user_mode();

    // threads_test();
    // process_test();

    // uint32_t esp;

    // GET_ESP(esp);
    // printk("ESP: " _GREEN "0x%x\n" _END, esp);

    // tss_set_stack_pointer(esp);
    // tss_set_stack_segment(0x10);

    // switch_to_user_mode();

    // Todo: Fix priority
    // task_set_priority(pid_tmp, TASK_PRIORITY_LOW);
    // task_set_priority(pid_tmp2, TASK_PRIORITY_LOW);
    // task_set_priority(pid_tmp3, TASK_PRIORITY_LOW);

    // switch_to_user_mode();

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