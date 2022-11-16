/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/21 11:26:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 12:44:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GDT_H
#define GDT_H

#define GDT_ADDR (0x00000800 + 0xC0000000)
#define GDT_SIZE 0x07

#include <stdint.h>

typedef struct s_gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity : 4;
    uint8_t flags : 4;
    uint8_t base_high;
} __attribute__((packed)) t_gdt_entry;

#define GDTEntry t_gdt_entry

typedef struct s_gdt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) t_gdt_ptr;

#define GDTPtr t_gdt_ptr

extern GDTPtr gdtr;

extern void gdt_flush(GDTPtr *gdt);
extern void gdt_init(void);

#endif /* !GDT_H */