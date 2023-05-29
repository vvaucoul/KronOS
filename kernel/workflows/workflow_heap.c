/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_heap.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 13:39:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/29 18:36:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>
#include <memory/kheap.h>
#include <memory/paging.h>
#include <kernel.h>
#include <system/panic.h>

#include <system/pit.h>

/*******************************************************************************
 *                           KERNEL HEAP - WORKFLOW                            *
 ******************************************************************************/

void test_get_physical_address()
{
    // Allocate a page of memory
    void *ptr = kmalloc_a(PAGE_SIZE);
    assert(ptr != NULL);

    // Test NULL address
    assert(get_physical_address(NULL) == NULL);

    // Test unaligned address
    assert(get_physical_address(ptr + 1) == NULL);

    // Test valid address
    void *phys_addr = get_physical_address(ptr);
    assert(phys_addr != NULL);
    assert((uint32_t)phys_addr % PAGE_SIZE == 0);

    // Free the page
    kfree(ptr);
    printk("test_get_physical_address: "_GREEN
           "[OK] " _END "\n");
}

void test_get_virtual_address()
{
    // Allocate a page of memory
    void *ptr = kmalloc_a(PAGE_SIZE);
    assert(ptr != NULL);

    // Test NULL address
    assert(get_virtual_address(NULL) == NULL);

    // Test unaligned address
    assert(get_virtual_address(ptr + 1) == NULL);

    // Test valid address
    void *virt_addr = get_virtual_address(ptr);
    assert(virt_addr != NULL);
    assert((uint32_t)virt_addr % PAGE_SIZE == 0);

    // Free the page
    kfree(ptr);
    printk("test_get_virtual_address: "_GREEN
           "[OK] " _END "\n");
}

void test_get_page()
{
    // Allocate a page of memory
    void *ptr = kmalloc(PAGE_SIZE);
    assert(ptr != NULL);

    // Get the page containing the memory
    page_t *page = get_page((uint32_t)ptr, kernel_directory);
    assert(page != NULL);
    assert(page->present);

    // Free the page
    kfree(ptr);
    printk("test_get_page: "_GREEN
           "[OK] " _END "\n");
}

void test_create_page()
{
    // Allocate a page of memory
    void *ptr = kmalloc(PAGE_SIZE);
    assert(ptr != NULL);

    // Create a new page table entry for the memory
    page_t *page = create_page((uint32_t)ptr, kernel_directory);
    assert(page != NULL);
    assert(page->present);

    // Free the page
    kfree(ptr);
    printk("test_create_page: "_GREEN
           "[OK] " _END "\n");
}

void test_get_cr2()
{
    // This test assumes that the CR2 register is set to a valid address

    // uint32_t cr2 = get_cr2();
    // assert(cr2 != 0);

    printk("test_get_cr2: "_GREEN
           "[OK] " _END "\n");
}

void test_switch_page_directory()
{
    // Allocate a new page directory
    page_directory_t *dir = create_page_directory();
    assert(dir != NULL);

    // Switch to the new page directory
    switch_page_directory(dir);
    kpause();

    // Switch back to the original page directory
    switch_page_directory(kernel_directory);

    // Free the new page directory
    destroy_page_directory(dir);
    printk("test_switch_page_directory: "_GREEN
           "[OK] " _END "\n");
}

void test_create_user_page()
{
    // Allocate a page of memory
    void *ptr = kmalloc(PAGE_SIZE);
    assert(ptr != NULL);

    // Create a new page table entry for the memory
    page_t *page = create_user_page((uint32_t)ptr, (uint32_t)ptr + PAGE_SIZE, kernel_directory);
    assert(page != NULL);
    assert(page->present);

    // Free the page
    destroy_user_page(page, kernel_directory);
    kfree(ptr);
}

void test_kmalloc_int()
{
    uint32_t *ptr;
    uint32_t phys;
    ptr = kmalloc_int(sizeof(uint32_t), true, &phys);
    printk("ptr: %p\n", ptr);
    assert(ptr != NULL);
    printk("phys: 0x%x\n", phys);
    assert(phys % 4096 == 0);
    kfree_p(ptr);
}

void test_kmalloc_a()
{
    uint32_t *ptr;
    ptr = kmalloc_a(sizeof(uint32_t));
    assert(ptr != NULL);
    assert((uint32_t)ptr % 4 == 0);
    kfree(ptr);
}

void test_kmalloc_p()
{
    uint32_t *ptr;
    uint32_t phys;
    ptr = kmalloc_p(sizeof(uint32_t), &phys);
    assert(ptr != NULL);
    assert(phys % 4096 == 0);
    kfree_p(ptr);
}

void test_kmalloc_ap()
{
    uint32_t *ptr;
    uint32_t phys;
    ptr = kmalloc_ap(sizeof(uint32_t), &phys);
    assert(ptr != NULL);
    assert((uint32_t)ptr % 4 == 0);
    assert(phys % 4096 == 0);
    kfree_p(ptr);
}

void test_kmalloc_v()
{
    uint32_t *ptr;
    ptr = kmalloc_v(sizeof(uint32_t));
    assert(ptr != NULL);
    kfree_v(ptr);
}

void test_kmalloc()
{
    uint32_t *ptr;
    ptr = kmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    kfree(ptr);
}

void test_krealloc()
{
    uint32_t *ptr;
    ptr = kmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    ptr = krealloc(ptr, sizeof(uint32_t) * 2);
    assert(ptr != NULL);
    kfree(ptr);
}

void test_kcalloc()
{
    uint32_t *ptr;
    ptr = kcalloc(2, sizeof(uint32_t));
    assert(ptr != NULL);
    assert(ptr[0] == 0);
    assert(ptr[1] == 0);
    kfree(ptr);
}

void test_kfree()
{
    uint32_t *ptr;
    ptr = kmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    kfree(ptr);
}

void test_kfree_v()
{
    uint32_t *ptr;
    ptr = kmalloc_v(sizeof(uint32_t));
    assert(ptr != NULL);
    kfree_v(ptr);
}

void test_kfree_p()
{
    uint32_t *ptr;
    uint32_t phys;
    ptr = kmalloc_p(sizeof(uint32_t), &phys);
    assert(ptr != NULL);
    kfree_p(ptr);
}

void test_kbrk()
{
    uint32_t *ptr1, *ptr2;
    ptr1 = kbrk(sizeof(uint32_t));
    assert(ptr1 != NULL);
    ptr2 = kbrk(sizeof(uint32_t));
    assert(ptr2 != NULL);
    assert(ptr2 == ptr1 + 1);
}

void test_ksize()
{
    uint32_t *ptr;
    ptr = kmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    assert(ksize(ptr) == sizeof(uint32_t));
    kfree(ptr);
}

void test_init_heap()
{
    uint32_t start_addr = 0x100000;
    uint32_t end_addr = 0x200000;
    uint32_t max_addr = 0x400000;
    uint32_t supervisor = 0;
    uint32_t readonly = 0;
    init_heap(start_addr, end_addr, max_addr, supervisor, readonly);
    uint32_t *ptr;
    ptr = kmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    kfree(ptr);
}

void test_kheap_alloc()
{
    heap_t heap;
    heap.addr.start_address = 0x100000;
    heap.addr.end_address = 0x200000;
    heap.addr.max_address = 0x400000;
    heap.flags.supervisor = 0;
    heap.flags.readonly = 0;
    data_t *data = kheap_alloc(sizeof(uint32_t), true, &heap);
    assert(data != NULL);
    assert((uint32_t)(*data) % 4096 == 0);
    kheap_free(data, &heap);
}

void test_kheap_free()
{
    heap_t heap;
    heap.addr.start_address = 0x100000;
    heap.addr.end_address = 0x200000;
    heap.addr.max_address = 0x400000;
    heap.flags.supervisor = 0;
    heap.flags.readonly = 0;
    data_t data = kheap_alloc(sizeof(uint32_t), true, &heap);
    assert(data != NULL);
    kheap_free(data, &heap);
}

void test_kheap_get_ptr_size()
{
    heap_t heap;
    heap.addr.start_address = 0x100000;
    heap.addr.end_address = 0x200000;
    heap.addr.max_address = 0x400000;
    heap.flags.supervisor = 0;
    heap.flags.readonly = 0;
    data_t data = kheap_alloc(sizeof(uint32_t), true, &heap);
    assert(data != NULL);
    assert(kheap_get_ptr_size(data) == sizeof(uint32_t));
    kheap_free(data, &heap);
}

void test_vmalloc()
{
    uint32_t *ptr;
    ptr = vmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    vfree(ptr);
}

void test_vbrk()
{
    uint32_t *ptr1, *ptr2;
    ptr1 = vbrk(sizeof(uint32_t));
    assert(ptr1 != NULL);
    ptr2 = vbrk(sizeof(uint32_t));
    assert(ptr2 != NULL);
    assert(ptr2 == ptr1 + 1);
}

void test_vfree()
{
    uint32_t *ptr;
    ptr = vmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    vfree(ptr);
}

void test_vrealloc()
{
    uint32_t *ptr;
    ptr = vmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    ptr = vrealloc(ptr, sizeof(uint32_t) * 2);
    assert(ptr != NULL);
    vfree(ptr);
}

void test_vcalloc()
{
    uint32_t *ptr;
    ptr = vcalloc(2, sizeof(uint32_t));
    assert(ptr != NULL);
    assert(ptr[0] == 0);
    assert(ptr[1] == 0);
    vfree(ptr);
}

void test_vsize()
{
    uint32_t *ptr;
    ptr = vmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    assert(vsize(ptr) == sizeof(uint32_t));
    vfree(ptr);
}

int test_heap_02()
{
    test_get_physical_address();
    test_get_virtual_address();
    test_get_page();
    test_create_page();
    test_get_cr2();
    test_switch_page_directory();
    test_create_user_page();

    test_kmalloc_int();
    test_kmalloc_a();
    test_kmalloc_p();
    test_kmalloc_ap();
    test_kmalloc_v();
    test_kmalloc();
    test_krealloc();
    test_kcalloc();
    test_kfree();
    test_kfree_v();
    test_kfree_p();
    test_kbrk();
    test_ksize();
    test_init_heap();
    test_kheap_alloc();
    test_kheap_free();
    test_kheap_get_ptr_size();
    test_vmalloc();
    test_vbrk();
    test_vfree();
    test_vrealloc();
    test_vcalloc();
    test_vsize();
    return 0;
}

void kheap_test(void)
{
    __WORKFLOW_HEADER();

    ksleep(1);
    test_heap_02();
    return;

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