/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bios.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/18 19:55:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 22:34:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BIOS_H
#define BIOS_H

#include <kernel.h>
#include <system/pit.h>

#define BIOS_DEFAULT_MEMORY 0x7c00
#define BIOS_MEMORY 0x7E00

extern void BIOS32_START();
extern void BIOS32_END();

extern void *bios32_gdt_ptr;
extern void *bios32_gdt_entries;
extern void *bios32_idt_ptr;
extern void *bios32_in_reg16_ptr;
extern void *bios32_out_reg16_ptr;
extern void *bios32_int_number_ptr;

#define REBASE_ADDRESS(x) (void *)(0x7c00 + (void *)x - (uint32_t)BIOS32_START)

extern void init_bios32();
extern void int86(uint8_t interrupt, regs16_t *in, regs16_t *out);

#endif /* !BIOS_H */