/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 14:56:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/18 17:43:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _PAGING_H
#define _PAGING_H

#include <kernel.h>
#include <system/isr.h>
#include <system/panic.h>

#include <system/sections.h>
#include <memory/pmm.h>

#define PAGE_TABLE_SIZE 1024
#define PAGE_DIRECTORY_SIZE 1024

#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT) // 2^12 = 4096
#define PAGE_MASK (~(PAGE_SIZE - 1))

/* ADDRESS CALCULATION MACROS */
#define PAGE_DIRECTORY_INDEX(x) (((uint32_t)x) >> 22)
#define PAGE_TABLE_INDEX(x) ((((uint32_t)x) >> 12) & 0x03FF)
#define PAGE_FRAME_INDEX(x) ((uint32_t)x & 0x00000FFF)

/* PAGE REGIRSTER MANIPULATION MACROS */
#define SET_PGBIT(cr0) (cr0 = cr0 | 0x80000000)
#define CLEAR_PSEBIT(cr4) (cr4 = cr4 & 0xFFFFFFEF)

typedef void __paging_data_t;
typedef uint32_t vaddr_t;
typedef void xvaddr_t;

#define VPAGE_MASK 0xFFFFF000
#define IS_ALIGNED(x) (((vaddr_t)x | VPAGE_MASK) == 0)
#define ALIGN_PAGE(x) ((vaddr_t)x & VPAGE_MASK + PAGE_SIZE)

#define __KERNEL_PAGE_MEMORY_INIT() ((uint32_t *)(&__kernel_end) + pmm_info.infos.max_blocks)

/*
** Bit Function
** _PAGE_PRESENT    Page is resident in memory and not swapped out
** _PAGE_PROTNONE	Page is resident but not accessable
** _PAGE_RW         Set if the page may be written to
** _PAGE_USER       Set if the page is accessible from user space
** _PAGE_DIRTY      Set if the page is written to
** _PAGE_ACCESSED   Set if the page is accessed
*/

typedef struct s_page
{
    uint32_t present : 1;   // Present
    uint32_t rw : 1;        // Read/Write
    uint32_t user : 1;      // User/Supervisor
    uint32_t pwt : 1;       // Page-Level Write-Through
    uint32_t pcd : 1;       // Page-Level Cache Disable
    uint32_t accessed : 1;  // Accessed
    uint32_t dirty : 1;     // Dirty
    uint32_t pat : 1;       // Page-Attribute Table
    uint32_t pageSize : 1;  // Page size
    uint32_t global : 1;    // Global
    uint32_t available : 3; // Available
    uint32_t frame : 20;    // Page Frame Address
} t_page;

/*
P: indicate if the page or table is in physical memory
• R/W: indicate if the page or table is accessible in writting (equals 1)
• U/S: equals 1 to allow access to non-preferred tasks
• A: indicate if the page or table was accessed
• D: (only for pages table) indicate if the page was written
• PS: (only for pages directory) indicate the size of pages :
◦ 0 = 4kb
◦ 1 = 4mb
Notes: Physical addresses in the pages directory or pages table are written using 20
bits because these addresses are aligned on 4kb, so the last 12bits should be equal to 0.
• A pages directory or pages table used 1024*4 = 4096 bytes = 4k
• A pages table can address 1024 * 4k = 4 Mb
• A pages directory can address 1024 (1024 4k) = 4 Gb
*/

typedef struct s_page_table
{
    t_page pages[PAGE_TABLE_SIZE];
} t_page_table __attribute__((aligned(PAGE_SIZE)));

typedef struct s_page_directory
{
    t_page_table *tables[PAGE_DIRECTORY_SIZE];
    uint32_t tablesPhysical[PAGE_DIRECTORY_SIZE];
    uint32_t physicalAddr;
} t_page_directory __attribute__((aligned(PAGE_SIZE)));

#define Page t_page
#define PageTable t_page_table
#define PageDirectory t_page_directory

extern PageDirectory *__kernel_page_directory __attribute__((aligned(PAGE_SIZE)));
extern PageDirectory *__current_page_directory __attribute__((aligned(PAGE_SIZE)));
extern PageTable __page_table __attribute__((aligned(PAGE_SIZE)));
extern bool __paging_enabled;
extern uint32_t *__kernel_page_memory;

/* ASM ENTRIES */
extern void enable_paging(void);
extern void load_page_directory(void *page);
extern void enable_large_pages(void);
extern void flush_tlb(void);

#define __enable_paging() enable_paging()
#define __load_page_directory(page) load_page_directory(page)
#define __enable_large_pages() enable_large_pages()
#define __flush_tlb() flush_tlb()

#define PAGING_OFFSET (sizeof(Page))

#define PAGE_ACCESS_LEVEL_PL0(x) (x.user == 0)
#define PAGE_ACCESS_LEVEL_PL3(x) (x.user == 1)
#define PAGE_READ_WRITE(x) (x.rw == 0)
#define PAGE_READ_ONLY(x) (x.rw == 1)

#ifndef __GET_CR2
#define __GET_CR2
static inline uint32_t __get_cr2(void)
{
    uint32_t cr2;
    asm volatile("mov %%cr2, %0"
                 : "=r"(cr2));
    return cr2;
}
#endif

#define KERNEL_VIRTUAL_BASE (__HIGHER_HALF_KERNEL__ ? 0xC0000000 : 0x80000000)

#define PAGE_FAULT_BUFFER_SIZE 256
#define PAGE_FAULT_ADDR(x) x = __get_cr2()

extern void *__request_new_page(size_t size);
extern void init_paging(void);
extern void __page_fault(struct regs *r);

extern __paging_data_t *virtual_to_phys(PageDirectory *dir, void *vaddr);
extern __paging_data_t *paging_malloc(uint32_t size, bool align);
extern void allocate_region(PageDirectory *dir, vaddr_t vaddr_start, vaddr_t vaddr_end, uint32_t hidden, bool is_kernel, bool is_writable);
extern void allocate_page(PageDirectory *dir, vaddr_t vaddr, vaddr_t frame, bool is_kernel, bool is_writable);

#define PAGE_ATTRIBUTE_PRESENT 0x1
#define PAGE_ATTRIBUTE_READ_WRITE 0x2
#define PAGE_ATTRIBUTE_USER 0x4
#define PAGE_ATTRIBUTE_WRITE_THROUGH 0x8
#define PAGE_ATTRIBUTE_CACHE_DISABLE 0x10
#define PAGE_ATTRIBUTE_ACCESSED 0x20

/*******************************************************************************
 *                          PAGE FAULT PANIC MESSAGES                          *
 ******************************************************************************/

#ifndef __PAGE_FAULT_PANIC_MESSAGES
#define __PAGE_FAULT_PANIC_MESSAGES

#define PAGE_FAULT_PANIC_000 "\n\tKernel tried to read a page and caused a protection fault"
#define PAGE_FAULT_PANIC_001 "\n\tKernel tried to read a page and caused a protection fault"
#define PAGE_FAULT_PANIC_010 "\n\tKernel tried to write a non-present page"
#define PAGE_FAULT_PANIC_011 "\n\tKernel tried to write a page and caused a protection fault"
#define PAGE_FAULT_PANIC_100 "\n\tUser tried to read a non-present page"
#define PAGE_FAULT_PANIC_101 "\n\tUser tried to read a page and caused a protection fault"
#define PAGE_FAULT_PANIC_110 "\n\tUser tried to write a non-present page"
#define PAGE_FAULT_PANIC_111 "\n\tUser tried to write a page and caused a protection fault"
#define PAGE_FAULT_PANIC_UNKNOWN "\n\tUnknown page fault"

#endif /* !__PAGE_FAULT_PANIC_MESSAGES */

#endif /* _PAGING_H */