/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_heap.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 13:39:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/15 19:22:46 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>

#include <memory/kheap.h>

#include <kernel.h>
#include <system/panic.h>

#include <system/pit.h>

/*******************************************************************************
 *                           KERNEL HEAP - WORKFLOW                            *
 ******************************************************************************/

void kheap_test(void)
{
    __WORKFLOW_HEADER();

    void *ptr = kmalloc(sizeof(char) * 15);

    if (ptr == NULL)
        __PANIC("Invalid PTR");
    else
    {
        kbzero(ptr, 15);
        kmemcpy(ptr, "Hello World !", 14);
        kprintf("- PTR: [%s]\n", ptr);
    }

    void *ptr_2 = kmalloc(1024);

    if (ptr_2 == NULL)
        __PANIC("Invalid PTR");
    else
    {
        kbzero(ptr_2, 1024);
        kmemcpy(ptr_2, "42 !", 4);
        kprintf("- PTR-2: [%s]\n", ptr_2);
        ((char *)(ptr_2))[2048] = 'A';
        kprintf("- PTR-2 [2048] = %c\n", ((char *)ptr_2)[2048]);
    }

    uint32_t size = ksize(ptr);
    kprintf("- Size of PTR: %d\n", size);

    uint32_t size_2 = ksize(ptr_2);
    kprintf("- Size of PTR-2: %d\n", size_2);

    kprintf("- Freeing PTR\n");
    kfree(ptr);
    kprintf("- Freeing PTR-2\n");
    kfree(ptr_2);

    kprintf("Check Expand Heap\n");

    uint32_t max_heap_size = KHEAP_GET_MAX_SIZE();
    uint32_t start_addr = (uint32_t)KHEAP_GET_START_ADDR();
    uint32_t end_addr = (uint32_t)KHEAP_GET_END_ADDR();
    uint32_t i = 0;
    uint32_t alloc_size = 1024;
    uint32_t loops = max_heap_size / alloc_size / 2;

    __UNUSED(start_addr);

    kprintf("Max Heap Size: %d Mo\n", max_heap_size);
    kprintf("Start Addr: 0x%x\n", start_addr);
    kprintf("End Addr: 0x%x\n", end_addr);

    kprintf("Do %d loops\n", loops);
    timer_wait(1000);
    while (i <= loops)
    {
        void *ptr = kmalloc(alloc_size);
        if (ptr == NULL)
            __PANIC("Invalid PTR");
        __UNUSED(ptr);
        kprintf("[%d] Allocated %d bytes\n", i, alloc_size);
        // timer_wait(100);
        i++;
    }
    kprintf("Do %d loops\n", loops * 4);
    timer_wait(1000);

    i = 0;
    while (i <= (loops * 4))
    {
        void *ptr = kmalloc(alloc_size);
        if (ptr == NULL)
            __PANIC("Invalid PTR");
        __UNUSED(ptr);
        kprintf("[%d] Allocated %d bytes\n", i, alloc_size);
        i++;
    }
    timer_wait(1000);

    i = 0;
    while (i <= (loops * 4))
    {
        char *ptr = kmalloc(alloc_size);
        if (ptr == NULL)
            __PANIC("Invalid PTR");
        else
            kmemcpy(ptr, "Hello World !", 14);
        kprintf("[%d/%d] Allocated %d bytes [%s]\n", i, loops * 4, alloc_size, ptr);
        i++;
    }
}