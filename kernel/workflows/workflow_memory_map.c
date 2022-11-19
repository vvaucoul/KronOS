/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_memory_map.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/28 13:37:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/18 15:40:03 by vvaucoul         ###   ########.fr       */
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
    __WORKFLOW_HEADER();
    for (uint32_t i = 0; i < MMAP_SIZE; i++)
    {
        kprintf(_GREEN "[%u]"_END
                       ": "_CYAN
                       "0x%08x"_END
                       " - "_CYAN
                       "0x%08x"_END
                       " ["_GREEN
                       "%uKo"_END
                       "] ("_CYAN
                       "%s"_END
                       ")\n",
                i, memory_map[i].addr_low, memory_map[i].addr_low + memory_map[i].len_low, memory_map[i].len_low / 1024, memory_map[i].type == 1 ? "Free" : "Reserved");
    }
    __WORKFLOW_FOOTER();
}