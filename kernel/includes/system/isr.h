/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isr.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:16:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 00:42:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISR_H
#define ISR_H

#include <kernel.h>
#include <system/idt.h>
#include <system/panic.h>

// ! ||--------------------------------------------------------------------------------||
// ! ||                           INTERRUPT SERVICE ROUTINES                           ||
// ! ||--------------------------------------------------------------------------------||

/* ISR Types */
#define FAULT 0
#define TRAP 1
#define INTERRUPT 2
#define ABORT 3

/* IDT Flags */
#define IDT_SELECTOR 0x08  // Kernel code segment offset
#define IDT_FLAG_GATE 0x8E // Interrupt gate

/* ISR Max Count */
#define ISR_MAX_COUNT 32
#define NB_INTERRUPT_HANDLERS 256

/* ISR Codes */
#define ISR_DIVIDE_BY_ZERO 0
#define ISR_DEBUG 1
#define ISR_NON_MASKABLE_INTERRUPT 2
#define ISR_BREAKPOINT 3
#define ISR_OVERFLOW 4
#define ISR_BOUND_RANGE_EXCEED 5
#define ISR_INVALID_OPCODE 6
#define ISR_DEVICE_NOT 7
#define ISR_DOUBLE_FAULT 8
#define ISR_COPROCESSOR 9
#define ISR_INVALID_TSS 10
#define ISR_SEGMENT_NOT 11
#define ISR_STACK_SEGMENT 12
#define ISR_GENERAL_PROTECTION 13
#define ISR_PAGE_FAULT 14
#define ISR_RESERVED 15
#define ISR_X87_FLOATING_POINT 16
#define ISR_ALIGNMENT_CHECK 17
#define ISR_MACHINE_CHECK 18
#define ISR_SIMD_FLOATING_POINT 19
#define ISR_VIRTUALIZATION 20
#define ISR_CONTROL_PROTECTION 21

#define ISR_TRIGGERS 1

/* ISR Functions */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

/* Structure of the registers saved by the ISR */
typedef struct regs {
	uint32_t gs, fs, es, ds;						 /* Segment registers */
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* General purpose registers */
	uint32_t int_no, err_code;						 /* Interrupt number and error code */
	uint32_t eip, cs, eflags, useresp, ss;			 /* Processor state */
} __attribute__((packed)) t_regs;

typedef uint8_t isr_code_t;

/* Structure to register interrupts */
typedef struct s_irqs {
	const char *name;
	isr_code_t code;
	panic_t type;
	const char *exception;
	bool zero;
	bool has_code;
} __attribute__((packed)) irqs_t;

typedef void (*InterruptHandler)(t_regs *);

void isr_install(void);
int isr_register_interrupt_handler(int num, InterruptHandler handler);
void fault_handler(t_regs *r);

/* ISR Trigger Functions */
#if ISR_TRIGGERS == 1
void isr_trigger_division_by_zero();
void isr_trigger_invalid_opcode();
void isr_trigger_page_fault();
void isr_trigger_general_protection_fault();
void isr_trigger_double_fault();
void isr_trigger_breakpoint();
void isr_trigger_overflow();
void isr_trigger_bound_range_exceed();
void isr_trigger_invalid_tss();
#endif

#endif /* !ISR_H */