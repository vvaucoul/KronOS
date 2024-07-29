/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bios.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/18 19:55:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 23:31:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BIOS_H
#define BIOS_H

#include <kernel.h>
#include <system/pit.h>
#include <system/isr.h>

/* Base address for BIOS interrupt handling */
#define BIOS_DEFAULT_MEMORY 0x5000
#define BIOS32_START 0x5000
#define BIOS32_END 0x6000

/* Define the memory locations for BIOS32 service */
#define BIOS32_INT_NUMBER (BIOS_DEFAULT_MEMORY + 0x100)
#define BIOS32_REGS16_INPUT (BIOS_DEFAULT_MEMORY + 0x200)
#define BIOS32_REGS16_INPUT_PTR (BIOS_DEFAULT_MEMORY + 0x210)
#define BIOS32_REGS16_OUTPUT (BIOS_DEFAULT_MEMORY + 0x220)
#define BIOS32_REGS16_OUTPUT_PTR (BIOS_DEFAULT_MEMORY + 0x230)

extern void *bios32_gdt_ptr;
extern void *bios32_gdt_entries;
extern void *bios32_idt_ptr;
extern void *bios32_in_reg16_ptr;
extern void *bios32_out_reg16_ptr;
extern void *bios32_int_number_ptr;

#define REBASE_ADDRESS(x) (void *)(0x7c00 + (void *)x - (uint32_t)BIOS32_START)

extern void init_bios32();
// extern void int86(uint8_t interrupt, regs16_t *in, regs16_t *out);

#endif /* !BIOS_H */