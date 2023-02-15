/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:29:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/15 14:37:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PAGING_H
#define PAGING_H

#include <kernel.h>
#include <asm/asm.h>
#include <system/pit.h>

#define PAGE_SIZE 0x1000
#define PAGE_TABLE_SIZE 1024

#define MEMORY_END_PAGE 0x1000000 // 16MB

#define IS_ALIGNED(x) ((x & 0xFFFFF000))
#define ALIGN(x) (x = ((x & 0xFFFFF000) + PAGE_SIZE))

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4
#define PAGE_WRITETHROUGH 0x8
#define PAGE_CACHE_DISABLE 0x10
#define PAGE_ACCESSED 0x20
#define PAGE_DIRTY 0x40
#define PAGE_SIZE_4MB 0x80
#define PAGE_GLOBAL 0x100
#define PAGE_NO_EXECUTE 0x80000000

typedef struct s_page
{
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t unused : 7;
    uint32_t frame : 20;
} page_t;

typedef struct s_page_table
{
    page_t pages[PAGE_TABLE_SIZE];
} page_table_t;

typedef struct s_page_directory
{
    page_table_t *tables[PAGE_TABLE_SIZE];
    uint32_t tablesPhysical[PAGE_TABLE_SIZE];
    uint32_t physicalAddr;
} page_directory_t;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;

extern void init_paging(void);
extern page_t *get_page(uint32_t address, page_directory_t *dir);
extern page_t *create_page(uint32_t address, page_directory_t *dir);

extern void page_fault(struct regs *r);

extern void enable_paging(page_directory_t *dir);
extern uint32_t get_cr2(void);

extern void *get_physical_address(void *addr);
extern void *get_virtual_address(void *addr);

extern void switch_page_directory(page_directory_t *dir);

extern page_t *create_user_page(uint32_t address, uint32_t end_addr,  page_directory_t *dir);
extern void destroy_user_page(page_t *page, page_directory_t *dir);

extern bool paging_enabled;

#endif /* !PAGING_H */