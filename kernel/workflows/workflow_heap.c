/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_heap.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 13:39:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/04 13:40:16 by vvaucoul         ###   ########.fr       */
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

    printk("\n\nKmalloc test\n");
    ksleep(1);

    void *ptr = kmalloc(10);
    void *ptr2 = kmalloc(10);
    void *ptr3 = kmalloc(10);

    printk("ptr : %p | ptr2 : %p | ptr3 : %p\n", ptr, ptr2, ptr3);

    kfree(ptr);
    kfree(ptr2);
    kfree(ptr3);

    uint32_t i = 0, alloc_size = 1024, total_alloc = 0;
    void *ptrs[1000];
    while (i < 1000)
    {
        ptrs[i] = kmalloc(alloc_size);
        if (ptrs[i] == NULL)
        {
            printk("Failed to allocate %u bytes at iteration %u\n", alloc_size, i);
            break;
        }
        total_alloc += alloc_size;
        printk("Allocated %u bytes at iteration %u | PTR %p | Total = %u\n", alloc_size, i, ptrs[i], total_alloc);
        // timer_wait(10);
        i++;
    }

    i = 0;

    while (i < 1000)
    {
        kfree(ptrs[i]);
        printk("Freed %u bytes at iteration %u | PTR %p | Total = %u\n", alloc_size, i, ptrs[i], total_alloc);
        total_alloc -= alloc_size;
        // timer_wait(10);
        i++;
    }
    printk("Freed %u bytes at iteration %u | PTR %p | Total = %u\n", alloc_size, i, NULL, total_alloc);

    ptr = kmalloc(1024 * 1024);
    printk("Allocated 1MB at %p\n", ptr);

    i = 0;
    while (i < 1000)
    {
        ptrs[i] = kmalloc(alloc_size);
        if (ptrs[i] == NULL)
        {
            printk("Failed to allocate %u bytes at iteration %u\n", alloc_size, i);
            break;
        }
        total_alloc += alloc_size;
        printk("Allocated %u bytes at iteration %u | PTR %p | Total = %u\n", alloc_size, i, ptrs[i], total_alloc);
        // timer_wait(10);
        kfree(ptrs[i]);
        total_alloc -= alloc_size;
        printk("Freed %u bytes at iteration %u | PTR %p | Total = %u\n", alloc_size, i, ptrs[i], total_alloc);
        i++;
    }

    kfree(ptr);

    printk("\n\nKcalloc test\n");
    ksleep(1);

    ptr = kcalloc(14, sizeof(char));
    ptr2 = kcalloc(14, sizeof(char));

    memcpy(ptr, "Hello World !", 13);
    memcpy(ptr2, "Hello World !", 13);
    printk("ptr : %s | ptr2 : %s\n", ptr, ptr2);

    printk("\n\nKrealloc test\n");
    ksleep(1);

    ptr = kmalloc(7);
    bzero(ptr, 7);
    memcpy(ptr, "Hello", 6);

    printk("ptr : %s\n", ptr);

    ptr = krealloc(ptr, 14);
    bzero(ptr, 14);
    memcpy(ptr, "Hello World !", 13);

    printk("ptr : %s\n", ptr);

    printk("\n\nKbrk test\n");
    ksleep(1);

    ptr = kbrk(0);
    printk("ptr : %p\n", ptr);

    ptr = kbrk(1024);
    printk("ptr : %p\n", ptr);

    ptr = kbrk(0);
    printk("ptr : %p\n", ptr);

    ptr = kbrk(1024 * 1024);
    printk("ptr : %p\n", ptr);

    printk("\n\nVirtual Memory functions test\n");
    ksleep(1);

    ptr = vmalloc(1024);
    printk("ptr : %p\n", ptr);

    ptr2 = vmalloc(1024 * 10);
    printk("ptr2 : %p\n", ptr2);

    printk("Size: %u\n", vsize(ptr2));

    vfree(ptr);
    vfree(ptr2);

    printk("\n\nKernel Panic with page fault\n");
    ksleep(1);

    ptr = (void *)0x12345678;
    printk("ptr : %p\n", ptr);

    __WORKFLOW_FOOTER();
}