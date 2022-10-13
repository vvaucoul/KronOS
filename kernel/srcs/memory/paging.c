/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:46:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/13 15:29:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/paging.h>
#include <memory/frames.h>
#include <asm/asm.h>

#include <memory/kheap.h>

PageDirectory *__kernel_page_directory __attribute__((aligned(PAGE_SIZE))) = NULL;
PageDirectory *__current_page_directory __attribute__((aligned(PAGE_SIZE))) = NULL;

bool __paging_enabled = false;

/* TMP */

// unsigned int __tmp_page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(PAGE_SIZE))) = {0};
// unsigned int __tmp_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_SIZE))) = {0};

// static void __init_tmp_pages()
// {
//     kmemset(__tmp_page_directory, 0, PAGE_DIRECTORY_SIZE * sizeof(unsigned int));
//     kmemset(__tmp_page_table, 0, PAGE_TABLE_SIZE * sizeof(unsigned int));

//     for (uint32_t i = 0; i < PAGE_DIRECTORY_SIZE; i++)
//     {
//         __tmp_page_directory[i] = 0x00000002;
//     }   

//     for (uint32_t i = 0; i < PAGE_TABLE_SIZE; i++)
//     {
//         __tmp_page_table[i] = (i * 0x1000) | 3;
//     }

//     __tmp_page_directory[0] = ((unsigned int)__tmp_page_table) | 0x03;

//     __load_page_directory(__tmp_page_directory);
//     __enable_paging();
//     // __enable_large_pages();
// }

// void identity_paging()
// {
// }

// void *__request_new_page(size_t size)
// {
//     __UNUSED(size);
// }

Page *get_page(uint32_t addr, int make, PageDirectory *dir)
{
    addr /= PAGE_SIZE;
    uint32_t table_idx = addr / PAGE_TABLE_SIZE;
    if (dir->tables[table_idx])
    {
        return &dir->tables[table_idx]->pages[addr % PAGE_TABLE_SIZE];
    }
    else if (make)
    {
        uint32_t tmp;
        dir->tables[table_idx] = (PageTable *)kmalloc_ap(sizeof(PageTable), &tmp);
        kmemset(dir->tables[table_idx], 0, sizeof(PageTable));
        dir->tablesPhysical[table_idx] = tmp | 0x7;
        return &dir->tables[table_idx]->pages[addr % PAGE_TABLE_SIZE];
    }
    return NULL;
}

static void switch_page_directory(PageDirectory *dir)
{
    __current_page_directory = dir;
    __load_page_directory(dir->tablesPhysical);
}

static void  __init()
{
    uint32_t end_mem = 0x1000000;
    __nframes = end_mem / PAGE_SIZE;
    __frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(__nframes));
    kmemset(__frames, 0, INDEX_FROM_BIT(__nframes));

    __kernel_page_directory = (PageDirectory *)kmalloc(sizeof(PageDirectory));
    __current_page_directory = __kernel_page_directory;

    uint32_t i = 0;

    while (i < __placement_address)
    {
        alloc_frame(get_page(i, 1, __kernel_page_directory), 0, 0);
        i += PAGE_SIZE;
    }
    isr_register_interrupt_handler(14, __page_fault);
    switch_page_directory(__kernel_page_directory);
}

static void __init_paging(void)
{
    // __init_tmp_pages();
    // isr_register_interrupt_handler(14, __page_fault);
    // __paging_enabled = true;
    // return;

    // TO DO

    __init();
    return ;

    /*

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
        // __page_directory.pages[0].present = 1;
        // __page_directory.pages[0].accessed = 0;
        // __page_directory.pages[0].user = 1;
        // __page_directory.pages[0].frame = 0x11A;

        //__page_directory.pages[0] = __page_table.pages[0];

        __page_directory.pages[0].present = 1;
        __page_directory.pages[0].rw = 1;
        __page_directory.pages[0].user = 1;
        __page_directory.pages[0].frame = ((uint32_t)__page_table.pages) >> 12;

        isr_register_interrupt_handler(14, __page_fault);

        // set cr3 point to page directory
        // asm volatile("mov %0, %%cr3" ::"r"(&__page_directory.pages));

        // // set bit in cr0 to enable paging
        // asm volatile("mov %%cr0, %0"
        //              : "=r"(cr0));
        // cr0 = cr0 | 0x80000000;
        // asm volatile("mov %0, %%cr0" ::"r"(cr0));

        // set cr3 to page directory address
        __load_page_directory((__page_directory.pages));
        // set cr0 to paging enabled
        __enable_paging();
        // enable 4MB pages
        // __enable_large_pages();
        kpause();
        __paging_enabled = true;
    */
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

    // 0 - 0 - 0
    if (us == false && rw == false && present == false)
        kmemjoin(buffer, PAGE_FAULT_PANIC_000, 0, kstrlen(PAGE_FAULT_PANIC_000));
    // 0 - 0 - 1
    else if (us == false && rw == false && present == true)
        kmemjoin(buffer, PAGE_FAULT_PANIC_001, 0, kstrlen(PAGE_FAULT_PANIC_001));
    // 0 - 1 - 0
    else if (us == false && rw == true && present == false)
        kmemjoin(buffer, PAGE_FAULT_PANIC_010, 0, kstrlen(PAGE_FAULT_PANIC_010));
    // 0 - 1 - 1
    else if (us == false && rw == true && present == true)
        kmemjoin(buffer, PAGE_FAULT_PANIC_011, 0, kstrlen(PAGE_FAULT_PANIC_011));
    // 1 - 0 - 0
    else if (us == true && rw == false && present == false)
        kmemjoin(buffer, PAGE_FAULT_PANIC_100, 0, kstrlen(PAGE_FAULT_PANIC_100));
    // 1 - 0 - 1
    else if (us == true && rw == false && present == true)
        kmemjoin(buffer, PAGE_FAULT_PANIC_101, 0, kstrlen(PAGE_FAULT_PANIC_101));
    // 1 - 1 - 0
    else if (us == true && rw == true && present == false)
        kmemjoin(buffer, PAGE_FAULT_PANIC_110, 0, kstrlen(PAGE_FAULT_PANIC_110));
    // 1 - 1 - 1
    else if (us == true && rw == true && present == true)
        kmemjoin(buffer, PAGE_FAULT_PANIC_111, 0, kstrlen(PAGE_FAULT_PANIC_111));
    else
        kmemjoin(buffer, PAGE_FAULT_PANIC_UNKNOWN, 0, kstrlen(PAGE_FAULT_PANIC_UNKNOWN));
    
    kmemjoin(buffer, "\n\tError: ", kstrlen(buffer), 9);
    if (us == false && rw == false && present == false)
        kmemjoin(buffer, "0 - 0 - 0", kstrlen(buffer), 9);
    else if (us == false && rw == false && present == true)
        kmemjoin(buffer, "0 - 0 - 1", kstrlen(buffer), 9);
    else if (us == false && rw == true && present == false)
        kmemjoin(buffer, "0 - 1 - 0", kstrlen(buffer), 9);
    else if (us == false && rw == true && present == true)
        kmemjoin(buffer, "0 - 1 - 1", kstrlen(buffer), 9);
    else if (us == true && rw == false && present == false)
        kmemjoin(buffer, "1 - 0 - 0", kstrlen(buffer), 9);
    else if (us == true && rw == false && present == true)
        kmemjoin(buffer, "1 - 0 - 1", kstrlen(buffer), 9);
    else if (us == true && rw == true && present == false)
        kmemjoin(buffer, "1 - 1 - 0", kstrlen(buffer), 9);
    else if (us == true && rw == true && present == true)
        kmemjoin(buffer, "1 - 1 - 1", kstrlen(buffer), 9);
    else
        kmemjoin(buffer, "Unknown error", kstrlen(buffer), 13);

    kmemjoin(buffer, "\n\tFaulting address: 0x", kstrlen(buffer), 22);

    char __kitoa_buffer[__KITOA_BUFFER_LENGTH__];
    kbzero(__kitoa_buffer, __KITOA_BUFFER_LENGTH__);
    kitoa(faulting_address, __kitoa_buffer);
    kmemjoin(buffer, __kitoa_buffer, kstrlen(buffer), kstrlen(__kitoa_buffer));
    kmemjoin(buffer, "\n", kstrlen(buffer), 1);
}

void __page_fault(struct regs *r)
{
    /*
    ** US RW  P - Description
    ** 0  0  0 - Supervisory process tried to read a non-present page entry
    ** 0  0  1 - Supervisory process tried to read a page and caused a protection fault
    ** 0  1  0 - Supervisory process tried to write to a non-present page entry
    ** 0  1  1 - Supervisory process tried to write a page and caused a protection fault
    ** 1  0  0 - User process tried to read a non-present page entry
    ** 1  0  1 - User process tried to read a page and caused a protection fault
    ** 1  1  0 - User process tried to write to a non-present page entry
    ** 1  1  1 - User process tried to write a page and caused a protection fault
    */

    char buffer[PAGE_FAULT_BUFFER_SIZE];

    kbzero(buffer, PAGE_FAULT_BUFFER_SIZE);
    __generate_page_fault_panic(buffer, r);
    __PANIC(buffer);
}