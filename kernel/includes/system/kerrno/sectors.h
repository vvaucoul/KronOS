/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sectors.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/17 17:51:49 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/17 18:24:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERRNO_SECTOR_H
# define KERRNO_SECTOR_H

/*******************************************************************************
 *                              KERRNO SECTOR GDT                              *
 ******************************************************************************/

#define KERRNO_GDT_LIMIT 0
#define __KERRNO_GDT_LIMIT_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_GDT, KERRNO_GDT_LIMIT, "GDT Limit")

/*******************************************************************************
 *                              KERRNO SECTOR ISR                              *
 ******************************************************************************/

#define KERRNO_ISR_DBZ 0
#define KERRNO_ISR_DEBUG 1
#define KERRNO_ISR_NMI 2
#define KERRNO_ISR_BREAKPOINT 3
#define KERRNO_ISR_OVERFLOW 4
#define KERRNO_ISR_BOUND 5
#define KERRNO_ISR_INVALID_OPCODE 6
#define KERRNO_ISR_COPROCESSOR_NOT_AVAILABLE 7
#define KERRNO_ISR_DOUBLE_FAULT 8
#define KERRNO_ISR_COPROCESSOR_SEGMENT_OVERRUN 9
#define KERRNO_ISR_INVALID_TSS 10
#define KERRNO_ISR_SEGMENT_NOT_PRESENT 11
#define KERRNO_ISR_STACK_SEGMENT_FAULT 12
#define KERRNO_ISR_GENERAL_PROTECTION 13
#define KERRNO_ISR_PAGE_FAULT 14
#define KERRNO_ISR_UNKNOWN 15
#define KERRNO_ISR_COPROCESSOR_FAULT 16
#define KERRNO_ISR_ALIGNMENT_CHECK 17
#define KERRNO_ISR_MACHINE_CHECK 18
#define KERRNO_ISR_RESERVED 19

#define __KERRNO_ISR_DBZ_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_DBZ, "Division by zero")
#define __KERRNO_ISR_DEBUG_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_DEBUG, "Debug")
#define __KERRNO_ISR_NMI_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_NMI, "NMI")
#define __KERRNO_ISR_BREAKPOINT_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_BREAKPOINT, "Breakpoint")
#define __KERRNO_ISR_OVERFLOW_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_OVERFLOW, "Overflow")
#define __KERRNO_ISR_BOUND_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_BOUND, "BOUND")
#define __KERRNO_ISR_INVALID_OPCODE_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_INVALID_OPCODE, "Invalid Opcode")
#define __KERRNO_ISR_COPROCESSOR_NOT_AVAILABLE_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_COPROCESSOR_NOT_AVAILABLE, "Coprocessor Not Available")
#define __KERRNO_ISR_DOUBLE_FAULT_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_DOUBLE_FAULT, "Double Fault")
#define __KERRNO_ISR_COPROCESSOR_SEGMENT_OVERRUN_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_COPROCESSOR_SEGMENT_OVERRUN, "Coprocessor Segment Overrun")
#define __KERRNO_ISR_INVALID_TSS_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_INVALID_TSS, "Invalid TSS")
#define __KERRNO_ISR_SEGMENT_NOT_PRESENT_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_SEGMENT_NOT_PRESENT, "Segment Not Present")
#define __KERRNO_ISR_STACK_SEGMENT_FAULT_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_STACK_SEGMENT_FAULT, "Stack Segment Fault")
#define __KERRNO_ISR_GENERAL_PROTECTION_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_GENERAL_PROTECTION, "General Protection")
#define __KERRNO_ISR_PAGE_FAULT_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_PAGE_FAULT, "Page Fault")
#define __KERRNO_ISR_UNKNOWN_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_UNKNOWN, "Unknown")
#define __KERRNO_ISR_COPROCESSOR_FAULT_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_COPROCESSOR_FAULT, "Coprocessor Fault")
#define __KERRNO_ISR_ALIGNMENT_CHECK_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_ALIGNMENT_CHECK, "Alignment Check")
#define __KERRNO_ISR_MACHINE_CHECK_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_MACHINE_CHECK, "Machine Check")
#define __KERRNO_ISR_RESERVED_STRUCT __KERRNO_STRUCT(__KERRNO_SECTOR_ISR, KERRNO_ISR_RESERVED, "Reserved")

/* to continue for all errors */

#endif /* KERRNO_SECTOR_H */