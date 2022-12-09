/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isr.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:16:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 15:06:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/isr.h>
#include <memory/memory.h>
#include <system/kerrno.h>

ISR g_interrupt_handlers[NB_INTERRUPT_HANDLERS] = {0};
irqs_t g_irqs[ISR_MAX_COUNT] = {0};

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

static void isr_register(uint8_t index, char *name, isr_code_t code, panic_t type, char *exception, bool zero, bool has_error_code)
{
    g_irqs[index].name = name;
    g_irqs[index].code = code;
    g_irqs[index].type = type;
    g_irqs[index].exception = exception;
    g_irqs[index].zero = zero;
    g_irqs[index].has_error_code = has_error_code;
}

static __attribute__((no_caller_saved_registers)) void __display_interrupt_frame(struct regs *r)
{
    printk("eax=0x%x, ebx=0x%x, ecx=0x%x, edx=0x%x\n", r->eax, r->ebx, r->ecx, r->edx);
    printk("edi=0x%x, esi=0x%x, ebp=0x%x, esp=0x%x\n", r->edi, r->esi, r->ebp, r->esp);
    printk("eip=0x%x, cs=0x%x, ss=0x%x, eflags=0x%x, useresp=0x%x\n", r->eip, r->ss, r->eflags, r->useresp);
}

__attribute__((interrupt)) void isr_err_00(struct regs *r)
{
    __PUSH_REGS();

    __display_interrupt_frame(r);
    __PANIC_INTERRUPT((const char *)g_irqs[r->int_no].name, r->int_no, g_irqs[r->int_no].type, r->err_code);

    __POP_REGS();
}

__attribute__((interrupt)) void isr_err_01(struct regs *r)
{
    __PUSH_REGS();

    __display_interrupt_frame(r);
    __PANIC_INTERRUPT((const char *)g_irqs[r->int_no].name, r->int_no, g_irqs[r->int_no].type, r->err_code);

    __POP_REGS();
}

void isrs_install()
{
    idt_set_gate(0, (unsigned)isr0, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(0, "Division By Zero", 0x0, FAULT, "#DE", false, false);

    idt_set_gate(1, (unsigned)isr1, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(1, "Debug", 0x1, FAULT, "#DB", false, false);

    idt_set_gate(2, (unsigned)isr2, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(2, "Non Maskable Interrupt", 0x2, INTERRUPT, "NMI", false, false);

    idt_set_gate(3, (unsigned)isr3, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(3, "Breakpoint Exception", 0x3, TRAP, "#BP", false, false);

    idt_set_gate(4, (unsigned)isr4, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(4, "Overflow Exception", 0x4, TRAP, "#OF", false, false);

    idt_set_gate(5, (unsigned)isr5, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(5, "Bound Range Exceeded", 0x5, FAULT, "#BR", false, false);

    idt_set_gate(6, (unsigned)isr6, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(6, "Invalid Opcode", 0x6, FAULT, "#UD", false, false);

    idt_set_gate(7, (unsigned)isr7, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(7, "Device Not Available", 0x7, FAULT, "#NM", false, false);

    idt_set_gate(8, (unsigned)isr8, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(8, "Double Fault", 0x8, ABORT, "#DF", true, true);

    idt_set_gate(9, (unsigned)isr9, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(9, "Coprocessor Segment Overrun", 0x9, FAULT, "COP", false, false);

    idt_set_gate(10, (unsigned)isr10, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(10, "Invalid TSS", 0xA, FAULT, "#TS", true, true);

    idt_set_gate(11, (unsigned)isr11, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(11, "Segment Not Present", 0xB, FAULT, "#NP", true, true);

    idt_set_gate(12, (unsigned)isr12, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(12, "Stack Fault", 0xC, FAULT, "#SS", true, true);

    idt_set_gate(13, (unsigned)isr13, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(13, "General Protection Fault", 0xD, ABORT, "#GP", true, true); // tmp

    idt_set_gate(14, (unsigned)isr14, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(14, "Page Fault", 0xE, FAULT, "#PF", true, true);

    idt_set_gate(15, (unsigned)isr15, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(15, "Reserved", 0xF, FAULT, "RES", false, false);

    idt_set_gate(16, (unsigned)isr16, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(16, "x87 Floating Point Exception", 0x10, FAULT, "#MF", false, false);

    idt_set_gate(17, (unsigned)isr17, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(17, "Alignment Check", 0x11, FAULT, "#AC", true, true);

    idt_set_gate(18, (unsigned)isr18, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(18, "Machine Check", 0x12, ABORT, "#MC", false, false);

    idt_set_gate(19, (unsigned)isr19, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(19, "SIMD Floating Point Exception", 0x13, FAULT, "#XM", false, false);

    idt_set_gate(20, (unsigned)isr20, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(20, "Virtualization Exception", 0x14, FAULT, "#VE", false, false);

    idt_set_gate(21, (unsigned)isr21, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(21, "Control Protection", 0x15, FAULT, "CP", true, true);

    idt_set_gate(22, (unsigned)isr22, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(22, "Reserved", 0x16, FAULT, "", false, false);

    idt_set_gate(23, (unsigned)isr23, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(23, "Hypervisor", 0x1C, FAULT, "HV", false, false);

    idt_set_gate(24, (unsigned)isr24, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(24, "VMM Communication", 0x1D, FAULT, "VC", true, true);

    idt_set_gate(25, (unsigned)isr25, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(25, "Security", 0x1E, FAULT, "SX", true, true);

    idt_set_gate(26, (unsigned)isr26, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(26, "Reserved", 0x1F, FAULT, "", false, false);

    idt_set_gate(27, (unsigned)isr27, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(27, "Triple Fault", 0x20, ABORT, "", false, false);

    idt_set_gate(28, (unsigned)isr28, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(28, "FPU Error Interrupt", 0x21, FAULT, "", false, false);

    idt_set_gate(29, (unsigned)isr29, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(29, "Reserved", 0x22, FAULT, "", false, false);

    idt_set_gate(30, (unsigned)isr30, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(30, "Reserved", 0x23, FAULT, "", false, false);

    idt_set_gate(31, (unsigned)isr31, IDT_SELECTOR, IDT_FLAG_GATE);
    isr_register(31, "Reserved", 0x24, FAULT, "", false, false);
}

void isr_register_interrupt_handler(int num, ISR handler)
{
    assert(num < NB_INTERRUPT_HANDLERS);
    idt_set_gate(num, (unsigned)handler, IDT_SELECTOR, IDT_FLAG_GATE);
}

void tmp(void)
{
    printk("TMP test\n");
}

void fault_handler(struct regs r)
{
    printk("Error %d\n", r.int_no);

    /* CPU Extend 8bits interrupts */
    // r.int_no &= 0xFF;

    printk("Error [%d] code: %d\n", r.int_no, r.err_code);

    KERNO_ASSIGN_ERROR(__KERRNO_SECTOR_ISR, r.int_no);

    if (r.int_no < 32)
    {
        __display_interrupt_frame(&r);
        __PANIC_INTERRUPT((const char *)g_irqs[r.int_no].name, r.int_no, g_irqs[r.int_no].type, r.err_code);
    }
    else
    {
        __PANIC_INTERRUPT("Unhandled Interrupt", r.int_no, ABORT, r.err_code);
    }

    // if (g_interrupt_handlers[r.int_no] != NULL)
    //     g_interrupt_handlers[r.int_no](&r);
}
