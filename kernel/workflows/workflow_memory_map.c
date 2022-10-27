/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_memory_map.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/28 13:37:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/25 16:51:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>

#include <multiboot/multiboot.h>
#include <memory/memory_map.h>

/*******************************************************************************
 *                          DISPLAY KERNEL MEMORY MAP                          *
 ******************************************************************************/

void display_kernel_memory_map(void)
{
    kprintf(COLOR_CYAN "Kernel Memory Map:\n" COLOR_END);
    /* Kernel */
    kprintf("- " COLOR_YELLOW "Kernel" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END, KMAP.sections.kernel.kernel_start);
    kprintf(" End: " COLOR_GREEN "0x%x" COLOR_END, KMAP.sections.kernel.kernel_end);
    kprintf(" Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " bytes)\n" COLOR_END, KMAP.sections.kernel.kernel_length, KMAP.sections.kernel.kernel_length);

    /* Text */
    kprintf("- " COLOR_YELLOW "Text" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " bytes)\n" COLOR_END, KMAP.sections.text.text_addr_start, KMAP.sections.text.text_addr_end, KMAP.sections.text.text_length, KMAP.sections.text.text_length);

    /* Rodata */
    kprintf("- " COLOR_YELLOW "Rodata" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " bytes)\n" COLOR_END COLOR_END, KMAP.sections.rodata.rodata_addr_start, KMAP.sections.rodata.rodata_addr_end, KMAP.sections.rodata.rodata_length, KMAP.sections.rodata.rodata_length);

    /* Data */
    kprintf("- " COLOR_YELLOW "Data" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " bytes)\n" COLOR_END COLOR_END, KMAP.sections.data.data_addr_start, KMAP.sections.data.data_addr_end, KMAP.sections.data.data_length, KMAP.sections.data.data_length);

    /* Bss */
    kprintf("- " COLOR_YELLOW "Bss" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " bytes)\n" COLOR_END, KMAP.sections.bss.bss_addr_start, KMAP.sections.bss.bss_addr_end, KMAP.sections.bss.bss_length, KMAP.sections.bss.bss_length);

    /* Total */
    kprintf("- " COLOR_YELLOW "Total Memory" COLOR_END ": " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " KB)\n" COLOR_END, KMAP.total.total_memory_length, KMAP.total.total_memory_length);

    /* Available */
    kprintf("- " COLOR_YELLOW "Available" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " KB)\n" COLOR_END, KMAP.available.start_addr, KMAP.available.end_addr, KMAP.available.length, KMAP.available.length / 1024);

    kprintf("- " COLOR_YELLOW "Extended" COLOR_END " Start: " COLOR_GREEN "0x%x" COLOR_END " End: " COLOR_GREEN "0x%x" COLOR_END " Length: " COLOR_GREEN "0x%x " COLOR_END "(" COLOR_GREEN "%u" COLOR_END " KB)\n" COLOR_END, KMAP.available_extended.start_addr, KMAP.available_extended.end_addr, KMAP.available_extended.length, KMAP.available_extended.length / 1024);
}