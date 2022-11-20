/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_heap.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 13:39:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 13:56:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>

// #include <memory/kheap.h>

#include <kernel.h>
#include <system/panic.h>

#include <system/pit.h>

/*******************************************************************************
 *                           KERNEL HEAP - WORKFLOW                            *
 ******************************************************************************/

void kheap_test(void)
{
    /*
    __WORKFLOW_HEADER();

    void *ptr = kmalloc(sizeof(char) * 15);

    if (ptr == NULL)
        __PANIC("Invalid PTR");
    else
    {
        bzero(ptr, 15);
        memcpy(ptr, "Hello World !", 14);
        printk("- PTR: [%s]\n", ptr);
    }

    void *ptr_2 = kmalloc(2048);

    if (ptr_2 == NULL)
        __PANIC("Invalid PTR");
    else
    {
        bzero(ptr_2, 1024);
        memcpy(ptr_2, "42 !", 4);
        printk("- PTR-2: [%s]\n", ptr_2);
        ((char *)(ptr_2))[2048] = 'A';
        printk("- PTR-2 [2048] = %c\n", ((char *)ptr_2)[2048]);
    }

    uint32_t size = ksize(ptr);
    printk("- Size of PTR: %d\n", size);

    uint32_t size_2 = ksize(ptr_2);
    printk("- Size of PTR-2: %d\n", size_2);

    printk("- Freeing PTR\n");
    kfree(ptr);
    printk("- Freeing PTR-2\n");
    kfree(ptr_2);

    printk("Check Expand Heap\n");

    uint32_t max_heap_size = KHEAP_GET_MAX_SIZE();
    uint32_t start_addr = (uint32_t)KHEAP_GET_START_ADDR();
    uint32_t end_addr = (uint32_t)KHEAP_GET_END_ADDR();
    uint32_t i = 0;
    uint32_t alloc_size = 1024;
    uint32_t loops = max_heap_size / alloc_size / 2;
    uint32_t total_alloc = 0;

    __UNUSED(start_addr);

    printk("Max Heap Size: %d Mo\n", max_heap_size);
    printk("Start Addr: 0x%x\n", start_addr);
    printk("End Addr: 0x%x\n", end_addr);

    printk("Do %d loops\n", loops);
    timer_wait(1000);
    while (i <= loops)
    {
        void *ptr = kmalloc(alloc_size);
        total_alloc += alloc_size;
        if (ptr == NULL)
            __PANIC("Invalid PTR");
        __UNUSED(ptr);
        printk("[%d/%d] Allocated %d bytes\n", i, loops, alloc_size);
        kfree(ptr);
        ptr = NULL;
        i++;
    }
    timer_wait(1000);
    printk("Do %d loops\n", loops * 4);
    timer_wait(1000);

    i = 0;
    while (i <= (loops * 4))
    {
        void *ptr = kmalloc(alloc_size);
        total_alloc += alloc_size;
        if (ptr == NULL)
            __PANIC("Invalid PTR");
        __UNUSED(ptr);
        printk("[%d/%d] Allocated %d bytes\n", i, loops * 4, alloc_size);
        kfree(ptr);
        ptr = NULL;
        i++;
    }
    timer_wait(1000);
    printk("Do %d loops and assign\n", loops * 4);
    timer_wait(1000);

    i = 0;
    while (i <= (loops * 4))
    {
        char *ptr = kmalloc(alloc_size);
        total_alloc += alloc_size;
        if (ptr == NULL)
            __PANIC("Invalid PTR");
        else
            memcpy(ptr, "Hello World !", 14);
        printk("[%d/%d] Allocated %d bytes [%s]\n", i, loops * 4, alloc_size, ptr);
        kfree(ptr);
        ptr = NULL;
        i++;
    }
    timer_wait(1000);
    printk("Total Allocated: %d (%d Ko) bytes\n", total_alloc, total_alloc / 1024);
    */
}