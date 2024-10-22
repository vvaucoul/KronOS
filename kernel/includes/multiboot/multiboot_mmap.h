/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot_mmap.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 11:33:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/21 15:16:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIBOOT_MMAP_H
#define MULTIBOOT_MMAP_H

#include <stdint.h>

#define KERNEL_STACK_MARKER 0x4B52304E // KRONOST (KRONOS STACK)
#define MAX_MEMORY_SECTIONS 10

#define MULTIBOOT_MEMORY_AVAILABLE 1

// Structure describing an available memory section
typedef struct memory_section {
	uint64_t addr;
	uint64_t len;
} memory_section_t;

int get_available_memory_sections(memory_section_t *sections, int max_sections, void *mb_info);
uint32_t get_available_memory(void *mb_info_ptr);

// Linker defined symbols
// extern uint32_t __kernel_text_section_start;
// extern uint32_t __kernel_text_section_end;
// extern uint32_t __kernel_data_section_start;
// extern uint32_t __kernel_data_section_end;
// extern uint32_t __kernel_rodata_section_start;
// extern uint32_t __kernel_rodata_section_end;
// extern uint32_t __kernel_bss_section_start;
// extern uint32_t __kernel_bss_section_end;
// extern uint32_t __kernel_section_start;
// extern uint32_t __kernel_section_end;

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

#endif /* !MULTIBOOT_MMAP_H */