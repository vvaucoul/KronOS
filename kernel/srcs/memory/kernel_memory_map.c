/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel_memory_map.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 12:06:57 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/13 15:08:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multiboot/multiboot.h>
#include <memory/memory_map.h>

KERNEL_MEMORY_MAP kernel_memory_map;

int get_kernel_memory_map(MultibootInfo *multiboot_info)
{
    if (multiboot_info == NULL)
        return (1);
    else if ((&kernel_memory_map.kernel) == NULL)
        return (1);

    KMAP.kernel.kernel_start = (uint32_t)&__kernel_section_start;
    KMAP.kernel.kernel_end = (uint32_t)&__kernel_section_end;
    KMAP.kernel.kernel_length = KMAP.kernel.kernel_end - KMAP.kernel.kernel_start;

    KMAP.text.text_addr_start = (uint32_t)&__kernel_text_section_start;
    KMAP.text.text_addr_end = (uint32_t)&__kernel_text_section_end;
    KMAP.text.text_length = KMAP.text.text_addr_end - KMAP.text.text_addr_start;

    KMAP.rodata.rodata_addr_start = (uint32_t)&__kernel_rodata_section_start;
    KMAP.rodata.rodata_addr_end = (uint32_t)&__kernel_rodata_section_end;
    KMAP.rodata.rodata_length = KMAP.rodata.rodata_addr_end - KMAP.rodata.rodata_addr_start;

    KMAP.data.data_addr_start = (uint32_t)&__kernel_data_section_start;
    KMAP.data.data_addr_end = (uint32_t)&__kernel_data_section_end;
    KMAP.data.data_length = KMAP.data.data_addr_end - KMAP.data.data_addr_start;

    KMAP.bss.bss_addr_start = (uint32_t)&__kernel_bss_section_start;
    KMAP.bss.bss_addr_end = (uint32_t)&__kernel_bss_section_end;
    KMAP.bss.bss_length = KMAP.bss.bss_addr_end - KMAP.bss.bss_addr_start;

    KMAP.total.total_memory_length = multiboot_info->mem_upper + multiboot_info->mem_lower;

    for (uint32_t i = 0; i < multiboot_info->mmap_length; i += sizeof(MultibootMemoryMap))
    {
        MultibootMemoryMap *mmap = (MultibootMemoryMap *)(multiboot_info->mmap_addr + i);
        if (mmap->type != __MULTIBOOT_MEMORY_AVAILABLE)
            continue;
        else
        {
            KMAP.available.start_addr = KMAP.kernel.kernel_end + 1024 * 1024;
            KMAP.available.end_addr = mmap->addr_low + mmap->len_low;
            KMAP.available.length = KMAP.available.end_addr - KMAP.available.start_addr;
            return (0);
        }
    }

    return (1);
}

void display_kernel_memory_map(void)
{
    kprintf(COLOR_CYAN "Kernel Memory Map:\n" COLOR_END);
    /* Kernel */
    kprintf("- " COLOR_YELLOW "Kernel" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END, KMAP.kernel.kernel_start);
    kprintf(" End: " COLOR_GREEN "0x%x" COLOR_END, KMAP.kernel.kernel_end);
    kprintf(" Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " bytes)\n" COLOR_END, KMAP.kernel.kernel_length, KMAP.kernel.kernel_length);

    /* Text */
    kprintf("- " COLOR_YELLOW "Text" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " bytes)\n" COLOR_END, KMAP.text.text_addr_start, KMAP.text.text_addr_end, KMAP.text.text_length, KMAP.text.text_length);

    /* Rodata */
    kprintf("- " COLOR_YELLOW "Rodata" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " bytes)\n" COLOR_END COLOR_END, KMAP.rodata.rodata_addr_start, KMAP.rodata.rodata_addr_end, KMAP.rodata.rodata_length, KMAP.rodata.rodata_length);
    
    /* Data */
    kprintf("- " COLOR_YELLOW "Data" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " bytes)\n" COLOR_END COLOR_END, KMAP.data.data_addr_start, KMAP.data.data_addr_end, KMAP.data.data_length, KMAP.data.data_length);
    
    /* Bss */
    kprintf("- " COLOR_YELLOW "Bss" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " bytes)\n" COLOR_END, KMAP.bss.bss_addr_start, KMAP.bss.bss_addr_end, KMAP.bss.bss_length, KMAP.bss.bss_length);
    
    /* Total */
    kprintf("- " COLOR_YELLOW "Total Memory" COLOR_END ": " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " KB)\n" COLOR_END, KMAP.total.total_memory_length, KMAP.total.total_memory_length);
    
    /* Available */
    kprintf("- " COLOR_YELLOW "Available" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " KB)\n" COLOR_END, KMAP.available.start_addr, KMAP.available.end_addr, KMAP.available.length, KMAP.available.length / 1024);
}