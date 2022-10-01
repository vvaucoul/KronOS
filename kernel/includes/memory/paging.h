/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 14:56:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/30 19:55:32 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _PAGING_H
#define _PAGING_H

#include <kernel.h>
#include <system/isr.h>
#include <system/panic.h>

#define PAGE_TABLE_SIZE 1024
#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_SIZE 4096

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
} t_page __attribute__((aligned(PAGE_SIZE)));

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
    t_page pages[PAGE_TABLE_SIZE] ;
} t_page_table __attribute__((aligned(PAGE_SIZE)));

typedef struct s_page_directory
{
    t_page pages[PAGE_DIRECTORY_SIZE];
} t_page_directory __attribute__((aligned(PAGE_SIZE)));

#define Page t_page
#define PageTable t_page_table
#define PageDirectory t_page_directory

extern PageDirectory __page_directory;
extern PageTable __page_table;
extern bool __paging_enabled;

// extern PageDirectory __page_directory __attribute__((aligned(PAGE_SIZE)));

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

#define PAGE_FAULT_BUFFER_SIZE 256
#define PAGE_FAULT_ADDR(x) x = __get_cr2()

extern void *__request_new_page(size_t size);
extern void init_paging(void);
extern void __page_fault(struct regs *r);

#endif /* _PAGING_H */