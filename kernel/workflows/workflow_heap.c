/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_heap.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 13:39:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/06 13:03:07 by vvaucoul         ###   ########.fr       */
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

    printk("ptr : 0X%x | ptr2 : 0X%x | ptr3 : 0X%x\n", ptr, ptr2, ptr3);

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
        printk("Allocated %u bytes at iteration %u | PTR 0x%x | Total = %u\n", alloc_size, i, ptrs[i], total_alloc);
        // timer_wait(10);
        i++;
    }

    i = 0;

    while (i < 1000)
    {
        kfree(ptrs[i]);
        printk("Freed %u bytes at iteration %u | PTR 0x%x | Total = %u\n", alloc_size, i, ptrs[i], total_alloc);
        total_alloc -= alloc_size;
        // timer_wait(10);
        i++;
    }
    printk("Freed %u bytes at iteration %u | PTR 0x%x | Total = %u\n", alloc_size, i, NULL, total_alloc);

    ptr = kmalloc(1024 * 1024);
    printk("Allocated 1MB at 0x%x\n", ptr);

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
        printk("Allocated %u bytes at iteration %u | PTR 0x%x | Total = %u\n", alloc_size, i, ptrs[i], total_alloc);
        // timer_wait(10);
        kfree(ptrs[i]);
        total_alloc -= alloc_size;
        printk("Freed %u bytes at iteration %u | PTR 0x%x | Total = %u\n", alloc_size, i, ptrs[i], total_alloc);
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
    printk("ptr : 0x%x (should be 0)\n", ptr);

    ptr = kbrk(1024);
    printk("ptr : 0x%x\n", ptr);

    ptr = kbrk(0);
    printk("ptr : 0x%x (should be 0)\n", ptr);

    ptr = kbrk(1024 * 1024);
    printk("ptr : 0x%x\n", ptr);

    printk("\n\nVirtual Memory functions test\n");
    ksleep(1);

    ptr = vmalloc(1024);
    printk("ptr : 0x%x\n", ptr);

    ptr2 = vmalloc(1024 * 10);
    printk("ptr2 : 0x%x\n", ptr2);

    printk("Size: %u\n", vsize(ptr2));

    vfree(ptr);
    vfree(ptr2);

    ptrs[0] = kmalloc(8);
    ptrs[1] = kmalloc(8);
    ptrs[2] = kmalloc(8);

    printk("ptrs[0] = 0x%x\n", ptrs[0]);
    printk("ptrs[1] = 0x%x\n", ptrs[1]);
    printk("ptrs[2] = 0x%x\n", ptrs[2]);
    ksleep(1);

    kfree(ptrs[0]);
    kfree(ptrs[1]);
    kfree(ptrs[2]);

    ptrs[4] = kmalloc(8);
    printk("ptrs[4] = 0x%x\n", ptrs[4]);

    kfree(ptrs[4]);

    ptrs[5] = kmalloc(1024 * 1024);
    printk("ptrs[5] = 0x%x\n", ptrs[5]);
    kfree(ptrs[5]);
    ksleep(1);

    printk("\n### VMALLOC ###\n");
    ksleep(1);

    void *ptr1 = vmalloc(12);
    printk("ptr1 = 0x%x\n", ptr1);
    vfree(ptr1);

    ptr2 = vmalloc(1024 * 1024);
    printk("ptr2 = 0x%x\n", ptr2);
    vfree(ptr2);
    ksleep(1);

    printk("\n### Physical addr ###\n");
    ksleep(1);
    void *ptr4 = kmalloc(10);
    void *ptr5 = kmalloc(10);
    void *ptr6 = kmalloc(10);
    printk("ptr1 = 0x%x\n", ptr4);
    printk("ptr2 = 0x%x\n", ptr5);
    printk("ptr3 = 0x%x\n", ptr6);

    kfree(ptr4);
    kfree(ptr5);
    kfree(ptr6);

    ksleep(1);

    ptr1 = kmalloc(10);
    void *r = krealloc(ptr1, 20);
    printk("r = 0x%x\n", r);

    void *c = kcalloc(10, sizeof(char));
    printk("c = 0x%x\n", c);

    kfree(c);
    kfree(r);

    void *vr = vrealloc(ptr1, 20);
    bzero(vr, 20);
    memcpy(vr, "Hello World", 12);
    printk("vr = 0x%x | %s\n", vr, (char *)vr);

    void *vc = vcalloc(13, sizeof(char));
    memcpy(vc, "Hello World", 12);
    printk("vc = 0x%x | %s\n", vc, (char *)vc);

    vfree(vr);
    ksleep(1);

    printk("\n\nKernel Panic with page fault\n");
    ksleep(1);

    ptr = (void *)0x12345678;
    printk("ptr : 0x%x\n", ptr);

    __WORKFLOW_FOOTER();
}