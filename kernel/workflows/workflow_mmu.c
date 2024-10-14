/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_mmu.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/02 10:50:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/02 12:15:57 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <mm/mm.h>
#include <mm/mmu.h>
#include <mm/mmuf.h>
#include <mm/shared.h>
#include <system/panic.h>
#include <workflows/workflows.h>

#include <system/pit.h>

#include <assert.h>
#include <string.h>

int mmu_compare_page_directories(page_directory_t *dir1, page_directory_t *dir2) {
	if (dir1 == dir2) return 1; // Same directory

	int identical = 1; // Assume directories are identical initially

	for (int i = 0; i < PAGE_ENTRIES; ++i) {
		if (dir1->tables[i] && dir2->tables[i]) {
			for (int j = 0; j < PAGE_ENTRIES; ++j) {
				page_t *page1 = &dir1->tables[i]->pages[j];
				page_t *page2 = &dir2->tables[i]->pages[j];

				if (page1->present != page2->present ||
					page1->rw != page2->rw ||
					page1->user != page2->user ||
					page1->accessed != page2->accessed ||
					page1->dirty != page2->dirty ||
					page1->nx != page2->nx ||
					page1->frame != page2->frame) {
					printk("Mismatch at table %d, page %d:\n", i, j);
					printk("  Dir1 - present: %d, rw: %d, user: %d, accessed: %d, dirty: %d, nx: %d, frame: %u\n",
						   page1->present, page1->rw, page1->user, page1->accessed, page1->dirty, page1->nx, page1->frame);
					printk("  Dir2 - present: %d, rw: %d, user: %d, accessed: %d, dirty: %d, nx: %d, frame: %u\n",
						   page2->present, page2->rw, page2->user, page2->accessed, page2->dirty, page2->nx, page2->frame);
					identical = 0;
				}
			}
		} else if (dir1->tables[i] || dir2->tables[i]) {
			printk("Mismatch at table %d: one table is NULL while the other is not\n", i);
			identical = 0;
		}
	}

	return identical;
}

// Helper function to simulate page fault handler
void simulate_page_fault(uint32_t address) {
	struct regs r;
	// Fill r with relevant info, if needed
	r.eip = address;
	r.err_code = 0; // Adjust this as necessary
	mmu_page_fault_handler(&r);
}

// Test MMU initialization
void test_mmu_init() {
	assert(mmu_get_kernel_directory() != NULL);
	assert(mmu_is_paging_enabled() == 1);
	printk("test_mmu_init: "_GREEN
		   "[OK]\n"_END);
}

// Test getting and setting current directory
void test_mmu_directory_management() {
	page_directory_t *kernel_dir = mmu_get_kernel_directory();
	mmu_set_current_directory(kernel_dir);
	assert(mmu_get_current_directory() == kernel_dir);
	printk("test_mmu_directory_management: "_GREEN
		   "[OK]\n"_END);
}

// Test creating and retrieving a page
void test_mmu_page_creation() {
	page_directory_t *dir = mmu_get_kernel_directory();
	uint32_t test_address = 0x1000; // 4KB address
	page_t *page = mmu_create_page(test_address, dir);
	assert(page != NULL);
	assert(page->present == 1);
	page_t *retrieved_page = mmu_get_page(test_address, dir);
	assert(retrieved_page == page);
	printk("test_mmu_page_creation: "_GREEN
		   "[OK]\n"_END);
}

// Test destroying a page
void test_mmu_page_destruction() {
	page_directory_t *dir = mmu_get_kernel_directory();
	uint32_t test_address = 0x1000;
	mmu_destroy_page(test_address, dir);
	page_t *page = mmu_get_page(test_address, dir);
	assert(page == NULL || page->present == 0);
	printk("test_mmu_page_destruction: "_GREEN
		   "[OK]\n"_END);
}

// Test cloning a page directory
void test_mmu_clone_directory() {
	page_directory_t *dir = mmu_get_kernel_directory();
	page_directory_t *clone = mmu_clone_page_directory(dir);
	assert(clone != NULL);
	if (mmu_compare_page_directories(dir, clone)) {
		printk("test_mmu_clone_directory: " _GREEN "[OK]\n" _END);
	} else {
		printk("test_mmu_clone_directory: " _RED "[FAIL]\n" _END);
	}

	mmu_destroy_page_directory(clone);
	kpause();
}

// Test address translation
void test_mmu_address_translation() {
	page_directory_t *dir = mmu_get_kernel_directory();
	uint32_t test_address = 0xC0000000; // 3GB address
	mmu_create_page(test_address, dir);
	uint32_t physical_address = mmu_get_physical_address(test_address);
	assert(physical_address != 0);
	uint32_t virtual_address = mmu_get_virtual_address(physical_address);
	assert(virtual_address == test_address);
	printk("test_mmu_address_translation: "_GREEN
		   "[OK]\n"_END);
}

// Test memory protection
void test_mmu_protection() {
	page_directory_t *dir = mmu_get_kernel_directory();
	uint32_t test_address = 0x3000; // 12KB address
	mmu_create_page(test_address, dir);
	mmu_protect_region(test_address, PAGE_SIZE, 0x2); // RW protection
	page_t *page = mmu_get_page(test_address, dir);
	assert(page->rw == 1);
	mmu_protect_region(test_address, PAGE_SIZE, 0x4); // User protection
	assert(page->user == 1);
	printk("test_mmu_protection: "_GREEN
		   "[OK]\n"_END);
}

// Test TLB flushing
void test_mmu_tlb_flush() {
	page_directory_t *dir = mmu_get_kernel_directory();
	uint32_t test_address = 0x4000; // 16KB address
	mmu_create_page(test_address, dir);
	mmu_flush_tlb_entry(test_address);
	mmu_flush_tlb();
	printk("test_mmu_tlb_flush: "_GREEN
		   "[OK]\n"_END);
}

void workflow_mmu(void) {
	__WORKFLOW_HEADER();

	if (mmu_is_paging_enabled() == true) {
		printk("MMU is enabled: "_GREEN
			   "[OK]\n"_END);
	} else {
		return;
	}

	test_mmu_init();
	test_mmu_directory_management();
	test_mmu_page_creation();
	test_mmu_page_destruction();
	test_mmu_clone_directory();
	kpause();
	test_mmu_address_translation();
	test_mmu_protection();
	test_mmu_tlb_flush();

	__WORKFLOW_FOOTER();
}