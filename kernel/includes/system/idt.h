/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   idt.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:08:45 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/24 12:59:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IDT_H
#define IDT_H

#include <kernel.h>

/* Defines an IDT entry */
struct idt_entry
{
    unsigned short base_low;  /* The lower 16 bits of the address to jump to when this interrupt fires */
    unsigned short selector;  /* Our kernel segment goes here! */
    unsigned char zero;       /* This will ALWAYS be set to 0! */
    unsigned char flags;      /* Set using the above table! */
    unsigned short base_high; /* The upper 16 bits of the address to jump to */
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

#define IDT_SIZE 256

extern struct idt_entry idt[IDT_SIZE];
extern struct idt_ptr idtp;

/* This exists in 'start.asm', and is used to load our IDT */
extern void idt_load(struct idt_ptr *idtp);
extern void idt_install();
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags);

extern void push_regs(void);
extern void pop_regs(void);

void print_idt_entry(uint8_t num);

#endif /* !IDT_H */