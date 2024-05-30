/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   page_fault.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:59:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/28 17:51:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/******************************************************************************/
/*                                                                            */
/*                       PAGE FAULT HANDLER DOCUMENTATION                     */
/*                                                                            */
/* ========================================================================== */
/*                                                                            */
/*    The page fault handler is designed to handle exceptions caused by       */
/*    invalid memory accesses. The handler provides detailed information      */
/*    about the nature of the fault and the state of the system when the      */
/*    fault occurs.                                                           */
/*                                                                            */
/* ========================================================================== */
/*                                                                            */
/*    FUNCTION:                                                               */
/*        void page_fault(struct regs *r)                                     */
/*                                                                            */
/*    DESCRIPTION:                                                            */
/*        Handles page fault exceptions by printing detailed information      */
/*        about the fault and the state of the registers at the time of the   */
/*        fault. The function also checks the faulting address and provides   */
/*        specific panic messages based on the type of fault detected.        */
/*                                                                            */
/*    PARAMETERS:                                                             */
/*        struct regs *r - Pointer to the register structure containing the   */
/*                         state of the registers at the time of the fault.   */
/*                                                                            */
/*    FAULT DETAILS:                                                          */
/*        The page fault handler decodes the error code to determine the      */
/*        cause of the fault. The error code is examined to check for the     */
/*        following conditions:                                               */
/*            - Page not present                                              */
/*            - Read-only violation                                           */
/*            - User-mode access                                              */
/*            - Reserved bits violation                                       */
/*            - Instruction fetch violation                                   */
/*                                                                            */
/*    SAMPLE OUTPUT:                                                          */
/*        The handler prints the following information to the console:        */
/*            - Faulting address                                              */
/*            - Error code interpretation                                     */
/*            - CR3 register value                                            */
/*            - Register state                                                */
/*            - Specific panic message based on the type of fault             */
/*                                                                            */
/******************************************************************************/

#include <drivers/vga.h>
#include <memory/memory.h>
#include <memory/paging.h>
#include <system/panic.h>

static void dump_page_info(uint32_t address) {
    page_t *page = get_page(address, current_directory);
    if (page) {
        printk("Page details for address 0x%08x:\n", address);
        printk("Present: %d, RW: %d, User: %d, Accessed: %d, Dirty: %d, Frame: 0x%08x\n",
               page->present, page->rw, page->user, page->accessed, page->dirty, page->frame << 12);
    } else {
        printk("No page information available for address 0x%08x\n", address);
    }
}

void page_fault(struct regs *r) {
    uint32_t faulting_address;

    faulting_address = get_cr2();

    int present = !(r->err_code & 0x1); // Page not present
    int rw = r->err_code & 0x2;         // Write operation
    int us = r->err_code & 0x4;         // Processor was in user-mode
    int reserved = r->err_code & 0x8;   // Overwritten CPU-reserved bits of page entry
    int id = r->err_code & 0x10;        // Caused by an instruction fetch

    printk(_RED "Page fault! "_END
                "( ");
    if (present)
        printk("present ");
    if (rw)
        printk("read-only ");
    if (us)
        printk("user-mode ");
    if (reserved)
        printk("reserved ");
    if (id)
        printk("instruction fetch ");
    printk(") at ");
    printk(_RED "0x%08x"_END, faulting_address);
    printk(" - EIP: 0x%08x\n", r->eip);

    dump_page_info(faulting_address);

    uint32_t cr3 = READ_CR3();
    printk("CR3: 0x%08x\n", cr3);

    // Print out registers
    printk("REGS:\n");
    printk("eax=0x%08x, ebx=0x%08x, ecx=0x%08x, edx=0x%08x\n", r->eax, r->ebx, r->ecx, r->edx);
    printk("edi=0x%08x, esi=0x%08x, ebp=0x%08x, esp=0x%08x\n", r->edi, r->esi, r->ebp, r->esp);
    printk("eip=0x%08x, cs=0x%08x, ss=0x%08x, ds=0x%08x\n", r->eip, r->cs, r->ss, r->ds);
    printk("fs=0x%08x, gs=0x%08x, es=0x%08x, eflags=0x%08x\n", r->fs, r->gs, r->es, r->eflags);

    // Determine the cause of the fault
    if (faulting_address == 0x0) {
        __PANIC("Page fault at NULL pointer");
    } else if (faulting_address >= KERNEL_BASE) {
        __PANIC("Page fault at kernel address");
    } else if (reserved) {
        __PANIC("Page fault at reserved address");
    } else if (rw && !present) {
        __PANIC("Page fault trying to write to non-present page");
    } else if (!us && present) {
        __PANIC("Page fault trying to execute kernel code");
    } else if (rw && us && present) {
        __PANIC("Page fault trying to write to read-only page");
    } else {
        __PANIC("Page fault");
    }
}
