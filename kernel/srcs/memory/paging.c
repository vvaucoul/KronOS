/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:46:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/24 17:15:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/paging.h>
#include <memory/frames.h>
#include <asm/asm.h>

#include <memory/kheap.h>

#include <system/pit.h>

PageDirectory *__kernel_page_directory __attribute__((aligned(PAGE_SIZE))) = NULL;
PageDirectory *__current_page_directory __attribute__((aligned(PAGE_SIZE))) = NULL;

bool __paging_enabled = false;

uint32_t *__kernel_page_memory = NULL;

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

/* Convert Virtual to Physical Address */
__paging_data_t *virtual_to_phys(PageDirectory *dir, xvaddr_t *vaddr)
{
    if (__paging_enabled == false)
        return ((__paging_data_t *)vaddr - KERNEL_VIRTUAL_BASE);
    else
    {
        uint32_t page_directory_index = PAGE_DIRECTORY_INDEX(vaddr);
        uint32_t page_table_index = PAGE_TABLE_INDEX(vaddr);
        uint32_t page_frame_offset = PAGE_FRAME_INDEX(vaddr);

        assert(dir->tables[page_directory_index] == NULL);

        PageTable *table = dir->tables[page_directory_index];
        assert(table->pages[page_table_index].present);

        uint32_t frame = table->pages[page_table_index].frame;
        uint32_t phys = (frame << 12) + page_frame_offset;
        return ((__paging_data_t *)phys);
    }
}

__paging_data_t *paging_malloc(uint32_t size, bool align)
{
    __paging_data_t *addr = __kernel_page_memory;

    if (align && (IS_ALIGNED(addr) == false))
        addr = (void *)ALIGN_PAGE(addr);
    __kernel_page_memory += size;
    return (addr);
}

void allocate_region(PageDirectory *dir, vaddr_t vaddr_start, vaddr_t vaddr_end, uint32_t hidden, bool is_kernel, bool is_writable)
{
    vaddr_t va_start = vaddr_start & VPAGE_MASK;
    vaddr_t va_end = vaddr_end & VPAGE_MASK;

    assert(va_start >= va_end);
    while (va_start <= va_end)
    {
        if (hidden == true)
            allocate_page(dir, va_start, va_start / PAGE_SIZE, is_kernel, is_writable);
        else
            allocate_page(dir, va_start, 0, is_kernel, is_writable);
        va_start += PAGE_SIZE;
    }
}

void allocate_page(PageDirectory *dir, vaddr_t vaddr, vaddr_t frame, bool is_kernel, bool is_writable)
{
    PageTable *table = NULL;

    assert(dir == NULL);
    uint32_t page_directory_index = PAGE_DIRECTORY_INDEX(vaddr);
    uint32_t page_table_index = PAGE_TABLE_INDEX(vaddr);

    table = dir->tables[page_directory_index];
    if (table == NULL)
    {
        // Check Heap enabled or not
        // if ()
        // {
        // }

        table = kmalloc_a(sizeof(PageTable));
        kmemset(table, 0, sizeof(PageTable));

        uint32_t table_phys = (uint32_t)virtual_to_phys(__kernel_page_directory, table);
        // dir->tables[page_directory_index]->pages.frame = table_phys >> 12;
        // dir->tables[page_directory_index]->pages.present = 1;
        // dir->tables[page_directory_index]->pages.rw = (is_writable) ? 1 : 0;
        // dir->tables[page_directory_index]->pages.user = (is_kernel) ? 0 : 1;
        // dir->tables[page_directory_index]->pages->pageSize =
    }
}

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
    return (NULL);
}

static void inline_enable_paging()
{
    uint32_t cr0;
    uint32_t cr4;

    asm volatile("mov %%cr4, %0"
                 : "=r"(cr4));
    CLEAR_PSEBIT(cr4);
    asm volatile("mov %0, %%cr4" ::"r"(cr4));

    asm volatile("mov %%cr0, %0"
                 : "=r"(cr0));
    SET_PGBIT(cr0);
    asm volatile("mov %0, %%cr0" ::"r"(cr0));

    __paging_enabled = true;
}

static void switch_page_directory(PageDirectory *dir)
{
    // __current_page_directory = dir;
    // __load_page_directory(&dir->tablesPhysical);

    uint32_t cr0 = 0;
    uint32_t cr4 = 0;

    __current_page_directory = dir;
    CLEAR_PSEBIT(cr4);
    asm volatile("mov %0, %%cr3" ::"r"(&dir->tablesPhysical));
    asm volatile("mov %%cr0, %0"
                 : "=r"(cr0));
}

static void __init()
{
    uint32_t end_mem = 0x1000000;
    __nframes = end_mem / PAGE_SIZE;
    __frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(__nframes));
    kmemset(__frames, 0, INDEX_FROM_BIT(__nframes));

    __kernel_page_directory = (PageDirectory *)kmalloc(sizeof(PageDirectory));
    kmemset(__kernel_page_directory, 0, sizeof(PageDirectory));
    __current_page_directory = __kernel_page_directory;

    uint32_t i = 0;
    Page *__current_page = NULL;

    while (i < KHEAP_GET_PLACEMENT_ADDR())
    {
        __current_page = get_page(i, 1, __kernel_page_directory);
        assert(__current_page == NULL);
        alloc_frame(__current_page, false, false);
        i += PAGE_SIZE;
    }
    isr_register_interrupt_handler(14, __page_fault);
    // __load_page_directory((__current_page_directory->tablesPhysical));
    // __enable_paging();
    switch_page_directory(__current_page_directory);
    kpause();
    __paging_enabled = true;
}

static void __init_paging(void)
{
    __kernel_page_memory = __KERNEL_PAGE_MEMORY_INIT();
    kprintf("Paging: Page Memory ADDR: 0x%x\n", __kernel_page_memory);
    // __kernel_page_directory = paging_malloc(sizeof(PageDirectory), true);
    // kmemset(__kernel_page_directory, 0, sizeof(PageDirectory));

    // uint32_t i = 

    // kpause();
    // __init();
    return;

    // __init_tmp_pages();
    // isr_register_interrupt_handler(14, __page_fault);
    // __paging_enabled = true;
    // return;

    // TO DO

    // __enable_paging();

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
    kuitoa_base(faulting_address, 16,__kitoa_buffer);
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