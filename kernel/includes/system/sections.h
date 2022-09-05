/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sections.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 16:11:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/05 02:28:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SECTIONS_H
#define SECTIONS_H

#include <kernel.h>

extern uint8_t __kernel_start;
extern uint8_t __kernel_end;

extern uint8_t __kernel_text_section_start;
extern uint8_t __kernel_text_section_end;

extern uint8_t __kernel_data_section_start;
extern uint8_t __kernel_data_section_end;

extern uint8_t __kernel_rodata_section_start;
extern uint8_t __kernel_rodata_section_end;

extern uint8_t __kernel_bss_section_start;
extern uint8_t __kernel_bss_section_end;

extern uint8_t __kernel_virtual_memory_start;
extern uint8_t __kernel_physical_memory_start;

extern uint8_t __multiboot_start;
extern uint8_t __multiboot_end;

extern void display_sections(void);

#endif /* SECTIONS */