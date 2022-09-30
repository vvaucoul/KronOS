/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_memory_map.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/28 13:37:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/28 13:50:35 by vvaucoul         ###   ########.fr       */
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