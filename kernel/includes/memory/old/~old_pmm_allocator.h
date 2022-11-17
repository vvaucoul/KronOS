/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm_allocator.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 14:21:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/22 19:27:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PMM_ALLOCATOR_H
#define __PMM_ALLOCATOR_H

#include <kernel.h>

enum e_pmm_block_state
{
    PMM_BLOCK_FREE,
    PMM_BLOCK_USED
};

typedef enum e_pmm_block_state __pmm_state_t;
typedef uint32_t __pmm_physical_addr_t;

#define PMM_NULL_ADDR 0xFFFFFFFF
#define PMM_SIZE 0x4000
#define PMM_DEFAULT_ADDR_VALUE 0xFF
#define PMM_BLOCK_SIZE 4096

#define PMM_SUCCESS 0
#define PMM_ERROR 1
#define PMM_ERROR_ADDR NULL

typedef struct __s_pmm_block
{
    uint32_t size;
    __pmm_state_t state;
    __pmm_physical_addr_t addr;
    uint32_t id;

    struct
    {
        struct __s_pmm_block *next;
        struct __s_pmm_block *prev;
    } list;
} __attribute__((packed, aligned(4))) __t_pmm_block;

#define PMM_BLOCK __t_pmm_block

extern PMM_BLOCK *__blocks;
extern PMM_BLOCK *__first_free_block;
extern PMM_BLOCK *__last_block;

#define __PMM_GET_TMP_BLOCK() __pmm_allocator_get_tmp_block()

#define __PMM_SET_BLOCK(block, addr, size, state, id) __pmm_allocator_set_block(block, addr, size, state, id)

#define __PMM_SET_BLOCK_NEXT(block, next) __pmm_allocator_set_block_next(block, next)

#define __PMM_SET_BLOCK_PREV(block, prev) __pmm_allocator_set_block_prev(block, prev)

#define __PMM_SHIFT_BLOCK(block)                                    \
    {                                                               \
        block = (PMM_BLOCK *)((uint32_t)block + sizeof(PMM_BLOCK)); \
    }

#define __PMM_UPDATE_REFERENCES(root, first_free_block, last_block) __pmm_allocator_update_references(root, first_free_block, last_block)

/*
** Init Blocks
*/
extern int __pmm_allocator_init(__pmm_physical_addr_t bitmap);

/*
** Init PMM Allocator
*/
extern int __pmm_allocator_init_blocks(uint32_t max_size);

#endif /* !__PMM_ALLOCATOR_H */