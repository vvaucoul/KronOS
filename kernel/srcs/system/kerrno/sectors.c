/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sectors.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/17 16:21:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/17 18:24:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/kerrno.h>
#include <kernel.h>

static void __kerrno_sector_gdt(void)
{
    __kerrno_table[__KERRNO_SECTOR_GDT][KERRNO_GDT_LIMIT] = __KERRNO_GDT_LIMIT_STRUCT;
}

static void __kerrno_sector_isr(void)
{
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_DBZ] = __KERRNO_ISR_DBZ_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_DEBUG] = __KERRNO_ISR_DEBUG_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_NMI] = __KERRNO_ISR_NMI_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_BREAKPOINT] = __KERRNO_ISR_BREAKPOINT_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_OVERFLOW] = __KERRNO_ISR_OVERFLOW_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_BOUND] = __KERRNO_ISR_BOUND_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_INVALID_OPCODE] = __KERRNO_ISR_INVALID_OPCODE_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_COPROCESSOR_NOT_AVAILABLE] = __KERRNO_ISR_COPROCESSOR_NOT_AVAILABLE_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_DOUBLE_FAULT] = __KERRNO_ISR_DOUBLE_FAULT_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_COPROCESSOR_SEGMENT_OVERRUN] = __KERRNO_ISR_COPROCESSOR_SEGMENT_OVERRUN_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_INVALID_TSS] = __KERRNO_ISR_INVALID_TSS_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_SEGMENT_NOT_PRESENT] = __KERRNO_ISR_SEGMENT_NOT_PRESENT_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_STACK_SEGMENT_FAULT] = __KERRNO_ISR_STACK_SEGMENT_FAULT_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_GENERAL_PROTECTION] = __KERRNO_ISR_GENERAL_PROTECTION_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_PAGE_FAULT] = __KERRNO_ISR_PAGE_FAULT_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_UNKNOWN] = __KERRNO_ISR_UNKNOWN_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_COPROCESSOR_FAULT] = __KERRNO_ISR_COPROCESSOR_FAULT_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_ALIGNMENT_CHECK] = __KERRNO_ISR_ALIGNMENT_CHECK_STRUCT;
    __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_MACHINE_CHECK] = __KERRNO_ISR_MACHINE_CHECK_STRUCT;
    for (int i = KERRNO_ISR_RESERVED; i < 31; i++)
        __kerrno_table[__KERRNO_SECTOR_ISR][KERRNO_ISR_RESERVED + i] = __KERRNO_ISR_RESERVED_STRUCT;
}


void __kerrno_sectors(void)
{
    void (*pf[2])(void);
    const size_t __kerrno_sector_functions_size = sizeof(pf) / sizeof(void *);

    pf[0] = &__kerrno_sector_gdt;
    pf[1] = &__kerrno_sector_isr;

    for (size_t i = 0; i < __kerrno_sector_functions_size; i++)
        pf[i]();
}