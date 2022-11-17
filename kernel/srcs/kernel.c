/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:55:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/17 02:02:08 by vvaucoul         ###   ########.fr       */
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

    // char buffer[PAGE_FAULT_BUFFER_SIZE];
    // uint32_t faulting_address = 0x10000;
    // kuitoa_base(faulting_address, 16, buffer);
    // kprintf("Buffer: 0x%s\n", buffer);
    // kpause();

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
    enable_fpu();
    kernel_log_info("LOG", "FPU");
    // kpause();

    // Require x64 Broadwell Intel (5th Gen) or higher
    // smp_init();
    // kernel_log_info("LOG", "SMP");
    // kpause();

    kprintf("Kernel start addr: " COLOR_GREEN "0x%x" COLOR_END "\n", KMAP.sections.kernel.kernel_end);
    kprintf("Kernel end addr: " COLOR_GREEN "0x%x" COLOR_END "\n", KMAP.available_extended.end_addr);
    kprintf("Kernel length: " COLOR_GREEN "0x%x (%u Mo)" COLOR_END "\n", KMAP.available_extended.length, KMAP.available_extended.length / 1024 / 1024);

    pmm_init(KMAP.available_extended.start_addr, KMAP.available_extended.length);
    pmm_loader_init();
    kernel_log_info("LOG", "PMM");

    // pmm_test();

    /*
    ** Init Kernel Heap
    ** 20 * 4096 = 81920 = 80 Ko
    */

   kprintf("PMM Blocks: %u\n", pmm_get_max_blocks());
   kprintf("PMM Used Blocks: %u\n", pmm_get_used_blocks());
   kprintf("PMM Size: %u Mo\n", pmm_get_memory_size() / 1024 / 1024);

   void *kheap_start_addr = pmm_alloc_blocks(PHYSICAL_MEMORY_BLOCKS);
   void *kheap_end_addr = (void *)(kheap_start_addr + ((uint32_t)pmm_get_next_available_block() * (PMM_BLOCK_SIZE)));

   kprintf("Kernel Heap start addr: " COLOR_GREEN "0x%x" COLOR_END "\n", kheap_start_addr);
   kprintf("Kernel Heap end addr: " COLOR_GREEN "0x%x" COLOR_END "\n", kheap_end_addr);
   kprintf("Kernel Heap Size: " COLOR_GREEN "%u (%u Ko)" COLOR_END "\n", (uint32_t)kheap_end_addr - (uint32_t)kheap_start_addr, ((uint32_t)kheap_end_addr - (uint32_t)kheap_start_addr) / 1024);

   if ((kheap_init(kheap_start_addr, kheap_end_addr)) == 1)
       __PANIC("Error: kheap_init failed");
   kernel_log_info("LOG", "KHEAP");

   // kheap_test();

   /*
   ** Init Kernel Paging
   */

   init_paging();
   kpause();
   kernel_log_info("LOG", "PAGING");
   kpause();
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
        kprintf("\n");
    ASM_STI();
    // kpause();
    // __PANIC("PANIC TEST");

    // kheap_test();
    // kpause();

    uchar_t *ptr = kmalloc(4);
    ptr[0] = 'A';
    ptr[1] = 'B';
    ptr[2] = 'C';
    ptr[3] = 0;
    kprintf("ptr = %s\n", ptr);
    kfree(ptr);

    uint32_t i = 0;
    const uint32_t alloc_size = 1024;

    while (i < 100)
    {
        ksh_clear();
        kprintf("\n" COLOR_CYAN "[%d]" COLOR_END ", Alloc " COLOR_GREEN "%d" COLOR_END " bytes\n", i, alloc_size);
        kprintf("Test: 1\n");
        void *ptr = kmalloc(alloc_size);
        kprintf("Test: 2\n");

        if (ptr == NULL)
        {
            kprintf(COLOR_RED "Error: ptr is NULL\n" COLOR_END);
            return (0);
        }
        else
        {
            kprintf("Test: 3\n");
            kbzero(ptr, alloc_size);
            kprintf("ptr = " COLOR_GREEN "%p" COLOR_END "\n", ptr);
        }
        kprintf("Allocated " COLOR_GREEN "[%u]" COLOR_END " bytes\n", alloc_size);
        ++i;
        timer_wait(10);
    }

    kpause();
    kronos_shell();
    return (0);
}