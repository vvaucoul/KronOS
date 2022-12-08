/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isr.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:16:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/08 21:55:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/isr.h>
#include <memory/memory.h>
#include <system/kerrno.h>

ISR g_interrupt_handlers[NB_INTERRUPT_HANDLERS] = {0};
irqs_t g_irqs[ISR_MAX_COUNT] = {0};

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

static void irq_register(uint8_t index, char *name, irq_code_t code, panic_t type, char *exception, bool zero)
{
    g_irqs[index].name = name;
    g_irqs[index].code = code;
    g_irqs[index].type = type;
    g_irqs[index].exception = exception;
    g_irqs[index].zero = zero;
}

void isrs_install()
{

    idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
    irq_register(0, "Division By Zero", 0x0, FAULT, "#DE", false);

    idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
    irq_register(1, "Debug", 0x1, FAULT, "#DB", false);

    idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
    irq_register(2, "Non Maskable Interrupt", 0x2, INTERRUPT, "NMI", false);

    idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
    irq_register(3, "Breakpoint Exception", 0x3, TRAP, "#BP", false);

    idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
    irq_register(4, "Overflow Exception", 0x4, TRAP, "#OF", false);

    idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
    irq_register(5, "Bound Range Exceeded", 0x5, FAULT, "#BR", false);

    idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
    irq_register(6, "Invalid Opcode", 0x6, FAULT, "#UD", false);

    idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);
    irq_register(7, "Device Not Available", 0x7, FAULT, "#NM", false);

    idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
    irq_register(8, "Double Fault", 0x8, ABORT, "#DF", true);

    idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
    irq_register(9, "Coprocessor Segment Overrun", 0x9, FAULT, "COP", false);

    idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
    irq_register(10, "Invalid TSS", 0xA, FAULT, "#TS", true);

    idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
    irq_register(11, "Segment Not Present", 0xB, FAULT, "#NP", true);

    idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
    irq_register(12, "Stack Fault", 0xC, FAULT, "#SS", true);

    idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
    irq_register(13, "General Protection Fault", 0xD, ABORT, "#GP", true); //tmp

    idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
    irq_register(14, "Page Fault", 0xE, FAULT, "#PF", true);

    idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);
    irq_register(15, "Reserved", 0xF, FAULT, "RES", false);

    idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
    irq_register(16, "x87 Floating Point Exception", 0x10, FAULT, "#MF", false);

    idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
    irq_register(17, "Alignment Check", 0x11, FAULT, "#AC", true);

    idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
    irq_register(18, "Machine Check", 0x12, ABORT, "#MC", false);

    idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
    irq_register(19, "SIMD Floating Point Exception", 0x13, FAULT, "#XM", false);

    idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
    irq_register(20, "Virtualization Exception", 0x14, FAULT, "#VE", false);

    idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
    irq_register(21, "Control Protection", 0x15, FAULT, "CP", true);

    idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
    irq_register(22, "Reserved", 0x16, FAULT, "", false);

    idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);
    irq_register(23, "Hypervisor", 0x1C, FAULT, "HV", false);

    idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
    irq_register(24, "VMM Communication", 0x1D, FAULT, "VC", true);

    idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
    irq_register(25, "Security", 0x1E, FAULT, "SX", true);

    idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
    irq_register(26, "Reserved", 0x1F, FAULT, "", false);

    idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
    irq_register(27, "Triple Fault", 0x20, ABORT, "", false);

    idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
    irq_register(28, "FPU Error Interrupt", 0x21, FAULT, "", false);

    idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
    irq_register(29, "Reserved", 0x22, FAULT, "", false);

    idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
    irq_register(30, "Reserved", 0x23, FAULT, "", false);

    idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
    irq_register(31, "Reserved", 0x24, FAULT, "", false);
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
        panic_t error = g_irqs[r->int_no].type;

        printk("Error Code: %u\n", r->err_code);

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
        __PANIC((const char *)exception_messages[r->int_no]);
    }
}
