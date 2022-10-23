/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 20:16:49 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/22 22:40:17 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMM_H
#define PMM_H

typedef uint32_t pmm_physical_addr_t;
typedef uint32_t pmm_info_t;

#define PMM_BLOCK_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE
#define PMM_BITS_ALIGN 32
#define PMM_DEFAULT_ADDR 0xFF

#define PMM_NULL_ADDR (void *)0xFFFFFFFF
#define PMM_SUCCESS 0
#define PMM_FAILURE 1

typedef struct s_pmm
{
    struct
    {
        pmm_info_t memory_size;
        pmm_info_t used_blocks;
        pmm_info_t max_blocks;
        pmm_info_t memory_map_start;
        pmm_info_t memory_map_end;
        pmm_info_t memory_map_length;
    } infos;

    pmm_physical_addr_t *blocks;
} __attribute__((packed)) t_pmm;

#define PMM_INFO t_pmm
#define PMM_BLOCKS t_pmm.blocks

extern PMM_INFO __pmm_info;

extern pmm_info_t pmm_get_max_blocks(void);
extern pmm_info_t pmm_get_used_blocks(void);
extern pmm_info_t pmm_get_used_blocks(void);
extern pmm_info_t pmm_get_free_blocks(void);
extern pmm_info_t pmm_get_memory_size(void);
extern pmm_info_t pmm_get_memory_map_start(void);
extern pmm_info_t pmm_get_memory_map_end(void);
extern pmm_info_t pmm_get_memory_map_length(void);

extern void pmm_init_region(const pmm_physical_addr_t base, const pmm_physical_addr_t size);
extern void pmm_deinit_region(const pmm_physical_addr_t base, const pmm_physical_addr_t size);
extern uint32_t pmm_get_next_available_block(void);
extern uint32_t pmm_get_next_available_blocks(const uint32_t size);
extern void *pmm_alloc_block(void);
extern void pmm_free_block(void *ptr);
extern void *pmm_alloc_blocks(const uint32_t size);
extern void pmm_free_blocks(void *ptr, const uint32_t size);
extern void pmm_init(const pmm_physical_addr_t bitmap, const uint32_t total_memory_size);

#endif /* !PMM_H */