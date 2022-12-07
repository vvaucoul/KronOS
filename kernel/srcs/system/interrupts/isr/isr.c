/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isr.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:16:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/07 11:24:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/isr.h>
#include <memory/memory.h>
#include <system/kerrno.h>

ISR g_interrupt_handlers[NB_INTERRUPT_HANDLERS] = {0};
panic_t exceptions_errors[32] = {
    FAULT,     // Divide-by-zero
    FAULT,     // Debug
    INTERRUPT, // Non-maskable
    TRAP,      // Breakpoint
    TRAP,      // Overflow
    FAULT,     // Bound Range Exceed
    FAULT,     // Invalid Opcode
    FAULT,     // Device Not
    ABORT,     // Double Fault
    FAULT,     // Coprocessor
    FAULT,     // Invalid TSS
    FAULT,     // Segment Not
    FAULT,     // Stack-Segment
    FAULT,     // General Protection
    FAULT,     // Page Fault
    FAULT,     // Unknown Interrupt
    FAULT,     // x87 FPU Floating-Point
    FAULT,     // Alignment Check
    ABORT,     // Machine Check
    FAULT,     // SIMD Floating-Point
    FAULT,     // Virtualization
    FAULT,     // Control Protection
    FAULT,     // Reserved
    FAULT,     // Hypervisor
    FAULT,     // VMM Communication
    FAULT,     // Security
    FAULT,     // Reserved
    FAULT,     // Triple Fault
    INTERRUPT  // FPU Error Interrupt
};

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

// Reserved
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

unsigned char *exception_messages[ISR_MAX_COUNT] =
    {
        (unsigned char *)"Division By Zero",
        (unsigned char *)"Debug",
        (unsigned char *)"Non Maskable Interrupt",
        (unsigned char *)"Breakpoint Exception",
        (unsigned char *)"Into Detected Overflow Exception",
        (unsigned char *)"Out of Bounds Exception",
        (unsigned char *)"Invalid Opcode Exception",
        (unsigned char *)"No Coprocessor Exception",
        (unsigned char *)"Double Fault Exception",
        (unsigned char *)"Coprocessor Segment Overrun Exception",
        (unsigned char *)"Bad TSS Exception",
        (unsigned char *)"Segment Not Present Exception",
        (unsigned char *)"Stack Fault Exception",
        (unsigned char *)"General Protection Fault Exception",
        (unsigned char *)"Page Fault Exception",
        (unsigned char *)"Unknown Interrupt Exception",
        (unsigned char *)"Coprocessor Fault Exception",
        (unsigned char *)"Alignment Check Exception",
        (unsigned char *)"Machine Check Exception",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved",
        (unsigned char *)"Reserved"};

void isrs_install()
{
    idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
    idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
    idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
    idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);
    idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
    idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
    idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
    idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
    idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
    idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
    idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
    idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);
    idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
    idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
    idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
    idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
    idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
    idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
    idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
    idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);
    idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
    idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
    idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
    idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
    idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
    idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
    idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
}

void isr_register_interrupt_handler(int num, ISR handler)
{
    if (num < NB_INTERRUPT_HANDLERS)
        idt_set_gate(num, (unsigned)handler, 0x08, 0x8E);
}

void fault_handler(struct regs *r)
{
    KERNO_ASSIGN_ERROR(__KERRNO_SECTOR_ISR, r->int_no);

    if (r->int_no < 32)
    {
        panic_t error = exceptions_errors[r->int_no];

        switch (error)
        {
        case ABORT:
            __PANIC((const char *)exception_messages[r->int_no]);
            break;
        case FAULT:
            __FAULT((const char *)exception_messages[r->int_no]);
            break;
        case TRAP:
            __TRAP((const char *)exception_messages[r->int_no]);
            break;
        case INTERRUPT:
            __INTERRUPT((const char *)exception_messages[r->int_no]);
            break;
        }
    }
    else
    {
        __FAULT((const char *)exception_messages[r->int_no]);
    }
}
