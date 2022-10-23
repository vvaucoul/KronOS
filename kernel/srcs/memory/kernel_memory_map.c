/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel_memory_map.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 12:06:57 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/23 20:41:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multiboot/multiboot.h>
#include <memory/memory_map.h>
#include <memory/memory.h>

KERNEL_MEMORY_MAP kernel_memory_map;

static int __init_kernel_mmap_deeper(const MultibootInfo *multiboot_info)
{
    MultibootMemoryMap *mmap = (MultibootMemoryMap *)(uint32_t)(multiboot_info->mmap_addr + (uint32_t)KERNEL_VIRTUAL_BASE);
    uint32_t __first_addr = 0x0;
    uint32_t __current_addr = 0x0;
    uint32_t __current_size = 0x0;

    do
    {
        assert(mmap == NULL);
        if (mmap->type == __MULTIBOOT_MEMORY_AVAILABLE &&
            (mmap->len_low + mmap->len_high) >= PAGE_SIZE &&
            mmap->addr_low < MEMORY_GRUB_RESERVED_SPACE &&
            (mmap->addr_low != 0x0 || __first_addr == 0x0))
        {
            __current_addr = mmap->addr_low;
            __current_size = mmap->len_low + mmap->len_high;

            // Grub reserved space
            if (__current_addr == 0x0)
                __first_addr = 0x1;
            else if (__current_addr == 0x100000)
            {
                // MEMORY_PAGE_SCALABILITY -> Security reasons
                // 1Mb bios + 3Mb Kernel = 4Mb <==> 1 Kernel Page Table
                __current_size -= ((PAGE_SIZE * MEMORY_PAGE_SCALABILITY) + (PAGE_TABLE_SIZE * PAGE_SIZE));
                __current_addr += ((PAGE_SIZE * MEMORY_PAGE_SCALABILITY) + (PAGE_TABLE_SIZE * PAGE_SIZE));
            }
            else if ((__current_addr % PAGE_SIZE) != 0)
            {
                // Align to page size
                __current_size -= (__current_size % PAGE_SIZE) - PAGE_SIZE;
                __current_addr -= (__current_addr % PAGE_SIZE) + PAGE_SIZE;
            }

            if (__current_size < PAGE_SIZE)
            {
                // Goto Next memory map chunk
                mmap = (MultibootMemoryMap *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
                continue;
            }

            for (uint32_t i = 0; i < __current_size; i += PAGE_SIZE)
            {
                // kprintf("Free Chunk: 0x%x\n", __current_addr);
                // Todo, store addr
                __current_addr += PAGE_SIZE;
            }
        }
        mmap = (MultibootMemoryMap *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));

    } while ((uint32_t)mmap < multiboot_info->mmap_addr + KERNEL_VIRTUAL_BASE + multiboot_info->mmap_length);

    /*
    if (tmp_grub_info && IS_GFLAG(tmp_grub_info->flags, GFLAG_MMAP)) {
        mmap = (t_mmap *)((uint32_t)tmp_grub_info->mmap_addr + KERNEL_SPACE_V_ADDR);

        while ((uint32_t)mmap < (uint32_t)tmp_grub_info->mmap_addr + KERNEL_SPACE_V_ADDR + tmp_grub_info->mmap_length) {

            if (mmap->type == AVAILABLE_MEMORY && (mmap->length_low + mmap->length_high) >= PAGE_SIZE
                && mmap->base_addr_low < 0xFFFFC000 && (mmap->base_addr_low != 0 || first_addr == 0)) {

                tmp_addr = mmap->base_addr_low;
                tmp_size = mmap->length_low + mmap->length_high;

                if (tmp_addr == 0x0) {
                    //work arround an issue, grub retrieve 2 available space at 0,
                    //avoid using the second one, TODO search about this
                    first_addr = 1;
                }

                if (tmp_addr == 0x100000) {
                    //preserve kernel at 1Mb => jump 3Mb further
                    //(1Mb bios + 3Mb kernel = 4Mb = 1 page table mapping of our kernel)
                    //add 4Mb space for security / scalability
                    tmp_size -= ((PAGE_SIZE * 768) + (1024 * PAGE_SIZE));
                    tmp_addr += ((PAGE_SIZE * 768) + (1024 * PAGE_SIZE));
                } else if (tmp_addr % PAGE_SIZE) {
                    //align memory chunk on PAGE_SIZE
                    tmp_size = tmp_size - (tmp_size % PAGE_SIZE) - PAGE_SIZE;
                    tmp_addr = tmp_addr - (tmp_addr % PAGE_SIZE) + PAGE_SIZE;
                }
                if (tmp_size < PAGE_SIZE) {
                    //chunk is no longer big enough to store a page after alignement
                    mmap = (t_mmap *)((uint32_t)mmap + mmap->size + sizeof(uint32_t));
                    continue ;
                }

                // mark memory chunk as free by unseting bit in bitmap
                for (uint32_t i = 0; i < tmp_size; i += PAGE_SIZE) {
                    pmm_unset(tmp_addr);
                    tmp_addr += PAGE_SIZE;
                }

            }
            mmap = (t_mmap *)((uint32_t)mmap + mmap->size + sizeof(uint32_t));
        }

    } else {
        return (1);
    }
    */
    return (0);
}

static int __init_kernel_mmap(const MultibootInfo *multiboot_info)
{
    for (uint32_t i = 0; i < multiboot_info->mmap_length + KERNEL_VIRTUAL_BASE; i += sizeof(MultibootMemoryMap))
    {
        MultibootMemoryMap *mmap = (MultibootMemoryMap *)(multiboot_info->mmap_addr + KERNEL_VIRTUAL_BASE + i);
        if (mmap->type != __MULTIBOOT_MEMORY_AVAILABLE)
            continue;
        else
        {
            KMAP.available.start_addr = KMAP.kernel.kernel_end + 1024 * 1024;
            KMAP.available.end_addr =  mmap->addr_low + mmap->len_low;
            KMAP.available.length = KMAP.available.end_addr - KMAP.available.start_addr;
            return (0);
        }
    }
    return (1);
}

int get_kernel_memory_map(const MultibootInfo *multiboot_info)
{
    assert(multiboot_info == NULL);
    assert((&kernel_memory_map.kernel) == NULL);

    KMAP.kernel.kernel_start = (uint32_t)&__kernel_section_start;
    KMAP.kernel.kernel_end = (uint32_t)&__kernel_section_end;
    KMAP.kernel.kernel_length = KMAP.kernel.kernel_end - KMAP.kernel.kernel_start;

    KMAP.text.text_addr_start = (uint32_t)&__kernel_text_section_start;
    KMAP.text.text_addr_end = (uint32_t)&__kernel_text_section_end;
    KMAP.text.text_length = KMAP.text.text_addr_end - KMAP.text.text_addr_start;

    KMAP.rodata.rodata_addr_start = (uint32_t)&__kernel_rodata_section_start;
    KMAP.rodata.rodata_addr_end = (uint32_t)&__kernel_rodata_section_end;
    KMAP.rodata.rodata_length = KMAP.rodata.rodata_addr_end - KMAP.rodata.rodata_addr_start;

    KMAP.data.data_addr_start = (uint32_t)&__kernel_data_section_start;
    KMAP.data.data_addr_end = (uint32_t)&__kernel_data_section_end;
    KMAP.data.data_length = KMAP.data.data_addr_end - KMAP.data.data_addr_start;

    KMAP.bss.bss_addr_start = (uint32_t)&__kernel_bss_section_start;
    KMAP.bss.bss_addr_end = (uint32_t)&__kernel_bss_section_end;
    KMAP.bss.bss_length = KMAP.bss.bss_addr_end - KMAP.bss.bss_addr_start;

    KMAP.total.total_memory_length = multiboot_info->mem_upper + multiboot_info->mem_lower;

    if ((__init_kernel_mmap(multiboot_info)) == 1)
        return (1);
    if ((__init_kernel_mmap_deeper(multiboot_info)) == 1)
        return (1);
    return (0);
}