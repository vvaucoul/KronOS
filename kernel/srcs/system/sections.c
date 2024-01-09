/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sections.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 16:18:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/sections.h>

uint32_t __kernel_section_start;
uint32_t __kernel_text_section_start;
uint32_t __kernel_text_section_end;
uint32_t __kernel_data_section_start;
uint32_t __kernel_data_section_end;
uint32_t __kernel_rodata_section_start;
uint32_t __kernel_rodata_section_end;
uint32_t __kernel_bss_section_start;
uint32_t __kernel_bss_section_end;
uint32_t __kernel_section_end;

uint32_t __multiboot_start;
uint32_t __multiboot_end;

uint32_t __kernel_physical_memory_start;
uint32_t __kernel_physical_memory_end;
uint32_t __kernel_virtual_memory_start;
uint32_t __kernel_virtual_memory_end;

static void __print_single_section(const char *__section_name, uint32_t *_addr, bool __nl)
{
    printk("" _CYAN "%s\n" _END, __section_name);
    printk("- START: " _GREEN "0x%x" _END "\n", _addr);
    if (__nl)
        printk("\n");
}

static void __print_section(const char *__section_name, uint32_t *_addr_start, uint32_t *_addr_end, bool __nl)
{
    const uint32_t __res = ((uint32_t)_addr_end - (uint32_t)_addr_start);

    printk("" _CYAN "%s\n" _END, __section_name);
    printk("- START: " _GREEN "0x%x" _END " | END: " _GREEN "0x%x" _END " | TOTAL: " _GREEN "0x%x" _END " - " _GREEN "%d" _END " Bytes\n", _addr_start, _addr_end, __res, __res);
    if (__nl)
        printk("\n");
}

void display_sections(void)
{
    printk(_GREEN "[SECTIONS]\n" _END);
    __print_section("KERNEL", &__kernel_section_start, &__kernel_section_end, false);
    __print_section("KERNEL_TEXT", &__kernel_text_section_start, &__kernel_text_section_end, false);
    __print_section("KERNEL_DATA", &__kernel_data_section_start, &__kernel_data_section_end, false);
    __print_section("KERNEL_RODATA", &__kernel_rodata_section_start, &__kernel_rodata_section_end, false);
    __print_section("KERNEL_BSS", &__kernel_bss_section_start, &__kernel_bss_section_end, false);
    __print_section("MULTIBOOT", &__multiboot_start, &__multiboot_end, false);
    __print_single_section("KERNEL_PHYSICAL_START", &__kernel_physical_memory_start, false);
    __print_single_section("KERNEL_VIRTUAL_START", &__kernel_virtual_memory_start, false);
}