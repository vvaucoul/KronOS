/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_pmm.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 10:22:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 13:56:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>
// #include <memory/pmm.h>
// #include <memory/memory_map.h>

void pmm_display(void)
{
    /*
    printk("PMM INFO:\n");
    printk("- " COLOR_YELLOW "Memory Size" COLOR_END ": " COLOR_GREEN "%u" COLOR_END " | " COLOR_GREEN "0x%x\n" COLOR_END, __pmm_info.infos.memory_size, __pmm_info.infos.memory_size);
    printk("- " COLOR_YELLOW "Max Blocks" COLOR_END ": " COLOR_GREEN "%u\n" COLOR_END, __pmm_info.infos.max_blocks);
    printk("- " COLOR_YELLOW "Memory Map Start" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __pmm_info.infos.memory_map_start);
    printk("- " COLOR_YELLOW "Memory Map End" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __pmm_info.infos.memory_map_end);
    printk("- " COLOR_YELLOW "Memory Map Length" COLOR_END ": " COLOR_GREEN "%u\n" COLOR_END, __pmm_info.infos.memory_map_length);
    printk("- " COLOR_YELLOW "Blocks" COLOR_END ": " COLOR_GREEN "0x%x\n" COLOR_END, __pmm_info.blocks);
    */
}

int pmm_test(void)
{
    return (0);
    /*
    __WORKFLOW_HEADER();
    pmm_display();
    printk("PMM Test: \n");

    printk("[Kernel Region " COLOR_GREEN "0" COLOR_END "-" COLOR_GREEN "%u" COLOR_END "] - [" COLOR_GREEN "0x%x" COLOR_END "-" COLOR_GREEN "0x%x" COLOR_END ": " COLOR_GREEN "%u MB" COLOR_END "]\n", pmm_get_next_available_block() - 1, KMAP.available.start_addr, pmm_get_memory_map_end(), (pmm_get_memory_map_end() - KMAP.available.start_addr) / 1024);

    uint32_t *ptr1 = pmm_alloc_block();
    assert(ptr1 == NULL);
    printk("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at index " COLOR_GREEN "%u" COLOR_END "\n", ptr1, (uint32_t)pmm_get_next_available_block());

    uint32_t *ptr2 = pmm_alloc_blocks(5);
    assert(ptr2 == NULL);

    printk("Alloc 5 Blocks at " COLOR_GREEN "0x%x" COLOR_END ", next available block at index " COLOR_GREEN "%u" COLOR_END "\n", ptr2, (uint32_t)pmm_get_next_available_block());

    uint32_t *ptr3 = pmm_alloc_block();
    assert(ptr3 == NULL);

    printk("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at index " COLOR_GREEN "%u" COLOR_END "\n", ptr3, (uint32_t)pmm_get_next_available_block());

    uint32_t *ptr4 = pmm_alloc_blocks(5);
    assert(ptr4 == NULL);

    printk("Alloc 5 Blocks at " COLOR_GREEN "0x%x" COLOR_END ", next available block at index " COLOR_GREEN "%u" COLOR_END "\n", ptr4, (uint32_t)pmm_get_next_available_block());

    uint32_t *ptr5 = pmm_alloc_block();
    assert(ptr5 == NULL);

    printk("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at index " COLOR_GREEN "%u" COLOR_END "\n", ptr5, (uint32_t)pmm_get_next_available_block());

    printk("\n");

    bzero(ptr1, PMM_BLOCK_SIZE);
    memcpy(ptr1, "Hello World", 12);
    printk("ptr1: %s at 0x%x \n", ptr1, ptr1);
    ptr2[0] = 0x12345678;
    ptr2[1] = 0x87654321;
    ptr2[2] = 0x12345678;
    ptr2[3] = 0x87654321;
    ptr2[4] = 0x12345678;
    printk("ptr2: %x %x %x %x %x at 0x%x \n", ptr2[0], ptr2[1], ptr2[2], ptr2[3], ptr2[4], ptr2);

    ptr3 = memcpy(ptr3, "42Born2Code", 12);
    printk("ptr3: %s at 0x%x \n", ptr3, ptr3);
    ptr4 = memcpy(ptr4, "Kernel", 7);
    printk("ptr4: %s at 0x%x \n", ptr4, ptr4);
    ptr5 = memcpy(ptr5, "Test123", 8);
    printk("ptr5: %s at 0x%x \n", ptr5, ptr5);

    printk("\n");

    pmm_free_block(ptr5);
    printk("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at index " COLOR_GREEN "%u" COLOR_END "\n", ptr5, pmm_get_next_available_block());
    pmm_free_blocks(ptr4, 5);
    printk("Free 5 Blocks at " COLOR_GREEN "0x%x" COLOR_END ", next available block at index " COLOR_GREEN "%u" COLOR_END "\n", ptr4, pmm_get_next_available_block());
    pmm_free_block(ptr3);
    printk("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at index " COLOR_GREEN "%u" COLOR_END "\n", ptr3, pmm_get_next_available_block());
    pmm_free_blocks(ptr2, 5);
    printk("Free 5 Blocks at " COLOR_GREEN "0x%x" COLOR_END ", next available block at index " COLOR_GREEN "%u" COLOR_END "\n", ptr2, pmm_get_next_available_block());
    pmm_free_block(ptr1);
    printk("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at index " COLOR_GREEN "%u" COLOR_END "\n", ptr1, pmm_get_next_available_block());
    printk("\nNext Available block " COLOR_GREEN "%u" COLOR_END "\n", pmm_get_next_available_block());
    return (0);
    */
}

// int pmm_defragment_test(void)
// {
//     printk("PMM Defragment Test: \n");
//     uint32_t *ptr1 = pmm_alloc_block();
//     printk("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "0x%x" COLOR_END "\n", ptr1, pmm_get_next_available_block());
//     uint32_t *ptr2 = pmm_alloc_block();
//     printk("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "0x%x" COLOR_END "\n", ptr2, pmm_get_next_available_block());
//     uint32_t *ptr3 = pmm_alloc_block();
//     printk("Alloc Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "0x%x" COLOR_END "\n", ptr3, pmm_get_next_available_block());

//     printk("\n");
//     pmm_display_blocks(3);
//     printk("\n");
//     pmm_free_block(ptr2);
//     printk("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "0x%x" COLOR_END "\n", ptr2, pmm_get_next_available_block());
//     pmm_display_blocks(3);
//     printk("\n");
//     pmm_defragment();
//     printk("Defragmentation done\n");
//     pmm_display_blocks(3);
//     printk("\n");
//     pmm_free_block(ptr1);
//     printk("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "0x%x" COLOR_END "\n", ptr1, pmm_get_next_available_block());
//     pmm_free_block(ptr3);
//     printk("Free Block at " COLOR_GREEN "0x%x" COLOR_END ", next available block at " COLOR_GREEN "0x%x" COLOR_END "\n", ptr3, pmm_get_next_available_block());
//     pmm_display_blocks(3);
//     printk("\n");
//     return (0);
// }