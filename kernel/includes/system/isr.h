/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isr.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:16:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 18:27:38 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISR_H
#define ISR_H

#include <kernel.h>
#include <system/idt.h>
#include <system/panic.h>

/*******************************************************************************
 *                         INTERRUPT SERVICE ROUTINES                          *
 ******************************************************************************/

/*
+--------------------+------------+------------+------------+-------+
| Error              | Code       | Type       | Exception  | Zero? |
+--------------------+------------+------------+------------+-------+
| Divide-by-zero     | 0 (0x0)    | Fault      | #DE        | No    |
| Debug              | 1 (0x1)    | Fault/Trap | #DB        | No    |
| Non-maskable       | 2 (0x2)    | Interrupt  | -          | No    |
| Breakpoint         | 3 (0x3)    | Trap       | #BP        | No    |
| Overflow           | 4 (0x4)    | Trap       | #OF        | No    |
| Bound Range Exceed | 5 (0x5)    | Fault      | #BR        | No    |
| Invalid Opcode     | 6 (0x6)    | Fault      | #UD        | No    |
| Device Not         | 7 (0x7)    | Fault      | #NM        | No    |
| Double Fault       | 8 (0x8)    | Abort      | #DF        | Yes   |
| Coprocessor        | 9 (0x9)    | Fault      | -          | No    |
| Invalid TSS        | 10 (0xA)   | Fault      | #TS        | Yes   |
| Segment Not        | 11 (0xB)   | Fault      | #NP        | Yes   |
| Stack-Segment      | 12 (0xC)   | Fault      | #SS        | Yes   |
| General Protection | 13 (0xD)   | Fault      | #GP        | Yes   |
| Page Fault         | 14 (0xE)   | Fault      | #PF        | Yes   |
| Reserved           | 15 (0xF)   | -          | -          | No    |
| x87 Floating-Point | 16 (0x10)  | Fault      | #MF        | No    |
| Alignment Check    | 17 (0x11)  | Fault      | #AC        | Yes   |
| Machine Check      | 18 (0x12)  | Abort      | #MC        | No    |
| SIMD Floating-Point| 19 (0x13)  | Fault      | #XM/#XF    | No    |
| Virtualization     | 20 (0x14)  | Fault      | #VE        | No    |
| Control Protection | 21 (0x15)  | Fault      | #CP        | Yes   |
| Reserved           | 22-27 (0x16| -          | -          | No    |
| Hypervisor         | 28 (0x1C)  | Fault      | #HV        | No    |
| VMM Communication  | 29 (0x1D)  | Fault      | #VC        | Yes   |
| Security           | 30 (0x1E)  | Fault      | #SX        | Yes   |
| Reserved           | 31 (0x1F)  | -          | -          | No    |
| Triple Fault       | -          | -          | -          | No    |
| FPU Error Interrupt| IRQ 13     | Interrupt  | #FERR      | No    |
+--------------------+------------+------------+------------+-------+
*/

/* Interrupt Service Routine */

/*  
    P: 1bit, DPL: 2bits, S: 1bit, TYPE: 4bits

    - P: Present
    - DPL: Descriptor Privilege Level
    - S: Storage Segment
    - TYPE: Gate Type

    Interrupt Gate (0xE) : 32bit Interrupt Handler
    Trap Gate (0xF) : 32bit Interrupt Handler, with IF=1
    Task Gate (0x5) : 32bit TSS Selector
    Call Gate (0xC) : 32bit Interrupt Handler, with IF=0
*/

#define IDT_SELECTOR 0x08
#define IDT_FLAG_GATE 0x8E

#define ISR_MAX_COUNT 32

typedef struct regs
{
    /* Segment registers */
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    /* 32-bit registers */
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t int_no;
    uint32_t err_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} __attribute__((packed)) t_regs;

typedef struct s_regs_16
{
    /* 16-bit registers */
    uint16_t di;
    uint16_t si;
    uint16_t bp;
    uint16_t sp;
    uint16_t bx;
    uint16_t dx;
    uint16_t cx;
    uint16_t ax;

    /* segments */
    uint16_t ds;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
    uint16_t ss;
    uint16_t eflags;
} __attribute__((packed)) regs16_t;

typedef uint8_t isr_code_t;

typedef struct s_irqs
{
    char *name;
    isr_code_t code;
    panic_t type;
    char *exception;
    bool zero;
    bool has_code;
} __attribute__((packed)) irqs_t;

extern irqs_t g_irqs[ISR_MAX_COUNT];

extern void isr0();  // Division By Zero Exception
extern void isr1();  // Debug Exception
extern void isr2();  // Non Maskable Interrupt Exception
extern void isr3();  // Breakpoint Exception
extern void isr4();  // Into Detected Overflow Exception
extern void isr5();  // Out of Bounds Exception
extern void isr6();  // Invalid Opcode Exception
extern void isr7();  // No Coprocessor Exception
extern void isr8();  // Double Fault Exception
extern void isr9();  // Coprocessor Segment Overrun Exception
extern void isr10(); // Bad TSS Exception
extern void isr11(); // Segment Not Present Exception
extern void isr12(); // Stack Fault Exception
extern void isr13(); // General Protection Fault Exception
extern void isr14(); // Page Fault Exception
extern void isr15(); // Unknown Interrupt Exception
extern void isr16(); // Coprocessor Fault Exception
extern void isr17(); // Alignment Check Exception
extern void isr18(); // Machine Check Exception
extern void isr19(); // Reserved
extern void isr20(); // Reserved
extern void isr21(); // Reserved
extern void isr22(); // Reserved
extern void isr23(); // Reserved
extern void isr24(); // Reserved
extern void isr25(); // Reserved
extern void isr26(); // Reserved
extern void isr27(); // Reserved
extern void isr28(); // Reserved
extern void isr29(); // Reserved
extern void isr30(); // Reserved
extern void isr31(); // Reserved

extern unsigned char *exception_messages[ISR_MAX_COUNT];

typedef void (*ISR)(t_regs *);

#define NB_INTERRUPT_HANDLERS 256

extern ISR g_interrupt_handlers[NB_INTERRUPT_HANDLERS];

extern void isrs_install();
extern void isr_register_interrupt_handler(int num, ISR handler);
extern void fault_handler(struct regs *r);

extern void clean_registers(void (*func)(struct regs *r));
extern void save_stack(void (*func)(struct regs *r));

#endif /* !ISR_H */