/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/21 12:19:08 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 12:44:19 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gdt.h"

GDTPtr gdtr;

static void segment_descriptor_init(GDTEntry *descriptor,
                                    uint32_t base_addr, uint32_t limit,
                                    uint8_t access, uint8_t flags)
{
    descriptor->limit_low = (limit & 0xffff);
    descriptor->base_low = (base_addr & 0xffff);
    descriptor->base_middle = (base_addr & 0xff0000) >> 16;
    descriptor->access = access;
    descriptor->granularity = (limit & 0xf0000) >> 16;
    descriptor->flags = (flags & 0x0f);
    descriptor->base_high = (base_addr & 0xff000000) >> 24;
}

void gdt_init(void)
{
    GDTEntry *gdt = (GDTEntry *)GDT_ADDR;

    segment_descriptor_init(&gdt[0], 0x0, 0x0, 0x0, 0x0);

    segment_descriptor_init(&gdt[1], 0x0, 0xffffffff, 0x9a, 0x0c);
    segment_descriptor_init(&gdt[2], 0x0, 0xffffffff, 0x92, 0x0c);
    segment_descriptor_init(&gdt[3], 0x0, 0xffffffff, 0x96, 0x0c);

    segment_descriptor_init(&gdt[4], 0x0, 0xffffffff, 0xFa, 0x0c);
    segment_descriptor_init(&gdt[5], 0x0, 0xffffffff, 0xF2, 0x0c);
    segment_descriptor_init(&gdt[6], 0x0, 0xffffffff, 0xF6, 0x0c);

    gdtr.limit = sizeof(gdt[0]) * GDT_SIZE;
    gdtr.base = GDT_ADDR;

    gdt_flush(&gdtr);
}