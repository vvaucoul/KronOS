/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tss.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 18:56:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/29 18:57:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TSS_H
#define TSS_H

typedef volatile struct __tss
{
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

} __attribute__((packed)) tss;

#endif /* TSS_H */