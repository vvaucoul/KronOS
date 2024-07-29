/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   idt.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:08:45 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 23:54:54 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

/* Defines an IDT entry */
struct idt_entry {
	uint16_t base_low;	/* The lower 16 bits of the address to jump to when this interrupt fires */
	uint16_t selector;	/* Our kernel segment goes here! */
	uint8_t zero;		/* This will ALWAYS be set to 0! */
	uint8_t flags;		/* Set using the above table! */
	uint16_t base_high; /* The upper 16 bits of the address to jump to */
} __attribute__((packed));

/* Defines a pointer to an array of interrupt handlers */
struct idt_ptr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

/* This exists in 'start.asm', and is used to load our IDT */
extern void idt_load(struct idt_ptr *idtp);
extern void idt_install();
extern void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

extern void push_regs(void);
extern void pop_regs(void);

void print_idt_entry(uint8_t num);

#endif /* !IDT_H */