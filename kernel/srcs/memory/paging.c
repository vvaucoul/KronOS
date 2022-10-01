/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:46:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/30 19:54:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/paging.h>
#include <asm/asm.h>

PageDirectory __page_directory = {0};
PageTable __page_table = {0};
bool __paging_enabled = false;

void *__request_new_page(size_t size)
{
    __UNUSED(size);
}

static void __init_paging(void)
{
    uint32_t cr0 = 0x0;

    kmemset(__page_directory.pages, 0, sizeof(Page) * PAGE_DIRECTORY_SIZE);
    kmemset(__page_table.pages, 0, sizeof(Page) * PAGE_TABLE_SIZE);

    // set all page directory read/write & user access
    for (uint32_t i = 0; i < PAGE_DIRECTORY_SIZE; i++)
    {
        __page_directory.pages[i].rw = 1;
        __page_directory.pages[i].user = 1;
    }

    // Enable 4MB pages
    for (uint32_t i = 0; i < PAGE_TABLE_SIZE; i++)
    {
        __page_table.pages[i].present = 1;
        __page_table.pages[i].rw = 1;
        __page_table.pages[i].user = 1;
        __page_table.pages[i].frame = (i * PAGE_SIZE) >> 12;
    }

    // set first page directory to be accessed with frame 0x11a(kernel region address)
    __page_directory.pages[0].present = 1;
    __page_directory.pages[0].accessed = 0;
    __page_directory.pages[0].user = 1;
    __page_directory.pages[0].frame = 0x11A;

    // isr_register_interrupt_handler(14, __page_fault);

    // set cr3 point to page directory
    // asm volatile("mov %0, %%cr3" ::"r"(&__page_directory.pages));

    // // set bit in cr0 to enable paging
    // asm volatile("mov %%cr0, %0"
    //              : "=r"(cr0));
    // cr0 = cr0 | 0x80000000;
    // asm volatile("mov %0, %%cr0" ::"r"(cr0));

    // set cr3 to page directory address
    __load_page_directory((__page_directory.pages));
    // enable 4MB pages
    __enable_large_pages();
    // set cr0 to paging enabled
    __enable_paging();
    kpause();
    __paging_enabled = true;
}

void init_paging(void)
{
    __init_paging();
}

static void __generate_page_fault_panic(char buffer[PAGE_FAULT_BUFFER_SIZE], struct regs *r)
{
    uint32_t faulting_address;

    PAGE_FAULT_ADDR(faulting_address); // Get the faulting address

    int present = !(r->err_code & 0x1); // Page not present
    int rw = r->err_code & 0x2;         // Write operation?
    int us = r->err_code & 0x4;         // Processor was in user-mode?
    int reserved = r->err_code & 0x8;   // Overwritten CPU-reserved bits of page entry?
    // int id = r->err_code & 0x10;        // Caused by an instruction fetch?

    // Output an error message.
    kmemjoin(buffer, "Page Fault! (", 0, 14);
    if (present)
        kmemjoin(buffer, "present ", kstrlen(buffer), 8);
    if (rw)
        kmemjoin(buffer, "read-only ", kstrlen(buffer), 10);
    if (us)
        kmemjoin(buffer, "user-mode ", kstrlen(buffer), 10);
    if (reserved)
        kmemjoin(buffer, "reserved ", kstrlen(buffer), 9);
    kmemjoin(buffer, ") at 0x", kstrlen(buffer), 7);

    char __kitoa_buffer[__KITOA_BUFFER_LENGTH__];
    kitoa(faulting_address, __kitoa_buffer);
    kmemjoin(buffer, __kitoa_buffer, kstrlen(buffer), kstrlen(__kitoa_buffer));
    kmemjoin(buffer, "\n", kstrlen(buffer), 1);
    kprintf(buffer);
    ASM_CLI();
    while (1)
        ;
}

void __page_fault(struct regs *r)
{
    /*
    US RW  P - Description
    0  0  0 - Supervisory process tried to read a non-present page entry
    0  0  1 - Supervisory process tried to read a page and caused a protection fault
    0  1  0 - Supervisory process tried to write to a non-present page entry
    0  1  1 - Supervisory process tried to write a page and caused a protection fault
    1  0  0 - User process tried to read a non-present page entry
    1  0  1 - User process tried to read a page and caused a protection fault
    1  1  0 - User process tried to write to a non-present page entry
    1  1  1 - User process tried to write a page and caused a protection fault
    */

    char buffer[PAGE_FAULT_BUFFER_SIZE];

    kbzero(buffer, PAGE_FAULT_BUFFER_SIZE);
    __generate_page_fault_panic(buffer, r);
    __PANIC(buffer);

    /*
        if (r->int_no & 0x1)
        {
            // User mode
            if (r->int_no & 0x2)
            {
                // Write
                __PANIC("Page fault: User mode write\n");
            }
            else
            {
                // Read
                __PANIC("Page fault: User mode read\n");
            }
        }
        else if (r->int_no & 0x2)
        {
            // Supervisor mode
            if (r->int_no & 0x2)
            {
                // Write
                __PANIC("Page fault: Supervisor mode write\n");
            }
            else
            {
                // Read
                __PANIC("Page fault: Supervisor mode read\n");
            }
        }
        else
        {
            // Kernel mode
            if (r->int_no & 0x2)
            {
                // Write
                __PANIC("Page fault: Kernel mode write\n");
            }
            else
            {
                // Read
                __PANIC("Page fault: Kernel mode read\n");
            }
        }
        */
}