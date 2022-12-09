/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   idt.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:08:45 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 00:46:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IDT_H
#define IDT_H

#include <kernel.h>

/* Defines an IDT entry */
struct idt_entry
{
    unsigned short base_low;
    unsigned short selector; /* Our kernel segment goes here! */
    unsigned char zero;      /* This will ALWAYS be set to 0! */
    unsigned char flags;     /* Set using the above table! */
    unsigned short base_high;
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

#endif /* !IDT_H */