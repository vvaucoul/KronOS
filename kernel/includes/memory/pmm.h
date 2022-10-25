/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 20:16:49 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/24 17:48:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMM_H
#define PMM_H

typedef void pmm_addr_t;
typedef uint32_t pmm_physical_addr_t;
typedef uint32_t pmm_info_t;

#define PMM_BLOCK_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE
#define PMM_BITS_ALIGN 32
#define PMM_DEFAULT_ADDR 0xFFFFFFFF

#define PMM_NULL_ADDR (void *)0xFFFFFFFF
#define PMM_SUCCESS 0
#define PMM_FAILURE -1

#define PMM_GET_BITMAP_ADDR(x) ((uint32_t)x >> 15)
#define PMM_ALIGN_PAGE_SIZE(x) ((x + PMM_BLOCK_ALIGN - 1) & ~(PMM_BLOCK_ALIGN - 1))

typedef struct s_pmm_region
{
    struct __s_region
    {
        pmm_physical_addr_t start_addr;
        pmm_physical_addr_t end_addr;
        uint32_t size;
    } __attribute__((aligned(4))) region;

    struct __s_region *regions;
    pmm_physical_addr_t start_addr;
    pmm_physical_addr_t end_addr;
    uint32_t count;
    uint32_t max_regions;
} __attribute__((packed)) t_pmm_region;

#define __SizeofRegion() ((uint32_t)sizeof(struct __s_region))

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
#define PMM_REGION t_pmm_region
#define PMM_BLOCKS t_pmm.blocks

extern PMM_INFO __pmm_info;
extern PMM_REGION __pmm_region;

/*
** PMM UTILS
*/
extern pmm_info_t pmm_get_max_blocks(void);
extern pmm_info_t pmm_get_used_blocks(void);
extern pmm_info_t pmm_get_used_blocks(void);
extern pmm_info_t pmm_get_free_blocks(void);
extern pmm_info_t pmm_get_memory_size(void);
extern pmm_info_t pmm_get_memory_map_start(void);
extern pmm_info_t pmm_get_memory_map_end(void);
extern pmm_info_t pmm_get_memory_map_length(void);

/*
** PMM REGIONS
*/
extern t_pmm_region *pmm_init_region(const pmm_physical_addr_t base, const pmm_physical_addr_t size);
extern t_pmm_region *pmm_deinit_region(const pmm_physical_addr_t base, const pmm_physical_addr_t size);
extern t_pmm_region *pmm_expand_region(t_pmm_region *region, const pmm_physical_addr_t expand_size);

/*
** PMM BLOCKS
*/
extern uint32_t pmm_get_next_available_block(void);
extern uint32_t pmm_get_next_available_blocks(const uint32_t size);
extern void *pmm_alloc_block(void);
extern void pmm_free_block(void *ptr);
extern void *pmm_alloc_blocks(const uint32_t size);
extern void pmm_free_blocks(void *ptr, const uint32_t size);

/*
** PMM INIT
*/
extern void pmm_loader_init();
extern void pmm_init(const pmm_physical_addr_t bitmap, const uint32_t total_memory_size);

#endif /* !PMM_H */