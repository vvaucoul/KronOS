/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sections.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 16:18:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/04 16:49:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/sections.h>

static void __print_single_section(const char *__section_name, uint8_t *_addr, bool __nl)
{
    kprintf(""COLOR_CYAN"%s\n"COLOR_END, __section_name);
    kprintf("- START: "COLOR_GREEN"0x%x"COLOR_END"\n", _addr);
       if (__nl)
        kprintf("\n");
}

static void __print_section(const char *__section_name, uint8_t *_addr_start, uint8_t *_addr_end, bool __nl)
{
    const uint32_t __res = ((uint32_t)_addr_end - (uint32_t)_addr_start);

    //kprintf("- %s: "COLOR_GREEN"0x%x"COLOR_END"\n", _s_start, _addr_start);
    //kprintf("- %s: "COLOR_GREEN"0x%x"COLOR_END"\n", _s_end, _addr_end);
    //kprintf("- %s: "COLOR_GREEN"0x%x"COLOR_END" | "COLOR_GREEN"%d"COLOR_END" Bytes\n", "TOTAL", __res, __res);

    kprintf(""COLOR_CYAN"%s\n"COLOR_END, __section_name);
    kprintf("- START: "COLOR_GREEN"0x%x"COLOR_END" | END: "COLOR_GREEN"0x%x"COLOR_END" | TOTAL: "COLOR_GREEN"0x%x"COLOR_END" - "COLOR_GREEN"%d"COLOR_END" Bytes\n", _addr_start, _addr_end, __res, __res);
    if (__nl)
        kprintf("\n");
}

void display_sections(void)
{
    kprintf(COLOR_GREEN "[SECTIONS]\n" COLOR_END);
    __print_section("KERNEL", &__kernel_start, &__kernel_end, false);
    __print_section("KERNEL_TEXT", &__kernel_text_section_start, &__kernel_text_section_end, false);
    __print_section("KERNEL_DATA", &__kernel_data_section_start, &__kernel_data_section_end, false);
    __print_section("KERNEL_RODATA", &__kernel_rodata_section_start, &__kernel_rodata_section_end, false);
    __print_section("KERNEL_BSS", &__kernel_bss_section_start, &__kernel_bss_section_end, false);
    __print_section("MULTIBOOT", &__multiboot_start, &__multiboot_end, false);
    __print_single_section("KERNEL_PHYSICAL_START", &__kernel_physical_memory_start, false);
    __print_single_section("KERNEL_VIRTUAL_START", &__kernel_virtual_memory_start, false);
}