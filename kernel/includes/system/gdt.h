/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:48:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/21 15:05:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GDT_H
#define GDT_H

#include <asm/asm.h>
#include <kernel.h>
#include <mm/mm.h>

#define SEG_DESCTYPE(x) ((x) << 0x04)
#define SEG_PRES(x) ((x) << 0x07)
#define SEG_SAVL(x) ((x) << 0x0C)
#define SEG_LONG(x) ((x) << 0x0D)
#define SEG_SIZE(x) ((x) << 0x0E)
#define SEG_GRAN(x) ((x) << 0x0F)
#define SEG_PRIV(x) (((x) & 0x03) << 0x05)

#define SEG_DATA_RD 0x00		// Read-Only
#define SEG_DATA_RDA 0x01		// Read-Only, accessed
#define SEG_DATA_RDWR 0x02		// Read/Write
#define SEG_DATA_RDWRA 0x03		// Read/Write, accessed
#define SEG_DATA_RDEXPD 0x04	// Read-Only, expand-down
#define SEG_DATA_RDEXPDA 0x05	// Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD 0x06	// Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX 0x08		// Execute-Only
#define SEG_CODE_EXA 0x09		// Execute-Only, accessed
#define SEG_CODE_EXRD 0x0A		// Execute/Read
#define SEG_CODE_EXRDA 0x0B		// Execute/Read, accessed
#define SEG_CODE_EXC 0x0C		// Execute-Only, conforming
#define SEG_CODE_EXCA 0x0D		// Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC 0x0E		// Execute/Read, conforming
#define SEG_CODE_EXRDCA 0x0F	// Execute/Read, conforming, accessed

#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
						 SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
						 SEG_PRIV(0) | SEG_CODE_EXRD

#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
						 SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
						 SEG_PRIV(0) | SEG_DATA_RDWR

#define GDT_STACK_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
						  SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
						  SEG_PRIV(0) | SEG_DATA_RDWREXPD

#define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
						 SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
						 SEG_PRIV(3) | SEG_CODE_EXRD

#define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
						 SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
						 SEG_PRIV(3) | SEG_DATA_RDWR

#define GDT_STACK_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
						  SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
						  SEG_PRIV(3) | SEG_DATA_RDWREXPD

#define GDT_ENTRY_FLAG_BASE 0xCF
#define GDT_ENTRY_FLAG_ZERO 0x0

#define GDT_ADDRESS 0xC0000800
#define GDT_SIZE 0x07

typedef struct s_gdt_entry {
	uint16_t limit_low;	 // Limit (bits 0-15)
	uint16_t base_low;	 // Base address (bits 0-15)
	uint8_t base_middle; // Base address (bits 16-23)
	uint8_t access;		 // Access flags
	uint8_t granularity; // Granularity flags (bits 16-19)
	uint8_t base_high;	 // Base address (bits 24-31)
} __attribute__((packed)) gdt_entry_t;

#define GDTEntry gdt_entry_t

typedef struct s_gdt_ptr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

#define GDTPtr gdt_ptr_t

extern GDTEntry *gdt;
extern GDTPtr gp;

#define __GDT_LOGS__ false

extern void gdt_init(void);
extern void gdt_add_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
extern void gdt_flush(uint32_t gdt_ptr);

/* Utils */
extern void print_gdt(void);

#endif /* !GDT_H_ */