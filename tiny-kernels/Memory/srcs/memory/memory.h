/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/04 15:26:45 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/04 17:49:38 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_H
#define MEMORY_H

#include "../kernel.h"

/*******************************************************************************
 *                                     PMM                                     *
 ******************************************************************************/

/*******************************************************************************
 *                                    HEAP                                     *
 ******************************************************************************/

#define KHEAP_START 0xC0000000
#define KHEAP_INITIAL_SIZE 0x100000
#define PHYS_MEM_SIZE 0x1000000

extern uint32_t kmalloc(uint32_t size);
extern uint32_t kmalloc_a(uint32_t size);
extern uint32_t kmalloc_p(uint32_t size, void *physical_addr);
extern uint32_t kmalloc_ap(uint32_t size, void *physical_addr);

extern uint32_t placement_address;

/*******************************************************************************
 *                                   PAGING                                    *
 ******************************************************************************/

#define PAGE_SIZE 0x1000

#define IS_ALIGNED(x) ((x & 0x00000FFF) == 0)
#define ALIGN(x) (x = (x & 0xFFFFF000) + PAGE_SIZE)

#define INDEX_FROM_BIT(x) (x / (8 * sizeof(x)))
#define OFFSET_FROM_BIT(x) (x % (8 * sizeof(x)))

typedef struct s_page
{
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t reserved : 2;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t reserved_2 : 2;
    uint32_t unused : 3;
    uint32_t frame : 20;
} t_page;

typedef struct s_page_table
{
    t_page pages[1024];
} t_page_table;

typedef struct s_page_directory
{
    t_page_table *tables[1024];
    uint32_t tablesPhysical[1024];
    uint32_t physicalAddr;
} t_page_directory;

extern void init_paging(void);

extern void init_frames(void);
extern void alloc_frame(t_page *page, int is_kernel, int is_writeable);
extern void free_frame(t_page *page);

extern void enable_paging(uint32_t *page_directory);

#endif /* !MEMORY_H */