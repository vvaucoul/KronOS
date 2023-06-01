/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_heap.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 13:39:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/06/01 12:15:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <memory/kheap.h>
#include <memory/paging.h>
#include <system/panic.h>
#include <workflows/workflows.h>

#include <system/pit.h>

/*******************************************************************************
 *                           KERNEL HEAP - WORKFLOW                            *
 ******************************************************************************/

void test_cr0() {
    uint32_t cr0 = 0;
    __asm__ __volatile__("mov %%cr0, %0"
                         : "=r"(cr0));

    // Check if paging is enabled
    assert((cr0 & 0x80000000) == 0x80000000);

    printk("test_cr0: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_get_physical_address() {
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
    kusleep(10);
}

void test_get_virtual_address() {
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
    kusleep(10);
}

void test_get_page() {
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
    kusleep(10);
}

void test_create_page() {
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
    kusleep(10);
}

void test_create_page_directory() {
    // Allocate a new page directory
    page_directory_t *dir = create_page_directory();
    assert(dir != NULL);

    verify_page_directory(dir);

    // Free the new page directory
    destroy_page_directory(dir);
    printk("test_create_page_directory: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_switch_page_directory() {
    // Allocate a new page directory
    // page_directory_t *dir = create_page_directory();
    // assert(dir != NULL);

    // verify_paging_enabled();
    // verify_page_directory(dir);

    // display_page_directory(dir);

    // uint32_t cr3 = READ_CR3();
    // // Switch to the new page directory

    // page_directory_t *dir = current_directory;
    // switch_page_directory(kernel_directory);

    // uint32_t new_cr3 = READ_CR3();
    // // Switch back to the original page directory
    // switch_page_directory(dir);

    // printk("cr3: 0x%x\n", cr3);
    // printk("new_cr3: 0x%x\n", new_cr3);

    // assert(cr3 != new_cr3);

    // Free the new page directory
    // destroy_page_directory(dir);
    printk("test_switch_page_directory: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_create_user_page() {
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
    printk("test_create_user_page: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kmalloc_int() {
    uint32_t *ptr;
    uint32_t phys;

    // Test allocation of a single uint32_t
    ptr = kmalloc_int(sizeof(uint32_t), true, &phys);
    assert(ptr != NULL);
    assert(phys % 4096 == 0);
    kfree(ptr);

    // Test allocation of a large buffer
    ptr = kmalloc_int(4096 * 10, true, &phys);
    assert(ptr != NULL);
    assert(phys % 4096 == 0);
    kfree(ptr);

    // Test allocation of an unaligned buffer
    ptr = kmalloc_int(10, false, &phys);
    assert(ptr != NULL);
    assert(phys % 4096 != 0);
    kfree(ptr);

    // Test allocation of a buffer that spans multiple pages
    ptr = kmalloc_int(4096 * 3 + 10, true, &phys);
    assert(ptr != NULL);
    assert(phys % 4096 == 0);
    kfree(ptr);

    printk("test_kmalloc_int: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kmalloc_a() {
    uint32_t *ptr;

    // Test allocation of a single uint32_t
    ptr = kmalloc_a(sizeof(uint32_t));
    assert(ptr != NULL);
    assert((uint32_t)ptr % 4 == 0);
    kfree(ptr);

    // Test allocation of a large buffer
    ptr = kmalloc_a(4096 * 10);
    assert(ptr != NULL);
    assert((uint32_t)ptr % 4096 == 0);
    kfree(ptr);

    // Test allocation of an unaligned buffer
    ptr = kmalloc_a(10);
    assert(ptr != NULL);
    assert((uint32_t)ptr % 4 == 0);
    kfree(ptr);

    // Test allocation of a buffer that spans multiple pages
    ptr = kmalloc_a(4096 * 3 + 10);
    assert(ptr != NULL);
    assert((uint32_t)ptr % 4096 == 0);
    kfree(ptr);

    printk("test_kmalloc_a: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kmalloc_p() {
    uint32_t *ptr;
    uint32_t phys;

    // Test allocation of a single uint32_t
    ptr = kmalloc_p(sizeof(uint32_t), &phys);
    assert(ptr != NULL);
    kfree(ptr);

    // Test allocation of a large buffer
    ptr = kmalloc_p(4096 * 10, &phys);
    assert(ptr != NULL);
    kfree(ptr);

    // Test allocation of a buffer that spans multiple pages
    ptr = kmalloc_p(4096 * 3 + 10, &phys);
    assert(ptr != NULL);
    kfree(ptr);

    printk("test_kmalloc_p: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kmalloc_ap() {
    uint32_t *ptr;
    uint32_t phys;

    // Test allocation of a single uint32_t
    ptr = kmalloc_ap(sizeof(uint32_t), &phys);
    assert(ptr != NULL);
    assert((uint32_t)ptr % 4 == 0);
    assert(phys % 4096 == 0);
    kfree(ptr);

    // Test allocation of a large buffer
    ptr = kmalloc_ap(4096 * 10, &phys);
    assert(ptr != NULL);
    assert((uint32_t)ptr % 4096 == 0);
    assert(phys % 4096 == 0);
    kfree(ptr);

    // Test allocation of an unaligned buffer
    ptr = kmalloc_ap(10, &phys);
    assert(ptr != NULL);
    assert((uint32_t)ptr % 4 == 0);
    assert(phys % 4096 == 0);
    kfree(ptr);

    // Test allocation of a buffer that spans multiple pages
    ptr = kmalloc_ap(4096 * 3 + 10, &phys);
    assert(ptr != NULL);
    assert((uint32_t)ptr % 4096 == 0);
    assert(phys % 4096 == 0);
    kfree(ptr);

    printk("test_kmalloc_ap: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kmalloc_v() {
    uint32_t *ptr;

    // Test allocation of a single uint32_t
    ptr = kmalloc_v(sizeof(uint32_t));
    assert(ptr != NULL);
    assert((uint32_t)ptr >= KERNEL_VIRTUAL_BASE);
    kfree_v(ptr);

    // Test allocation of a large buffer
    ptr = kmalloc_v(4096 * 10);
    assert(ptr != NULL);
    assert((uint32_t)ptr >= KERNEL_VIRTUAL_BASE);
    kfree_v(ptr);

    // Test allocation of an unaligned buffer
    ptr = kmalloc_v(10);
    assert(ptr != NULL);
    assert((uint32_t)ptr >= KERNEL_VIRTUAL_BASE);
    kfree_v(ptr);

    // Test allocation of a buffer that spans multiple pages
    ptr = kmalloc_v(4096 * 3 + 10);
    assert(ptr != NULL);
    assert((uint32_t)ptr >= KERNEL_VIRTUAL_BASE);
    kfree_v(ptr);

    printk("test_kmalloc_v: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kmalloc() {
    uint32_t *ptr;

    // Test allocation of a single uint32_t
    ptr = kmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    kfree(ptr);

    // Test allocation of a large buffer
    ptr = kmalloc(4096 * 10);
    assert(ptr != NULL);
    kfree(ptr);

    // Test allocation of an unaligned buffer
    ptr = kmalloc(10);
    assert(ptr != NULL);
    kfree(ptr);

    // Test allocation of a buffer that spans multiple pages
    ptr = kmalloc(4096 * 3 + 10);
    assert(ptr != NULL);
    kfree(ptr);

    printk("test_kmalloc: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_krealloc() {
    uint32_t *ptr;

    // Test reallocation of a single uint32_t
    ptr = kmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    ptr = krealloc(ptr, sizeof(uint32_t) * 2);
    assert(ptr != NULL);
    kfree(ptr);

    // Test reallocation of a large buffer
    ptr = kmalloc(4096 * 10);
    assert(ptr != NULL);
    ptr = krealloc(ptr, 4096 * 20);
    assert(ptr != NULL);
    kfree(ptr);

    // Test reallocation of an unaligned buffer
    ptr = kmalloc(10);
    assert(ptr != NULL);
    ptr = krealloc(ptr, 20);
    assert(ptr != NULL);
    kfree(ptr);

    // Test reallocation of a buffer that spans multiple pages
    ptr = kmalloc(4096 * 3 + 10);
    assert(ptr != NULL);
    ptr = krealloc(ptr, 4096 * 6 + 20);
    assert(ptr != NULL);
    kfree(ptr);

    printk("test_krealloc: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kcalloc() {
    uint32_t *ptr;

    // Test allocation of a single uint32_t
    ptr = kcalloc(1, sizeof(uint32_t));
    assert(ptr != NULL);
    kfree(ptr);

    // Test allocation of a large buffer
    ptr = kcalloc(10, 4096);
    assert(ptr != NULL);
    kfree(ptr);

    // Test allocation of an unaligned buffer
    ptr = kcalloc(1, 10);
    assert(ptr != NULL);
    kfree(ptr);

    // Test allocation of a buffer that spans multiple pages
    ptr = kcalloc(3, 4096);
    assert(ptr != NULL);
    kfree(ptr);

    printk("test_kcalloc: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kfree() {
    uint32_t *ptr;
    ptr = kmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    kfree(ptr);

    printk("test_kfree: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kfree_v() {
    uint32_t *ptr;
    ptr = kmalloc_v(sizeof(uint32_t));
    assert(ptr != NULL);
    kfree_v(ptr);

    printk("test_kfree_v: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kfree_p() {
    uint32_t *ptr;
    uint32_t phys;
    ptr = kmalloc_p(sizeof(uint32_t), &phys);
    assert(ptr != NULL);
    kfree_p(&phys);

    printk("test_kfree_p: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

void test_kbrk() {
    uint32_t *ptr1, *ptr2;
    ptr1 = kbrk(sizeof(uint32_t));
    assert(ptr1 != NULL);
    ptr2 = kbrk(sizeof(uint32_t));
    assert(ptr2 != NULL);
    assert(ptr2 == ptr1 + 1);

    printk("test_kbrk: "_GREEN
           "[OK] " _END "\n");

    kusleep(10);
}

void test_ksize() {
    uint32_t *ptr;
    ptr = kmalloc(sizeof(uint32_t));
    assert(ptr != NULL);
    assert(ksize(ptr) == sizeof(uint32_t));
    kfree(ptr);

    printk("test_ksize: "_GREEN
           "[OK] " _END "\n");
    kusleep(10);
}

int test_paging() {
    __WORKFLOW_HEADER();
    ksleep(1);

    test_cr0();
    test_get_physical_address();
    test_get_virtual_address();
    test_get_page();
    test_create_page();
    // test_create_page_directory();
    // test_switch_page_directory();
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

    __WORKFLOW_FOOTER();

    return 0;
}

void kheap_test(void) {

    ksleep(1);
    test_paging();
    __WORKFLOW_HEADER();
    ksleep(4);

    void *ptr = kmalloc(10);
    void *ptr2 = kmalloc(10);
    void *ptr3 = kmalloc(10);

    ptr = kmalloc(1024);
    printk("Allocated 1024 bytes at 0x%x\n", ptr);

    ptr2 = kmalloc(2048);
    printk("Allocated 2048 bytes at 0x%x\n", ptr2);

    ptr3 = kmalloc(1024);
    printk("Allocated 1024 bytes at 0x%x\n", ptr3);

    printk("ptr : 0X%x | ptr2 : 0X%x | ptr3 : 0X%x\n", ptr, ptr2, ptr3);

    kfree(ptr);
    kfree(ptr2);
    kfree(ptr3);

    kusleep(10);

    uint32_t i = 0, alloc_size = 1024, total_alloc = 0;
    void *ptrs[1000];

    while (i < 512) {
        ptrs[i] = kmalloc(alloc_size);
        if (ptrs[i] == NULL) {
            printk("Failed to allocate %u bytes at iteration %u\n", alloc_size, i);
            break;
        }
        total_alloc += alloc_size;
        printk("Allocated %u bytes at iteration %u | PTR 0x%x | Total = %u\n", alloc_size, i, ptrs[i], total_alloc);
        i++;
    }

    i = 0;
    while (i < 512) {
        kfree(ptrs[i]);
        printk("Freed %u bytes at iteration %u | PTR 0x%x | Total = %u\n", alloc_size, i, ptrs[i], total_alloc);
        total_alloc -= alloc_size;
        i++;
    }
    printk("Freed %u bytes at iteration %u | PTR 0x%x | Total = %u\n", alloc_size, i, NULL, total_alloc);

    kusleep(10);

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

    // Generate page fault

    // printk("\n### Page fault ###\n");

    // volatile uint32_t *ptr_fault = (volatile uint32_t *)0xdeadbeef;
    // uint32_t val = *ptr_fault;
    // __UNUSED(val);

    __WORKFLOW_FOOTER();
}