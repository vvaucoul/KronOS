/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 14:56:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/30 18:14:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _PAGING_H
#define _PAGING_H

#include <kernel.h>

#define PAGE_TABLE_SIZE 1024
#define PAGE_SIZE 4096

typedef struct s_page
{
    int p : 1;     // Present
    int rw : 1;    // Read/Write
    int us : 1;    // User/Supervisor
    int pwt : 1;   // Page-Level Write-Through
    int pcd : 1;   // Page-Level Cache Disable
    int a : 1;     // Accessed
    int d : 1;     // Dirty
    int pat : 1;   // Page-Attribute Table
    int ps : 1;    // Page size
    int g : 1;     // Global
    int avail : 3; // Available
    int pfa : 20;  // Page Frame Address
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

typedef struct s_table
{
    int p : 1;     // Present
    int rw : 1;    // Read/Write
    int us : 1;    // User/Supervisor
    int pwt : 1;   // Page-Level Write-Through
    int pcd : 1;   // Page-Level Cache Disable
    int a : 1;     // Accessed
    int d : 1;     // Dirty
    int pat : 1;   // Page-Attribute Table
    int g : 1;     // Global
    int avail : 3; // Available
    int pfa : 20;  // Page Frame Address
} t_table;

typedef struct s_page_table
{
    t_page pages[PAGE_TABLE_SIZE];
} t_page_table;

typedef struct s_page_directory
{
    t_page_table tables[PAGE_TABLE_SIZE];
} t_page_directory;

#define Page t_page
#define Table t_table
#define PageTable t_page_table
#define PageDirectory t_page_directory

extern uint32_t __page_directory[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_SIZE)));

// extern PageDirectory __page_directory __attribute__((aligned(PAGE_SIZE)));

extern void enable_paging(void);
extern void load_page_directory(void *page);
extern void enable_large_pages(void);

#define __enable_paging() enable_paging()
#define __load_page_directory(page) load_page_directory(page)
#define __enable_large_pages() enable_large_pages()

#define PAGING_OFFSET (sizeof(Page))

extern void *__request_new_page(size_t size);

extern void __pagination_init(void);

#endif /* _PAGING_H */