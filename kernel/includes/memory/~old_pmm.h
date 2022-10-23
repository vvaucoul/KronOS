/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 13:17:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 18:46:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMM_H
#define PMM_H

#include <kernel.h>
#include <system/kerrno.h>

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
typedef uint32_t pmm_info_t;

#define PMM_BLOCK_SIZE 4096
#define PMM_SIZE 0x4000
#define PMM_END_ADDR 0xFFFFFFFF - (__HIGHER_HALF_KERNEL__ == false ? 0xC0000000 : 0x0)

#define PMM_NULL_ADDR 0xFFFFFFFF

#define PMM_SHIFT_BLOCK(x) (x = (PMM_BLOCK *)((uint32_t)x + sizeof(PMM_BLOCK)))
#define PMM_OUT_OF_MEMORY(pmm) (pmm.infos.used_blocks >= pmm.infos.max_blocks)

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
    int id;

    struct // Double linked list
    {
        struct s_pmm_block *next;
        struct s_pmm_block *prev;
    } list;
} __attribute__((packed)) pmm_block_t;

typedef struct s_pmm
{
    struct
    {
        pmm_info_t memory_size;
        pmm_info_t max_blocks;
        pmm_info_t used_blocks;
        pmm_info_t memory_map_start;
        pmm_info_t memory_map_end;
        pmm_info_t memory_map_length;
    } infos;

    pmm_block_t *blocks;
} __attribute__((packed)) t_pmm;

#define PMM_INFO t_pmm
#define PMM_BLOCK pmm_block_t

#define PMM_ERR_NO_FREE_BLOCK -1
#define PMM_ERR_NO_AVAILABLE_BLOCK -2

extern PMM_INFO pmm_info;

#define PMM_INIT_BLOCK() ((PMM_BLOCK){0, PMM_BLOCK_FREE, 0x00})

#define __PMM_GET_BLOCK(x)   \
    {                        \
        x = pmm_info.blocks; \
        assert(x == NULL);   \
    }

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
extern PMM_BLOCK *pmm_get_next_available_block(void);

/*
** Get the next available blocks of size
*/
extern PMM_BLOCK *pmm_get_next_available_blocks(uint32_t size);

/*
** Defragment PMM
*/
extern void pmm_defragment(void);

/*******************************************************************************
 *                                  PMM UTILS                                  *
 ******************************************************************************/

extern pmm_info_t pmm_get_max_blocks(void);
extern pmm_info_t pmm_get_memory_size(void);
extern pmm_info_t pmm_get_memory_map_start(void);
extern pmm_info_t pmm_get_memory_map_end(void);
extern pmm_info_t pmm_get_memory_map_length(void);
extern pmm_info_t pmm_get_block_size(void);

/*******************************************************************************
 *                                  PMM TEST                                   *
 ******************************************************************************/

extern int pmm_test(void);
extern void pmm_display(void);
extern void pmm_display_blocks(uint32_t size);
extern int pmm_defragment_test(void);

#endif /* !PMM_H */