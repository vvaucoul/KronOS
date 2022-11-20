/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 18:36:20 by vvaucoul         ###   ########.fr       */
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
#include <system/bsod.h>

#include <drivers/keyboard.h>
#include <drivers/display.h>
#include <drivers/vbe.h>

#include <multiboot/multiboot.h>

#include <memory/memory.h>
#include <memory/memory_map.h>
#include <memory/kheap.h>
#include <memory/paging.h>

// #include <memory/pmm.h>
// #include <memory/smp.h>

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
    // printk(_RED "Red: Test <0x%08x>\n", &magic_number, 8);
    ksh_header();
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
        display_multiboot_infos();
    }
    gdt_install();
    kernel_log_info("LOG", "GDT");
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

    {
        // WIP
        // init_vbe_mode();
        // kpause();
        // Init VBE if video mode is VBE (0)
    }

    display_kernel_memory_map();

    // display_sections();
    // printk("End addr: 0x%x\n", &__kernel_section_end);
    // kpause();

    init_paging();
    kernel_log_info("LOG", "PAGING");

    // kpause();
    // Require x64 Broadwell Intel (5th Gen) or higher
    // smp_init();
    // kernel_log_info("LOG", "SMP");
    // kpause();

    // display_kernel_memory_map();

    // printk("Kernel start addr: " _GREEN "0x%x" _END "\n", KMAP.sections.kernel.kernel_end);
    // printk("Kernel end addr: " _GREEN "0x%x" _END "\n", KMAP.available_extended.end_addr);
    // printk("Kernel length: " _GREEN "0x%x (%u Mo)" _END "\n", KMAP.available_extended.length, KMAP.available_extended.length / 1024 / 1024);

    // pmm_init(KMAP.available.start_addr, KMAP.available.length);
    // pmm_init(KMAP.available.start_addr, KMAP.available.length + KMAP.available_extended.length);

    // Todo: Fix memory map & pmm & multiboot kernel memory map

    // pmm_loader_init();
    // pmm_init_region(KMAP.available.start_addr, PMM_BLOCK_SIZE * 10);

    // kernel_log_info("LOG", "PMM");
    // pmm_test();
    // kpause();

    /*
    ** Init Kernel Heap
    ** 20 * 4096 = 80 Ko
    */

    // printk("PMM Blocks: %u\n", pmm_get_max_blocks());
    // printk("PMM Used Blocks: %u\n", pmm_get_used_blocks());
    // printk("PMM Size: %u Mo\n", pmm_get_memory_size() / 1024 / 1024);

    // void *kheap_start_addr = pmm_alloc_blocks(PHYSICAL_MEMORY_BLOCKS);
    // void *kheap_end_addr = (void *)(kheap_start_addr + ((uint32_t)pmm_get_next_available_block() * (PMM_BLOCK_SIZE)));

    // printk("Kernel Heap start addr: " _GREEN "0x%x" _END "\n", kheap_start_addr);
    // printk("Kernel Heap end addr: " _GREEN "0x%x" _END "\n", kheap_end_addr);
    // printk("Kernel Heap Size: " _GREEN "%u (%u Ko)" _END "\n", (uint32_t)kheap_end_addr - (uint32_t)kheap_start_addr, ((uint32_t)kheap_end_addr - (uint32_t)kheap_start_addr) / 1024);
    // kpause();
    // kheap_test();
    // kpause();

    // if ((kheap_init(kheap_start_addr, kheap_end_addr)) == 1)
    //     __PANIC("Error: kheap_init failed");
    // kernel_log_info("LOG", "KHEAP");

    // kheap_test();

    /*
    ** Init Kernel Paging
    */
    return (0);
}
int init_multiboot_kernel(hex_t magic_number, hex_t addr)
{
    __UNUSED(magic_number);
    __UNUSED(addr);
    // if (multiboot_check_magic_number(magic_number) == false)
    //     return (1);
    // __multiboot_info = (MultibootInfo *)(addr);
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

    printk("Heap Test\n");

    char *str = (char *)kmalloc(13);
    bzero(str, 13);
    memcpy(str, "Hello World!", 12);

    printk("str: %s\n", str);
    printk("Size: %u\n", ksize(str));
    kfree(str);

    void *ptr = kmalloc(2);
    printk("ptr: 0x%08x\n", ptr);
    printk("Size: %u\n", ksize(ptr));

    printk("vaddr: 0x%08x\n", (&ptr) + 0xC0000000);
    // printk("Size: %u\n", vsize(vaddr));

    printk("End\n");
    // kpause();

    // kpause();
    // kheap_test();
    kronos_shell();
    return (0);
}