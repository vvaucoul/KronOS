/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/24 17:49:29 by vvaucoul         ###   ########.fr       */
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

#include <multiboot/multiboot.h>

#include <memory/memory.h>
#include <memory/memory_map.h>
#include <memory/pmm.h>
#include <memory/kheap.h>
#include <memory/smp.h>

#include <workflows/workflows.h>

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

void kernel_log_info(const char *part, const char *name)
{
    if (__DISPLAY_INIT_LOG__)
        kprintf(COLOR_YELLOW "[%s] " COLOR_END "- " COLOR_GREEN "[INIT] " COLOR_CYAN "%s " COLOR_END "\n", part, name);
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
    else
    {
        __multiboot_info = (MultibootInfo *)((hex_t *)((hex_t)addr + KERNEL_VIRTUAL_BASE));
        assert(__multiboot_info == NULL);
        if (multiboot_init(__multiboot_info))
            __PANIC("Error: multiboot_init failed");
        kernel_log_info("LOG", "MULTIBOOT");
        if (get_kernel_memory_map(__multiboot_info))
            __PANIC("Error: kernel memory map failed");
        if (get_user_memory_map(__multiboot_info))
            __PANIC("Error: user memory map failed");
        kernel_log_info("LOG", "KERNEL MEMORY MAP");
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

    // Require x64 Broadwell Intel (5th Gen) or higher
    // smp_init();
    // kernel_log_info("LOG", "SMP");
    // kpause();

    kprintf("Kernel start addr: " COLOR_GREEN "0x%x" COLOR_END "\n", KMAP.available.start_addr);
    kprintf("Kernel end addr: " COLOR_GREEN "0x%x" COLOR_END "\n", KMAP.available.end_addr);
    kprintf("Kernel length: " COLOR_GREEN "0x%x (%u Mo)" COLOR_END "\n", KMAP.available.length, KMAP.available.length / 1024 / 1024);

    kprintf("User start addr: " COLOR_GREEN "0x%x" COLOR_END "\n", UMAP.available.start_addr);
    kprintf("User end addr: " COLOR_GREEN "0x%x" COLOR_END "\n", UMAP.available.end_addr);
    kprintf("User length: " COLOR_GREEN "0x%x (%u Mo)" COLOR_END "\n", UMAP.available.length, UMAP.available.length / 1024 / 1024);

    kprintf("\nLoader\n");
    pmm_loader_init(__multiboot_info);
    kprintf("\nLoader End\n");
    kpause();

    pmm_init(KMAP.available.start_addr, KMAP.available.length);
    // kpause();
    // Alloc 4096 * 10 -> 40 Ko blocks of memory
    // pmm_init_region(0x0, PMM_BLOCK_SIZE * 100); // 0 => TMP, le temps de trouver pourquoi...
    // pmm_init_region((KMAP.available.start_addr - KERNEL_VIRTUAL_BASE), PMM_BLOCK_SIZE * 10);
    kpause();

    // uint32_t _i_block = pmm_get_next_available_block();
    // kprintf("Next Block: %u\n", _i_block);

    // void *block = pmm_alloc_block();
    // kprintf("New Block: 0x%x\n", block);

    // _i_block = pmm_get_next_available_block();
    // kprintf("Next Block: %u\n", _i_block);

    // void *blocks = pmm_alloc_blocks(5);
    // for (uint32_t i = 0; i < 5; i ++)
    // {
    //     kprintf("New Block: 0x%x\n", blocks + (i * PMM_BLOCK_SIZE));
    // }
    // _i_block = pmm_get_next_available_block();
    // kprintf("Next Block: %u\n", _i_block);

    // kpause();
    pmm_test();
    kernel_log_info("LOG", "PMM");
    kpause();

    /*
    ** Init Kernel Heap with 8MB
    - 20 * 4096 = 81920
    */

    void *kheap_start_addr = pmm_alloc_blocks(PHYSICAL_MEMORY_BLOCKS);
    void *kheap_end_addr = (void *)(kheap_start_addr + ((uint32_t)pmm_get_next_available_block() * (PMM_BLOCK_SIZE)));

    kprintf("Kernel Heap start addr: " COLOR_GREEN "0x%x" COLOR_END "\n", kheap_start_addr);
    kprintf("Kernel Heap end addr: " COLOR_GREEN "0x%x" COLOR_END "\n", kheap_end_addr);

    if ((kheap_init(kheap_start_addr, kheap_end_addr)) == 1)
        __PANIC("Error: kheap_init failed");
    kpause();
    kheap_test();
    kpause();
    kernel_log_info("LOG", "KHEAP");

    // kheap_test();

    // kpause();
    init_paging();
    kernel_log_info("LOG", "PAGING");
    // kpause();

    enable_fpu();
    kernel_log_info("LOG", "FPU");
    // kpause();
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
    kprintf("\n");
    ASM_STI();
    // kpause();
    // __PANIC("PANIC TEST");

    // kheap_test();
    // kpause();

    /*
       uchar_t *ptr = kmalloc(1024);
       ptr[0] = 'A';
       ptr[1] = 'B';
       ptr[2] = 'C';
       kprintf("ptr = %s\n", ptr);
       kfree(ptr);

       uint32_t i = 0;
       const uint32_t alloc_size = (1024);

       while (i < 100)
       {
           ksh_clear();
           kprintf("\n" COLOR_CYAN "[%d]" COLOR_END ", Alloc " COLOR_GREEN "%d" COLOR_END " bytes\n", i, alloc_size);
           void *ptr = kmalloc(alloc_size);

           if (ptr == NULL)
           {
               kprintf(COLOR_RED "Error: ptr is NULL\n" COLOR_END);
               break;
               return (0);
           }
           else
           {
               kbzero(ptr, alloc_size);
               kprintf("ptr = " COLOR_GREEN "%p" COLOR_END "\n", ptr);
           }
           kprintf("Allocated " COLOR_GREEN "[%u]" COLOR_END " bytes\n", alloc_size);
           ++i;
           timer_wait(10);
       }
       */

    kronos_shell();
    return (0);
}