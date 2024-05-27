/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:29:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/27 19:25:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PAGING_H
#define PAGING_H

#include <asm/asm.h>
#include <kernel.h>
#include <system/pit.h>

#define PAGE_SIZE 0x1000 // 4KB
#define PAGE_TABLE_SIZE 0x0400 // 1024

#define MEMORY_END_PAGE 0x1000000 // 16MB

#define CR0_PG_BIT (1 << 31) // Enable paging

#define PAGE_MASK 0xFFFFF000

#define IS_ALIGNED(x) ((x & PAGE_MASK))
#define IS_PAGE_ALIGNED(x) ((x & PAGE_MASK) == x)
#define IS_FRAME_ALIGNED(FRAME) (((FRAME) << 12) % PAGE_SIZE == 0)
#define IS_PAGE_MAPPED(x) (get_page(x, current_directory)->frame != 0)
#define IS_PAGE_READABLE(x) (get_page(x, current_directory)->rw)
#define ALIGN(x) (x = ((x & PAGE_MASK) + PAGE_SIZE))

#define VIRTUAL_TO_PHYSICAL(dir, vaddr) (get_physical_address(dir, vaddr))
#define PHYSICAL_TO_VIRTUAL(dir, paddr) (get_virtual_address(dir, paddr))

#define PAGEDIR_INDEX(vaddr) (((uint32_t)vaddr) >> 22)
#define PAGETBL_INDEX(vaddr) ((((uint32_t)vaddr) >>12) & 0x3FF)
#define PAGEFRAME_INDEX(vaddr) (((uint32_t)vaddr) & 0xFFF)

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4
#define PAGE_WRITETHROUGH 0x8
#define PAGE_CACHE_DISABLE 0x10
#define PAGE_ACCESSED 0x20
#define PAGE_DIRTY 0x40
#define PAGE_DIR_SIZE_BIT 0x80
#define PAGE_GLOBAL 0x100
#define PAGE_NO_EXECUTE 0x80000000

typedef struct s_page {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t unused : 7;
    uint32_t frame : 20;
} page_t;

typedef struct s_page_table {
    page_t pages[PAGE_TABLE_SIZE];
} page_table_t;

typedef struct s_page_directory {
    page_table_t *tables[PAGE_TABLE_SIZE];
    uint32_t tablesPhysical[PAGE_TABLE_SIZE];
    uint32_t physicalAddr;
} page_directory_t;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;

void init_paging(void);
page_t *get_page(uint32_t address, page_directory_t *dir);
page_t *create_page(uint32_t address, page_directory_t *dir);

void page_fault(struct regs *r);

void enable_paging(page_directory_t *dir);
uint32_t get_cr2(void);

void *get_physical_address(page_directory_t *dir, void *addr);
void *get_virtual_address(page_directory_t *dir, void *addr);

void switch_page_directory(page_directory_t *dir);
void flush_tlb_entry(uint32_t addr);

page_t *create_user_page(uint32_t address, uint32_t end_addr, page_directory_t *dir);
void destroy_user_page(page_t *page, page_directory_t *dir);

page_directory_t *clone_page_directory(page_directory_t *dir);
page_table_t *clone_table(page_table_t *src, uint32_t *physAddr);
void copy_page_physical(uint32_t src, uint32_t dest);

void destroy_page_directory(page_directory_t *dir);

int is_paging_enabled(void);

int verify_page_directory(page_directory_t *dir);
int verify_page_table(page_table_t *table, uint32_t table_idx);
int verify_page(page_t *page, uint32_t page_idx, uint32_t table_idx);
void display_page_directory(page_directory_t *dir);

extern bool paging_enabled;

#define E_PAGING_NOT_ENABLED "Paging not enabled"
#define E_INVALID_ADDRESS "Invalid address"
#define E_ADDRESS_NOT_ALIGNED "Address not aligned"
#define E_SWITCH_PAGE_DIRECTORY "Failed to switch page directory"

#define READ_CR3() ({                \
    uint32_t cr3;                    \
    __asm__ volatile("mov %%cr3, %0" \
                     : "=r"(cr3));   \
    cr3;                             \
})

#endif /* !PAGING_H */