/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tss.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 18:56:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/23 15:45:02 by vvaucoul         ###   ########.fr       */
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
#define TSS_KERNEL_ACCESS 0b11100101 // 0xE9

#define TSS_SIZE 0x02

typedef volatile struct __tss {
    unsigned short link;
    unsigned short link_h;

    unsigned long esp0;
    unsigned short ss0;
    unsigned short ss0_h;

    unsigned long esp1;
    unsigned short ss1;
    unsigned short ss1_h;

    unsigned long esp2;
    unsigned short ss2;
    unsigned short ss2_h;

    unsigned long cr3;
    unsigned long eip;
    unsigned long eflags;

    unsigned long eax;
    unsigned long ecx;
    unsigned long edx;
    unsigned long ebx;

    unsigned long esp;
    unsigned long ebp;

    unsigned long esi;
    unsigned long edi;

    unsigned short es;
    unsigned short es_h;

    unsigned short cs;
    unsigned short cs_h;

    unsigned short ss;
    unsigned short ss_h;

    unsigned short ds;
    unsigned short ds_h;

    unsigned short fs;
    unsigned short fs_h;

    unsigned short gs;
    unsigned short gs_h;

    unsigned short ldt;
    unsigned short ldt_h;

    unsigned short trap;
    unsigned short iomap;

} __attribute__((packed)) tss_entry_t;

extern tss_entry_t tss_entry;

extern void tss_flush(tss_entry_t *tss_entry);
// extern void tss_flush();
extern void tss_init(uint32_t idx, uint32_t kss, uint32_t kesp);
extern void tss_set_stack_segment(uint32_t kss);
extern void tss_set_stack_pointer(uint32_t kesp);

#endif /* !TSS_H */