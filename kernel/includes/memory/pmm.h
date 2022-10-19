/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 13:17:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/18 17:42:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMM_H
#define PMM_H

#include <kernel.h>

/*
** PMM: Physical Memory Manager
** Manage the physical memory of the system
** Low level memory management
*/

/*
1. do we have enough free virtual memory?
2. if so, use some structure to record memory allocation status (depends on the allocator)
3. if not, then ask the VMM to enlarge available address space (sbrk/mmap/mapkernelheap etc.)
4. VMM in turn calls the PMM to allocate RAM
5. the newly allocated RAM is recorded in the appropriate paging tables by the VMM
6. go to step 2.
*/

typedef uint32_t pmm_physical_addr_t;

#define PMM_BLOCK_SIZE 4096

enum e_pmm_block_state
{
    PMM_BLOCK_FREE,
    PMM_BLOCK_USED
};

typedef struct s_pmm_block
{
    uint32_t size;
    enum e_pmm_block_state state;
    pmm_physical_addr_t addr;
} pmm_block_t;

typedef struct s_pmm
{
    struct
    {
        uint32_t memory_size;
        uint32_t max_blocks;
        uint32_t used_blocks;
        uint32_t memory_map_start;
        uint32_t memory_map_end;
        uint32_t memory_map_length;
    } infos;

    pmm_block_t *blocks;
} t_pmm;

#define PMM_INFO t_pmm
#define PMM_BLOCK pmm_block_t

#define PMM_OUT_OF_MEMORY(pmm) (pmm.infos.used_blocks >= pmm.infos.max_blocks)
#define PMM_ERR_NO_FREE_BLOCK -1
#define PMM_ERR_NO_AVAILABLE_BLOCK -2

extern PMM_INFO pmm_info;

#define PMM_INIT_BLOCK() ((PMM_BLOCK){0, PMM_BLOCK_FREE, 0x00})

/*
** Init the Physical Memory Manager
*/
extern int pmm_init(pmm_physical_addr_t bitmap, uint32_t total_memory_size);

/*
** Allocate a block of memory
*/
extern void *pmm_alloc_block(void);

/*
** Free a block of memory
*/
extern void pmm_free_block(void *addr);

/*
** Allocate a block of memory of size 'size'
*/
extern void *pmm_alloc_blocks(uint32_t size);

/*
** Free a block of memory of size 'size'
*/
extern void pmm_free_blocks(void *addr, uint32_t size);

/*
** Get the next available block
*/
extern int pmm_get_next_available_block(void);

/*
** Get the next available blocks of size
*/
extern int pmm_get_next_available_blocks(uint32_t size);

/*******************************************************************************
 *                                  PMM UTILS                                  *
 ******************************************************************************/

extern uint32_t pmm_get_max_blocks(void);
extern uint32_t pmm_get_memory_size(void);
extern uint32_t pmm_get_memory_map_start(void);
extern uint32_t pmm_get_memory_map_end(void);
extern uint32_t pmm_get_memory_map_length(void);
extern uint32_t pmm_get_block_size(void);
extern void pmm_defragment(void);

/*******************************************************************************
 *                                  PMM TEST                                   *
 ******************************************************************************/

extern int pmm_test(void);
extern void pmm_display(void);
extern void pmm_display_blocks(uint32_t size);
extern int pmm_defragment_test(void);

#endif /* !PMM_H */