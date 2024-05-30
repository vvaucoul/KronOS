/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tss.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 18:56:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/28 17:30:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TSS_H
#define TSS_H

#include <kernel.h>


/* Kernel tss, access(0xE9 = 1 11 0 1 0 0 1)
    1   present
    11  ring 3
    0   should always be 1, why 0? may be this value doesn't matter at all
    1   code?
    0   can not be executed by ring lower or equal to DPL,
    0   not readable
    1   access bit, always 0, cpu set this to 1 when accessing this sector(why 0 now?)
*/
#define TSS_KERNEL_ACCESS 0xE9
#define TSS_SIZE 0x01

typedef struct {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap;
} __attribute__((packed)) tss_entry_t;

extern tss_entry_t tss_entry;

extern void tss_flush(void);
extern void tss_init(uint32_t idx, uint32_t ss0, uint32_t esp0);
extern void tss_set_stack_segment(uint32_t ss0);
extern void tss_set_stack_pointer(uint32_t esp0);
extern void tss_set_stack(uint32_t ss0, uint32_t esp0);
extern void print_tss(void);
extern void check_gdt_tss(void);

#endif /* !TSS_H */