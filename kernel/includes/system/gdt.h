/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:48:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/07/08 19:00:15 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GDT_H
#define GDT_H

#include "../kernel.h"
#include "../asm/asm.h"

// REF: https://www.youtube.com/watch?v=5LbXClJhxcs

typedef enum
{
    GDT_ACCESS_CODE_READABLE = 0x02,
    GDT_ACCESS_DATA_WRITABLE = 0x02,
    GDT_ACCESS_STACK_WRITABLE = 0x02,

    GDT_ACCESS_CODE_CONFORMING = 0x04,
    GDT_ACCESS_DATA_DIRECTION_NORMAL = 0x00,
    GDT_ACCESS_DATA_DIRECTION_DOWN = 0x04,

    GDT_ACCESS_DATA_SEGMENT = 0x10,
    GDT_ACCESS_CODE_SEGMENT = 0x18,
    GDT_ACCESS_STACK_SEGMENT = 0x1A,

    GDT_ACCESS_DESCRIPTOR_TTS = 0x00,

    GDT_ACCESS_RING0 = 0x00,
    GDT_ACCESS_RING1 = 0x20,
    GDT_ACCESS_RING2 = 0x40,
    GDT_ACCESS_RING3 = 0x60,

    GDT_ACCESS_PRESENT = 0x80,
} GDT_ACCESS;

typedef enum
{
    GDT_FLAG_64_BIT = 0x20,
    GDT_FLAG_32_BIT = 0x40,
    GDT_FLAG_16_BIT = 0x00,

    GDT_FLAG_GRANULARITY_1B = 0x00,
    GDT_FLAG_GRANULARITY_4K = 0x80,
} GDT_FLAGS;

#define SEG_DESCTYPE(x) ((x) << 0x04)
#define SEG_PRES(x) ((x) << 0x07)
#define SEG_SAVL(x) ((x) << 0x0C)
#define SEG_LONG(x) ((x) << 0x0D)
#define SEG_SIZE(x) ((x) << 0x0E)
#define SEG_GRAN(x) ((x) << 0x0F)
#define SEG_PRIV(x) (((x)&0x03) << 0x05)

#define SEG_DATA_RD 0x00        // Read-Only
#define SEG_DATA_RDA 0x01       // Read-Only, accessed
#define SEG_DATA_RDWR 0x02      // Read/Write
#define SEG_DATA_RDWRA 0x03     // Read/Write, accessed
#define SEG_DATA_RDEXPD 0x04    // Read-Only, expand-down
#define SEG_DATA_RDEXPDA 0x05   // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD 0x06  // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX 0x08        // Execute-Only
#define SEG_CODE_EXA 0x09       // Execute-Only, accessed
#define SEG_CODE_EXRD 0x0A      // Execute/Read
#define SEG_CODE_EXRDA 0x0B     // Execute/Read, accessed
#define SEG_CODE_EXC 0x0C       // Execute-Only, conforming
#define SEG_CODE_EXCA 0x0D      // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC 0x0E     // Execute/Read, conforming
#define SEG_CODE_EXRDCA 0x0F    // Execute/Read, conforming, accessed

/* 0x9A */
#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(0) | SEG_CODE_EXRD

/* 0x92 */
#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(0) | SEG_DATA_RDWR

/* 0x96 */
#define GDT_STACK_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                          SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                          SEG_PRIV(0) | SEG_DATA_RDWREXPD

/* 0xFA */
#define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(3) | SEG_CODE_EXRD

/* 0xF2 */
#define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(3) | SEG_DATA_RDWR

/* 0xF6 */
#define GDT_STACK_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                          SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                          SEG_PRIV(3) | SEG_DATA_RDWREXPD

#define __GDT_ADDR__ 0x00000800
#define GDT_SIZE 7

/*
    "code": kernel code, used to stored the executable binary code
    "data": kernel data
    "stack": kernel stack, used to stored the call stack during kernel execution
    "ucode": user code, used to stored the executable binary code for user program
    "udata": user program data
    "ustack": user stack, used to stored the call stack during execution in userland
*/

#define GDT_LIMIT_LOW(limit) ((limit)&0xFFFF)
#define GDT_BASE_LOW(base) ((base)&0xFFFF)
#define GDT_BASE_MIDDLE(base) (((base) >> 16) & 0xFF)
#define GDT_BASE_HIGH(base) (((base) >> 24) & 0xFF)
#define GDT_FLAGS_LIMIT_HI(limit, flags) (((limit >> 16) & 0xF) | flags & 0xF0)
#define GDT_ACCESS(access) (access)

#define GDT_ENTRY(base, limit, access, flags) \
    {                                         \
        GDT_LIMIT_LOW(limit),                 \
            GDT_BASE_LOW(base),               \
            GDT_BASE_MIDDLE(base),            \
            GDT_ACCESS(access),               \
            GDT_FLAGS_LIMIT_HI(limit, flags), \
            GDT_BASE_HIGH(base),              \
    }

typedef struct s_gdt_entry
{
    uint16_t limit_low;  // Limit (bits 0-15)
    uint16_t base_low;   // Base address (bits 0-15)
    uint8_t base_middle; // Base address (bits 16-23)
    uint8_t access;      // Access flags
    uint8_t granularity; // Granularity flags (bits 16-19)
    uint8_t base_high;   // Base address (bits 24-31)
} __attribute__((packed)) t_gdt_entry;

#define GDTEntry t_gdt_entry

typedef struct s_gdt_ptr
{
    uint16_t limit;
    GDTEntry *base;
} __attribute__((packed)) t_gdt_ptr;

#define GDTPtr t_gdt_ptr

extern GDTEntry gdt[GDT_SIZE];
extern GDTPtr *gp;

extern void gdt_flush(uint32_t gdt_ptr);
extern void gdt_install(void);
extern void print_stack(void);
extern void print_gdt(void);
extern void gdt_test(void);

#endif